#include <SoftwareSerial.h>

SoftwareSerial Serial1(10, 11); // RX (10), TX (11)

#define OPCODE_E    0b011110    // TX (piloto -> Avião)
#define OPCODE_R    0b011111    // RX (Avião -> piloto)

// Pinos seguros para UART no ESP32
#define RX_PIN 16
#define TX_PIN 17

#define FLAG_STD    0b00
#define FLAG_TEMP    0b01
#define FLAG_COMB    0b10
#define FLAG_ALT    0b11

#define CMD_SUBIR   0b00000001
#define CMD_DESCER  0b00000010
#define CMD_CHECAR  0b00000011

byte altura_atual = 0;

void setup() {
  Serial1.begin(2400);
  //Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.begin(2400);
}

void loop() {
  // Fica escutando a piloto
  if (Serial1.available() >= 4) {
    
    byte expected_opcode = (OPCODE_E);
    Serial.println("opcode");
    byte firstbyte = Serial1.read();
    byte opcode = firstbyte >> 2; // Extrai os 6 bits mais significativos
    byte flag = firstbyte & 0b00000011; // Extrai os 2 bits menos significativos
    
    Serial.println(opcode);
    if (opcode == expected_opcode) {
      byte cmd = Serial1.read();
      byte val = Serial1.read();
      byte chk = Serial1.read();
      
  	  Serial.println("Saidas");
      Serial.println(flag);
      Serial.println(cmd);
      Serial.println(val);
      Serial.println(chk);

      byte header_resposta;
      // Se o pacote chegou íntegro, responde
      if ((cmd ^ val) == chk) {
		    if(flag == FLAG_STD) {
          header_resposta = (OPCODE_R << 2) | 0b00;
          if(cmd == CMD_SUBIR) {
            altura_atual += val; // Exemplo de subida
          }
          else if(cmd == CMD_DESCER) {
            altura_atual -= val; // Exemplo de descida
            //nao pode ir <0
            if(altura_atual <= 0) altura_atual = 0;
          }
        }
        else if(flag == FLAG_TEMP){
          header_resposta = (OPCODE_R << 2) | 0b01;
          val = 25; 
          chk = cmd ^ val;
        }
        else if(flag == FLAG_COMB){
          header_resposta = (OPCODE_R << 2) | 0b10;
          val = 200; 
          chk = cmd ^ val;
        }
        else if(flag == FLAG_ALT){
          header_resposta = (OPCODE_R << 2) | 0b11;
          val = altura_atual; 
          chk = cmd ^ val;
        }
        // Ecoa os dados de volta para a piloto validar
        Serial1.write(header_resposta);
        Serial1.write(cmd);
        Serial1.write(val);
        Serial1.write(chk);
      }
    }
    else {
      // Se o header estiver errado, limpa o buffer para não travar a próxima leitura
      while(Serial1.available() > 0) Serial1.read();
    }
  }
}