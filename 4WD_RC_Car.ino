/// ---Pinos--- ///

//Motores Dianteiros
#define FrontEngineL 4 //Unused
#define FrontEngineR 5 //Unused
#define FrontEngineV 3 //PWM~ //Unused

//Motores Traseiros
#define RearEngineL 7 //Unused
#define RearEngineR 8 //Unused
#define RearEngineV 6 //PWM~ //Unused

//Outros
#define FirstPin 3
#define LastPin 7

//TODO: need tests with bluetooth module
/// ---Controles--- ///
#define Foward w //Unused
#define Backward s //Unused
#define Left a //Unused
#define Right d //Unused
#define Break S //Unused
#define HandBreak x //Unused
#define Speed W //Unused
#define Light l //Unused
#define Reset r //Unused

/// ---Definições--- ///
const int deltaV = 2;
const int lowSpeed = 64; //Minimo 0
const int mediumSpeed = 128;
const int highSpeed = 255; //Maximo 255

/// ---Variaveis--- ///
int velocity = 0; //0-255
int targetSpeed = 0; //0-255
int speedMode = 2; //1-3
int moveDirection = 0 //-1 = reverse, 0 = stopped, 1 = forward //Unused
int turnDirection = 0 //-1 = Left, 0 = stopped, 1 = Right //Unused
bool lightON = false; //Unused
bool handBreakUP = false; //Unused
char key;

/// ---Codigo--- ///

//Velocidade
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

//Frente - ré
void FowardBackward(int d) {
  
  //...
}

//Esquerda - Direita
void LeftRight(int d) {

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

  //TODO: does buffer clear after read?
  if(Serial.available() > 0) {

    key = Serial.read();
  }

  //Update Target Speed
  switch(speedMode) {

    case 1:
    targetSpeed = lowSpeed;
    break;

    case 2:
    targetSpeed = mediumSpeed;
    break;

    case 3:
    targetSpeed = highSpeed;
    break;

    default:
    targetSpeed = mediumSpeed;
    break;
  }

  if(moveDirection != 0 || turnDirection != 0) {

    UpdateSpeed();
    FowardBackward(moveDirection);
    LeftRight(turnDirection);
  }
}
