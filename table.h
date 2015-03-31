#ifndef TABLE_INCLUDE
#define TABLE_INCLUDE
#define T Table_T
typedef struct T *T;

/*struct T {
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
};*/

extern T      Table_new      (int hint, int cmp(const void *x, const void  *y), 
									unsigned hash(const void *key));
extern void   Table_free     (T *table);
extern int    Table_length   (T table);
extern void  *Table_put      (T table, const void *key, void *value);
extern void  *Table_get      (T table, const void *key);
extern void  *Table_remove   (T table, const void *key);
extern void   Table_map      (T table, void apply(const void *key,void **value,void *cl),void *cl);
extern void **Table_toArray  (T table, void *end);

#undef T
#endif
