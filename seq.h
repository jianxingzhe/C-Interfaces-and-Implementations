#ifndef SEQ_INCLUDE
#define SEQ_INCLUDE

#define T Seq_T
typedef struct T *T;

extern T 	 Seq_new   (int hint);
extern T	 Seq_seq   (void *x, ...);
extern void  Seq_free  (T *seq);
extern int   Seq_length(T seq);
extern void *Seq_get   (T seq, int i);
extern void *Seq_put   (T seq, int i, void *x);
extern void *Seq_addlo (T seq, void *x);/*将x加到序列低端*/
extern void *Seq_addhi (T seq, void *x); /*将x加到序列高端*/
extern void *Seq_remlo (T seq);
extern void *Seq_remhi (T seq);

#undef T
#endif
