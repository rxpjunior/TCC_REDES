/*
Primeiro teste de leitura do LDR
*/

#define pinoAnalogico A0 // Pino analogico do ESP8266
int resolucaoPlaca = 1024 ; // A resolucao da placa, no caso do Esp8266 vai de 0 a 1023 = 1024
int leitura = 0; // A variavel que ira receber a leitura

void setup() {
  Serial.begin(115200); 
}

void loop() {
  leitura = analogRead(pinoAnalogico); // fazendo a leitura da porta
  Serial.print("Valor lido: ");
  Serial.println(leitura);
  if (leitura < resolucaoPlaca / 2) { //se leitura form menor que metade da resolucao esta escuro, senao claro
    Serial.print("Ambiente ");
    Serial.println("esta escuro");
  } else {
    Serial.print("Ambiente ");
    Serial.println("esta claro");
  }
  delay(2000); // aguade 1 segundo
}
