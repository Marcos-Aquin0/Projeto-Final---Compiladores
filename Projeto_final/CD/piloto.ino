// opcode (6) | flag(2) | comando(8) | valor (8) | checksum (8) 
// #include <SoftwareSerial.h>
// SoftwareSerial Serial1(10, 11);

#define CMD_SUBIR   0b00000001  
#define CMD_DESCER  0b00000010  
#define CMD_CHECAR  0b00000011  
#define FLAG_STD    0b00
#define FLAG_TEMP    0b01
#define FLAG_COMB    0b10
#define FLAG_ALT    0b11

// Opcodes de 6 bits
#define OPCODE_E    0b011110    // TX (piloto -> Avião)
#define OPCODE_R    0b011111    // RX (Avião -> piloto)

// Pinos seguros para UART no ESP32
#define RX_PIN 16
#define TX_PIN 17

byte flag; 

void setup() {
  // Comunicação com o PC (Monitor Serial) via USB
  // Serial.begin(2400); 
  Serial.begin(115200); 
  
  // Comunicação com a FPGA - ESP32
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN); 
  // Serial1.begin(2400);
  // Aguarda a porta do PC iniciar
  while (!Serial); 
  while(!Serial1);
  
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
  Serial.println("[1] - Temperatura do Motor");
  Serial.println("[2] - Nivel de Combustível");
  Serial.println("[3] - Altura atual");
  Serial.println("Digite o numero correspondente:");
}

void loop() {
  // ==========================================================
  // PC -> ESP32 -> FPGA (Enviando comandos para o avião)
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
            flag = FLAG_STD;
            enviarComandoParaFPGA(CMD_SUBIR, graus, flag);
        } 
    } 
    else if (tecla == 'S' || tecla == 's') {
        Serial.println("\n Quantos graus?");
        while(Serial.available() == 0); 

        if(Serial.available() > 0){
            int graus = Serial.parseInt();
            flag = FLAG_STD;
            enviarComandoParaFPGA(CMD_DESCER, graus, flag);
        } 
    }
    else if (tecla == 'C' || tecla == 'c') {
        menu2();
        while(Serial.available() == 0); 

        if(Serial.available() > 0){
            int input = Serial.parseInt();
            if(input == 1){
              flag = FLAG_TEMP;
            }
            else if(input == 2){
              flag = FLAG_COMB;
            }
            else if(input == 3){
              flag = FLAG_ALT;
            }
            enviarComandoParaFPGA(CMD_CHECAR, 0, flag);
        } 
    }
  }

  // ==========================================================
  // FPGA -> ESP32 -> PC (Recebendo status do avião)
  // ==========================================================
  unsigned long tempoInicio = millis();
  while (millis() - tempoInicio < 10000){
    if (Serial1.available() >= 4) { // Verifica a Serial1 (FPGA)
      byte expected_header = (OPCODE_R << 2) | flag;
      byte header = Serial1.read();
      Serial.println(header);
      if (header == expected_header) { // Verifica o header do pacote
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
      else {
        // Se o header estiver errado, limpa o buffer para não travar a próxima leitura
        while(Serial1.available() > 0) Serial1.read();
      }
    break;
    }
  }

}

// Envia os 4 bytes fisicamente para a FPGA usando a Serial1
void enviarComandoParaFPGA(byte comando, byte valor, byte f) {
  byte header = (OPCODE_E << 2) | flag;
  byte checksum = comando ^ valor; 
  
  Serial1.write(header);
  delay(1000);
  Serial1.write(comando);
  delay(1000);
  Serial1.write(valor);
  delay(1000);
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
    Serial.println("Check OK. Sistema operante ");
    if(flag == FLAG_TEMP)      Serial.println("Temperatura do Motor");
    else if(flag == FLAG_COMB) Serial.println("Nivel de Combustivel");
    else if(flag == FLAG_ALT)  Serial.println("Altura atual");
    Serial.println(valor);
  }
  else {
    Serial.println("!!! FALHA CRÍTICA !!!");
  }
  Serial.println("-------------------------------------");
}