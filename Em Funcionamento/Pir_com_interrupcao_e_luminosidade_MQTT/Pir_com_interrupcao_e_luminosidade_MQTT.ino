/*********
 Módulo iluminação
 Utilizando um Sensor de presença e fotoresistor através de interrupções
 envio de dados ao Broker MQTT
 canal de envio exterior/iluminacao (se 0 apagado, se 1 aceso)
*********/
//Inclusão das Bibliotecas
#include <PubSubClient.h>
#include <WiFi.h> 

const char* ssid = "REDETESTE";
const char* password = "REDETESTE";
const char* mqtt_server = "192.168.18.40"; //Broker local rodando em Debian - allow_anonymous true e listener 1883 0.0.0.0 
const int   mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
#define tempoAcendimentoLampada 10 //Acendimento da lampada em milisegundos
#define tempoEnvioDadosSensorPresenca 2 //Envio dos dados MQTT do sensor de presenca em milisegundos

// Definição dos pinos
const int sensorMovimento = 39; //Leitura Sensor Movimento
const int lampada = 26; //Acionamento Rele Lampada
const int sensorLuminosidade = 34; //Fotoresistor

// Temporizador: variáveis auxiliares
unsigned long tempoAtualSensorPresenca = millis();
unsigned long ultimoAcionamento = 0;
boolean tempoInicio = false;

//Demais variaveis
int luminosidade;
long ultimaMensagem = 0;
char mensagem[50];
int valor;

// Funcao que verifica se ocorreu movimento, aciona o rele e inicia o temporizador
// Sera utilizada na interrupcao
void IRAM_ATTR detectaMovimento() {
  //A lampada só será acesa se o ambiente estiver escuro
  if(luminosidade < 1000){
    Serial.println("MOVIMENTO DETECTADO!!!");
    digitalWrite(lampada, HIGH);
    tempoInicio = true;
    ultimoAcionamento = millis();
  }
  else{
    Serial.println("MOVIMENTO DETECTADO MAS AMBIENTE ESTA CLARO!!!");
    tempoInicio = true;
    ultimoAcionamento = millis();
  }
}

void setup() {
  Serial.begin(115200);

  setup_wifi();
  
  client.setServer(mqtt_server, mqtt_port);
 
  // Sensor de movimento configurado para modo INPUT_PULLUP
  pinMode(sensorMovimento, INPUT_PULLUP);
  // Configurando o pino do sensor de movimento como um pino de interrupcao, atribuir a função de interrupção e definir o modo RISING
  attachInterrupt(digitalPinToInterrupt(sensorMovimento), detectaMovimento, RISING);

  // Set LED to LOW
  pinMode(lampada, OUTPUT);
  digitalWrite(lampada, LOW);

}
//Funcao de conexao com a rede
void setup_wifi() {
  delay(10);
  // Inciando a conexao com a rede
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    WiFi.begin(ssid, password);
    
    Serial.print(".");
    delay(5000);
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereco IP: ");
  Serial.println(WiFi.localIP());
}

//Funcao de conexao ao Broker MQTT
void reconnect() {
  // Loop até que seja estabelecida a conexao
  while (!client.connected()) {
    Serial.print("Tentando conexão com o MQTT...");
    // Tentando conectar
    if (client.connect("ESP32Client")) 
    {
      Serial.println("conectado");
      // Uma vez conectado, publique um anúncio...
      client.publish("exterior/iluminacao", "Publicando Mensagens, ESP32 CENTRO DE MEMORIA");
    } else {
      Serial.print("falha, codigo=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      //Aguarda 5 segundos para tentar novamente
      delay(5000);
    }
  }
}

void loop() {
  //BLOCO REFERENTE A LUMINOSIDADE
  //Verifica a intensidade de luminosidade no momento
  luminosidade = analogRead(sensorLuminosidade);
  // Momento atual
  tempoAtualSensorPresenca = millis();
  // Desliga a lampada depois do numero de segundos definidos na variavel tempoAcendimentoLampada
  if(tempoInicio && (tempoAtualSensorPresenca - ultimoAcionamento > (tempoAcendimentoLampada*1000))) {
    Serial.println("SEM DETECÇÃO DE MOVIMENTO...");
    digitalWrite(lampada, LOW);
    tempoInicio = false;
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  //BLOCO REFERENTE AO ENVIO DA MENSAGEM MQTT REFERENTE A LUMINOSIDADE
  long tempoAtualEnvioMqttSensorPresenca = millis();
  if (tempoAtualEnvioMqttSensorPresenca - ultimaMensagem > tempoEnvioDadosSensorPresenca*1000) {
    ultimaMensagem = tempoAtualEnvioMqttSensorPresenca;
    valor = digitalRead(lampada);
    snprintf (mensagem, 75, "Iluminacao: #%ld", valor);
    Serial.print("Publish message:  ");
    Serial.println(mensagem);
    client.publish("exterior/iluminacao", mensagem);
  }
}
