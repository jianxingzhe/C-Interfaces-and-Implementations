#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include "table.h"
#include "mem.h"
#include "atom.h"

int  getword(FILE *fp, char *buf, int size, int first(int c),int rest(int c));
void wf(char *name, FILE *fp);
int  compare(const void *, const void *);
void vfree  (const void*, void **,void *);
int first(int );
int rest (int);



int main(int argc, char *argv[]){
	int i;
	for(i=1;i<argc;i++){
		FILE *fp = fopen(argv[i],"r");
		if(fp == NULL){
			fprintf(stderr,"%s: can't open '%s'(%s)\n",argv[0],argv[i],strerror(errno));
			return EXIT_FAILURE;
		}else{
			wf(argv[i],fp);
			fclose(fp);
		}
	}
	if(argc == 1) wf(NULL,stdin);
	return EXIT_SUCCESS;
}

int  getword(FILE *fp, char *buf, int size, int first(int c),int rest(int c)){
	int i=0,c;
	assert(fp && buf && size > 1 && first && rest);
	c = getc(fp);
	for(;c!=EOF;c=getc(fp))
		if(first(c)){
			if(i<size-1)
				buf[i++] = c;
			c = getc(fp);
			break;
		}
	for(;c!=EOF && rest(c);c=getc(fp))
		if(i<size-1)
			buf[i++] = c;
	if(i<size)
		buf[i] = '\0';
	else
		buf[size-1] = '\0';
	if(c != EOF)
		ungetc(c,fp);

	return i>0;
}


int compare(const void *x ,const void *y){
	return strcmp(*(char**)x,*(char**)y);
}
void vfree(const void *key, void **count,void *cl){
	FREE(*count);
}

void wf(char *name, FILE *fp){
	Table_T table = Table_new(0,NULL,NULL);
	char buf[128];

	while(getword(fp,buf,sizeof buf, first,rest)){
		const char *word;
		int i,*count;
		for(i=0;buf[i]!='\0';i++)
			buf[i] = tolower(buf[i]);
		word = Atom_string(buf);
		count = Table_get(table,word);
		if(count)
			(*count)++;
		else{
			NEW(count);
			*count=1;
			Table_put(table,word,count);
		}
	}
	if(name)
		printf("%s:\n",name);
	int i;
	void **array = Table_toArray(table,NULL);
	qsort(array,Table_length(table),2*sizeof (*array),compare);

	for(i=0;array[i];i+=2)
		printf("%d\t%s\n",*(int*)array[i+1],(char*)array[i]);
	FREE(array);

	Table_map(table,vfree,NULL);
	Table_free(&table);
}

int first(int c){
	return isalpha(c);
}

int rest(int c){
	return isalpha(c) || c == '_';
}
