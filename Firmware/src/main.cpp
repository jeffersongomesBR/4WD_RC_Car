#include <Arduino.h>
#include <NewPing.h>

/// ---Pinos--- ///

//Ponte H
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5
#define ENA 10 //PWM~
#define ENB 11 //PWM~

//Sensor de proximidade
#define TRIG 6 //Unused
#define ECHO 7 //Unused

/// ---Comandos--- ///
#define KeyPrefix '/' //Unused

//Keys
#define Foward 'w'
#define Backward 's'
#define Left 'a'
#define Right 'd'
#define Stop 'x'
#define GearUpKey '+'
#define GearDownKey '-'
#define Light 'l' //Unused

/// ---Definições--- ///
const bool serialDebug = true; //Enviar variaveis ao monitor serial (REMOVER EM RELEASE!)
const bool debug = true; //Enviar eventos de debug ao monitor serial (REMOVER EM RELEASE!)
const uint16_t dbgRate = 1000; //Atraso de envio de informações ao monitor serial (ms)

//Velocidades
const uint8_t gear[7] = {0, 43, 86, 129, 172, 215, 255}; //Minimo 0, Maximo 255

/// ---Variaveis--- ///
bool lightON = false; //Unused
bool stopped = false; //Unused
bool signal = false; //blink led
uint8_t hBridge[6] = {IN1, IN2, IN3, IN4, ENA, ENB};
uint8_t selectedGear = 0; //0-6
uint8_t turnDirection = 0; //-1 = Left, 1 = Right //Unused
int16_t velocity = 0; //-255/255
u32 preTimeDbg = 0; //usado pelo loop checkpoint
u32 distance = 0; //1 = 1 cm

/// ---Objects--- ///
NewPing sonar(TRIG, ECHO, 400); //4 meters max

/// ---Properties--- ///
//TODO: Use distance to avoid colisions
u32 Distance() {

  if(!sonar) {

    return sonar.ping_cm();
  }
  else {

    Serial.println("Error reading distance!")
  }

  return 0;
}

/// ---Codigo--- ///

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

void UpdateSpeed() {

  if(selectedGear > 6) {

    if(selectedGear >= 250) { //selectedGear doesn't go negative

      selectedGear = 0;
    }
    else {

      selectedGear = 6;
    }
  }

  velocity = (selectedGear <= 6) ? gear[selectedGear] : gear[6];

  SetSpeed(velocity);
}

//TODO: UpdateRotation();

//Switch gear to a selected gear
void SwitchGear(int8_t to) {

  selectedGear = to;
  UpdateSpeed();
}

//Switch gear up
void GearUp() {

  selectedGear++;
  UpdateSpeed();
}

//Switch gear down
void GearDown() {

  selectedGear--;
  UpdateSpeed();
}

void ReadKey(char key) {

  if(key == GearUpKey) {
    //TODO: speed variations

    GearUp();

    if(debug)
    Serial.println("GearUp to: " + selectedGear);
  }

  if(key == GearDownKey) {

    GearDown();

    if(debug)
    Serial.println("GearDown to: " + selectedGear);
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

//TODO: Console commands
/*void ReadCommand(String command) {

  //...
}*/

//TODO: Read serial buffer and clear it

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
  ReadKey(Serial.read());

  //Chronometer
  u32 currentTime = millis();

  //TODO: remove this
  if(serialDebug) {
    
    if(currentTime - preTimeDbg >= dbgRate) {

      preTimeDbg = currentTime;
      Dbg();
    }
  }
}
