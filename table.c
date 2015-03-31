#include <limits.h>
#include <stddef.h>
#include <assert.h>
#include "mem.h"
#include "table.h"

#define T Table_T

struct T {
	int size;
	int length;
	unsigned timestamp;
	int (*cmp)(const void *x,const void *y);
	unsigned (*hash)(const void *key);
	struct binding {
		struct binding	*link;
	    const void *key;
		void *value;
	} **buckets;
};

static int cmpatom(const void *x, const void *y){
	return x != y;
}
static unsigned hashatom(const void *key){
	return (unsigned long)key>>2;
}

T Table_new(int hint, int cmp(const void *x,const void *y),unsigned hash(const void *key)){
	T table;
	int i;
	//table接口使用素数作为其哈希表的大小，因为它无法控制键的哈希吗如何计算。
	//primes中的值时最接近2^n的素数
	static int primes[] = {509,509,1021,2053,4093,8191,16381,32771,65521,INT_MAX};
	assert(hint>=0);

	for(i=1;primes[i]<hint;i++);//找到第一个大于等于hint的索引值

	table = ALLOC(sizeof (*table) + primes[i-1]*sizeof (table->buckets[0]));
	table->size = primes[i-1];
	//如果没有指定比较和哈希函数，使用默认的函数
	table->cmp  = cmp ? cmp : cmpatom;
	table->hash = hash? hash: hashatom;
	table->buckets = (struct binding **)(table + 1);

	for(i=0;i<table->size;i++)
		table->buckets[i] = NULL;
	table->length = 0;
	table->timestamp=0;
	return table;
}

void *Table_get(T table, const void *key){
	int i;
	struct binding *p;
	assert(table);
	assert(key);

	i=(*table->hash)(key) % table->size;//计算哈希值
	for(p=table->buckets[i];p;p=p->link)//在链表中查找
		if((*table->cmp)(key,p->key) == 0)
			break;

	return p ? p->value : NULL;
}

void *Table_put(T table, const void *key, void *value){
	int i;
	struct binding *p;
	void *prev;

	assert(table);
	assert(key);
	
	i=(*table->hash)(key)%table->size;
	for(p=table->buckets[i];p;p=p->link)//在链表中查找
		if((*table->cmp)(key,p->key) == 0)
			break;

	if(p == NULL){//如果在链表中没有找到，则新建一个binding 结构体，并将它插在链表头部
		NEW(p);
		p->key = key;
		p->link = table->buckets[i];
		table->buckets[i] = p;
		table->length++;
		prev = NULL;
	}else 
		prev = p->value;

	p->value = value;
	table->timestamp++;
	return prev;
}

int Table_length(T table){//length表示binding 实例的数目
	assert(table);
	return table->length;
}

void Table_map(T table, void apply(const void *key, void **value, void *cl), void *cl){
	int i;
	unsigned stamp;
	struct binding *p;

	assert(table);
	assert(apply);
/*
 *timestamp用来实现Table_map必须强制实施的一项已检查的运行时错误：
 *在Table_map访问表中各个binding实例时，表不能改变。Table_map在进入时保存了timestamp的值。
 *每次调用apply之后，它通过断言来检查表的timestamp是否等于该保存值。
*/
	stamp = table->timestamp;
	for(i=0;i<table->size;i++)
		for(p=table->buckets[i];p;p=p->link){
			apply(p->key,&p->value,cl);
			assert(table->timestamp == stamp);
		}
}

void *Table_remove(T table,const void *key){
	int i;
	struct binding **pp;
	assert(table);
	assert(key);
	table->timestamp++;

	i = (*table->hash)(key) % table->size;
	for(pp=&table->buckets[i]; *pp ; pp = &(*pp)->link)
		if((*table->cmp)(key,(*pp)->key) == 0){
			struct binding *p = *pp;
			void *value = p->value;
			*pp = p->link;
			FREE(p);
			table->length--;
			return value;
		}
	return NULL;
}

void **Table_toArray(T table, void *end){
	int i,j=0;
	void **array;
	struct binding *p;
	assert(table);
	array = ALLOC((2*table->length + 1)*sizeof (*array));
	for(i=0;i<table->size;i++)
		for(p=table->buckets[i]; p; p=p->link){
			array[j++] = (void*)p->key;
			array[j++] = p->value;
		}
	array[j] = end;
	return array;
}

void Table_free(T *table){
	assert(table && *table);
	if((*table)->length  > 0){
		int i;
		struct binding *p,*q;
		for(i=0;i<(*table)->size;i++)
			for(p=(*table)->buckets[i];p;p=q){
				q = p->link;
				FREE(p);
			}
	}
	FREE(*table);
}
