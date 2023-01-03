/*
Teste de leitura de ambiente e, se escuro e ccom movimento causa acendimento de led
*/

#define pinoAnalogico A0 // Pino analogico do ESP8266
#define Led 5 //Aqui usamos a porta D1 para acendimento do LED
#define sensorPresenca 4 // Aqui usamos a porta D2 para detectar situacao de leitura do sensor de movimento
int resolucaoPlaca = 1024 ; // A resolucao da placa, no caso do Esp8266 vai de 0 a 1023 = 1024
int leituraLuminosidade = 0; // A variavel que ira receber a leitura da luminosidade
int leituraPresenca = LOW; // A variavel que ira receber a leitura do presenca

void setup() {
  Serial.begin(115200); 
  pinMode(Led, OUTPUT);
  pinMode(sensorPresenca, INPUT);
}

void loop() {
  //Leitura das entradas
  leituraLuminosidade = analogRead(pinoAnalogico); // fazendo a leitura da luminosidade
  Serial.print("Valor de Luminosidade lido: ");
  Serial.println(leituraLuminosidade);
  
  leituraPresenca = digitalRead(sensorPresenca); // fazendo a leitura do sensor de presenca
  Serial.print("Verificacao de presenca(0 = Nao / 1 = Sim): ");
  Serial.println(leituraPresenca);
  
  
  if (leituraLuminosidade < resolucaoPlaca / 2) { //se leitura form menor que metade da resolucao esta escuro, senao claro
    Serial.print("Ambiente ");
    Serial.println("esta escuro");
    if (leituraPresenca == HIGH){ // O led somente acendera se estiver escuro e houver preresenca detectada
      digitalWrite(Led, HIGH);      
    }
    else{
      digitalWrite(Led, LOW);
    }
    
  } else {
    Serial.print("Ambiente ");
    Serial.println("esta claro");
    digitalWrite(Led, LOW);
  }
  delay(1000); // aguade 2 segundos
}
