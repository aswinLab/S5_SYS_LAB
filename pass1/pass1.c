#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 100 //Max length of a line to be written in intermediate file
#define MAX_LABEL_LEN 30 //Max length of a label 
#define MAX_OPCODE_LEN 10 // Max length of an opcode mnunoics
#define MAX_OPTAB_ENTRIES 100 // max number of entries allowed in optab
#define MAX_SYMBOLS 100 //Max number of entries allowed in symtab

typedef struct { // Structure to carry optab values
    char mnemonic[MAX_OPCODE_LEN]; // Mnemonic names here
    char opcode[5]; // OPcode values of the Mnemonic
} OptabEntry; 

typedef struct { // Structure to carry symtab values
    char label[MAX_LABEL_LEN]; // Symtab label names
    int address; // locttr value of each labels
} Symbol;

OptabEntry optab[MAX_OPTAB_ENTRIES]; // OPtab entries are strored in this array from optab.txt
int optab_size = 0; // size optab - number of lines in optab

Symbol symtab[MAX_SYMBOLS]; // SYMTAB entries are stored in this array
int symtab_size = 0; // Number of lines in SYMTAB

int LOCCTR = 0; // LOCCTR value
int start_address = 0; // Starting addr
int program_length = 0; // length of the program


void load_optab(const char* filename); // function to load optab.txt
int search_optab(const char* mnemonic); // function to search optab for values
int add_symbol(const char* label, int address); // function to add label to symtab struct
void write_symtab(const char* filename); // Write the symtab values into symtab.txt
int process_byte_operand(const char* operand); // process opreands like byte, resw
void process_asm(const char* input_file, const char* intermediate_file); // take entries from assembly file

// Main
int main() {
    load_optab("optab.txt");
    process_asm("input.asm", "intermediate.txt");
    write_symtab("symtab.txt");

    printf("Program length: %X (%d decimal)\n", program_length, program_length);
    return 0;
}


void load_optab(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open OPTAB file");
        exit(1);
    }

    while (fscanf(fp, "%s %s", optab[optab_size].mnemonic, optab[optab_size].opcode) == 2) {
        optab_size++; // for each line, optab lenght increases - avoids any false values
    }

    fclose(fp);
}

// Search OPTAB for mnemonic
int search_optab(const char* mnemonic) {
    for (int i = 0; i < optab_size; i++) {
        if (strcmp(optab[i].mnemonic, mnemonic) == 0)
            return 1;
    }
    return 0;
}

// Add label to SYMTAB
int add_symbol(const char* label, int address) {
    for (int i = 0; i < symtab_size; i++) {
        if (strcmp(symtab[i].label, label) == 0) {
            printf("Error: Duplicate symbol %s\n", label);
            return 0;
        }
    }
    strcpy(symtab[symtab_size].label, label);
    symtab[symtab_size].address = address;
    symtab_size++;
    return 1;
}

// Write SYMTAB to file
void write_symtab(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("Failed to write SYMTAB");
        exit(1);
    }

    for (int i = 0; i < symtab_size; i++) {
        fprintf(fp, "%s\t%04X\n", symtab[i].label, symtab[i].address);
    }

    fclose(fp);
}


int process_byte_operand(const char* operand) {
    if (operand[0] == 'C') {
        // Format: C'EOF'
        int len = strlen(operand) - 3; // exclude C' and '
        return len;
    }
    return 1;
}

// Process Assembly File
void process_asm(const char* input_file, const char* intermediate_file) {
    FILE* fin = fopen(input_file, "r");
    FILE* fout = fopen(intermediate_file, "w");
    if (!fin || !fout) {
        perror("File error");
        exit(1);
    }

    char line[MAX_LINE_LEN], label[MAX_LABEL_LEN], opcode[MAX_OPCODE_LEN], operand[30];
    int first_line = 1;

    while (fgets(line, sizeof(line), fin)) {
        int fields = sscanf(line, "%s %s %s", label, opcode, operand);

        if (fields == 0 || line[0] == '.') continue; // skip empty/comment lines

        if (first_line && strcmp(opcode, "START") == 0) {
            sscanf(operand, "%x", &start_address);
            LOCCTR = start_address;
            fprintf(fout, "%04X\t%s", LOCCTR, line);
            first_line = 0;
            continue;
        }

        if (first_line) {
            LOCCTR = 0;
            first_line = 0;
        }

        char cur_label[MAX_LABEL_LEN] = "", cur_opcode[MAX_OPCODE_LEN] = "", cur_operand[30] = "";

        if (fields == 3) {
            strcpy(cur_label, label);
            strcpy(cur_opcode, opcode);
            strcpy(cur_operand, operand);
        } else if (fields == 2) {
            strcpy(cur_label, "");
            strcpy(cur_opcode, label);
            strcpy(cur_operand, opcode);
        } else if (fields == 1) {
            strcpy(cur_label, "");
            strcpy(cur_opcode, label);
            strcpy(cur_operand, "");
        }

        fprintf(fout, "%04X\t%s", LOCCTR, line);

        if (strlen(cur_label) > 0)
            add_symbol(cur_label, LOCCTR);

        if (search_optab(cur_opcode)) {
            LOCCTR += 3;
        } else if (strcmp(cur_opcode, "WORD") == 0) {
            LOCCTR += 3;
        } else if (strcmp(cur_opcode, "RESW") == 0) {
            LOCCTR += 3 * atoi(cur_operand);
        } else if (strcmp(cur_opcode, "RESB") == 0) {
            LOCCTR += atoi(cur_operand);
        } else if (strcmp(cur_opcode, "BYTE") == 0) {
            LOCCTR += process_byte_operand(cur_operand);
        } else if (strcmp(cur_opcode, "END") == 0) {
            break;
        } else {
            printf("Warning: Unrecognized opcode %s\n", cur_opcode);
        }
    }

    program_length = LOCCTR - start_address;

    fclose(fin);
    fclose(fout);
}
