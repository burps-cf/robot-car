//www.elegoo.com

// burps-cf: for scaling speed
#include <math.h>

#include <Servo.h>  //servo library
Servo myservo;      // create servo object to control servo

int Echo = A4;  
int Trig = A5; 

#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11
#define carSpeed 150
int rightDistance = 0, leftDistance = 0, middleDistance = 0;

// burps-cf: adjustable forward speed
int forwardSpeed = carSpeed;

void forward(){ 
  analogWrite(ENA, forwardSpeed);
  analogWrite(ENB, forwardSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Forward");
}

void back() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Back");
}

void left() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH); 
  Serial.println("Left");
}

void right() {
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Right");
}

void stop() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  Serial.println("Stop!");
} 

//Ultrasonic distance measurement Sub function
int Distance_test() {
  digitalWrite(Trig, LOW);   
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);  
  delayMicroseconds(20);
  digitalWrite(Trig, LOW);   
  float Fdistance = pulseIn(Echo, HIGH);  
  Fdistance= Fdistance / 58;       
  return (int)Fdistance;
}

// burps-cf:
// compute adjusted speed according to the distance
// to the obstacle in front of the car
int adjustSpeed(int distance) {

  int newSpeed;

  // slow down when approach an obstacle in front
  if (distance <= 50) {
    newSpeed = int(exp((float)distance/10.0));
  } else {
    newSpeed = carSpeed;
  }

  // sane boundaries
  newSpeed = (newSpeed > carSpeed) ? carSpeed : newSpeed;
  newSpeed = (newSpeed < 1) ? 1 : newSpeed;

  Serial.print ("New speed: ");
  Serial.println (newSpeed);

  return newSpeed;

} // adjustSpeed

void setup() { 
  myservo.attach(3);  // attach servo on pin 3 to servo object
  Serial.begin(9600);     
  pinMode(Echo, INPUT);    
  pinMode(Trig, OUTPUT);  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  stop();
} 

void loop() { 
    myservo.write(90);  //setservo position according to scaled value
    delay(500); 
    middleDistance = Distance_test();

    // burps-cf: slow down if obstacle ahead
    forwardSpeed = adjustSpeed(middleDistance);

    if(middleDistance <= 20) {     
      stop();
      delay(500);                         
      myservo.write(10);          
      delay(1000);      
      rightDistance = Distance_test();
      
      delay(500);
      myservo.write(90);              
      delay(1000);                                                  
      myservo.write(180);              
      delay(1000); 
      leftDistance = Distance_test();
      
      delay(500);
      myservo.write(90);
      delay(1000);

      // burps-cf: don't let the car get boxed-in
      // TODO: would AND work better than OR?
      if((rightDistance <= 20) || (leftDistance <= 20)) {
        back();
        delay(180);
      }
      // burps-cf: prioritise right-hand turns
      else if(rightDistance >= leftDistance) {
        right();
        delay(360);
      }
      else if(rightDistance < leftDistance) {
        left();
        delay(360);
      }
      // burps-cf: should not ever to execute this branch?
      else {
        forward();
      }
    }  
    else {
        forward();
    }                     
}

