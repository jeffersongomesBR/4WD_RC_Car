#include <Arduino.h>

/// ---Pinos--- ///

//Ponte H
#define IN1 2 //Unused
#define IN2 3 //Unused
#define IN3 4 //Unused
#define IN4 5 //Unused
#define ENA A0 //PWM~ //Unused
#define ENB A1 //PWM~ //Unused

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
int hBridge[6] = {IN1, IN2, IN3, IN4, ENA, ENB};
int velocity = 0; //-255/255
int targetSpeed = 0; //0-255
int turnDirection = 0; //-1 = Left, 1 = Right //Unused
int left = 0; //left speed
int right = 0; //right speed
bool lightON = false; //Unused
bool stopped = false; //Unused
bool signal = false; //blink led
char stream; //Received bluetooth data
unsigned long previusTime = 0; //ms

/// ---Codigo--- ///

//Velocidade (Loop)
void UpdateSpeed() {

  //Activity
  if(signal) {

    digitalWrite(LED_BUILTIN, LOW);
    signal = false;
  }
  else  {

    digitalWrite(LED_BUILTIN, HIGH);
    signal = true;
  }

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
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
  else if (velocity < 0) {

    //Backward
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
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
    //TODO: speed variations

    Serial.println("Foward");
  }

  if(key == Backward) {

    Serial.println("Backward");
  }
  
  if(key == Left) {

    Serial.println("Left");
  }

  if(key == Right) {

    Serial.println("Right");
  }

  if(key == Stop) {

    Serial.println("Stop");
  }

  if(key == Light) {

    Serial.println("Light");
  }

  if(key == Reset) {

    Serial.println("Restarting...");
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