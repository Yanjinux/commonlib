

#ifndef _HASH_H
#define _HASH_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

/*
 * 操作返回状态
 */
#define DICT_OK 0
#define DICT_ERR 1
#define DEFAULT_HASH_NODE 20
#define FREE_ENTRY(entry) do { \
	free(entry->key);\
	free(entry);\
	}while(0) 

#define FREE_HASH(hs) do{ \
	free(hs->table);
	hs->table = NULL；
}while(0)

static uint32_t dict_hash_function_seed = 5381;


/* Unused arguments generate annoying warnings... */
#define DICT_NOTUSED(V) ((void) V)

#define HASH_FOR_EACH(dict,hash) \
	for((dict = hash_first(hash)) ; dict != NULL ; dict = hash_next(hash,dict))

/*
	释放hash表的内存，调用后hash不能使用  
	传入函数func 为释放val的函数 ， 可以不设置
*/
void
hash_destroy(struct hash *hs ,freeValCB func);

/*
 * 哈希表节点
 */
typedef struct dictEntry {

    // 键
    void *key;
	void *val;

    // 链往后继节点
    struct dictEntry *next;

} dictEntry;

/*
 * 哈希表
 */
typedef struct hash {

    // 哈希表节点指针数组（俗称桶，bucket）
    dictEntry **table;

    // 指针数组的大小
    unsigned long size;

    // 指针数组的长度掩码，用于计算索引值
    unsigned long sizemask;

    // 哈希表现有的节点数量
    unsigned long used;     //没有使用

} hash;

/*
	计算hash key
*/
unsigned int hashGenHashFunction(const void *key, int len);

/*
	返回下一个dictentry
*/

struct dictEntry *
hash_next(struct hash * hash, struct dictEntry * entry);

/*
	获得第一个hash
*/
struct dictEntry *
hash_first(struct hash * hash);


struct hash *
hash_init(struct hash * dict , int hash_node);

/*
   复制长度为len的字符串val，并返回指针，内存由调用者释放
*/
void * dup_string(char * val , int len);

/*
	添加hash
*/
int
hash_add(struct hash *dict , char * key , void * val);

/*
	添加key val 保证所对应的key 唯一
	如果添加失败返回 -1
*/
int
hash_add_first(struct hash * dict, char * key, void * val);


/*
  删除指定key的hash ,返回val
*/
void *
hash_delete(struct hash *dict , char * key);

/*
	删除并释放val
*/
void
hash_delete_and_free(struct hash * dict , char * key);
/*
	查找对应的hash并返回entry
	找不到就返回NULL
*/
struct dictEntry*
hash_find(struct hash * dict , char * key);

/*
	查找对应key 所对应的entry 并返回val
*/
void *
hash_find_data(struct hash * dict, char *key);

/*
	打印所有键值对
*/
void
hash_dump(struct hash * dict);

#endif /* __DICT_H */


