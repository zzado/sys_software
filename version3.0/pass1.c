#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"header.h"



void CANARY(char* input_file)
{
	FILE *source_file = fopen(input_file, "r");
	if(source_file == NULL){
		fprintf(stderr, "INPUT FILE isn't exist\n");
		exit(1);
	}
	FILE *temp_file = fopen("temp", "w");
	if(temp_file == NULL){
		fprintf(stderr, "temp file null\n");
		exit(1);
	}
	char INSTRUCTIONSBuf[100];
	char LABELBuf[100];
	int canary_num = 1;
	int canary_val = 12345;
	char buf[100];
	char OPERANDBuf[100];

	while( fgets(buf, 100, source_file) != NULL ){
		if(buf[0] == '\t'){
			for(int i = 0; i < sizeof(buf)/sizeof(char); i++) {
					buf[i] = buf[i+1];
			}
			LABELBuf[0] = '\0';
			INSTRUCTIONSBuf[4] = '\0';
			if( !strcmp(strncpy(INSTRUCTIONSBuf, buf, 4), "RSUB") || !strcmp(strncpy(INSTRUCTIONSBuf, buf, 4), "EXIT") ) {
				strcpy(INSTRUCTIONSBuf, strtok(buf, "\n"));
				OPERANDBuf[0] = '\0';
			}
		
			else{
				strcpy(INSTRUCTIONSBuf, strtok(buf, "\t"));
				strcpy(OPERANDBuf, strtok(NULL, "\n"));
			}
		}
		else{
			strcpy(LABELBuf, strtok(buf, "\t"));
			strcpy(INSTRUCTIONSBuf, strtok(NULL, "\t"));
			strcpy(OPERANDBuf, strtok(NULL, "\n"));
		}


		if( !strcmp(INSTRUCTIONSBuf, "END") ){
			canary_num--;
			fprintf(temp_file, "CANARY0\tWORD\t12345\n");
			for(int j = 1; j <= canary_num; j++){
				fprintf(temp_file, "CHECK%d\tLDA\tCANARY%d\n",j, j);
				fprintf(temp_file, "\tCOMP\tCANARY0\n");	
				fprintf(temp_file, "\tJEQ\tCHECK%d\n", j+1);
				fprintf(temp_file, "\tEXIT\n");
			}
			fprintf(temp_file, "CHECK%d\tLDA\tCANARY0\n", canary_num+1);
		}
		if(LABELBuf[0] == '\0' && OPERANDBuf[0] == '\0')
			fprintf(temp_file, "\t%s\n", INSTRUCTIONSBuf);
		else if(OPERANDBuf[0] == '\0')
			fprintf(temp_file, "%s\t%s\n", LABELBuf, INSTRUCTIONSBuf);
		else if(LABELBuf[0] == '\0')
			fprintf(temp_file, "\t%s\t%s\n", INSTRUCTIONSBuf, OPERANDBuf);
		else
			fprintf(temp_file, "%s\t%s\t%s\n", LABELBuf, INSTRUCTIONSBuf, OPERANDBuf);		

			if( !strcmp(INSTRUCTIONSBuf, "RESW") || !strcmp(INSTRUCTIONSBuf, "RESB") ){
			fprintf(temp_file, "CANARY%d\tWORD\t%d\n", canary_num, canary_val);
			canary_num++;
			continue;
		}
	}
	fclose(source_file);
	fclose(temp_file);
}


int search_OPTAB(OPTAB* optab, char* INSTRUCTIONSBuf){
	for(int i = 0; i < 61; i++){
		if( !strcmp(optab[i].INSTRUCTIONS, INSTRUCTIONSBuf) )
			return 1;
	}
	return 0;
}

char* get_OBJCODE(OPTAB* optab, char* INSTRUCTIONSBuf){
	for(int i = 0; i < 61; i++){
		if( !strcmp(optab[i].INSTRUCTIONS, INSTRUCTIONSBuf) )
			return optab[i].OPCODE;
	}
}

void get_OPTAB(OPTAB* optab) {
	FILE *optable_file = fopen("OPTAB", "r");
	if( optable_file == NULL) {
		fprintf(stderr, "optab null\n");
		exit(1);
	}
	char buf[100];
	int i = 0;

	while( fgets(buf,100,optable_file) != NULL ) {  //optable파일에서 구조체로 옮기기
		strcpy(optab[i].INSTRUCTIONS, strtok(buf, "\t"));
		strcpy(optab[i].OPCODE, strtok(NULL, "\n"));
		i++;
	}
	fclose(optable_file);
}

void PASS1(INFO *info, SYMTAB *symtab, SOURCE *source, char* input_file) {

	FILE *source_file = fopen(input_file, "r");
	if( source_file == NULL) {
		fprintf(stderr, "INPUT FILE isn't exist!\n");
		exit(1);
	}
	FILE *pass1Result = fopen("PASS1RESULT", "wb");
	if( pass1Result == NULL) {
		fprintf(stderr, "pass1Result null\n");
		exit(1);
	}
	FILE *symtab_file = fopen("SYMTAB", "wb");
	if( symtab_file == NULL) {
		fprintf(stderr, "symtab null\n");
		exit(1);
	}
	FILE *info_file = fopen("INFO", "wb");
	if( info_file == NULL) {
		fprintf(stderr, "info null\n");
		exit(1);
	}
	
	OPTAB optab[61];
	char buf[100];
	int i = 0;
	int h = 0;
	int LOC = 0;
	int canary_num = 1;
	char INSTRUCTIONSBuf[10];
	SOURCE sourceBuf;
	sourceBuf.isINDEX = 0;
	
	get_OPTAB(optab);
	
	while( fgets(buf, 100, source_file) != NULL) {
		if( buf[0] == '.') {		//주석인 경우
			continue;
		}
		else if( buf[0] == '\t') { 		//label이 없는 경우
			for(int m = 0; m < sizeof(buf)/sizeof(char); m++) {
				buf[m] = buf[m+1];
			}
			sourceBuf.LABEL[0] = '\0';
			INSTRUCTIONSBuf[4] = '\0';
			if( !strcmp(strncpy(INSTRUCTIONSBuf, buf, 4), "RSUB") ) {
				strcpy(sourceBuf.INSTRUCTIONS, strtok(buf, "\n"));
				sourceBuf.OPERAND[0] = '\0';
			}
			else if( !strcmp(strncpy(INSTRUCTIONSBuf, buf, 4), "EXIT") ) {
				strcpy(sourceBuf.INSTRUCTIONS, strtok(buf, "\n"));
				sourceBuf.OPERAND[0] = '\0';
			}
			else {
				strcpy(sourceBuf.INSTRUCTIONS, strtok(buf, "\t"));
				strcpy(sourceBuf.OPERAND, strtok(NULL, "\n"));
			}
			
		}
		else {				//label이 있는 경우
			strcpy(sourceBuf.LABEL, strtok(buf, "\t"));
			strcpy(sourceBuf.INSTRUCTIONS, strtok(NULL, "\t"));
			strcpy(sourceBuf.OPERAND, strtok(NULL, "\n"));
		}
		if( !strcmp(sourceBuf.INSTRUCTIONS, "START")) {
			LOC = (int)strtol(sourceBuf.OPERAND, NULL, 16);
			strcpy(info->PROGRAM_NAME, sourceBuf.LABEL);
			strcpy(info->PROGRAM_START_LOC, sourceBuf.OPERAND);
			sourceBuf.LOC = LOC;
			source[i] = sourceBuf;
			symtab[h].LOC = LOC;
			strcpy(symtab[h].LABEL, sourceBuf.LABEL);
			i++;
			h++;
			continue;
		}
		if( !strcmp(sourceBuf.INSTRUCTIONS, "END")) {
			sourceBuf.LOC = source[i-1].LOC;
			source[i] = sourceBuf;
			symtab[h].LOC = sourceBuf.LOC;
			strcpy(symtab[h].LABEL, sourceBuf.LABEL);
			break;
		}
		for(int j =0; j <sizeof(symtab)/sizeof(SYMTAB); j++) {
			if( !strcmp(symtab[j].LABEL, sourceBuf.LABEL) && (sourceBuf.LABEL[0] != '\0') ) {
				fprintf(stderr, "err: duplicated symbol\n");
				exit(1);
			}
		}
		if( search_OPTAB(optab, sourceBuf.INSTRUCTIONS) ) {
			sourceBuf.LOC = LOC;
			source[i] = sourceBuf;
			strcpy(source[i].OBJ_CODE, get_OBJCODE(optab, sourceBuf.INSTRUCTIONS));
			if(strstr(source[i].OPERAND, ",X") != NULL){
				source[i].isINDEX = 1;		//수정했음.
				strcpy(source[i].OPERAND, strtok(source[i].OPERAND, ","));
			}	
			if( sourceBuf.LABEL[0] != '\0' ) {
			symtab[h].LOC = LOC;
				strcpy(symtab[h].LABEL, sourceBuf.LABEL);
				h++;
			}
			LOC = LOC + 3;
			i++;
//			printf("%d  %s  %s  %s\n", sourceBuf.LOC, sourceBuf.LABEL, sourceBuf.INSTRUCTIONS, sourceBuf.OPERAND);
		}
		else if( !strcmp(sourceBuf.INSTRUCTIONS, "WORD") ) {
			sourceBuf.LOC = LOC;
			source[i] = sourceBuf;
			symtab[h].LOC = LOC;
			strcpy(symtab[h].LABEL, sourceBuf.LABEL);
			LOC = LOC + 3;
			i++;
			h++;
//			printf("%d  %s  %s  %s\n", sourceBuf.LOC, sourceBuf.LABEL, sourceBuf.INSTRUCTIONS, sourceBuf.OPERAND);
		}
		else if( !strcmp(sourceBuf.INSTRUCTIONS, "RESW") ) {
			sourceBuf.LOC = LOC;
			source[i] = sourceBuf;
			symtab[h].LOC = LOC;
			strcpy(symtab[h].LABEL, sourceBuf.LABEL);
			LOC = LOC + (3 * atoi(sourceBuf.OPERAND));
			i++;
			h++;
//			printf("%d  %s  %s  %s\n", sourceBuf.LOC, sourceBuf.LABEL, sourceBuf.INSTRUCTIONS, sourceBuf.OPERAND);
		}
		else if( !strcmp(sourceBuf.INSTRUCTIONS, "RESB") ) {
			sourceBuf.LOC = LOC;
			source[i] = sourceBuf;
			symtab[h].LOC = LOC;
			strcpy(symtab[h].LABEL, sourceBuf.LABEL);
			LOC = LOC + atoi(sourceBuf.OPERAND);
			i++;
			h++;
//			printf("%d  %s  %s  %s\n", sourceBuf.LOC, sourceBuf.LABEL, sourceBuf.INSTRUCTIONS, sourceBuf.OPERAND);
		}
		else if( !strcmp(sourceBuf.INSTRUCTIONS, "BYTE") ) {
			sourceBuf.LOC = LOC;
			source[i] = sourceBuf;
			strcpy(symtab[h].LABEL, sourceBuf.LABEL);
			symtab[h].LOC = LOC;
			if( source[i].OPERAND[0] == 'C')
				LOC = LOC + (strlen(source[i].OPERAND) -3);
			else
				LOC = LOC + (strlen(source[i].OPERAND) -3)/2;
			i++;
			h++;
//			printf("%d  %s  %s  %s\n", sourceBuf.LOC, sourceBuf.LABEL, sourceBuf.INSTRUCTIONS, sourceBuf.OPERAND);
		}
		else {
			fprintf(stderr, "err: invalid operation code\n");
			exit(1);
		}
	}
	fclose(source_file);


	info->SOURCE_LENGTH = i+1;	
	info->SYMTAB_LENGTH = h;	
	info->PROGRAM_LENGTH = LOC - (int)strtol(source[0].OPERAND, NULL, 16);
	for(int k = 0; k < sizeof(source)/sizeof(SOURCE); k++) {
		fwrite(&source[k], sizeof(SOURCE), 1, pass1Result);
		fwrite(&symtab[k], sizeof(SYMTAB), 1, symtab_file);
	}

	fwrite(&info, sizeof(INFO), 1, info_file);

	fclose(pass1Result);
	fclose(symtab_file);
	fclose(info_file);
}
