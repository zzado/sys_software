#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct line {
	long loc;
	char label[10];
	char opcode[10];
	char operand[10];
	int is_indexing;
	char object_code[10];
}typedef line;

struct symtab {
	long loc;
	char label[10];
}typedef symtab;

struct optab {
	char opcode[10];
	char hex[10];
}typedef optab;

struct tab_info {
	int line_length;
	int symtab_length;
	long program_size;
	char program_name[10];
	char program_start_loc[10];
}typedef tab_info;

int searchopt(optab* opTable, char* opcodeBuf){
	for(int i = 0; i < 60; i++){
		if( !strcmp(opTable[i].opcode, opcodeBuf) )
			return 1;
	}
	return 0;
}



int main(int argc, char* argv[]) {
	FILE *source;
	source = fopen("test.sic", "r");
	if( source == NULL) {
		fprintf(stderr, "source null\n");
		exit(1);
	}
	FILE *allOpcode;
	allOpcode = fopen("optable", "r");
	if( allOpcode == NULL) {
		fprintf(stderr, "optab null\n");
		exit(1);
	}
	FILE *pass1Result;
	pass1Result = fopen("pass1Result", "wb");
	if( pass1Result == NULL) {
		fprintf(stderr, "pass1Result null\n");
		exit(1);
	}
	FILE *symtable;
	symtable = fopen("symtable", "wb");
	if( symtable == NULL) {
		fprintf(stderr, "symtab null\n");
		exit(1);
	}
	FILE *info = fopen("tab_info", "wb");
	if( info == NULL) {
		fprintf(stderr, "tab_info null\n");
		exit(1);
	}
	
	tab_info table_info;
	optab opTable[60];
	symtab symTable[100];
	char buf[100];
	line a[100];
	int i = 0;
	int h = 0;
	long loc = 0;
	char opcodebuf[10];
	line lineBuf;
	lineBuf.is_indexing = 0;
	
	while( fgets(buf,100,allOpcode) != NULL ) {  //optable파일에서 구조체로 옮기기
		strcpy(opTable[i].opcode, strtok(buf, "\t"));
		strcpy(opTable[i].hex, strtok(NULL, "\n"));
		i++;
	}
	fclose(allOpcode);

	i = 0;
	while( fgets(buf, 100, source) != NULL) {
		if( buf[0] == '\t') { 		//label이 없는 경우
			for(int m = 0; m < sizeof(buf)/sizeof(char); m++) {
				buf[m] = buf[m+1];
			}
			lineBuf.label[0] = '\t';
			lineBuf.label[1] = '\0';
			opcodebuf[4] = '\0';
			if( !strcmp(strncpy(opcodebuf, buf, 4), "RSUB") ) {
				strcpy(lineBuf.opcode, strtok(buf, "\t"));
				lineBuf.operand[0] = '\t';
				lineBuf.operand[1] = '\0';
			}
			else {
				strcpy(lineBuf.opcode, strtok(buf, "\t"));
				strcpy(lineBuf.operand, strtok(NULL, "\n"));
			}
		}
		else {				//label이 있는 경우
			strcpy(lineBuf.label, strtok(buf, "\t"));
			strcpy(lineBuf.opcode, strtok(NULL, "\t"));
			strcpy(lineBuf.operand, strtok(NULL, "\n"));
		}
		if( !strcmp(lineBuf.opcode, "START")) {
			loc = strtol(lineBuf.operand, NULL, 16);
			strcpy(table_info.program_name, lineBuf.label);
			strcpy(table_info.program_start_loc, lineBuf.operand);
			lineBuf.loc = loc;
			a[i] = lineBuf;
			symTable[h].loc = loc;
			strcpy(symTable[h].label, lineBuf.label);

//			printf("%lX %s %s %s\n",a[i].loc, a[i].label, a[i].opcode, a[i].operand);
			i++;
			h++;
			continue;
		}
		if( !strcmp(lineBuf.opcode, "END")) {
			lineBuf.loc = a[i-1].loc;
			a[i] = lineBuf;
			symTable[h].loc = lineBuf.loc;
			strcpy(symTable[h].label, lineBuf.label);

//			printf("%lX %s %s %s\n", a[i].loc, a[i].label, a[i].opcode, a[i].operand);
			break;
		}
		for(int j =0; j <sizeof(symTable)/sizeof(symtab); j++) {
			if( !strcmp(symTable[j].label, lineBuf.label) ) {
				fprintf(stderr, "err: duplicated symbol\n");
				exit(1);
			}
		}
		if( searchopt(opTable, lineBuf.opcode) ) {
			lineBuf.loc = loc;
			a[i] = lineBuf;
			//if( a[i].operand[strlen(a[i].operand) -1] == ',' && a[i].operand[strlen(a[i].operand)] == 'X'){
			if(strstr(a[i].operand, ",X") != NULL){
				a[i].is_indexing = 1;		//수정했음.
				strcpy(a[i].operand, strtok(a[i].operand, ","));
			}	
//			printf("%lX %s %s %s\n",a[i].loc, a[i].label, a[i].opcode, a[i].operand);
			if( lineBuf.label[0] != '\t' ) {
			symTable[h].loc = loc;
				strcpy(symTable[h].label, lineBuf.label);
				h++;
			}
			loc = loc + 3L;
			i++;
		}
		else if( !strcmp(lineBuf.opcode, "WORD") ) {
			lineBuf.loc = loc;
			a[i] = lineBuf;
//			printf("%lX %s %s %s\n",a[i].loc, a[i].label, a[i].opcode, a[i].operand);
			symTable[h].loc = loc;
			strcpy(symTable[h].label, lineBuf.label);
			loc = loc + 3L;
			i++;
			h++;
		}
		else if( !strcmp(lineBuf.opcode, "RESW") ) {
			lineBuf.loc = loc;
			a[i] = lineBuf;
//			printf("%lX %s %s %s\n",a[i].loc, a[i].label, a[i].opcode, a[i].operand);
			symTable[h].loc = loc;
			strcpy(symTable[h].label, lineBuf.label);
			loc = loc + (3L * atol(lineBuf.operand));
			i++;
			h++;
		}
		else if( !strcmp(lineBuf.opcode, "RESB") ) {
			lineBuf.loc = loc;
			a[i] = lineBuf;
//			printf("%lX %s %s %s\n",a[i].loc, a[i].label, a[i].opcode, a[i].operand);
			symTable[h].loc = loc;
			strcpy(symTable[h].label, lineBuf.label);
			loc = loc + atol(lineBuf.operand);
			i++;
			h++;
		}
		else if( !strcmp(lineBuf.opcode, "BYTE") ) {
			lineBuf.loc = loc;
			a[i] = lineBuf;
//			printf("%lX %s %s %s\n",a[i].loc, a[i].label, a[i].opcode, a[i].operand);
			symTable[h].loc = loc;
			strcpy(symTable[h].label, lineBuf.label);
			if( a[i].operand[0] == 'C')
				loc = loc + (strlen(a[i].operand) -3L);
			else
				loc = loc + (strlen(a[i].operand) -3L)/2L;
			i++;
			h++;
		}
		else {
			fprintf(stderr, "err: invalid operation code\n");
			exit(1);
		}
	}
	fclose(source);
	table_info.line_length = i+1;	
	table_info.symtab_length = h;	
	table_info.program_size = loc - strtol(a[0].operand, NULL, 16);

	for(int k = 0; k < sizeof(a)/sizeof(line); k++) {
		fwrite(&a[k], sizeof(line), 1, pass1Result);
		fwrite(&symTable[k], sizeof(symtab), 1, symtable);
	}

	fwrite(&table_info, sizeof(tab_info), 1, info);

	fclose(pass1Result);
	fclose(symtable);
	fclose(info);
}
