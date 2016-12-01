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
void CANARY(char* inputfile);
int main(int argc, char *argv[]){

	if(argc == 1 ){
		help();
		exit(1);
	}
	else if(argc ==2){
		if(!strcmp(argv[1], "-help")){
			help();
			exit(1);
		}
		else if(!strcmp(argv[1], "-obj")){
			printf("Please input source file\n");
			exit(1);
		}
	}
	else if(argc ==3){
		if(!strcmp(argv[1], "-obj")){
			char *input_file = argv[2];
   			INFO info;
        		SYMTAB symtab[SIZE];
        		SOURCE source[SIZE];
			PASS1(&info, symtab, source, input_file);
       			make_ObjectCode(&info, symtab, source);        
        		make_ObjectProgram(&info, source, input_file);
			del_TABLES();
		}
	}
	else if(argc ==4){
		if(!strcmp(argv[1], "-obj") && !strcmp(argv[3],"-canary")){
			char *input_file = argv[2];
   			INFO info;
        		SYMTAB symtab[SIZE];
        		SOURCE source[SIZE];
			CANARY(input_file);		
			PASS1(&info, symtab, source, "temp");
       			make_ObjectCode(&info, symtab, source);        
        		make_ObjectProgram(&info, source, input_file);
			system("/bin/rm temp");
			del_TABLES();	
		}
		else if(!strcmp(argv[1], "-obj") && !strcmp(argv[3], "-table")){
			char *input_file = argv[2];
   			INFO info;
        		SYMTAB symtab[SIZE];
        		SOURCE source[SIZE];
			CANARY(input_file);		
			PASS1(&info, symtab, source, input_file);
       			make_ObjectCode(&info, symtab, source);        
        		make_ObjectProgram(&info, source, input_file);
		}
	}
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
	


