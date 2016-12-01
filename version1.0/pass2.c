#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define SIZE 100
struct line{
	long loc;
	char label[10];
	char opcode[10];
	char operand[10];	
	int is_indexing;	//0이면 flse 1이면 true
	char object_code[10];
}typedef line;

struct symtab{
	long loc;
	char label[10];
}typedef symtab;	

struct tab_info{
	int source_length;
	int symtab_length;
	long program_size;
	char program_name[10];
	char program_start_loc[10];
}typedef tab_info;

struct optab{
	char opcode[10];
	char hex[10];
}typedef optab;

void padding_ADDR(char* string);	// 4글자로 패딩 채우기 (주소)
void padding_WORD(char* string);	// 6글자로 패딩 채우기 (1WORD)
void convert_BYTE(char* string);
int main(int argc, char* argv[]){

	FILE *get_optab = fopen("optable", "r");
	optab optab_data[60];	
	char buf[SIZE];
	int optab_length = 0;
	while( fgets(buf, SIZE, get_optab) != NULL){
		strcpy(optab_data[optab_length].opcode, strtok(buf, "\t"));
		strcpy(optab_data[optab_length].hex, strtok(NULL, "\n"));
		optab_length++;
	}
	
	fclose(get_optab);
	
	FILE *get_info = fopen("tab_info","rb");
	tab_info info_data;
	fread(&info_data, sizeof(tab_info), 1, get_info);
	fclose(get_info);
	
	FILE *get_pass1 = fopen("pass1Result", "rb");
	line source_data[SIZE];	
	fread(source_data, sizeof(line), SIZE, get_pass1);	
	fclose(get_pass1);
	
	FILE *get_symtab = fopen("symtable", "rb");	
	symtab symtab_data[60];
	fread(symtab_data, sizeof(symtab), 60, get_symtab);
	fclose(get_symtab);
	// PASS1 output 얻기		
	// PASS2
	char str_buf[10];
	char *str_ptr;		
	int int_buf;	
	for( int i=0; i<info_data.source_length; i++ ){
		for( int j=0; j<optab_length; j++ ){
 			if(!strcmp(source_data[i].opcode, optab_data[j].opcode)){
				strcpy(source_data[i].object_code , optab_data[j].hex);
				// object code에 opcode 넣기
				break;
			}
		}
		for( int j=0; j<info_data.symtab_length; j++ ){
			if(!strcmp(source_data[i].opcode, "END")){			
				memset(source_data[i].object_code, '\x00', 9);
				break;
			}
			else if(!strcmp(source_data[i].operand, symtab_data[j].label)){
				sprintf(str_buf, "%lX", symtab_data[j].loc);
				padding_ADDR(str_buf);
				strcat(source_data[i].object_code, str_buf);
				if(source_data[i].is_indexing == 1){
					int_buf = (int)strtol(source_data[i].object_code, NULL, 16);
					sprintf(source_data[i].object_code, "%X", int_buf + 32768);	//32768 == 80 00
				}
				break;
			}
			else if(!strcmp(source_data[i].opcode, "RSUB")){
				strcat(source_data[i].object_code, "0000");
				break;
			}
			else if(!strcmp(source_data[i].opcode, "WORD")){
				long temp = strtol(source_data[i].operand, NULL, 10);
				sprintf(source_data[i].object_code,"%lX",temp);
				padding_WORD(source_data[i].object_code);
				break;	
			}
			else if(!strcmp(source_data[i].opcode, "BYTE")){
				if(source_data[i].operand[0] == 'C'){
					strcpy(str_buf, source_data[i].operand);
					strtok(str_buf,"\'");
					str_ptr = strtok(NULL,"\'");
					convert_BYTE(str_ptr);
					strcpy(source_data[i].object_code, str_ptr);
					break;
				}
				else{
					//'X'일 경우
					strcpy(str_buf, source_data[i].operand);
					strtok(str_buf,"\'");
					strcpy(source_data[i].object_code, strtok(NULL,"\'"));
					break;
				}

			}
		}
	}

	printf("Program SIZE: %lX NAME: %s START LOC: %s\n", info_data.program_size, info_data.program_name, info_data.program_start_loc);	
        printf("loc \t label \t opcode \t operand \t object_code\n");
        for(int i=0; i<info_data.source_length; i++){
                printf("%lX %s %s %s %s\n", source_data[i].loc, source_data[i].label, source_data[i].opcode, source_data[i].operand, source_data[i].object_code);
        }

	// object program 출력
	
	FILE *result = fopen("result","w");
	char head[30];
	padding_WORD(info_data.program_start_loc);
	sprintf(str_buf, "%lX", info_data.program_size);
	padding_WORD(str_buf);
	sprintf(head, "H %-6s %s %s",info_data.program_name, info_data.program_start_loc, str_buf);
	printf("%s\n", head);
	
	int i;
	int j=1;	
	int count, object_length;	
	char object_code[80];
	char starting_addr[20];
	char text[100];
	while(j < info_data.source_length-1){	
		if(strcmp(source_data[j].object_code, "")){
			sprintf(starting_addr, "%lX", source_data[j].loc);	
			padding_WORD(starting_addr);

			// starting_addr 만들기

			i = j;
			count = 0;
			object_length = 0;
			while(strcmp(source_data[i].object_code, "")){
				if(count == 6) break;
				strcat(object_code, source_data[i].object_code);	
				object_length = object_length + (strlen(source_data[i].object_code)/2);
				printf("%s ",source_data[i].object_code);
				i++;
				count++;			
			}
			j = i;
			printf("\n");
		}
		else{ j++; }
	}

	char end[30];
	sprintf(end, "E %s", info_data.program_start_loc);
	printf("%s\n", end);




}

void padding_ADDR(char* string){		// 1WORD = 3BYTE
	int length = strlen(string);
	char temp[10];
	memset(temp, '\x00', 9);	//문자열 버퍼 초기화
	for(int i=0; i< 4-length; i++){	
		temp[i] = '0';	
	}	
	strcat(temp,string);
	strcpy(string, temp);
}

void padding_WORD(char* string){		// 1WORD = 3BYTE
	int length = strlen(string);
	char temp[10];
	memset(temp, '\x00', 9);	//문자열 버퍼 초기화
	for(int i=0; i< 6-length; i++){	
		temp[i] = '0';	
	}	
	strcat(temp,string);
	strcpy(string, temp);
}

void convert_BYTE(char* string){
	int length = strlen(string);
	char temp[10];
	char str_buf[10];
	memset(temp, '\x00', 9);
	memset(str_buf, '\x00', 9);	
	for(int i=0; i<length; i++){
		sprintf(temp, "%X", string[i]);
		strcat(str_buf, temp);	
	}
	strcpy(string, str_buf);
}
