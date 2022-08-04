#include <Arduino.h>

/// ---Pinos--- ///

//Ponte H
#define IN1 2 //Unused
#define IN2 3 //Unused
#define IN3 4 //Unused
#define IN4 5 //Unused
#define ENA 10 //PWM~ //Unused
#define ENB 11 //PWM~ //Unused

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
const int deltaV = 8; //Utilizado na interpolação da velocidade
const int speedUpRate = 100; //Tempo de atualização de velocidade (ms)
const int lowSpeed = 64; //Minimo 0
const int mediumSpeed = 128;
const int highSpeed = 255; //Maximo 255
const int dbgRate = 1000; //Mesma coisa do speedUpRate
const bool serialDebug = true;

/// ---Variaveis--- ///
int hBridge[6] = {IN1, IN2, IN3, IN4, ENA, ENB};
int velocity = 0; //-255/255
int targetSpeed = 0; //0-255
int turnDirection = 0; //-1 = Left, 1 = Right //Unused
int left = 0; //left speed (0-255)
int right = 0; //right speed (0-255)
bool lightON = false; //Unused
bool stopped = false; //Unused
bool signal = false; //blink led
char stream; //Received bluetooth data
unsigned long preTimeSpeed = 0; //loop checkpoint
unsigned long preTimeDbg = 0; //loop checkpoint

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

    if(velocity > 0) {

      left = left + left;
      right /= 2;
    }
    else {

      left = lowSpeed;
      right = lowSpeed * -1;
    }
  }
  else if(turnDirection > 0) {

    if(velocity > 0) {

      right = right + right;
      left /= 2;
    }
    else {

      right = lowSpeed;
      left = lowSpeed * -1;
    }
  }

  //Fix values to bellow 255
  if(left > 255) {

    left = 255;
  }
  else if(left < -255) {

    left = -255;
  }

  if(right > 255) {

    right = 255;
  }
  else if(right <-255) {

    right = -255;
  }
}

void Break() {

  //TODO: set all speed to 0 instantly, and all directions to LOW
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
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
    
    switch(targetSpeed) {

      case highSpeed * -1:
        targetSpeed = mediumSpeed * -1;
        break;
      case mediumSpeed * -1:
        targetSpeed = lowSpeed * -1;
        break;
      case lowSpeed * -1:
        targetSpeed = 0;
        break;
      case 0:
        targetSpeed = lowSpeed;
        break;
      case lowSpeed:
        targetSpeed = mediumSpeed;
        break;
      case mediumSpeed:
        targetSpeed = highSpeed;
        break;
    }
  }

  if(key == Backward) {

    Serial.println("Backward");
    
    switch(targetSpeed) {

      case highSpeed:
        targetSpeed = mediumSpeed;
        break;
      case mediumSpeed:
        targetSpeed = lowSpeed;
        break;
      case lowSpeed:
        targetSpeed = 0;
        break;
      case 0:
        targetSpeed = lowSpeed * -1;
        break;
      case lowSpeed * -1:
        targetSpeed = mediumSpeed * -1;
        break;
      case mediumSpeed * -1:
        targetSpeed = highSpeed * -1;
        break;
    }
  }
  
  if(key == Left) {

    Serial.println("Left");

    if(turnDirection >= 0)
      turnDirection--;
  }

  if(key == Right) {

    Serial.println("Right");

    if(turnDirection <= 0)
      turnDirection++;
  }

  if(key == Stop) {

    Serial.println("Stop");

    turnDirection = 0;
    targetSpeed = 0;
    Break();
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

void Activity() {

  if(signal) {

    digitalWrite(LED_BUILTIN, LOW);
    signal = false;
  }
  else  {

    digitalWrite(LED_BUILTIN, HIGH);
    signal = true;
  }
}

void Dbg() {

  Activity();

  //Debug var
  Serial.println("/// Debug ///");
  Serial.println((String)"velocity=" + velocity);
  Serial.println((String)"targetSpeed=" + targetSpeed);
  Serial.println((String)"left=" + left);
  Serial.println((String)"right=" + right);
}

void setup() {

  //Set hBridge as output
  for(int i = 0; i < 6; i++) {

    pinMode(hBridge[i], OUTPUT);
  }

  Serial.begin(9600);
  Serial.println("Starting...");

  for(int i = 0; i < 5; i++) {

    Activity();
    delay(1);
  }
}

void loop() {

  //stream only change after new read!
  if(Serial.available() > 0) {

    stream = Serial.read();
    Control(stream);
  }

  //Chronometer
  unsigned long currentTime = millis();

  if(currentTime - preTimeSpeed >= speedUpRate) {
    
    preTimeSpeed = currentTime;
    UpdateSpeed();
  }

  if(serialDebug) {
    
    if(currentTime - preTimeDbg >= dbgRate) {

      preTimeDbg = currentTime;
      Dbg();
    }
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