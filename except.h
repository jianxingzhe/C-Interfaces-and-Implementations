#ifndef EXCEPT_INCLUDE
#define EXCEPT_INCLUDE
#include <setjmp.h>

#define T Except_T
typedef struct  T
{
	const char *reason;
}T;

typedef struct Except_Frame Except_Frame;
struct Except_Frame
{
	Except_Frame *prev;
	jmp_buf env;
	const char *file;
	int line;
	const T *exception;
};

enum 
{
	Except_entered=0,Except_raised,
	Except_handled,Except_finalized
};

extern Except_Frame *Except_stack;

void Except_raise(const T *e, const char *file, int line);

#define RAISE(e)  Except_raise(&(e),__FILE__,__LINE__)
#define RERAISE	  Except_raise(Except_Frame.exception, Except_Frame.file, Except_Frame.line)
#define RETURN    switch (Except_stack = &Execpt_stack->prev,0) default:return;

#define TRY		  do {\
					volatile int Except_flag;\
					Except_Frame Except_Frame;\
					Except_Frame.prev = Except_stack;\
					Except_stack = &Except_Frame;\
					Except_flag = setjmp(Except_Frame.env);\
					if(Except_flag == Except_entered){

#define EXCEPT(e) if(Except_flag == Except_entered)\
					Except_stack = &Except_stack->prev; \
				  }else if(Except_Frame.exception == &(e)){\
					Except_flag = Except_handled;

#define ELSE	  if(Except_flag == Except_entered)\
					Except_stack = Except_stack->prev;\
				  }else{\
					Except_flag = Except_handled;

#define FINALLY	  if(Except_flag == Except_entered)\
					Except_stack = Except_stack->prev;\
				   } { \
				  if(Except_flag == Except_entered)\
					Except_flag = Except_finalized;

#define END_TRY		if(Except_flag == Except_entered)\
						Except_stack = Except_stack->prev;\
					} if(Except_flag == Except_raised) RERAISE;\
				  }while (0)

#undef T
#endif