/*********
 Utilizando um Sensor de presença através de interrupções
*********/

#define tempoAcendimentoLampada 10 //Acendimento da lampada em milisegundos

// Definição dos pinos
const int sensorMovimento = 39; //Leitura Sensor Movimento
const int lampada = 26; //Acionamento Rele Lampada
const int sensorLuminosidade = 34; //Fotoresistor

// Temporizador: variáveis auxiliares
unsigned long tempoAtual = millis();
unsigned long ultimoAcionamento = 0;
boolean tempoInicio = false;

// Funcao que verifica se ocorreu movimento, aciona o rele e inicia o temporizador
// Sera utilizada na interrupcao
void IRAM_ATTR detectaMovimento() {
  Serial.println("MOVIMENTO DETECTADO!!!");
  digitalWrite(lampada, HIGH);
  tempoInicio = true;
  ultimoAcionamento = millis();
}

void setup() {
  Serial.begin(115200);
   
  // Sensor de movimento configurado para modo INPUT_PULLUP
  pinMode(sensorMovimento, INPUT_PULLUP);
  // Configurando o pino do sensor de movimento como um pino de interrupcao, atribuir a função de interrupção e definir o modo RISING
  attachInterrupt(digitalPinToInterrupt(sensorMovimento), detectaMovimento, RISING);

  // Set LED to LOW
  pinMode(lampada, OUTPUT);
  digitalWrite(lampada, LOW);
}

void loop() {
  
   // Momento atual
  tempoAtual = millis();
  // Desliga a lampada depois do numero de segundos definidos na variavel tempoAcendimentoLampada
  if(tempoInicio && (tempoAtual - ultimoAcionamento > (tempoAcendimentoLampada*1000))) {
    Serial.println("SEM DETECÇÃO DE MOVIMENTO...");
    digitalWrite(lampada, LOW);
    tempoInicio = false;
  }
}
