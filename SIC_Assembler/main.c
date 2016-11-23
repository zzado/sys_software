#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "header.h"
#define SIZE 100

void PASS1(INFO *info, SYMTAB *symtab, SOURCE *source, char* input_file);
void make_ObjectProgram(INFO *info, SOURCE *source, char* input_file);
void make_ObjectCode(INFO *info, SYMTAB *symtab, SOURCE *source);
void get_PASS1_RESULTS(INFO *info, SYMTAB *symtab, SOURCE *source);
void help();
void del_TABLES();
int main(int argc, char *argv[]){

	if(!strcmp(argv[1], "-help"))
	{
		help();
		exit(1);
	}
	else if(!strcmp(argv[1], "-obj" )&& argc < 3){
		printf("Please input source file\n");
		exit(1);
	}

   	INFO info;
        SYMTAB symtab[SIZE];
        SOURCE source[SIZE];
	char *input_file = argv[2];
	//PASS1	
	PASS1(&info, symtab, source, input_file);
	
	//PASS2
	//get_PASS1_RESULTS(&info, symtab, source);	 
        make_ObjectCode(&info, symtab, source);        
        make_ObjectProgram(&info, source, input_file);
	if(strcmp(argv[2], "-table")) del_TABLES();
}

void del_TABLES(){
	system("/bin/rm SYMTAB");
	system("/bin/rm INFO");
	system("/bin/rm PASS1RESULT");
}

void help(){
	printf("Help\n");
	printf("-obj [input_file] : Make ObjectProgram of input file\n");
	printf("-obj [input_file] -canary : Set canary bit to buffer and add subroutin for check\n");
	printf("-obj [input_file] -table : Do not delete PASS1's TABLE\n");
	printf("-help : show useage and option\n");
}
	


