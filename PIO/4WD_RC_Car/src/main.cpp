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
const bool serialDebug = true; //Enviar variaveis ao monitor serial (REMOVER EM RELEASE!)
const bool debug = true; //Enviar eventos de debug ao monitor serial (REMOVER EM RELEASE!)
const uint8_t deltaV = 8; //Utilizado na interpolação da velocidade (0-255)
const uint8_t speedUpRate = 100; //Tempo de atualização de velocidade (ms)
const uint16_t dbgRate = 1000; //Mesma coisa do speedUpRate

//Velocidades
const uint8_t firstGear = 43; //Minimo 0
const uint8_t secondGear = 86;
const uint8_t thirdGear = 129;
const uint8_t fourthGear = 172;
const uint8_t fivethGear = 215;
const uint8_t sixGear = 255; //Maximo 255

/// ---Variaveis--- ///
bool lightON = false; //Unused
bool stopped = false; //Unused
bool signal = false; //blink led
uint8_t hBridge[6] = {IN1, IN2, IN3, IN4, ENA, ENB};
uint8_t gear = 0; //0-6
uint8_t turnDirection = 0; //-1 = Left, 1 = Right
uint8_t left = 0; //left speed (0-255)
uint8_t right = 0; //right speed (0-255)
int16_t velocity = 0; //-255/255
u32 preTimeSpeed = 0; //loop checkpoint
u32 preTimeDbg = 0; //loop checkpoint

/// ---Codigo--- ///

//Read serial buffer and clear it
void ReadBuffer() {

  int buffer[64];
  int lenght = Serial.available();
  bool hasKeyPrefix = Serial.find(KeyPrefix);

  for(int i = 0; i < lenght; i++) {

    buffer[i] = Serial.read();
  }

  if(hasKeyPrefix) {

    ReadKey(buffer[1]);
  }
}

void Activity() {

  if(signal) {

    digitalWrite(LED_BUILTIN, 0);
    signal = false;
  }
  else  {

    digitalWrite(LED_BUILTIN, 1);
    signal = true;
  }
}

//Send vars to serial monitor
void Dbg() {

  Activity();

  //Debug var
  Serial.println("/// Debug ///");
  Serial.println((String)"velocity=" + velocity);
  Serial.println((String)"left=" + left);
  Serial.println((String)"right=" + right);
}

void UpdateSpeed() {

  switch (gear) {

    case 0 :
      velocity = 0;
    break;

    case 1 :
      velocity = firstGear;
    break;

    case 2 :
      velocity = secondGear;
    break;

    case 3 :
      velocity = thirdGear;
    break;

    case 4 :
      velocity = fourthGear;
    break;

    case 5 :
      velocity = fivethGear;
    break;

    case 6 :
      velocity = sixGear;
    break;
    
    default :
      if(gear > 6) {

        gear = 6;
      }

      if(gear == 255) { //gear doesn't go negative

        gear = 0;
      }
    break;
  }

  SetSpeed(velocity);
}

//TODO: UpdateRotation();

//TODO: Need tests
void SetDirection(int8_t leftWheel, int8_t rightWheel) {

  //Left-------------------------------
  if(leftWheel < 0) {

    digitalWrite(hBridge[0], 0);
    digitalWrite(hBridge[1], 1);
  }
  else if (leftWheel > 0) {

    digitalWrite(hBridge[0], 1);
    digitalWrite(hBridge[1], 0);
  }
  else {

    digitalWrite(hBridge[0], 0);
    digitalWrite(hBridge[1], 0);
  }

  //Right------------------------------
  if(rightWheel < 0) {

    digitalWrite(hBridge[2], 0);
    digitalWrite(hBridge[3], 1);
  }
  else if (rightWheel > 0) {

    digitalWrite(hBridge[2], 1);
    digitalWrite(hBridge[3], 0);
  }
  else {

    digitalWrite(hBridge[2], 0);
    digitalWrite(hBridge[3], 0);
  }
}

//Set left/right wheels speed
void SetSpeed(uint8_t leftWheel, uint8_t rightWheel) {

  analogWrite(hBridge[4], leftWheel);
  analogWrite(hBridge[5], rightWheel);
}

//Set both wheels speed
void SetSpeed(uint8_t speed) {

  SetSpeed(speed, speed);
}

//TODO: Console commands
/*void ReadCommand(String command) {

  //...
}*/

void ReadKey(char key) {

  if(key == GearUp) {
    //TODO: speed variations

    gear++;
    UpdateSpeed();

    if(debug)
    Serial.println("GearUp to: " + gear);
  }

  if(key == GearDown) {

    gear--;
    UpdateSpeed();

    if(debug)
    Serial.println("GearDown to: " + gear);
  }

  if(key == Foward) {

    if(debug)
    Serial.println("Foward");

    SetDirection(1, 1);
  }

  if (key == Backward) {

    if(debug)
    Serial.println("Backward");

    SetDirection(-1, -1);
  }
  
  if(key == Left) {

    if(debug)
    Serial.println("Left");

    SetDirection(-1, 1);
    //turnDirection = turnDirection >= 0 ? turnDirection-- : -1; <- Unused
  }

  if(key == Right) {

    if(debug)
    Serial.println("Right");

    SetDirection(1, -1);
    //turnDirection = turnDirection <= 0 ? turnDirection++ : 1; <- Unused
  }

  if(key == Stop) {

    if(debug)
    Serial.println("Stop");

    //turnDirection = 0; <- Unused
    SetDirection(0, 0);
  }

  if(key == Light) {

    //TODO: Headlight
    if(debug)
    Serial.println("Light not implemented yet");
  }
}

//WARN: INPUT DOESN'T WORK AT STARTUP!
void setup() {

  //Set hBridge as output
  for(int i = 0; i < 6; i++) {

    pinMode(hBridge[i], OUTPUT);
  }

  Serial.begin(9600);
  Serial.println("Starting...");

  //Blink led
  for(int i = 0; i < 5; i++) {

    Activity();
    delay(1000);
  }
}

void loop() {

  if(Serial.available() > 0)
  ReadBuffer();

  //Chronometer
  u32 currentTime = millis();

  //TODO: remove this
  if(serialDebug) {
    
    if(currentTime - preTimeDbg >= dbgRate) {

      preTimeDbg = currentTime;
      Dbg();
    }
  }

  if(turnDirection != 0) {

    LeftRight();
  }

  //Apply speed values
  SetSpeed(left, right);

  //TODO: Control by seting target speeds
  //TODO: Missing Loops (check functions!)
}