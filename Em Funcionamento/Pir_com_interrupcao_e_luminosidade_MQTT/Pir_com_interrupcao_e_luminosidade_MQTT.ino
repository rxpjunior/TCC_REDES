/*********
 Módulo iluminação
 Utilizando um Sensor de presença e fotoresistor através de interrupções
 envio de dados ao Broker MQTT
 Topicos
 1 - canal de envio automatico, feito pelo sensor de monimento: exterior/iluminacao - (se 0 apagado, se 1 aceso)
 2 - canal de envio manual da situacao do pino de acendimento da lampada: exterior/ligalampada (se {0} apagar a lampada, se {1} acender a lampada)

 Topicos do projeto - No Debian
 Manual Publicar Acendimento da lampada manual - mosquitto_pub -t exterior/ligalampada -m {"1"}
 Manual Publicar Desligamento da lampada manual - mosquitto_pub -t exterior/ligalampada -m {"0"}
 Manual Assinar situacao do acionamento da lampada manual - mosquitto_sub -h 192.168.18.40 -t exterior/ligalampada
 Esp32 Assinar a situacao da lampada no momento - mosquitto_sub -h 192.168.18.40 -t exterior/iluminacao
 
*********/
//Inclusão das Bibliotecas
#include <PubSubClient.h>
#include <WiFi.h> 

const char* ssid = "REDETESTE";
const char* password = "Tatanka*2000";
const char* mqtt_server = "192.168.18.40"; //Broker local rodando em Debian - allow_anonymous true e listener 1883 0.0.0.0 
const int   mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
#define tempoAcendimentoLampada 10 //Tempo de acendimento da lampada em segundos
#define tempoEnvioDadosSensorPresenca 2 //Intervalo de envio dos dados MQTT do sensor de presenca em segundos

// Definição dos pinos
const int sensorMovimento = 39; //Pino que faz leitura Sensor Movimento
const int lampada = 26; //Pino de acionamento Rele Lampada
const int sensorLuminosidade = 34; //Pino que faz leitura Fotoresistor (luminosidade)

// Temporizador da Iluminacao: variáveis auxiliares
unsigned long momentoAtual = millis();
unsigned long momentoUltimoAcionamentoLampada = 0;
boolean interrupcaoLampadaAcionada = false; // Controla se a interrupcao esta ou não acionada
boolean lampadaAcionadaManualmente = false; //Verifica se a lampada foi acionada manualmente ou nao

//Demais variaveis
int luminosidade;
long ultimaMensagemMqqtLampada = 0;
char mensagem[50];
int valor;

// Funcao que verifica se ocorreu movimento, aciona o rele e inicia o temporizador
// Sera utilizada na interrupcao
void IRAM_ATTR detectaMovimento() {
  //A lampada só será acesa se o ambiente estiver escuro
  if(luminosidade < 1000){
    Serial.println("MOVIMENTO DETECTADO!!!");
    digitalWrite(lampada, HIGH);
    interrupcaoLampadaAcionada = true;
    momentoUltimoAcionamentoLampada = millis();
  }
  else{
    Serial.println("MOVIMENTO DETECTADO MAS AMBIENTE ESTA CLARO!!!");
    interrupcaoLampadaAcionada = true;
    momentoUltimoAcionamentoLampada = millis();
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

  //Inicializacao da funcao que fara o subscribe do topico acendimento manual
  client.setCallback(callback);
}

//Funcao de conexao com a rede
void setup_wifi() {
  delay(10);
  // Inciando a conexao com a rede
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, password);
    Serial.print(".");
    delay(5000);
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereco IP: ");
  Serial.println(WiFi.localIP());
}

//Funcao de recepcao de topico no ESP para ligar ou desligar a lampada de forma manual
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Mensagem recebido no topico: ");
  Serial.print(topic);
  Serial.print(". Mensagem: ");
  String ligarLampada;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    ligarLampada += (char)message[i];
  }
  Serial.println();
 
  // Se uma mensagem for recebida no topico exterior/ligalampada, verifica-se se ela é {1} ou {0}
  // Altera a saída de acordo com a mensagem
  if (String(topic) == "exterior/ligalampada") {
    Serial.print("Mudando saida para ");
    if(ligarLampada == "{1}"){
      Serial.println("ligado");
      digitalWrite(lampada, HIGH);
      lampadaAcionadaManualmente = true;
    }
    else if(ligarLampada == "{0}"){
      Serial.println("desligado");
      digitalWrite(lampada, LOW);
      lampadaAcionadaManualmente = false;
    }
  }
}

//Funcao de conexao ao Broker MQTT
void reconnect() {
  // Loop até que seja estabelecida a conexao
  while (!client.connected()) {
    Serial.print("Tentando conexão com o MQTT...");
    // Tentando conectar
    if (client.connect("ESP32Client")){
      Serial.println("conectado");
      // Uma vez conectado, publique um anúncio...
      client.publish("exterior/iluminacao", "Publicando Mensagens, ESP32 CENTRO DE MEMORIA");
      // Inscricao no topico de acendimento manual da lampada
      client.subscribe("exterior/ligalampada");
    } 
    else {
      Serial.print("falha, codigo=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      //Aguarda 5 segundos para tentar novamente
      delay(5000);
    }
  }
}

void loop() {
  ////////////BLOCO REFERENTE A LUMINOSIDADE////////////
  //Verifica a intensidade de luminosidade no momento
  luminosidade = analogRead(sensorLuminosidade);
  // Momento atual
  momentoAtual = millis();
  // Desliga a lampada depois do numero de segundos definidos na variavel tempoAcendimentoLampada
  if(interrupcaoLampadaAcionada && (momentoAtual - momentoUltimoAcionamentoLampada > (tempoAcendimentoLampada*1000))) {
    Serial.println("SEM DETECÇÃO DE MOVIMENTO...");
    interrupcaoLampadaAcionada = false;
    //Apagara a lampada apenas e ela nao foi acionada manualmente
    if(!lampadaAcionadaManualmente){
      digitalWrite(lampada, LOW);
    }
 }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
    
  ////////////BLOCO REFERENTE AO ENVIO DA MENSAGEM MQTT REFERENTE A LUMINOSIDADE////////////
  //Verifica se ja se passou o intervalo de envio de mensagens e se sim evia
  if (momentoAtual - ultimaMensagemMqqtLampada > tempoEnvioDadosSensorPresenca*1000) {
    ultimaMensagemMqqtLampada = momentoAtual;
    valor = digitalRead(lampada); // Le o valor do pino de acionamento do rele, se 0 desligado, se 1 ligado
  String aux;
  if (valor == 1){
    aux = "Ligado";   
  }
  else{
    aux = "Desligado";
  }
    snprintf (mensagem, 75, "Iluminacao: %s", aux);
    Serial.print("Publicando mensagem:  ");
    Serial.println(mensagem);
    client.publish("exterior/iluminacao", mensagem);
  }
  
}
