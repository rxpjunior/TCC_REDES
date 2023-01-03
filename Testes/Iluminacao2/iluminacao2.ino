/*
Teste de leitura de ambiente e, se escuro, acendimento de led
*/

#define pinoAnalogico A0 // Pino analogico do ESP8266
#define Led 5 //Aqui usamos a porta D1 para acendimento do LED
int resolucaoPlaca = 1024 ; // A resolucao da placa, no caso do Esp8266 vai de 0 a 1023 = 1024
int leitura = 0; // A variavel que ira receber a leitura

void setup() {
  Serial.begin(115200); 
  pinMode(Led, OUTPUT);
}

void loop() {
  leitura = analogRead(pinoAnalogico); // fazendo a leitura da porta
  Serial.print("Valor lido: ");
  Serial.println(leitura);
  if (leitura < resolucaoPlaca / 2) { //se leitura form menor que metade da resolucao esta escuro, senao claro
    Serial.print("Ambiente ");
    Serial.println("esta escuro");
    digitalWrite(Led, HIGH);
  } else {
    Serial.print("Ambiente ");
    Serial.println("esta claro");
    digitalWrite(Led, LOW);
  }
  delay(2000); // aguade 2 segundos
}
