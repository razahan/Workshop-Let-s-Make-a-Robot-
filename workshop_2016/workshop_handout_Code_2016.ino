//workshop_cse_fest_2016.ino
// #include<Servo.h>

//Motors
#define leftMotor_Fwd 2
#define leftMotor_Rev 4
#define leftMotor_En 3

#define rightMotor_Fwd 7
#define rightMotor_Rev 8
#define rightMotor_En 6



//HCSR04
#define sonarTrigger_pin 10
#define sonarEcho_pin 11
#define sonarMaximumDuration 25000 //microseconds
#define sonarMinimumDuration 3000 //microseconds
#define sonarNoObstcleDuration 38000 //microseconds
#define NoObstacle -2
#define OutofRange -1

#define EmergencyBrake 5 //cm
#define MinDistance 2 //cm
#define GoStraight 400//cm
#define CheckLeftRightDistance 10 //cm
#define Left 1
#define Right 2
#define Reverse 3
#define Brake 4


void initMotors()
{
	pinMode(leftMotor_Fwd, OUTPUT);
	pinMode(leftMotor_Rev, OUTPUT);
	pinMode(leftMotor_En, OUTPUT);

	pinMode(rightMotor_Fwd, OUTPUT);
	pinMode(rightMotor_Rev, OUTPUT);
	pinMode(rightMotor_En, OUTPUT);

	brake();
}

void run_motor(unsigned char Fwd_pin, unsigned char Fwd_pin_state, unsigned char Rev_pin, unsigned char Rev_pin_state , unsigned char En_pin, unsigned int En_pin_value)
{
	digitalWrite(Fwd_pin, Fwd_pin_state);
	digitalWrite(Rev_pin, Rev_pin_state);
	analogWrite(En_pin, En_pin_value);
}

void goForward(unsigned char speed)
{
	run_motor(leftMotor_Fwd,HIGH,leftMotor_Rev, LOW, leftMotor_En,speed);
	run_motor(rightMotor_Fwd,HIGH,rightMotor_Rev, LOW, rightMotor_En,speed);
}

void goReverse(unsigned char speed)
{
	run_motor(leftMotor_Fwd,LOW,leftMotor_Rev, HIGH, leftMotor_En,speed);
	run_motor(rightMotor_Fwd,LOW,rightMotor_Rev, HIGH, rightMotor_En,speed);
}

void pivotLeft(unsigned char speed)
{
	run_motor(leftMotor_Fwd,LOW,leftMotor_Rev, HIGH, leftMotor_En,speed);
	run_motor(rightMotor_Fwd,HIGH,rightMotor_Rev, LOW, rightMotor_En,speed);
}

void pivotRight(unsigned char speed)
{
	run_motor(leftMotor_Fwd,HIGH,leftMotor_Rev, LOW, leftMotor_En,speed);
	run_motor(rightMotor_Fwd,LOW,rightMotor_Rev, HIGH, rightMotor_En,speed);
}

void brake()
{
	analogWrite(leftMotor_En, 0);
	analogWrite(rightMotor_En, 0);
}

void initSonar()
{
	pinMode(sonarTrigger_pin, OUTPUT);
	pinMode(sonarEcho_pin, INPUT);
}

long readSonar()
{
	long duration, distance;
	digitalWrite(sonarTrigger_pin, LOW); 
 	delayMicroseconds(2); 

 	digitalWrite(sonarTrigger_pin, HIGH);
 	delayMicroseconds(10); 
 
 	digitalWrite(sonarTrigger_pin, LOW);
 	duration = pulseIn(sonarEcho_pin, HIGH);
 
 	
 	if (duration>=sonarNoObstcleDuration)
  {
    // Serial.println("No obstacle");
    return NoObstacle;
  }
  else //if((duration>=sonarMinimumDuration)&&(duration<=sonarMaximumDuration))
  {
    distance = microsecondsToCentimeters(duration);//Calculate the distance (in cm) based on the speed of sound.
    Serial.print("distance= ");

    Serial.println(distance);
    return distance;
      
  }
  
   	
}
long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

unsigned char checkLeftRight()
{
  long distanceLeft =0, distanceRight=0;
  
  pivotLeft(125);
  delay(500);
  brake();
  distanceLeft = readSonar();
  
  pivotRight(125);
  delay(1000);
  brake();
  distanceRight = readSonar();
  
  pivotLeft(125);
  delay(500);
  brake();
  
  if (distanceLeft>=distanceRight)
  {
    
    if((distanceLeft==NoObstacle))
    {
      return Left;
    }
     else if(distanceLeft <= EmergencyBrake)
    {
      return Reverse;
    }
    else if((distanceLeft>EmergencyBrake)&&(distanceLeft <= GoStraight))
    {
      return Left;
    }
    
    else
    {
      return Brake;
    }
  }
  else if (distanceRight > distanceLeft)
  {
    if((distanceRight==NoObstacle))
    {
      return Right;
    }
    else if(distanceRight <= EmergencyBrake)
    {
      return Reverse;
    }

    else if((distanceRight>=EmergencyBrake)&&(distanceRight <= GoStraight))
    {
      return Right;
    }
    else
    {
      return Brake;
    }
    
    
  }
}

void setup() {
  // put your setup code here, to run once:
  initMotors();
  initSonar();
  Serial.begin(9600);
  
}



long distance = 0;
unsigned char decision = Brake;

void loop() {

  
  distance = readSonar();
  if ((distance == NoObstacle))
  {
    Serial.println("Going No obstacle");
    goForward(125);
    delay(300);
    brake();
    delay(100);
  }
  else if((distance>=MinDistance) && (distance<=EmergencyBrake))
  {
    brake();
    delay(100);
    goReverse(125);
    delay(200);
    brake();
    delay(100);
  }
  
  else if((distance > EmergencyBrake) && (distance <= CheckLeftRightDistance))
  {
    brake();
    decision = checkLeftRight();
    if (decision == Left)
    {
      // Serial.println("Going Left");
      pivotLeft(125);
      delay(500);
      brake();
      goForward(125);
      delay(500);
      brake();
      delay(100);
    }
    else if(decision == Right)
    {
      // Serial.println("Going Right");
      pivotRight(125);
      delay(500);
      brake();
      goForward(125);
      delay(500);
      brake();
      delay(100);
    }
    else if (decision == Reverse)
    {
      // Serial.println("Going Revere");
      goReverse(125);
      delay(500);
      brake();
      delay(100);
    }
    else
    {
      // Serial.println("Going Left right last else");
      brake();
      delay(100);
    }    
  }
  else if((distance > CheckLeftRightDistance) && (distance <= GoStraight))
  {
    // Serial.println("Going Straight");
    goForward(125);
    delay(500);
    brake();
    delay(100);
  }



  else
  {
    // Serial.println("Last Else");
    brake();
    delay(100);
  }
    
}
