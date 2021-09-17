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
#define ToggleSpeed W
#define Light l

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

void setup() {

  for(int i = FirstPin; i < LastPin; i++) {

    pinMode(i, OUTPUT);
  }
}

void loop() {
  
  //...
}
