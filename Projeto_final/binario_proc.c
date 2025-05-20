#include "binario_proc.h"

// Estrutura para armazenar mapeamentos de rótulos e índices
typedef struct {
    char* label;
    int index;
} LabelMapping;

// Array global para armazenar os mapeamentos
LabelMapping mappingLabel[MAX_LABELS];
int labelCount = 0;

// Função para adicionar um mapeamento de rótulo
void addLabelMapping(const char* label, int index) {
    if (labelCount < MAX_LABELS && label[0] != '\0') {
        mappingLabel[labelCount].label = strdup(label);
        mappingLabel[labelCount].index = index;
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
    // Extrai o número do rótulo da linha e adiciona em um vetor contendo o nome da label e o indice
    // exemplo: "0 - main:"; "1 - L0:"
    
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
        
        // Escrever a linha processada no arquivo de saída (se não estiver vazia)
        if (processed_line[0] != '\0' || strchr(processed_line, '\n') != NULL) {
            fputs(processed_line, output);
            
            // Garantir que cada linha termine com uma quebra de linha
            if (strchr(processed_line, '\n') == NULL) {
                fputs("\n", output);
            }
        }
    }
    
    fclose(output);
    return 0;
}

// label vira comentario referente ao label, o salto é +1