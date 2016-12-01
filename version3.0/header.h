struct SOURCE{
	int LOC;
	char LABEL[10];
	char INSTRUCTIONS[10];
	char OPERAND[10];
	int isINDEX;
	char OBJ_CODE[10];
}typedef SOURCE;

struct SYMTAB{
	int LOC;
	char LABEL[10];
}typedef SYMTAB;

struct OPTAB{
	char INSTRUCTIONS[10];
	char OPCODE[10];
}typedef OPTAB;

struct INFO{
	int SOURCE_LENGTH;
	int SYMTAB_LENGTH;
	int PROGRAM_LENGTH;
	char PROGRAM_NAME[10];
	char PROGRAM_START_LOC[10];
}typedef INFO;


	// SOURCE source[100];
	// OPTAB optab[60];
	// SYMTAB symtab[100];
	

	// SYMTAB
	// OPTAB
	// INFO
	// PASS1_RESULT
