/// ---Pinos--- ///

//Motores Dianteiros
#define FrontEngineL 4
#define FrontEngineR 5
#define FrontEngineV 3 //PWM~

//Motores Traseiros
#define RearEngineL 7
#define RearEngineR 8
#define RearEngineV 6 //PWM~

//Outros
#define FirstPin 3
#define LastPin 7

/// ---Controles--- ///
#define Foward w
#define Backward s
#define Left a
#define Right d
#define Break S
#define HandBreak x
#define Speed W
#define Light l
#define Reset r

/// ---Definições--- ///
const int deltaV = 1;
const int lowSpeed = 64; //Minimo 0
const int mediumSpeed = 128;
const int highSpeed = 255; //Maximo 255

/// ---Variaveis--- ///
int @speed = 0; //0-255
int targetSpeed = 0; //0-3
bool lightON = false;
bool handBreakUP = false;
char key;

/// ---Codigo--- ///

//Frente - ré
void FowardBackward() {
  
  //...
}

//Esquerda - Direita
void LeftRight() {

  //...
}

//Freio Traseiro
void RearBreak() {

  //...
}

//Freio Dianteiro
void FrontBreak() {

  //...
}

//Luzes
void ToggleLight() {

  //...
}

//Velocidade
void UpdateSpeed() {

  //...
}

//Reset
void Reset() {

  //...
}

void setup() {

  for(int i = FirstPin; i < LastPin; i++) {

    pinMode(i, OUTPUT);
  }

  Serial.begin(9600);
}

void loop() {
  
  if(Serial.available() > 0) {

    key = Serial.read();
  }

  if(key == w) {

    FowardBackward();
  }
}
