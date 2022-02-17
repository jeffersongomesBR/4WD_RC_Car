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
#define Foward 'w' //Unused
#define Backward 's' //Unused
#define Left 'a' //Unused
#define Right 'd' //Unused
#define Stop 'x' //Unused
#define Light 'l' //Unused
#define Reset 'r' //Unused

/// ---Definições--- ///
const int deltaV = 2; //Utilizado na interpolação da velocidade
const int speedUpRate = 100; //Frequencia de atualização de velocidade (ms)
const int lowSpeed = 64; //Minimo 0
const int mediumSpeed = 128;
const int highSpeed = 255; //Maximo 255

/// ---Variaveis--- ///
int hBridge[6] = {D1, D2, D3, D4, ENA, ENB};
int velocity = 0; //-255/255
int targetSpeed = 0; //0-255
int turnDirection = 0; //-1 = Left, 1 = Right //Unused
int left = 0; //left speed
int right = 0; //right speed
bool lightON = false; //Unused
bool stopped = false; //Unused
char stream; //Received bluetooth data
unsigned long previusTime = 0; //ms

/// ---Codigo--- ///

//Velocidade (Loop)
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

  //Split velocity to left and right
  left = abs(velocity);
  right = left;
}

//Frente - ré (Loop)
//TODO: Test
void FowardBackward() {

  if(velocity > 0) {

    //Foward
    digitalWrite(D1, HIGH);
    digitalWrite(D2, LOW);
    digitalWrite(D3, HIGH);
    digitalWrite(D4, LOW);
  }
  else if (velocity < 0) {

    //Backward
    digitalWrite(D1, LOW);
    digitalWrite(D2, HIGH);
    digitalWrite(D3, LOW);
    digitalWrite(D4, HIGH);
  }
}

//Esquerda - Direita (Loop)
void LeftRight() {

  //Turn speeds
  if(turnDirection < 0) {

    left += left;
  }
  else if(turnDirection > 0) {

    right += right;
  }

  //Fix values to bellow 255
  if(left > 255) {

    right -= left - 255;
    left = 255;
  }

  if(right > 255) {

    left -= right - 255;
    right = 255;
  }

  //TODO: spin instead of turn if velocity = 0 (stopped)
}

void Break() {

  //TODO: set all speed to 0 instantly, and all directions to LOW
}

//Luzes
void ToggleLight() {

  //TODO: just lightup a led
}

//Reset Board
void(*ResetBoard) (void) = 0;

void Control(char key) {

  if(key == Foward) {

    Serial.print("Foward");
  }

  if(key == Backward) {

    Serial.print("Backward");
  }
  
  if(key == Left) {

    Serial.print("Left");
  }

  if(key == Right) {

    Serial.print("Right");
  }

  if(key == Stop) {

    Serial.print("Stop");
  }

  if(key == Light) {

    Serial.print("Light");
  }

  if(key == Reset) {

    Serial.print("Restarting...");
    delay(3000);
    ResetBoard();
  }
}

void setup() {

  //Set hBridge as output
  for(int i = 0; i < 6; i++) {

    pinMode(hBridge[i], OUTPUT);
  }

  Serial.begin(9600);
}

void loop() {

  //stream only change after new read!
  if(Serial.available() > 0) {

    stream = Serial.read();
    Control(stream);
  }

  unsigned long currentTime = millis();

  if(currentTime - previusTime >= speedUpRate) {
    
    previusTime = currentTime;
    UpdateSpeed();
  }

  if(velocity != 0) {

    FowardBackward();
  }

  if(turnDirection != 0) {

    LeftRight();
  }

  //Apply speed values
  analogWrite(ENA, left);
  analogWrite(ENB, right);

  //TODO: Control by seting target speeds
  //TODO: Missing Loops (check functions!)
}