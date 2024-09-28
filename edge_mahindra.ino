#include <WiFi.h>
#include <PubSubClient.h>

// CONFIGURAÇÕES - VARIÁVEIS EDITÁVEIS

// NOME DA REDE WIFI
const char* default_SSID = "Wokwi-GUEST"; 

// SENHA DA REDE WIFI
const char* default_PASSWORD = "";

// IP DO BROKER MQTT
const char* default_BROKER_MQTT = "54.235.126.90"; 

// PORTA DO BROKER MQTT
const int default_BROKER_PORT = 1883; 

// TÓPICO MQTT DE ESCUTA
const char* default_TOPICO_SUBSCRIBE = "/TEF/obstacle001/cmd"; 

// TÓPICOS MQTT DE ENVIO DE INFORMAÇÕES PARA BROKER
const char* default_TOPICO_PUBLISH_LEFT_DISTANCE = "/TEF/obstacle001/attrs/ld";
const char* default_TOPICO_PUBLISH_LEFT_OBSTACLE = "/TEF/obstacle001/attrs/lo";

const char* default_TOPICO_PUBLISH_FRONT_DISTANCE = "/TEF/obstacle001/attrs/fd";
const char* default_TOPICO_PUBLISH_FRONT_OBSTACLE = "/TEF/obstacle001/attrs/fo";

const char* default_TOPICO_PUBLISH_RIGHT_DISTANCE = "/TEF/obstacle001/attrs/rd";
const char* default_TOPICO_PUBLISH_RIGHT_OBSTACLE = "/TEF/obstacle001/attrs/ro";

// ID MQTT
const char* default_ID_MQTT = "fiware_001"; 

// DECLARAÇÃO DA VARIÁVEL PARA O PREFIXO DO TÓPICO
const char* topicPrefix = "obstacle001";

// VARIÁVEIS PARA CONFIGURAÇÕES EDITAVÉIS
char* SSID = const_cast<char*>(default_SSID);
char* PASSWORD = const_cast<char*>(default_PASSWORD);
char* BROKER_MQTT = const_cast<char*>(default_BROKER_MQTT);
int BROKER_PORT = default_BROKER_PORT;

char* TOPICO_SUBSCRIBE = const_cast<char*>(default_TOPICO_SUBSCRIBE);

char* TOPICO_PUBLISH_LEFT_DISTANCE = const_cast<char*>(default_TOPICO_PUBLISH_LEFT_DISTANCE);
char* TOPICO_PUBLISH_LEFT_OBSTACLE = const_cast<char*>(default_TOPICO_PUBLISH_LEFT_OBSTACLE);

char* TOPICO_PUBLISH_FRONT_DISTANCE = const_cast<char*>(default_TOPICO_PUBLISH_FRONT_DISTANCE);
char* TOPICO_PUBLISH_FRONT_OBSTACLE = const_cast<char*>(default_TOPICO_PUBLISH_FRONT_OBSTACLE);

char* TOPICO_PUBLISH_RIGHT_DISTANCE = const_cast<char*>(default_TOPICO_PUBLISH_RIGHT_DISTANCE);
char* TOPICO_PUBLISH_RIGHT_OBSTACLE = const_cast<char*>(default_TOPICO_PUBLISH_RIGHT_OBSTACLE);

char* ID_MQTT = const_cast<char*>(default_ID_MQTT);

// CRIA UM CLIENTE WIFI PARA A CONEXÃO
WiFiClient espClient;

// CRIA UM CLIENTE MQTT UTILIZANDO O CLIENTE WIFI
PubSubClient MQTT(espClient);

// FUNÇÃO INICIAR PARA CONECTAR WIFI
void initWiFi() {
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    reconectWiFi();
}

// FUNÇÃO PARA INICIAR O MQTT
void initMQTT() {
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);
    MQTT.setCallback(mqtt_callback);
}

// DEFININDO PINOS DOS SENSORES DE DISTÂNCIA
#define trigLeft 23    
#define echoLeft 22

#define trigFront 21
#define echoFront 19

#define trigRight 18
#define echoRight 5

// DECLARANDO O PROTÓTIPO DAS FUNÇÕES
float DistanceCm(int trig, int echo);
void setPin(int trig, int echo);

void setup() {
  initWiFi();
  initMQTT();

  // INICIANDO COMUNICAÇÃO COM MONITOR SERIAL
  Serial.begin(115200);

  delay(5000);

  // CHAMANDO FUNÇÃO PARA SETAR PINOS DOS SENSORES
  setPin(trigLeft, echoLeft);
  setPin(trigFront, echoFront);
  setPin(trigRight, echoRight);
}

void loop() {
  VerificaConexoesWiFIEMQTT();

  // CRIANDO VARIÁVEIS PARA AS DISTÂNCIAS
  float distLeft, distFront, distRight;

  // CRIANDO VARIÁVEIS PARA VERIFICAR SE TEM OBSTÁCULOS
  bool obsLeft, obsFront, obsRight;

  // ARMAZENANDO DISTÂNCIA NAS VARIÁVEIS
  distLeft = DistanceCm(trigLeft, echoLeft);
  distFront = DistanceCm(trigFront, echoFront);
  distRight = DistanceCm(trigRight, echoRight);

  // CONDICIONAL PARA VERIFICAR OBSTÁCULO A ESQUERDA
  if(distLeft < 50){
    obsLeft = true;
  } else {
    obsLeft = false;
  }

  // CONDICIONAL PARA VERIFICAR OBSTÁCULO A FRENTE
  if(distFront < 50){
    obsFront = true;
  } else {
    obsFront = false;
  }

  // CONDICIONAL PARA VERIFICAR OBSTÁCULO A DIREITA
  if(distRight < 50){
    obsRight = true;
  } else {
    obsRight = false;
  }

  // ESTRUTURA CONDICIONAL PARA APRESENTAR ONDE TEM OBSTÁCULOS
  if(obsLeft && obsFront && obsRight){
    Serial.println("Obstáculo: ESQUERDA | FRENTE | DIREITA ");
  } else if(obsLeft && obsFront){
    Serial.println("Obstáculo: ESQUERDA | FRENTE");
  } else if(obsLeft && obsRight){
    Serial.println("Obstáculo: ESQUERDA | DIREITA");
  } else if(obsFront && obsRight){
    Serial.println("Obstáculo: FRENTE | DIREITA");
  } else if(obsLeft){
    Serial.println("Obstáculo: ESQUERDA");
  } else if(obsFront){
    Serial.println("Obstáculo: FRENTE");
  } else if(obsRight){
    Serial.println("Obstáculo: DIREITA");
  } else{
    Serial.println("SEM OBSTÁCULOS...");
  }

  // TRANSFORMANDO VALORES EM STRING E PUBLICANDO NO TÓPICO CORRESPONDENTE
  String leftDistance = String(distLeft);
  String leftObstacle = String(obsLeft);
  MQTT.publish(TOPICO_PUBLISH_LEFT_DISTANCE, leftDistance.c_str());
  MQTT.publish(TOPICO_PUBLISH_LEFT_OBSTACLE, leftObstacle.c_str());

  String frontDistance = String(distFront);
  String frontObstacle = String(obsFront);
  MQTT.publish(TOPICO_PUBLISH_FRONT_DISTANCE, frontDistance.c_str());
  MQTT.publish(TOPICO_PUBLISH_FRONT_OBSTACLE, frontObstacle.c_str());

  String rightDistance = String(distRight);
  String rightObstacle = String(obsRight);
  MQTT.publish(TOPICO_PUBLISH_RIGHT_DISTANCE, rightDistance.c_str());
  MQTT.publish(TOPICO_PUBLISH_RIGHT_OBSTACLE, rightObstacle.c_str());

  // MANTÉM CONEXÃO MQTT E PROCESSA DADOS
  MQTT.loop();
}

void reconectWiFi() {
  // VERIFICA SE JÁ ESTÁ CONECTADO AO WIFI
  if (WiFi.status() == WL_CONNECTED)
      return;
  
  // INICIA A CONEXÃO COM A REDE WIFI
  WiFi.begin(SSID, PASSWORD);

  // AGUARDA ATÉ QUE A CONEXÃO SEJA ESTABELECIDA
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
  }

  // IMPRIME INFORMAÇÕES E MENSAGEM DE SUCESSO
  Serial.println();
  Serial.println("Conectado com sucesso na rede ");
  Serial.print(SSID);
  Serial.println("IP obtido: ");
  Serial.println(WiFi.localIP());
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String msg;

  // CONSTRÓI A MENSAGEM A PARTIR DO PAYLOAD RECEBIDO
  for (int i = 0; i < length; i++) {
      char c = (char)payload[i];
      msg += c;
  }

  // EXIBE A MENSAGEM RECEBIDA NO CONSOLE
  Serial.print("- Mensagem recebida: ");
  Serial.println(msg);
}

void VerificaConexoesWiFIEMQTT() {
  // VERIFICA SE A CONEXÃO MQTT ESTÁ ATIVA
  if (!MQTT.connected())
      reconnectMQTT();
  
  // TENTA RECONECTAR AO WIFI
  reconectWiFi();
}

void reconnectMQTT() {
  // LOOP ATÉ CONSEGUIR CONECTAR AO BROKER MQTT
  while (!MQTT.connected()) {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);

    // TENTA CONECTAR AO BROKER MQTT
    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      MQTT.subscribe(TOPICO_SUBSCRIBE);
    } else {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Haverá nova tentativa de conexão em 2s");
      delay(2000);
    }
  }
}

// CRIANDO FUNÇÃO PARA CALCULAR DISTÂNCIA DE UM SENSOR
float DistanceCm(int trig, int echo){
  // CRIANDO VARIÁVEL PARA O TEMPO ENTRE, A DISTÂNCIA E O OBSTÁCULO
  long int temp;

  // INICIALIZANDO O SENSOR
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  // ARMAZENANDO O TEMPO ENTRE A DISTÂNCIA E O OBSTÁCULO
  temp = pulseIn(echo, HIGH);

  // RETORNANDO DISTÂNCIA EM CENTÍMETROS, EM FUNÇÃO DA VELOCIDADE DO SOM, E PELO TEMPO EM SEGUNDOS
  return(100 * 344.0 * (temp / 1E6 / 2));  
}

// CRIANDO FUNÇÃO PARA DEFINIR A FUNÇÃO DOS PINO DO SENSOR
void setPin(int trig, int echo){
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  digitalWrite(trig, LOW);
  delayMicroseconds(10);
}
