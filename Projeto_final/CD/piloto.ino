// #include <SoftwareSerial.h>

// // Cria uma porta serial virtual nos pinos 10 (RX) e 11 (TX)
// SoftwareSerial Serial1(10, 11);

// opcode (6) | flags(2) | comando(8) | valor (8) | checksum (8) 
#define CMD_SUBIR   0b00000001  
#define CMD_DESCER  0b00000010  
#define CMD_CHECAR  0b00000011  

// Opcodes de 6 bits
#define OPCODE_E    0b011110    // TX (piloto -> Avião)
#define OPCODE_R    0b011111    // RX (Avião -> piloto)

void setup() {
  // Comunicação com o PC (Monitor Serial) via USB
  Serial.begin(9600); 
  // Comunicação com a FPGA (Pinos 18 TX1 / 19 RX1)
  Serial1.begin(9600); 
  
  // Aguarda a porta do PC iniciar
  while (!Serial); 
  
  Serial.println("=====================================");
  Serial.println("       SISTEMA FLY BY WIRE   ");
  Serial.println("=====================================");
}

void menu() {
  Serial.println("\nComandos disponiveis:");
  Serial.println("[W] - Subir");
  Serial.println("[S] - Descer");
  Serial.println("[C] - Checar sistemas");
  Serial.println("Digite seu comando:");
}

void menu2() {
  Serial.println("\nEscolha o sistema a ser checado:");
  Serial.println("[1] - Status do Motor");
  Serial.println("[2] - Nivel de Combustível");
  Serial.println("[3] - Temperatura do Motor");
  Serial.println("Digite o numero correspondente:");
}

void loop() {
  // ==========================================================
  // PC -> ARDUINO -> FPGA (Enviando comandos para o avião)
  // ==========================================================
  
  menu(); // Exibe o menu de comandos para o usuário
  while(Serial.available() == 0); 
  if (Serial.available() > 0) {
    char tecla = Serial.read();
    
    if (tecla == 'W' || tecla == 'w') {
      
        Serial.println("\n Quantos graus?");
        while(Serial.available() == 0); 

        if(Serial.available() > 0){
            int graus = Serial.parseInt();
            enviarComandoParaFPGA(CMD_SUBIR, graus);
        } 
    } 
    else if (tecla == 'S' || tecla == 's') {
        Serial.println("\n Quantos graus?");
        while(Serial.available() == 0); 

        if(Serial.available() > 0){
            int graus = Serial.parseInt();
            enviarComandoParaFPGA(CMD_DESCER, graus);
        } 
    }
    else if (tecla == 'C' || tecla == 'c') {
        menu2();
        while(Serial.available() == 0); 

        if(Serial.available() > 0){
            int check = Serial.parseInt();
            enviarComandoParaFPGA(CMD_CHECAR, check);
        } 
    }
  }

  // ==========================================================
  // FPGA -> ARDUINO -> PC (Recebendo status do avião)
  // ==========================================================
  
  if (Serial1.available() >= 4) { // Verifica a Serial1 (FPGA)
    byte expected_header = (OPCODE_R << 2) | 0b00;
    if (Serial1.read() == expected_header) { // Verifica o header do pacote
      byte cmd_recebido = Serial1.read();
      byte val_recebido = Serial1.read();
      byte chk_recebido = Serial1.read();
      
      // Sanity Check: Verifica integridade do pacote
      byte chk_calculado = cmd_recebido ^ val_recebido;
      
      if (chk_calculado == chk_recebido) {
        exibirRespostaNoPC(cmd_recebido, val_recebido);
      } else {
        Serial.println("[ALERTA] Interferência no link! Pacote corrompido.");
      }
    }
  }

}

// Envia os 4 bytes fisicamente para a FPGA usando a Serial1
void enviarComandoParaFPGA(byte comando, byte valor) {
  byte header = (OPCODE_E << 2) | 0b00;
  byte checksum = comando ^ valor; 
  
  Serial1.write(header);
  Serial1.write(comando);
  Serial1.write(valor);
  Serial1.write(checksum);
}

// Formata e imprime a resposta da FPGA no Monitor Serial do PC
void exibirRespostaNoPC(byte comando, byte valor) {
  Serial.print("[AVIÃO] Resposta Recebida -> ");
  
  if (comando == CMD_SUBIR) { 
    Serial.print("Confirmado: Subindo ");
    Serial.print(valor);
    Serial.println(" graus.");
  } 
  else if (comando == CMD_DESCER) { 
    Serial.print("Confirmado: Descendo ");
    Serial.print(valor);
    Serial.println(" graus.");
  }
  else if (comando == CMD_CHECAR) {
    Serial.print("Check OK. Sistema ");
    Serial.print(valor);
    Serial.println(" operante.");
  }
  else {
    Serial.println("!!! FALHA CRÍTICA !!!");
  }
  Serial.println("-------------------------------------");
}