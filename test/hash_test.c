#include "hash.h"


void func(void *a){
	printf("func call:%s\n",(char*)a);
}
booltest_hash_destroy()
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

	test_hash_destroy();

}
