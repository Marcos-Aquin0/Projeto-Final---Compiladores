#include <SoftwareSerial.h>

SoftwareSerial SerialTorre(10, 11); // RX (10), TX (11)

#define OPCODE_E    0b011110    // TX (Torre -> Avião)
#define OPCODE_R    0b011111    // RX (Avião -> Torre)

void setup() {
  SerialTorre.begin(9600);
  Serial.begin(9600);
}

void loop() {
  // Fica escutando a Torre
  if (Serial.available() >= 4) {
    
    byte expected_header = (OPCODE_E << 2) | 0b00; // Cabeçalho que a torre envia
    Serial.println(expected_header);
    
    if (Serial.read() == expected_header) {
      byte cmd = Serial.read();
      byte val = Serial.read();
      byte chk = Serial.read();

      Serial.println(val);
      // Se o pacote chegou íntegro, responde
      if ((cmd ^ val) == chk) {
        
        // Monta o cabeçalho de resposta da FPGA
        byte header_resposta = (OPCODE_R << 2) | 0b00;
        
        // Ecoa os dados de volta para a torre validar
        Serial.write(header_resposta);
        Serial.write(cmd);
        Serial.write(val);
        Serial.write(chk);
      }
    }
  }
}