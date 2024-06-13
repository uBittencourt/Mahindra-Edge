// BIBLIOTECA PARA DISPLAY I2C
#include <LiquidCrystal_I2C.h>

// DEFININDO PINOS DOS SENSORES DE DISTÂNCIA
#define trigLeft 12
#define echoLeft 11

#define trigFront 10
#define echoFront 9

#define trigRight 8
#define echoRight 7

// DEFININDO OS PINOS DOS LEDS
#define ledLeft 6
#define ledFront 5
#define ledRight 4

// DECLARANDO O PROTÓTIPO DAS FUNÇÕES
float DistanceCm(int trig, int echo);
void setPin(int trig, int echo);

// INICIALIZANDO OBJETO PARA COMUNICAÇÃO COM DISPLAY
LiquidCrystal_I2C lcd(32, 16, 2);

// FAZENDO DECLARAÇÃO DOS CARACTERES DO DISPLAY
byte car1[] = { B00000, B00000, B00001, B00111, B01110, B01100, B00000, B00000 };
byte car2[] = { B00011, B01111, B11111, B11000, B00000, B11100, B11100, B11111 };
byte car3[] = { B11111, B11111, B11111, B01110, B01110, B01110, B11111, B11111 };
byte car4[] = { B11000, B11110, B11111, B00011, B00000, B00111, B00111, B11111 };
byte car5[] = { B00000, B00000, B10000, B11100, B01110, B00110, B00000, B00000 };
byte wind1[] = { B00000, B00010, B00010, B00010, B01000, B01001, B01001, B00001 };
byte wind2[] = { B00000, B01000, B01000, B01000, B00010, B10010, B10010, B10000 };

void setup()
{ 
  // CHAMANDO FUNÇÃO PARA SETAR OS PINOS DOS SENSORES
  setPin(trigLeft, echoLeft);
  setPin(trigFront, echoFront);
  setPin(trigRight, echoRight);
  
  // DEFININDO OS PINOS LEDS COMO SAÍDA
  pinMode(ledLeft, OUTPUT);
  pinMode(ledFront, OUTPUT);
  pinMode(ledRight, OUTPUT);
  
  // INICIALIZANDO DISPLAY E LIGANDO O BACKLIGHT
  lcd.init();
  lcd.backlight();
  lcd.display();
  
  // CRIANDO OS CARACTERES DECLARADOS ANTERIORMENTE
  lcd.createChar(1, car1);
  lcd.createChar(2, car2);
  lcd.createChar(3, car3);
  lcd.createChar(4, car4);
  lcd.createChar(5, car5);
  lcd.createChar(6, wind1);
  lcd.createChar(7, wind2);
  
  // PRINTANDO MENSAGEM INICIAL
  lcd.setCursor(3,0);
  lcd.print("Formula E");
  lcd.setCursor(0,1);
  lcd.print("Cade o Obstaculo");
  
  delay(1000);
  
  // PRINTANDO CARRO DA FÓRMULA E
  lcd.clear();
  lcd.setCursor(5,1);
  lcd.write(1);
  lcd.write(2);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
}

void loop()
{ 
  // PRINTANDO VENTO NAS LATERAIS DO CARRO
  lcd.setCursor(4,1);
  lcd.write(6);
  lcd.setCursor(10,1);
  lcd.write(7);
  
  // CRIANDO VARIÁVEIS PARA AS DISTÂNCIAS
  float distLeft, distFront, distRight;
  
  // ARMAZENANDO DISTÂNCIAS NAS VARIÁVEIS
  distLeft = DistanceCm(trigLeft, echoLeft);
  distFront = DistanceCm(trigFront, echoFront);
  distRight = DistanceCm(trigRight, echoRight);
  
  // CONDICIONAL PARA VERIFICAR OBSTÁCULO A ESQUERDA
  if(distLeft < 50){
    // APRESENTANDO OBSTACULO NO DISPLAY E LIGANDO LED
    lcd.setCursor(2,1);
    lcd.print("!");
    digitalWrite(ledLeft, HIGH);
  } else{
    // APAGANDO O OBSTÁCULO DO DISPLAY
  	lcd.setCursor(2,1);
    lcd.print(" ");
    digitalWrite(ledLeft, LOW);
  }
  
  // CONDICIONAL PARA VERIFICAR OBSTÁCULO A FRENTE
  if(distFront < 50){
    // APRESENTANDO OBSTACULO NO DISPLAY E LIGANDO LED
    lcd.setCursor(7,0);
    lcd.print("!");
    digitalWrite(ledFront, HIGH);
  } else{
    // APAGANDO OBSTÁCULO DO DISPLAY
  	lcd.setCursor(7,0);
    lcd.print(" ");
    digitalWrite(ledFront, LOW);
  }
  
  // CONDICIONAL PARA VERIFICAR OBSTÁCULO A DIREITA
  if(distRight < 50){
    // APRESENTANDO OBSTACULO NO DISPLAY E LIGANDO LED
    lcd.setCursor(12,1);
    lcd.print("!");
    digitalWrite(ledRight, HIGH);
  } else{
    // APAGANDO O OBSTÁCULO DO DISPLAY
  	lcd.setCursor(12,1);
    lcd.print(" ");
    digitalWrite(ledRight, LOW);
  }
  
  // APAGANDO VENTO NAS LATERAIS DO CARRO
  delay(1000);
  lcd.setCursor(4,1);
  lcd.print(" ");
  lcd.setCursor(10,1);
  lcd.print(" ");
}

// CRIANDO FUNÇÃO PARA CALCULAR A DISTÂNCIA DE UM SENSOR ESPECÍFICO 
float DistanceCm(int trig, int echo){
  // CRIANDO VARIÁVEL PARA O TEMPO ENTRE A DISTÂNCIA COM O OBSTÁCULO
  long int temp;
  
  // INICIALIZANDO O SENSOR
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  // ARMAZENANDO O TEMPO ENTRE A DISTÂNCIA NA VARIÁVEL
  temp = pulseIn(echo, HIGH);
  
  // RETORNANDO DISTÂNCIA EM CENTÍMETROS, EM FUNÇÃO DA VELOCIDADE DO SOM, E PELO TEMPO EM SEGUNDOS
  return(100 * 344.0 * (temp / 1E6 / 2));
}

// CRIANDO FUNÇÃO PARA DEFINIR A FUNÇÃO DOS PINOS DO SENSOR
void setPin(int trig, int echo){
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  digitalWrite(trig, LOW);
  delayMicroseconds(10);
}
           