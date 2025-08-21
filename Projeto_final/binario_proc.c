#include "binario_proc.h"

// Array global para armazenar os mapeamentos
LabelMapping mappingLabel[MAX_LABELS];
int labelCount = 0;

// Função para adicionar um mapeamento de rótulo
void addLabelMapping(const char* label, int index) {
    if (labelCount < MAX_LABELS && label[0] != '\0') {
        mappingLabel[labelCount].label = strdup(label);
        mappingLabel[labelCount].index = index-labelCount;
        labelCount++;
    }
}

// Função para liberar a memória alocada para os rótulos
void freeLabelMappings() {
    for (int i = 0; i < labelCount; i++) {
        free(mappingLabel[i].label);
    }
    labelCount = 0;
}

void getLabelNumber(char* line, char* label, int* index) {    
    *index = -1; // Valor padrão se não encontrar índice
    label[0] = '\0'; // Valor padrão para o rótulo
    
    // Limpar espaços iniciais
    char* start = line;
    while (*start && isspace(*start)) {
        start++;
    }
    
    // Verificar o formato "número - label:"
    char* dash_pos = strstr(start, " - ");
    if (dash_pos != NULL) {
        // Extrair o número (índice)
        char index_str[32] = {0};
        int i = 0;
        for (char* p = start; p < dash_pos && i < 31; p++) {
            if (isdigit(*p)) {
                index_str[i++] = *p;
            }
        }
        index_str[i] = '\0';
        
        // Converter string para inteiro
        if (index_str[0] != '\0') {
            *index = atoi(index_str);
            // *index = *index + 1; // Incrementar o índice em 1, para começar pela instrução 1 da função
        }
        
        // Extrair o rótulo (label)
        char* label_start = dash_pos + 3; // Pular o " - "
        
        // Encontrar o final do rótulo (antes de ':' ou fim da linha)
        char* colon_pos = strchr(label_start, ':');
        
        if (colon_pos != NULL) {
            // Temos um rótulo seguido por ':'
            int length = colon_pos - label_start;
            strncpy(label, label_start, length);
            label[length] = '\0'; // Garantir terminação da string
        } else {
            label[0] = '\0'; // Não encontramos um rótulo válido
        }
    }
    
    // Remover espaços no fim do rótulo
    int len = strlen(label);
    while (len > 0 && isspace(label[len-1])) {
        label[--len] = '\0';
    }
}

InstructionInfo instructionTable[] = {
    {"add", TYPE_R, 0, 2},     // 000000 ... 0010
    {"sub", TYPE_R, 0, 3},     // 000000 ... 0011
    {"mul", TYPE_R, 0, 4},     // 000000 ... 0100
    {"div", TYPE_R, 0, 5},     // 000000 ... 0101
    {"and", TYPE_R, 0, 0},     // 000000 ... 0000
    {"or", TYPE_R, 0, 1},      // 000000 ... 0001
    {"nor", TYPE_R, 0, 8},     // 000000 ... 1000
    {"sr", TYPE_R, 1, 6},      // 000001 ... 0110
    {"sl", TYPE_R, 1, 7},      // 000001 ... 0111
    {"jr", TYPE_R, 2, 9},      // 000010 ... 1001
    
    {"andi", TYPE_I, 3, 0},    // 000011 ...
    {"ori", TYPE_I, 4, 0},     // 000100 ...
    {"addi", TYPE_I, 5, 0},    // 000101 ...
    {"subi", TYPE_I, 6, 0},    // 000110 ...
    {"li", TYPE_I, 7, 0},      // 000111 ...
    {"lw", TYPE_I, 8, 0},      // 001000 ... (lw1)
    {"lw2", TYPE_I, 9, 0},      // 001001 ... (lw2)
    {"lw3", TYPE_I, 10, 0},     // 001010 ... (lw3) - endereçamentos
    {"sw", TYPE_I, 11, 0},     // 001011 ...
    {"beq", TYPE_I, 12, 0},    // 001100 ...
    {"blt", TYPE_I, 13, 0},    // 001101 ...
    {"bgt", TYPE_I, 14, 0},    // 001110 ...
    {"bnq", TYPE_I, 15, 0},    // 001111 ... 
    {"bge", TYPE_I, 23, 0},    // 010000 ... 
    {"ble", TYPE_I, 24, 0},    // 010001 ... 
    {"in", TYPE_I, 16, 0},     // 010000 ...
    {"out", TYPE_I, 17, 0},    // 010001 ...
    {"move", TYPE_I, 18, 0},   // 010010 ...
    
    
    {"j", TYPE_J, 19, 0},      // 010011 ...
    {"jal", TYPE_J, 20, 0},    // 010100 ...
    {"nop", TYPE_J, 22, 0},    // 010110 ...
    
    {"halt", TYPE_MK, 21, 0},   // 010101 desligar SO
    {"msgLcd",        TYPE_MK, 26, 0}, // opcode 011010
    {"saltoUser",  TYPE_MU, 27, 0}, // opcode 011011
    {"syscall",        TYPE_MU, 28, 0}, // opcode 011100
    
    {"", TYPE_INVALID, 0, 0}
};

int extractRegister(const char* regStr) {
    if (regStr[0] == '$' && regStr[1] == 'r') {
        return atoi(regStr + 2); 
    }
    return -1; 
}

InstructionInfo* findInstruction(const char* mnemonic) {
    for (int i = 0; instructionTable[i].type != TYPE_INVALID; i++) {
        if (strcasecmp(instructionTable[i].mnemonic, mnemonic) == 0) {
            return &instructionTable[i];
        }
    }
    return NULL; 
}

void generateBinary(const char* instruction, char* binaryOutput, int index_atual) {
    char instr_copy[MAX_LINE_LENGTH];
    strcpy(instr_copy, instruction);
    
    char* mnemonic = strtok(instr_copy, " \t\n\r");
    if (!mnemonic) {
        strcpy(binaryOutput, "01011000000000000000000000000000"); // Default é o nop
        return;
    }
    
    InstructionInfo* info = findInstruction(mnemonic);
    if (!info) {
        sprintf(binaryOutput, "// Instrução desconhecida: %s", mnemonic);
        return;
    }
    
    unsigned int binary = 0;
    
    switch (info->type) {
        case TYPE_R: {
            // Formato: add $rd $rs $rt
            char* rdStr = strtok(NULL, " ,\t\n\r");
            char* rsStr = strtok(NULL, " ,\t\n\r");
            char* rtStr = strtok(NULL, " ,\t\n\r");
            
            int rd = extractRegister(rdStr);
            int rs = extractRegister(rsStr);
            int rt = extractRegister(rtStr);
            
            if (rd == REG_RA){
                rt = rd;
                rs = 0;
                rd = 0;

            }
            else if (rd != REG_RA && (rd < 0 || rs < 0 || rt < 0)) {
                sprintf(binaryOutput, "// Registrador inválido: %s", instruction);
                return;
            }
            
            //opcode(6) rs(6) rt(6) rd(6) shamt(4) funct(4)
            binary = (info->opcode << 26) | (rs << 20) | (rt << 14) | (rd << 8) | (0 << 4) | info->funct;
            break;
        }
        
        case TYPE_I: {
            char* rsStr = strtok(NULL, " ,\t\n\r");
            char* rtStr = NULL;
            int immediate = 0;
            int rs, rt;
            
            if (strcmp(mnemonic, "li") == 0) {
                // li $rs, immediate
                rs = 0;
                rt = extractRegister(rsStr);
                char* immStr = strtok(NULL, " ,\t\n\r");
                immediate = atoi(immStr);
            } 
            else if (strcmp(mnemonic, "beq") == 0 || strcmp(mnemonic, "bnq") == 0 || 
                     strcmp(mnemonic, "blt") == 0 || strcmp(mnemonic, "bgt") == 0 ||
                     strcmp(mnemonic, "bge") == 0 || strcmp(mnemonic, "ble") == 0){
                // beq $rs, $rt, label
                rs = extractRegister(rsStr);
                rtStr = strtok(NULL, " ,\t\n\r");
                rt = extractRegister(rtStr);
                
                // pegar a posição da label
                char* labelStr = strtok(NULL, " ,\t\n\r");
                int targetIndex = -1;
                for (int i = 0; i < labelCount; i++) {
                    if (strcmp(mappingLabel[i].label, labelStr) == 0) {
                        targetIndex = mappingLabel[i].index;
                        break;
                    }
                }
                
                if (targetIndex != -1) {
                    immediate = targetIndex;
                } else {
                    immediate = atoi(labelStr); 
                }
            }
            else if (strcmp(mnemonic, "lw") == 0 || strcmp(mnemonic, "sw") == 0) {
                // lw $rt, offset($rs) or sw $rt, offset($rs)
                rt = extractRegister(rsStr);
                
                char* offsetBaseStr = strtok(NULL, " \t\n\r");
                char offsetStr[32] = {0};
                char baseRegStr[32] = {0};
                
                int i = 0;
                while (offsetBaseStr[i] && offsetBaseStr[i] != '(') {
                    offsetStr[i] = offsetBaseStr[i];
                    i++;
                }
                offsetStr[i] = '\0';
                
                if (offsetBaseStr[i] == '(') {
                    int j = 0;
                    i++; // Skip '('
                    while (offsetBaseStr[i] && offsetBaseStr[i] != ')') {
                        baseRegStr[j++] = offsetBaseStr[i++];
                    }
                    baseRegStr[j] = '\0';
                }
                
                rs = extractRegister(baseRegStr);
                immediate = atoi(offsetStr)*2; // por algum motivo, o offset no addi e no subi é 8 ao invés de 4, então é necessário multiplicar por 2
                
                if (strcmp(mnemonic, "lw") == 0) {
                    info->opcode = 8;      // LW1
                    // else if (immediate > 0) info->opcode = 9;  // LW2
                    // else info->opcode = 10;                    // LW3
                }
            }
            else if (strcmp(mnemonic, "in") == 0 || strcmp(mnemonic, "out") == 0) {
                // in $rd or out $rs
                if (strcmp(mnemonic, "in") == 0) {
                    rt = extractRegister(rsStr);
                    rs = 0; // Not used
                } else { // out
                    rs = extractRegister(rsStr);
                    rt = 0; // Not used
                }
                immediate = 0;
            }
            else if (strcmp(mnemonic, "move") == 0) {
                // move $rd, $rs
                rt = extractRegister(rsStr);
                char* rsStr2 = strtok(NULL, " ,\t\n\r");
                rs = extractRegister(rsStr2);
                immediate = 0;
            }
            else {
                // addi, subi, andi, ori - format: op $rt, $rs, immediate
                rt = extractRegister(rsStr);
                char* rsStr2 = strtok(NULL, " ,\t\n\r");
                rs = extractRegister(rsStr2);
                char* immStr = strtok(NULL, " ,\t\n\r");
                immediate = atoi(immStr);
            }
            
            // Check for valid registers
            if ((rs < 0 && strcmp(mnemonic, "li") != 0) || 
                (rt < 0 && strcmp(mnemonic, "out") != 0)) {
                sprintf(binaryOutput, "// Invalid register in: %s", instruction);
                return;
            }
            
            // Build binary: opcode(6) rs(6) rt(6) immediate(14)
            binary = (info->opcode << 26) | (rs << 20) | (rt << 14) | (immediate & 0x3FFF); // 14-bit immediate
            break;
        }
        
        case TYPE_J: {
            int immediate = 0;
            // Format: j target or jal target
            if (strcmp(mnemonic, "j") == 0 || strcmp(mnemonic, "jal") == 0) {
                char* targetStr = strtok(NULL, " \t\n\r");
                int targetAddr = -1;
                
                // Check if target is a label
                for (int i = 0; i < labelCount; i++) {
                    if (strcmp(mappingLabel[i].label, targetStr) == 0) {
                        targetAddr = mappingLabel[i].index;
                        break;
                    }
                }
                
                if (targetAddr == -1) {
                    // Try to convert direct address
                    targetAddr = atoi(targetStr);
                }
                
                // Build binary: opcode(6) target(26)
                binary = (info->opcode << 26) | (targetAddr & 0x3FFFFFF); // 26-bit target
            }
            else if (strcmp(mnemonic, "nop") == 0){
                char* immStr = strtok(NULL, " ,\t\n\r");
                immediate = atoi(immStr);
                binary = (info->opcode << 26) | (immediate & 0x3FFF);
            }
            break;
        }

        case TYPE_MK: { // Para userOrKernel e msgLcd
            // Formato: opcode immediate
            char* rsStr = strtok(NULL, " ,\t\n\r");
            int rs = extractRegister(rsStr);
            
            if(strcmp(mnemonic, "halt")==0){
                binary = (info->opcode << 26) | (0 & 0x3FFFFFF); // opcode(6) | immediate(26)
            } else{
                // opcode(6) | immediate(26)
                binary = (info->opcode << 26) | (rs & 0x3FFFFFF); // 26-bit immediate
            }
            break;
        }

        case TYPE_MU: { // Para saltoUser e syscall
            // Formato: opcode $rs, immediate
            if(mnemonic == "syscall"){
                // syscall $rs
                char* rsStr = strtok(NULL, " ,\t\n\r");
                int rs = extractRegister(rsStr);
                
                if (rs < 0) {
                    sprintf(binaryOutput, "// Registrador inválido: %s", instruction);
                    return;
                }
                
                // opcode(6) | rs(26)
                binary = (info->opcode << 26) | (rs & 0x3FFFFFF); // 26-bit immediate
            } else {
                char* immStr = strtok(NULL, " ,\t\n\r");
                int immediate = atoi(immStr);

                if (immediate < 0) {
                    sprintf(binaryOutput, "// Registrador inválido: %s", instruction);
                    return;
                }
                
                // opcode(6) | immediate(26)
                binary = (info->opcode << 26) | (immediate & 0xFFFFF);
                
            }
            break;
        }
        
        case TYPE_INVALID:
            sprintf(binaryOutput, "// Invalid instruction type for: %s", mnemonic);
            return;
    }
    
    // Convert binary to string representation (32-bit)
    char binaryStr[33];
    for (int i = 31; i >= 0; i--) {
        binaryStr[31-i] = ((binary >> i) & 1) ? '1' : '0';
    }
    binaryStr[32] = '\0';
    
    sprintf(binaryOutput, "%s\n", binaryStr);
}

int read_assembly_file(FILE* input_file) {
    FILE* output = fopen("Output/binary.txt", "w");
    if (output == NULL) {
        printf("Erro: Não foi possível abrir o arquivo de saída.\n");
        return -1;
    }
    
    // Primeira passagem: Construir o mapeamento de rótulos
    char line[MAX_LINE_LENGTH];
    char label[MAX_LINE_LENGTH];
    int index;
    
    // Inicializar o array de mapeamento
    freeLabelMappings();
    
    // Primeira passagem: extrair todos os rótulos e índices
    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        // Verificar se a linha contém um rótulo
        char* colon_pos = strchr(line, ':');
        if (colon_pos != NULL) {
            // Extrair o rótulo e o índice
            getLabelNumber(line, label, &index);
            if (label[0] != '\0') {
                addLabelMapping(label, index);
            }
        }
    }
    
    // Exibir os mapeamentos encontrados
    printf("Mapeamentos de rótulos encontrados:\n");
    for (int i = 0; i < labelCount; i++) {
        printf("%s -> %d\n", mappingLabel[i].label, mappingLabel[i].index);
    }
    
    // Voltar ao início do arquivo para a segunda passagem
    rewind(input_file);
    int index_atual = 0;
    // Processar o arquivo linha por linha
    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        char processed_line[MAX_LINE_LENGTH];
        char comment[MAX_LINE_LENGTH] = ""; // Para armazenar comentários
        
        // Remover o índice no formato "0 - " do início da linha
        char *current_pos = line;
        
        // Verificar se a linha começa com um número seguido de " - "
        char *dash_pos = strstr(line, " - ");
        if (dash_pos != NULL) {
            // Verificar se todos os caracteres antes do " - " são dígitos
            int is_index = 1;
            for (char *p = line; p < dash_pos; p++) {
                if (!isdigit(*p) && !isspace(*p)) {
                    is_index = 0;
                    break;
                }
            }
            
            // Se for um índice, pular para depois do " - "
            if (is_index) {
                current_pos = dash_pos + 3; // Pular o " - " (3 caracteres)
            }
        }
        
        // Verificar se a linha contém um rótulo (termina com ":")
        char* colon_pos = strchr(current_pos, ':');
        if (colon_pos != NULL) {
            // Verificar se há comentário após o rótulo
            char* comment_pos = strchr(colon_pos + 1, '#');
            if (comment_pos != NULL) {
                // Salvar o comentário para uso posterior
                strcpy(comment, comment_pos);
                *comment_pos = '\0'; // Corta a string no início do comentário
            }
            
            // Remover o rótulo e os ":" e qualquer espaço após ele
            char* after_colon = colon_pos + 1;
            while (*after_colon && isspace(*after_colon)) {
                after_colon++;
            }
            
            // Se houver texto após o rótulo, copiar apenas essa parte
            if (*after_colon) {
                strcpy(processed_line, after_colon);
            } else {
                processed_line[0] = '\0'; // Linha vazia se não houver nada após o rótulo
            }
            
            // Adicionar o comentário de volta, se existir
            if (comment[0] != '\0') {
                // Substituir '#' por '//'
                comment[0] = '\0';
                strcat(processed_line, "// ");
                strcat(processed_line, comment + 1);
            }
        } else {
            // Não é um rótulo, apenas processar comentários normalmente
            strcpy(processed_line, current_pos);
            
            // Processar comentários: substituir '#' por '//'
            char *comment_pos = strchr(processed_line, '#');
            if (comment_pos != NULL) {
                *comment_pos = '\0'; // Termina a string no marcador de comentário
                
                // Recria a linha com o novo marcador de comentário
                char temp[MAX_LINE_LENGTH];
                snprintf(temp, MAX_LINE_LENGTH, "%s// %s", processed_line, comment_pos + 1);
                strcpy(processed_line, temp);
            }
        }
            
        // Now generate binary for the processed instruction
        if (processed_line[0] != '\0' && processed_line[0] != '/') {
            char binaryInstruction[MAX_LINE_LENGTH];
            generateBinary(processed_line, binaryInstruction, index_atual);
            fprintf(output, "%s", binaryInstruction);
            index_atual++;
        } else if (processed_line[0] != '\0') {
            // This is a comment line, pass it through
            // fprintf(output, "%s", processed_line);
        }
    }
    
    fclose(output);
    return 0;

}

// label vira comentario referente ao label, o salto é +1