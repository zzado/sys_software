#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "header.h"
#define SIZE 100

void get_PASS1_RESULTS(INFO *info, SYMTAB *symtab, SOURCE *source);
void padding_VALUE(char* string, int size);        // 4글자로 패딩 채우기 (주소)
void convert_BYTE(char* string);
void make_ObjectProgram(INFO *info, SOURCE *source, char* output);
void make_ObjectCode(INFO *info, SYMTAB *symtab, SOURCE *source);

void get_PASS1_RESULTS(INFO *info, SYMTAB *symtab, SOURCE *source){
	
	FILE *get_info = fopen("INFO","rb");
	fread(info, sizeof(INFO), 1, get_info);
	fclose(get_info);
	FILE *get_pass1 = fopen("PASS1RESULT", "rb");
	fread(source, sizeof(SOURCE), SIZE, get_pass1);	
	fclose(get_pass1);
	FILE *get_symtab = fopen("SYMTAB", "rb");
	fread(symtab, sizeof(SYMTAB), SIZE, get_symtab);
	fclose(get_symtab);
	
}

void make_ObjectCode(INFO *info, SYMTAB *symtab, SOURCE *source){
	char strBuf[10];
	char *strPtr;		
	int iBuf;	
	for( int i=0; i<info->SOURCE_LENGTH; i++ ){
		for( int j=0; j<info->SYMTAB_LENGTH; j++ ){
			if(!strcmp(source[i].INSTRUCTIONS, "END")){		
				memset(source[i].OBJ_CODE, '\x00', 9);
				break;
			}
			else if(!strcmp(source[i].OPERAND, symtab[j].LABEL)){
				sprintf(strBuf, "%X", symtab[j].LOC);
				padding_VALUE(strBuf, 4);
				strcat(source[i].OBJ_CODE, strBuf);
				if(source[i].isINDEX == 1){
					iBuf = (int)strtol(source[i].OBJ_CODE, NULL, 16);
					sprintf(source[i].OBJ_CODE, "%X", iBuf + 32768);	//32768 == 80 00
				}
				break;
			}
			else if(!strcmp(source[i].INSTRUCTIONS, "RSUB")){
				strcat(source[i].OBJ_CODE, "0000");
				break;
			}
			else if(!strcmp(source[i].INSTRUCTIONS, "WORD")){
				int temp = (int)strtol(source[i].OPERAND, NULL, 10);
				sprintf(source[i].OBJ_CODE,"%X",temp);
				padding_VALUE(source[i].OBJ_CODE,6);
				break;	
			}
			else if(!strcmp(source[i].INSTRUCTIONS, "BYTE")){
				if(source[i].OPERAND[0] == 'C'){
					strcpy(strBuf, source[i].OPERAND);
					strtok(strBuf,"\'");
					strPtr = strtok(NULL,"\'");
					convert_BYTE(strPtr);
					strcpy(source[i].OBJ_CODE, strPtr);
					break;
				}
				else{
					//'X'일 경우
					strcpy(strBuf, source[i].OPERAND);
					strtok(strBuf,"\'");
					strcpy(source[i].OBJ_CODE, strtok(NULL,"\'"));
					break;
				}

			}
		}
	}
}

void make_ObjectProgram(INFO *info, SOURCE *source, char *output){	
	strtok(output,".");
	FILE *result = fopen(output,"w");
	char HeaderRecord[30];
	char strBuf[10];
	padding_VALUE(info->PROGRAM_START_LOC, 6);
	sprintf(strBuf, "%X", info->PROGRAM_LENGTH);
	padding_VALUE(strBuf, 6);
	sprintf(HeaderRecord, "H %-6s %s %s", info->PROGRAM_NAME, info->PROGRAM_START_LOC, strBuf);	
	fprintf(result, "%s\n", HeaderRecord);	
	
	int OBJ_LENGTH = 0;;	
	char Starting_ADDR[20];
	char TextRecord[200] = "";
	char temp[100];
	int line_count = 0;
	for(int i = 0; i<info->SOURCE_LENGTH; i++){
		memset(temp,'\x00',sizeof(temp));	
		line_count = 0;
		OBJ_LENGTH = 0;
		if(strcmp(source[i].OBJ_CODE,"")){
			sprintf(strBuf, "%X", source[i].LOC);
			padding_VALUE(strBuf,6);
			strcpy(Starting_ADDR, strBuf);
			strcpy(temp, source[i].OBJ_CODE);
			OBJ_LENGTH += (strlen(source[i].OBJ_CODE)/2);
			while(line_count < 9){
				i++;
				if(strcmp(source[i].OBJ_CODE,"")){
					sprintf(temp, "%s %s", temp, source[i].OBJ_CODE);
					OBJ_LENGTH += (strlen(source[i].OBJ_CODE)/2);
					line_count ++;
				}
				else{ break; }
			}
			sprintf(strBuf, "%X", OBJ_LENGTH);
			padding_VALUE(strBuf, 2);
			sprintf(TextRecord,"T %s %s %s",Starting_ADDR, strBuf, temp);
			fprintf(result, "%s\n",TextRecord);
		}
	}

	char EndRecord[30];
	sprintf(EndRecord, "E %s", info->PROGRAM_START_LOC);
	fprintf(result,"%s\n", EndRecord);
	fclose(result);

}

void padding_VALUE(char* string, int size){		// 1WORD = 3BYTE
	int length = strlen(string);
	char temp[10];
	memset(temp, '\x00', 9);	//문자열 버퍼 초기화
	for(int i=0; i< size-length; i++){	
		temp[i] = '0';	
	}	
	strcat(temp,string);
	strcpy(string, temp);
}

void convert_BYTE(char* string){
	int length = strlen(string);
	char temp[10];
	char strBuf[10];
	memset(temp, '\x00', 9);
	memset(strBuf, '\x00', 9);	
	for(int i=0; i<length; i++){
		sprintf(temp, "%X", string[i]);
		strcat(strBuf, temp);	
	}
	strcpy(string, strBuf);
}
