// #include <SoftwareSerial.h>

// SoftwareSerial Serialpiloto(10, 11); // RX (10), TX (11)

#define OPCODE_E    0b011110    // TX (piloto -> Avião)
#define OPCODE_R    0b011111    // RX (Avião -> piloto)

// Pinos seguros para UART no ESP32
#define RX_PIN 16
#define TX_PIN 17

void setup() {
  // Serialpiloto.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.begin(9600);
}

void loop() {
  // Fica escutando a piloto
  if (Serial1.available() >= 4) {
    
    byte expected_header = (OPCODE_E << 2) | 0b00; // Cabeçalho que a piloto envia
    Serial1.println(expected_header);
    
    if (Serial1.read() == expected_header) {
      byte cmd = Serial1.read();
      byte val = Serial1.read();
      byte chk = Serial1.read();

      Serial1.println(val);
      // Se o pacote chegou íntegro, responde
      if ((cmd ^ val) == chk) {
        
        // Monta o cabeçalho de resposta da FPGA
        byte header_resposta = (OPCODE_R << 2) | 0b00;
        
        // Ecoa os dados de volta para a piloto validar
        Serial1.write(header_resposta);
        Serial1.write(cmd);
        Serial1.write(val);
        Serial1.write(chk);
      }
    }
  }
}