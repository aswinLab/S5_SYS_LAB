#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 100
#define MAX_LABEL_LEN 30
#define MAX_OPCODE_LEN 10
#define MAX_OPTAB_ENTRIES 100
#define MAX_SYMBOLS 100

typedef struct {
    char mnemonic[MAX_OPCODE_LEN];
    char opcode[5];
} OptabEntry;

typedef struct {
    char label[MAX_LABEL_LEN];
    int address;
} Symbol;

OptabEntry optab[MAX_OPTAB_ENTRIES];
int optab_size = 0;

Symbol symtab[MAX_SYMBOLS];
int symtab_size = 0;

int start_address = 0;
int program_length = 0;

// Function declarations
void load_optab(const char* filename);
void load_symtab(const char* filename);
char* get_opcode(const char* mnemonic);
int get_sym_address(const char* label);
void pass2(const char* intermediate_file, const char* object_file);

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <intermediate file>\n", argv[0]);
        return 1;
    }

    load_optab("../pass1/optab.txt");
    load_symtab("../pass1/symtab.txt");

    pass2(argv[1], "op");

    return 0;
}

void load_optab(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open OPTAB file");
        exit(1);
    }

    optab_size = 0;
    while (fscanf(fp, "%s %s", optab[optab_size].mnemonic, optab[optab_size].opcode) == 2) {
        optab_size++;
    }
    fclose(fp);
}

void load_symtab(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open SYMTAB file");
        exit(1);
    }

    symtab_size = 0;
    while (fscanf(fp, "%s %x", symtab[symtab_size].label, &symtab[symtab_size].address) == 2) {
        symtab_size++;
    }
    fclose(fp);
}

char* get_opcode(const char* mnemonic) {
    for (int i = 0; i < optab_size; i++) {
        if (strcmp(optab[i].mnemonic, mnemonic) == 0) {
            return optab[i].opcode;
        }
    }
    return NULL;
}

int get_sym_address(const char* label) {
    for (int i = 0; i < symtab_size; i++) {
        if (strcmp(symtab[i].label, label) == 0) {
            return symtab[i].address;
        }
    }
    return -1; // not found
}

void pass2(const char* intermediate_file, const char* object_file) {
    FILE* fin = fopen(intermediate_file, "r");
    FILE* fout = fopen(object_file, "w");
    if (!fin || !fout) {
        perror("File open error");
        exit(1);
    }

    char line[MAX_LINE_LEN];
    char address_str[10], label[MAX_LABEL_LEN], opcode[MAX_OPCODE_LEN], operand[30];
    int start_flag = 0;
    int prog_start = 0, prog_length = 0;
    char program_name[20] = "";

    // For building text records
    char text_record[1000] = "";  // larger buffer to avoid overflow
    int text_start_addr = 0;
    int text_length = 0; // length in bytes

    while (fgets(line, sizeof(line), fin)) {
        // Parse intermediate file line format:
        // address \t <asm line> (label opcode operand)
        char *token = strtok(line, "\t\n");
        if (!token) continue;

        strcpy(address_str, token);
        int loc = (int)strtol(address_str, NULL, 16);

        token = strtok(NULL, "\n");
        if (!token) continue;
        char rest_line[MAX_LINE_LEN];
        strcpy(rest_line, token);

        // Initialize fields
        label[0] = opcode[0] = operand[0] = '\0';

        // Parse label, opcode, operand
        // Note: operand may contain commas or be missing
        int fields = sscanf(rest_line, "%s %s %s", label, opcode, operand);

        // Handle START directive
        if (!start_flag && strcmp(opcode, "START") == 0) {
            start_flag = 1;
            prog_start = loc;
            strcpy(program_name, label);
            fprintf(fout, "H%-6s%06X%06X\n", program_name, prog_start, program_length);
            text_start_addr = prog_start;
            continue;
        }

        // Handle END directive
        if (strcmp(opcode, "END") == 0) {
            // Flush remaining text record if any
            if (text_length > 0) {
                fprintf(fout, "T%06X%02X%s\n", text_start_addr, text_length, text_record);
            }
            fprintf(fout, "E%06X\n", prog_start);
            break;
        }

        char object_code[50] = "";

        char *op_code_hex = get_opcode(opcode);

        if (op_code_hex != NULL) {
            // Opcode found in OPTAB

            // Handle indexed addressing
            int indexed = 0;
            char operand_copy[30] = "";
            if (strlen(operand) > 0) {
                strcpy(operand_copy, operand);

                char *comma = strchr(operand_copy, ',');
                if (comma) {
                    *comma = '\0';
                    if (strcmp(comma + 1, "X") == 0) {
                        indexed = 1;
                    } else {
                        printf("Warning: Unsupported addressing mode in operand %s\n", operand);
                    }
                }
            }

            int sym_addr = 0;
            if (strlen(operand_copy) > 0) {
                sym_addr = get_sym_address(operand_copy);
                if (sym_addr == -1) {
                    printf("Error: Symbol %s not found\n", operand_copy);
                    sym_addr = 0;
                }
            }

            if (indexed) {
                sym_addr |= 0x8000; // Set indexed addressing bit (bit 15)
            }

            // Compose object code: opcode (2 hex chars) + 4 hex chars address
            sprintf(object_code, "%s%04X", op_code_hex, sym_addr);

        } else if (strcmp(opcode, "WORD") == 0) {
            // WORD: 3 bytes integer in hex
            int val = atoi(operand);
            sprintf(object_code, "%06X", val & 0xFFFFFF);

        } else if (strcmp(opcode, "BYTE") == 0) {
            // BYTE: C'...' or X'...'
            if (operand[0] == 'C') {
                int len = strlen(operand);
                char temp[50] = "";
                int j = 0;
                for (int i = 2; i < len - 1; i++) {
                    sprintf(temp + j, "%02X", operand[i]);
                    j += 2;
                }
                strcpy(object_code, temp);
            } else if (operand[0] == 'X') {
                int len = strlen(operand);
                char temp[50] = "";
                int k = 0;
                for (int i = 2; i < len - 1; i++) {
                    temp[k++] = operand[i];
                }
                temp[k] = '\0';
                strcpy(object_code, temp);
            } else {
                printf("Warning: Invalid BYTE operand format: %s\n", operand);
            }
        } else if (strcmp(opcode, "RESW") == 0 || strcmp(opcode, "RESB") == 0) {
            // No object code, flush text record if any
            if (text_length > 0) {
                fprintf(fout, "T%06X%02X%s\n", text_start_addr, text_length, text_record);
                text_record[0] = '\0';
                text_length = 0;
            }
            continue; // skip writing object code
        } else {
            // Opcode not recognized
            printf("Warning: Opcode %s not recognized in pass2\n", opcode);
            continue;
        }

        // Add object_code to text record if generated
        if (strlen(object_code) > 0) {
            int obj_len = strlen(object_code) / 2; // length in bytes

            // If adding this object code exceeds 30 bytes, flush current record first
            if (text_length + obj_len > 30) {
                fprintf(fout, "T%06X%02X%s\n", text_start_addr, text_length, text_record);
                text_record[0] = '\0';
                text_length = 0;
                text_start_addr = loc;
            }

            strcat(text_record, object_code);
            text_length += obj_len;
        }
    }

    fclose(fin);
    fclose(fout);
}
