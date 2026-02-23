// // Definição dos Comandos de Aviação (Protocolo SkyLink)
// opcode (6) | flags(2) | comando (8) | valor (8) | checksum (8) 
// #define CMD_SUBIR   0x01
// #define CMD_DESCER  0x02
// #define CMD_CHECAR  0x03

// #define START_BYTE  0xAA

void setup() {
  // 1. Porta Serial 0: Comunicação com o PC (Monitor Serial) via USB
  Serial.begin(9600); 
  // 2. Porta Serial 1: Comunicação com a FPGA (Pinos 18 TX1 / 19 RX1)
  Serial1.begin(9600); 
  
  // Aguarda a porta do PC iniciar
  while (!Serial) { ; }
  
  Serial.println("=====================================");
  Serial.println("       SISTEMA FLY BY WIRE   ");
  Serial.println("=====================================");
}

void menu() {
  Serial.println("\nComandos disponíveis:");
  Serial.println("[W] - Subir");
  Serial.println("[S] - Descer");
  Serial.println("[C] - Checar sistemas");
  Serial.println("Digite seu comando:");
}

void menu2() {
  Serial.println("\nEscolha o sistema a ser checado:");
  Serial.println("[1] - Status do Motor");
  Serial.println("[2] - Nível de Combustível");
  Serial.println("[3] - Temperatura do Motor");
  Serial.println("Digite o número correspondente:");
}

void loop() {
  // ==========================================================
  // PC -> ARDUINO -> FPGA (Enviando comandos para o avião)
  // ==========================================================
  
  
  menu(); // Exibe o menu de comandos para o usuário
  while(Serial.available() == 0); 
  if (Serial.available() > 0) {
    char tecla = Serial.read();
    
    if (tecla == 'S' || tecla == 's') {
      Serial.println("\n Quantos graus?");
      while(Serial.available() == 0); 

      if(Serial.available() > 0){
      int graus = Serial.readInt();
      enviarComandoParaFPGA(CMD_SUBIR, graus);
      } 
    } 
    else if (tecla == 'D' || tecla == 'd') {
        Serial.println("\n Quantos graus?");
        while(Serial.available() == 0); 

        if(Serial.available() > 0){
        int graus = Serial.readInt();
        enviarComandoParaFPGA(CMD_DESCER, graus);
        } 
    }
    else if (tecla == 'C' || tecla == 'c') {
        menu2();
        while(Serial.available() == 0); 

        if(Serial.available() > 0){
        char check = Serial.read();
        enviarComandoParaFPGA(CMD_CHECAR, check);
        } 
    }
  }

  // ==========================================================
  // FPGA -> ARDUINO -> PC (Recebendo status do avião)
  // ==========================================================
  
  if (Serial1.available() >= 4) { // Verifica a Serial1 (FPGA)
    if (Serial1.read() == START_BYTE) {
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
  byte checksum = comando ^ valor; 
  
  Serial1.write(opcode+flags);
  Serial1.write(comando);
  Serial1.write(valor);
  Serial1.write(checksum);
}

// Formata e imprime a resposta da FPGA no Monitor Serial do PC
void exibirRespostaNoPC(byte comando, byte valor) {
  Serial.print("[AVIÃO] Resposta Recebida -> ");
  
  if (comando == 0xAA) { // Exemplo: Código de Sucesso
    Serial.print("Comando aceito. Novo Status: ");
    Serial.println(valor);
  } 
  else if (comando == 0xEE) { // Exemplo: Código de Erro/Falha
    Serial.println("!!! FALHA CRÍTICA REJEITADA PELO TMR !!!");
  }
  else {
    Serial.print("Desconhecida | CMD: 0x");
    Serial.print(comando, HEX);
    Serial.print(" | VALOR: ");
    Serial.println(valor);
  }
  Serial.println("-------------------------------------");
}