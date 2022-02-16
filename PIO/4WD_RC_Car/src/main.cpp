#include <Arduino.h>

/// ---Pinos--- ///

//Ponte H
#define D1 3 //Unused
#define D2 4 //Unused
#define D3 5 //Unused
#define D4 6 //Unused
#define ENA 8 //PWM~ //Unused
#define ENB 6 //PWM~ //Unused

//TODO: need tests with bluetooth module
/// ---Controles--- ///
#define Foward w //Unused
#define Backward s //Unused
#define Left a //Unused
#define Right d //Unused
#define Stop x //Unused
#define Light l //Unused
#define Reset r //Unused

/// ---Definições--- ///
const int deltaV = 2; //Utilizado na interpolação da velocidade
const int speedUpRate = 100; //Frequencia de atualização de velocidade (ms)
const int lowSpeed = 64; //Minimo 0
const int mediumSpeed = 128;
const int highSpeed = 255; //Maximo 255

/// ---Variaveis--- ///
int hBridge[6] = {D1, D2, D3, D4, ENA, ENB};
int velocity = 0; //0-255
int targetSpeed = 0; //0-255
int moveDirection = 0; //-1 = reverse, 0 = stopped, 1 = forward //Unused
int turnDirection = 0; //-1 = Left, 0 = stopped, 1 = Right //Unused
bool lightON = false; //Unused
bool stopped = false; //Unused
char stream; //Received bluetooth data
unsigned long previusTime = 0; //ms

/// ---Codigo--- ///

//Velocidade (Loop)
//TODO: Will include turn speed
void UpdateSpeed() {

  if(velocity < targetSpeed) {

    velocity += deltaV;
  }
  else if (velocity > targetSpeed) {

    velocity -= deltaV;
  }

  //Get Difference between values
  int speedDifference = targetSpeed - velocity;
  speedDifference = abs(speedDifference);

  //Round speed
  if(speedDifference < deltaV) {

    velocity = targetSpeed;
  }
}

//Frente - ré (Loop)
//TODO: Finish this
void FowardBackward(int d) {

  //Foward
  digitalWrite(D1, HIGH);
  digitalWrite(D2, LOW);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, LOW);

  //Backward
  digitalWrite(D1, LOW);
  digitalWrite(D2, HIGH);
  digitalWrite(D3, LOW);
  digitalWrite(D4, HIGH);
}

//Esquerda - Direita (Loop)
void LeftRight(int d) {

  //TODO: its just multiply the speed of one side...
  //spin instead of turn if velocity = 0 (stopped)
}

void Break() {

  //TODO: set all speed to 0 instantly, and all directions to LOW
}

//Luzes
void ToggleLight() {

  //TODO: just lightup a led
}

//Reset
void Reset() {

  //TODO: reset arduino
}

void setup() {

  //Set hBridge as output
  for(int i = 0; i < 6; i++) {

    pinMode(hBridge[i], OUTPUT);
  }

  Serial.begin(9600);
}

void loop() {

  //TODO: does buffer clear after read?
  if(Serial.available() > 0) {

    stream = Serial.read();
  }

  unsigned long currentTime = millis();

  if(currentTime - previusTime >= speedUpRate) {
    
    previusTime = currentTime;
    UpdateSpeed();
  }

  //TODO: Control by seting target speeds
  //TODO: Missing Loops (check functions!)
}