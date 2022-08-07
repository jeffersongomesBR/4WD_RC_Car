#include <Arduino.h>

/// ---Pinos--- ///

//Ponte H
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5
#define ENA 10 //PWM~
#define ENB 11 //PWM~

/// ---Comandos--- ///
#define KeyPrefix '/'

//Keys
#define Foward 'w'
#define Backward 's'
#define Left 'a'
#define Right 'd'
#define Stop 'x'
#define GearUp '+'
#define GearDown '-'
#define Light 'l' //Unused

/// ---Definições--- ///
const int dbgRate = 1000; //Mesma coisa do speedUpRate
const bool serialDebug = true; //Enviar variaveis ao monitor serial
const uint8_t deltaV = 8; //Utilizado na interpolação da velocidade (0-255)
const uint8_t speedUpRate = 100; //Tempo de atualização de velocidade (ms)

/// ---Variaveis--- ///
int hBridge[6] = {IN1, IN2, IN3, IN4, ENA, ENB};
int velocity = 0; //-255/255
int targetSpeed = 0; //0-255
int turnDirection = 0; //-1 = Left, 1 = Right
int left = 0; //left speed (0-255)
int right = 0; //right speed (0-255)
bool lightON = false; //Unused
bool stopped = false; //Unused
bool signal = false; //blink led
unsigned long preTimeSpeed = 0; //loop checkpoint
unsigned long preTimeDbg = 0; //loop checkpoint

/// ---Codigo--- ///

//Velocidade (Loop)
void UpdateSpeed() {

  if(velocity < targetSpeed) {

    case 1 :
      velocity = firstGear;
    break;

    case 2 :
      velocity = secondGear;
    break;

    case 3 :
      velocity = thirdGear;
    break;

  //Get Difference between values
  int speedDifference = targetSpeed - velocity;
  speedDifference = abs(speedDifference);

    case 5 :
      velocity = fivethGear;
    break;

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

    digitalWrite(hBridge[0], 0);
    digitalWrite(hBridge[1], 0);
  }
  else if(turnDirection > 0) {

    if(velocity > 0) {

      right = right + right;
      left /= 2;
    }
    else {

    digitalWrite(hBridge[2], 0);
    digitalWrite(hBridge[3], 1);
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
void ReadKey(char key) {

  if(key == GearUp) {
    //TODO: speed variations

    gear++;

    if(debug)
    Serial.println("GearUp to: " + gear);
  }

  if(key == GearDown) {

    gear--;

    if(debug)
    Serial.println("GearDown to: " + gear);
  }

  if(key == Foward) {

    Serial.println("Foward");

    SetDirection(1, 1);
  }

  if (key == Backward) {

    Serial.println("Backward");

    SetDirection(-1, -1);
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
    //TODO: Headlight
  }


    Serial.println("Restarting...");
    delay(3000);
  }

/*
  //Turn speeds
  if(turnDirection < 0) {

    if(velocity > 0) {

      left += left;
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
*/
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

//WARN: INPUT DOEST WORK AT STARTUP!
void setup() {

  //Set hBridge as output
  for(int i = 0; i < 6; i++) {

    pinMode(hBridge[i], OUTPUT);
  }

  Serial.begin(9600);
  Serial.println("Starting...");

  for(int i = 0; i < 5; i++) {

    Activity();
    delay(1000);
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