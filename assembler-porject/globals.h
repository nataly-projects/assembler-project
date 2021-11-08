/* Constants */
#ifndef _GLOBALS_H
#define _GLOBALS_H

/** Boolean (T/F) definition */
typedef enum booleans {
	FALSE = 0, 
  TRUE = 1
} bool;

/** Maximum size of code image and data image */
#define CODE_ARR_IMG_LENGTH 1200

/** Maximum length of a single source line  */
#define MAX_LINE_LENGTH 80

/** Maximum length of label */
#define MAX_LABEL_LENGTH 31

/** Maximum files to process */
#define MAX_FILES_TO_PROCESS 3

/** Initial IC and DC value */
#define IC_INIT_VALUE 100
#define DC_INIT_VALUE 0

#define NONE_REG -3

#define BYTE 8


/* Operand types */
typedef enum operand_type {
	IMMEDIATE_TYPE = 0, 
	REGISTER_TYPE = 1, 
  LABEL_TYPE = 2,
	/* Failed/Not detected type */
	NONE_TYPE = -1
} operand_type;

/* Commands opcode */
typedef enum opcodes {
/* R commands */
  ADD_OP = 0,
	SUB_OP = 0,
	AND_OP = 0,
	OR_OP = 0,
  NOR_OP = 0,

	MOVE_OP = 1,
	MVHI_OP = 1,
	MVLO_OP = 1,

/* I commands */
	ADDI_OP = 10,
	SUBI_OP = 11,
	ANDI_OP = 12,
	ORI_OP = 13,
	NORI_OP = 14,

	BNE_OP = 15,
	BEQ_OP = 16,
	BLT_OP = 17,
  BGT_OP = 18,

  LB_OP = 19,
  SB_OP = 20,
  LW_OP = 21,
  SW_OP = 22,
  LH_OP = 23,
  SH_OP = 24,

/* J commands */
  JMP_OP = 30,
  LA_OP = 31,
  CALL_OP = 32,
	STOP_OP = 63,

	/** Failed/Error */
	NONE_OP = -1
} opcode;

/* Commands funct */
typedef enum funct {
	/* OPCODE 0 */
	ADD_FUNCT = 1,
	SUB_FUNCT = 2,
  AND_FUNCT = 3,
  OR_FUNCT = 4,
  NOR_FUNCT = 5,

	/* OPCODE 1 */
	MOVE_FUNCT = 1,
	MVHI_FUNCT = 2,
	MVLO_FUNCT = 3,

	/** Default (No need/Error) */
	NONE_FUNCT = 0
} funct;


/* Instruction type */
typedef enum instruction {
  DH_INST,
  DW_INST,
  DB_INST,
	EXTERN_INST,
	ENTRY_INST,
	ASCIZ_INST,

	/* Not found */
	NONE_INST,

	/* Parsing/syntax error */
	ERROR_INST
} instruction;

/* Structure of an R command */
typedef struct r_command {
	unsigned int NONE: 6;
	unsigned int funct: 5;
	unsigned int rd: 5;
	unsigned int rt: 5;
	unsigned int rs: 5;
} r_command;

/* Structure of an I command */
typedef struct i_command {
	unsigned int immed: 16;
	unsigned int rt: 5;
	unsigned int rs: 5;
} i_command;

/* Structure of an J command */
typedef struct j_command {
	unsigned int address: 25;
	unsigned int reg: 1;
} j_command;

/* Represents a single code word */
typedef struct code_word {
	unsigned int opcode: 6;
	char command;
  /* The command can be r, j or i */
  union commad_type {
		r_command* r;
		j_command* j;
    i_command* i;
	} commad_type;
} code_word;


/* Represents a single data word. */
typedef struct data_word {
	instruction ins;
	/* The data content itself */
	int data;
} data_word;

/* Represents a general machine code word contents */
typedef struct machine_word {
	/* if it represents code (not additional data), this field contains the total length required by the code. if it's data, this field is 0. */
	short length;
	/* The content can be code or data */
	union word {
		data_word* data;
		code_word* code;
	} word;
} machine_word;

/* Represents a single source line, including it's details */
typedef struct line_info {	
	long line_number; /* Line number in file */
	char* file_name;
	char* content; /* Line content (source) */
} line_info;


#endif