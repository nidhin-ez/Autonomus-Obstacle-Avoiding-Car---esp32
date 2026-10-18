#include <ESP32Servo.h>

Servo servo;

const int servoPin = 18;
const int echoPin = 19;
const int trigPin = 5;

const int stby = 13;
const int pwma = 25;
const int Ain1 = 26;
const int Ain2 = 27;

const int pwmb = 14;
const int Bin1 = 32;
const int Bin2 = 33;

const int pwmFreq = 1000;
const int pwmRes = 8;
const int motorSpeed = 190;


int leftAngle = 150;
int centreAngle = 90;
int rightAngle = 30;
int defaultAngle = centreAngle;

float rightDistance = 0;
float leftDistance = 0;
float centreDistance = 0;
unsigned long scanTime =0;
unsigned long moveTime = 0;
unsigned long startTime = 0;
unsigned long turnTime = 0;
bool doMovement = false;
bool isRotating = false;
float safetyCheckDistance = 0;
bool safetyCheckTriggered = false;

typedef enum{
  returnToCentre,
  moveLeft,
  waitLeft,
  readLeft,
  moveRight,
  waitRight,
  readRight,
  decision,
  executeMove,
  waitReturnToCentre,
  readCentre
}state;
state servoState  = moveLeft;

typedef enum{
  goRight,
  goLeft,
  goStraight,
  stop
}motor;

typedef enum{
  turnLeft,
  turnRight,
  waitToTurn,
  goingStraight
}turnAndGo;

turnAndGo motorTurn;

motor Mmovement = stop;

void stopMotor();
bool safetyCheck(motor *movement);
void setup() {
  Serial.begin(115200);
  servo.attach(servoPin);
  pinMode(stby,OUTPUT);
  pinMode(Ain1,OUTPUT);
  pinMode(Ain2,OUTPUT);
  pinMode(Bin1,OUTPUT);
  pinMode(Bin2,OUTPUT);

  ledcAttach(pwma,pwmFreq,pwmRes);
  ledcAttach(pwmb,pwmFreq,pwmRes);

  digitalWrite(stby,HIGH);
  stopMotor();
  servo.write(centreAngle);
  Serial.println("90 deg");
  delay(1000);
  servo.write(rightAngle);
  delay(1000);
  servo.write(leftAngle);
  delay(1000);
  servo.write(centreAngle);
  delay(1000);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

}

void motorGoStraight(){
  Serial.print("going Straight");
  if(safetyCheck(&Mmovement) ==  true){
    stopMotor();
    safetyCheckTriggered = true;
    return;
  }
  digitalWrite(Ain1,HIGH);
  digitalWrite(Ain2,LOW);
  digitalWrite(Bin1,HIGH);
  digitalWrite(Bin2,LOW);

  ledcWrite(pwma,motorSpeed);
  ledcWrite(pwmb,motorSpeed);
}

void stopMotor(){
  ledcWrite(pwma,0);
  ledcWrite(pwmb,0);
  
  digitalWrite(Ain1, LOW);
  digitalWrite(Ain2, LOW);
  digitalWrite(Bin1, LOW);
  digitalWrite(Bin2, LOW); 
}

void motorGoRight(unsigned long currentTime,turnAndGo *PmotorTurn){
   switch(*PmotorTurn){
    case turnRight:{
        digitalWrite(Ain1, HIGH);
        digitalWrite(Ain2, LOW);
        digitalWrite(Bin1, LOW);
        digitalWrite(Bin2, HIGH);

        ledcWrite(pwma,motorSpeed);
        ledcWrite(pwmb,motorSpeed);
        *PmotorTurn = waitToTurn;
        break;
      }

    case waitToTurn:{
        if(currentTime - turnTime > 389){
        *PmotorTurn = goingStraight;
      }
      break;
      }
  
    case goingStraight:{
      motorGoStraight();
      break;
    }
  
    case turnLeft:{
      break;
    }
  }
}


void motorGoLeft(unsigned long currentTime,turnAndGo *PmotorTurn){
  switch(*PmotorTurn){
    case turnLeft:{
        digitalWrite(Ain1, LOW);
        digitalWrite(Ain2, HIGH);
        digitalWrite(Bin1, HIGH);
        digitalWrite(Bin2, LOW);

        ledcWrite(pwma,motorSpeed);
        ledcWrite(pwmb,motorSpeed);
        *PmotorTurn = waitToTurn;
        break;
    }
    case waitToTurn:{
        if(currentTime - turnTime > 389){
        *PmotorTurn = goingStraight;
      }
      break;
    }
    case goingStraight:{
      motorGoStraight();
      break;
    }
    case turnRight:{
      break;
    }
  }

}

float readDistance(){
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  unsigned long time=pulseIn(echoPin,HIGH,30000);

  if(time == 0){
    return -1; // returns one for invalid echo recieved

  }
  else{
    return (time*0.0343)/2; /// returns the distance by this formula where .0343 is the speed of the wave/sound per micro second
  }
}

bool safetyCheck(motor *movement){
  safetyCheckDistance = readDistance();
  if(safetyCheckDistance < 20 && safetyCheckDistance > 0){
    *movement = stop;
    stopMotor();
    return true;
  }
  else{
    return 0;
  }
}

void scanCheck(unsigned long currentTime,state *currentServoState, motor *Pmovement){
  switch(*currentServoState){
    case moveLeft:{
      servo.write(leftAngle);
      moveTime = currentTime;
      *currentServoState = waitLeft;
      break;
    }
    case waitLeft:{
      if(currentTime - moveTime > 500){
        *currentServoState = readLeft;
      }
      break;
    }
    case readLeft:{
      leftDistance = readDistance();
      Serial.print("leftDistance: ");
      Serial.println(leftDistance);
      *currentServoState = moveRight;
      break;
    }
    case moveRight:{
      servo.write(rightAngle);
      moveTime = currentTime;
      *currentServoState = waitRight;
      break;
    }
    
    case waitRight:{
      if(currentTime - moveTime > 500){
        *currentServoState = readRight;
        }
      break;
    }
    case readRight:{  
        rightDistance = readDistance();
        Serial.print("RightDistance: ");
        Serial.println(rightDistance);
        *currentServoState = returnToCentre;
        break;
    }
    case returnToCentre:{
      servo.write(centreAngle);
      moveTime = currentTime;
      *currentServoState = waitReturnToCentre;
      break;
    }
    case waitReturnToCentre:{
      if(currentTime - moveTime > 500){
        *currentServoState = readCentre;
      }
      break;
    }

    case readCentre:{
      centreDistance = readDistance();
      Serial.print(centreDistance);
      *currentServoState = decision;
      break;
    }

    case decision:{ //I used doMovement for the movement is ensured after each cycle or else there will be a case where the movement wont take place
      Serial.println("decision stage reached");
      if(rightDistance < 0 || leftDistance < 0 || centreDistance < 0){
        *Pmovement = stop;
        doMovement = true;
        Serial.print("Warning invalid reading");
      }
      else if(rightDistance < 25 && leftDistance < 25 && centreDistance < 25){
        *Pmovement = stop;
        doMovement = true;
      }
      else if(centreDistance >= 25){
        *Pmovement = goStraight;
        doMovement = true;
      }
      else if(leftDistance > rightDistance && centreDistance < 25){           
        if(leftDistance < 25 ){
          *Pmovement = stop;
        }
        else{
          *Pmovement = goLeft;
          turnTime = currentTime;
          motorTurn = turnLeft;
          isRotating = true;
        }
        doMovement  = true;
      }
      else if(rightDistance > leftDistance && centreDistance  < 25){
        if(rightDistance <25 && centreDistance <=25){
          *Pmovement = stop;
        }
        else{
          *Pmovement = goRight;
          turnTime = currentTime;
          motorTurn = turnRight;
          isRotating = true;
        }
        doMovement = true;
      }
      else{
        *Pmovement = stop;
        doMovement = true;
      }

      startTime = currentTime;
      *currentServoState = executeMove;
      break;
      
      
    }
    case executeMove:{
      doMovement = true;
        if(isRotating){
          if(currentTime - startTime > 3000){
            *Pmovement = stop;
            *currentServoState = moveLeft;
            isRotating = false;
          }
        }
        else{
          if(currentTime - startTime > 15000){
          *Pmovement = stop;
          *currentServoState = moveLeft;
          }
        }
      }
      break;
  }
}

void movement(motor *Pmovement,unsigned long currentTime){
  if (doMovement){
    switch(*Pmovement){
      case goRight:{
        Serial.println("goRight");
        motorGoRight(currentTime,&motorTurn);
        break;
        }
      case goLeft:{
        Serial.println("goLeft");
        motorGoLeft(currentTime,&motorTurn);
        break;
        }
      
      case goStraight:{
        Serial.println("goStraight");
        motorGoStraight();
        break;
        }
      
      case stop:{
        Serial.println("stop");
        stopMotor();
        break;
      }
    }
  }doMovement = false;
}

void loop(){
  unsigned long currentTime = millis();
  scanCheck(currentTime,&servoState,&Mmovement);
  movement(&Mmovement,currentTime);

  if(safetyCheckTriggered){
    servoState = moveLeft;
    isRotating = false;
    doMovement = false;
    Mmovement = stop;
    stopMotor();
    safetyCheckTriggered = false;
  }
}