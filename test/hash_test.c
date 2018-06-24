#include "hash.h"


void func(void *a){
	printf("func call:%s\n",(char*)a);
}

int 
test_hash_destroy()
{
	struct  hash hs;
	hash_init(&hs,0);

	hash_add(&hs, "hello", "y");
	hash_add(&hs, "hello", "b");
	hash_add(&hs, "world", "z");

	struct dictEntry * dict;
	HASH_FOR_EACH(dict, &hs){
		printf("key:%s val:%sn", dict->key,dict->val);
	}

	hash_destroy(&hs,  func);

}



int main()
{
    struct hash h;
    hash_init(&h,0);
	hash_add(&h, "/a/+/b", NULL);
	dictEntry * dc = hash_find_reg(&h, "/a/b/c");
	if(dc){
		printf("/a/b/c in %s\n", dc->key);
	}
	dc = hash_find_reg(&h, "/c/b/c");
	if(!dc){
		printf("c/b/c not\n");
	}
	test_hash_destroy();

}
