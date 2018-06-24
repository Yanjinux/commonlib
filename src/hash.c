#include "hash.h"
#include <stdbool.h>
unsigned int hashGenHashFunction(const void *key, int len) {
    /* 'm' and 'r' are mixing constants generated offline.
     They're not really 'magic', they just happen to work well.  */
    uint32_t seed = dict_hash_function_seed;
    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    /* Initialize the hash to a 'random' value */
    uint32_t h = seed ^ len;

    /* Mix 4 bytes at a time into the hash */
    const unsigned char *data = (const unsigned char *)key;

    while(len >= 4) {
        uint32_t k = *(uint32_t*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    /* Handle the last few bytes of the input array  */
    switch(len) {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0]; h *= m;
    };

    /* Do a few final mixes of the hash to ensure the last few
     * bytes are well-incorporated. */
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return (unsigned int)h;
}





void
hash_destroy(struct hash *hs ,freeValCB func){
	struct dictEntry * dict;
	struct dictEntry * next;
	int index = 0;
	if( func == NULL){
		for( index = 0 ; index < hs->size; index++){
			dict = hs->table[index];
			while(dict != NULL){
				next = dict->next;
				FREE_ENTRY(dict);
				dict = next;
			}
			hs->table[index] = NULL;
		}
			
	} else {
		for( index = 0 ; index < hs->size; index++){
				dict = hs->table[index];
				while(dict != NULL){
					next = dict->next;
					func(dict->val);
					FREE_ENTRY(dict);	
					dict = next;		
				}
				hs->table[index] = NULL;
			}
	}
	FREE_HASH(hs);
}

struct hash *
hash_init(struct hash * dict , int hash_node){
	if (hash_node < 0 || !dict){
		return NULL;
	}

	if(hash_node == 0){
		hash_node = DEFAULT_HASH_NODE;
	}

	dict->table = (dictEntry **)malloc(sizeof(dictEntry *)*hash_node);
	if (dict->table == NULL){
		return NULL;
	}

	dict->size = hash_node;
	dict->sizemask = hash_node - 1;
	dict->used = 0;
	return dict;
}

/*
   复制长度为len的字符串val，并返回指针，内存由调用者释放
*/
void * dup_string(char * val , int len){
	if (val == NULL || len < 0 ){
		return NULL;
	}

	char * tmp = (char *)malloc(sizeof(char)*(len+1));
	if( tmp == NULL){
		err_log("%s\n","hash_init malloc fail");
		return NULL;
	}
	memcpy(tmp , val , len);
	tmp[len]='\0';
	return tmp;
}


/*
	添加hash
*/
int
hash_add(struct hash *dict , char * key , void * val){
	if( !dict || !key || !val){
		return -1;
	}
	int key_len = strlen(key);
	unsigned int key_index = hashGenHashFunction(key,key_len)%dict->sizemask;
	struct dictEntry * entry = (struct dictEntry *)malloc(sizeof(struct dictEntry));
	entry->key = dup_string(key,key_len);
	entry->val = val;
	entry->next = dict->table[key_index];
	dict->table[key_index] = entry;
	dict->used++;
	return 0;
}



/*
	添加key val 保证所对应的key 唯一
	如果添加失败返回 -1
*/
int
hash_add_first(struct hash * dict, char * key, void * val){
	if(hash_find(dict, key)){
		return -1;
	}
	return hash_add(dict, key, val);
}

/*
  删除指定key的hash ,返回val
*/
void *
hash_delete(struct hash *dict , char * key){
	if (!dict || !key) {
		return NULL;
	}

	int key_len = strlen(key);
	unsigned int key_index = hashGenHashFunction(key,key_len)%dict->sizemask;
	struct dictEntry * entry = dict->table[key_index];
	struct dictEntry * pre_entry = NULL;
	while(entry != NULL){
		if(!strncmp(entry->key, key, key_len)){
			if( pre_entry == NULL){
				dict->table[key_index] = entry->next;
			} else {
				pre_entry->next = entry->next;
			}

			void * tmp_ptr = entry->val;

			FREE_ENTRY(entry);
			dict->used--;
			return tmp_ptr;
		} else {
			pre_entry = entry;
			entry = entry->next;
		}
	}
	return NULL;

}




/*
	删除并释放val
*/
void
hash_delete_and_free(struct hash * dict , char * key){
	void * val = hash_delete(dict , key);
	if (val){
		free(val);
	}
}

/*
	查找对应的hash并返回entry
	找不到就返回NULL
*/
struct  dictEntry *
hash_find(struct hash * dict , char * key){
	if ( !dict || !key){
		return NULL;
	}

	int key_len = strlen(key);
	unsigned int key_index = hashGenHashFunction(key,key_len)%dict->sizemask;
	struct dictEntry * entry = dict->table[key_index];
	while(entry != NULL){
		if(!strncmp(entry->key, key, key_len)){
			return entry;
		} else {
			entry = entry->next;
		}
	}
	return NULL;
}

int 
hash_reg_match_mqtt(const char *sub, size_t sublen, const char *topic, size_t topiclen, bool *result)
{
        int spos, tpos;
        bool multilevel_wildcard = false;

        if(!result) return -1;
        *result = false;

        if(!sub || !topic){
                return -1;
        }

        if(!sublen || !topiclen){
                *result = false;
                return -1;
        }
        /* ￥代表系统树*/
        if(sublen && topiclen){
                if((sub[0] == '$' && topic[0] != '$')
                                || (topic[0] == '$' && sub[0] != '$')){

                        return 0;
                }
        }

        spos = 0;
        tpos = 0;

        while(spos < sublen && tpos <= topiclen){
                if(sub[spos] == topic[tpos]){
                        /* 处理 /http/#   and /http */
                        if(tpos == topiclen-1){
                                /* Check for e.g. foo matching foo/# */
                                if(spos == sublen-3
                                                && sub[spos+1] == '/'
                                                && sub[spos+2] == '#'){
                                        *result = true;
                                        multilevel_wildcard = true;
                                        return 0;
                                }
                        }
                        spos++;
                        tpos++;
                        /* 处理完美匹配 */
                        if(spos == sublen && tpos == topiclen){
                                *result = true;
                                return 0;
                        }else if(tpos == topiclen && spos == sublen-1 && sub[spos] == '+'){
                                if(spos > 0 && sub[spos-1] != '/'){
                                        return -1;
                                }
                                spos++;
                                *result = true;
                                return 0;
                        }
                }else{
                        if(sub[spos] == '+'){
                                /* Check for bad "+foo" or "a/+foo" subscription */
                                if(spos > 0 && sub[spos-1] != '/'){
                                        return -1;
                                }
                                /* Check for bad "foo+" or "foo+/a" subscription */
                                if(spos < sublen-1 && sub[spos+1] != '/'){
                                        return -1;
                                }
                                spos++;
                                while(tpos < topiclen && topic[tpos] != '/'){
                                        tpos++;
                                }
                                if(tpos == topiclen && spos == sublen){
                                        *result = true;
                                        return 0;
                                }
                        }else if(sub[spos] == '#'){
                                if(spos > 0 && sub[spos-1] != '/'){
                                        return -1;
                                }
                                multilevel_wildcard = true;
                                if(spos+1 != sublen){
                                        return -1;
                                }else{
                                        *result = true;
                                        return 0;
                                }
                        }else{
                                /* Check for e.g. foo/bar matching foo/+/# */
                                if(spos > 0
                                                && spos+2 == sublen
                                                && tpos == topiclen
                                                && sub[spos-1] == '+'
                                                && sub[spos] == '/'
                                                && sub[spos+1] == '#')
                                {
                                        *result = true;
                                        multilevel_wildcard = true;
                                        return 0;
                                }
                                return 0;
                        }
                }
        }
        if(multilevel_wildcard == false && (tpos < topiclen || spos < sublen)){
                *result = false;
        }

        return 0;
}

struct  dictEntry *
hash_find_reg(struct hash *dict, char *key){
	struct dictEntry * res = NULL;
	bool flag = false;
	HASH_FOR_EACH(res , dict){	
		hash_reg_match_mqtt(res->key, strlen(res->key), key, strlen(key), &flag);
		if(flag){
			return res;
		}
	}
	return NULL;
}


/*
	查找对应key 所对应的entry 并返回val
*/
void *
hash_find_data(struct hash * dict, char *key){
	struct dictEntry * entry = hash_find(dict,key);
	if( entry){
		return entry->val;
	}
	return NULL;
}



void
hash_dump_key(struct hash * dict){
	if( dict == NULL){
		return ;
	}
	int count = 0;
	int i = 0;
	struct dictEntry * dc = NULL;
	for( count = 0 , i = 0 ; count < dict->used ; i++ ){
		dc = dict->table[i];
		if( dc != NULL){
			while(dc != NULL){
				count++;
				printf("key:%s\n",dc->key);
				dc = dc->next;
			}
		}

	}

}

struct dictEntry *
hash_next(struct hash * hash, struct dictEntry * entry){
	if(entry == NULL || hash == NULL){
		return NULL;
	}

	if( entry->next){
		return entry->next;
	} else {
		int key = hashGenHashFunction(entry->key,strlen(entry->key))%hash->sizemask + 1;
		if( key >= hash->sizemask){
			return NULL;
		} else {
			for( ; key <= hash->sizemask ; key++){
				if( hash->table[key] != NULL){
					return hash->table[key];
				}
			}
		}
	}
	return NULL;
}

struct dictEntry *
hash_first(struct hash * hash){
	if(hash == NULL || hash->used == 0){
		return 0;
	}

	int i = 0;
	for( i = 0 ; i <= hash->sizemask; i++){
		if(hash->table[i] != NULL){
			return hash->table[i];
		}
	}
	return NULL;
}



