/*
This is the Line Tracking Program for the Robot Car.

Goal: Make the robot follow a specific trajectory in real time using black tape lines.

Main Theme: learn to use the line tracking module.
*/

//Libraries
// Here we include the Servo motor Library.
#include <Servo.h>

//Motor A
// Here we define pin 8 of Arduino as the motor driver's Input connection for motors A.
#define INA 8
// Here we define pwm pin 5 of Arduino as the motor driver's enable connection for motors A.
#define PWMA 5

//Motor B
// Here we define pin 7 of Arduino as the motor driver's Input connection for motors B.
#define INB 7
// Here we define pwm pin 6 of Arduino as the motor driver's enable connection for motors B.
#define PWMB 6

//STBY
// Here we define pin 3 of Arduino as the standby connection of the motor driver.
#define STBY 3

//Direction
// Here we define clockwise as 1, later used with digital.write (1 = HIGH).
#define clockWise 1
// Here we define anticlockwise as 0, later used with digital.write (0 = LOW).
#define antiClockwise 0

//Servo
// Here we create servo object to control a servo.
Servo myservo;
// Here we made an integer variable called "position" and gave it the value of 0. (used later in nod function)
int position = 0;

//Line Trackers
// Here we define analog input pin 1 of Arduino as the middle line tracking sensor.
#define sensorMid A1
// Here we define analog input pin 0 of Arduino as the right line tracking sensor.
#define sensorR A0
// Here we define analog input pin 2 of Arduino as the left line tracking sensor.
#define sensorL A2

//Ultrasonic
// Here we define pin 13 of Arduino as the trigger pin of ultrasonic sensor.
#define Trigger 13
// Here we define pin 12 of Arduino as the echo pin of ultrasonic sensor.
#define Echo 12
// Here we made two float variables named "duration" and "distance"
float duration, distance;

//Time
// Here we made an unsigned long variable called "previousTime" and gave it the value of 0.
unsigned long previousTime = 0;
// Here we made a constant long variable called "prinTime" and gave it the value of 150. (duration of printing)
const long prinTime = 150;

//SRCH
// Here we made an integer variable and gave it the value of 0;
int search = 0;

void setup() 
{
//Motors
  // Here we declare pin 8 of Arduino as an OUTPUT pin.
  pinMode(INA, OUTPUT);
  // Here we declare pin 5 of Arduino as an OUTPUT pin.
  pinMode(PWMA, OUTPUT);
  // Here we declare pin 7 of Arduino as an OUTPUT pin.
  pinMode(INB, OUTPUT);
  // Here we declare pin 6 of Arduino as an OUTPUT pin.
  pinMode(PWMB, OUTPUT);
  // Here we activate the Standby (pin 3) to activate the motors. (digital output 5V to pin 3)
  digitalWrite(STBY, 1);

//LineSensors
  // Here we declared analog input pin 1 as an input pin.
  pinMode(sensorMid, INPUT);
  // Here we declared analog input pin 0 as an input pin.
  pinMode(sensorR, INPUT);
  // Here we declared analog input pin 2 as an input pin.
  pinMode(sensorL, INPUT);

//Servo motor
  // Servo motor connected to pin 10 of Arduino.
  myservo.attach(10);

//UltSonic
  //Here we declared pin 13 of Arduino as an output pin.
  pinMode(Trigger, OUTPUT);
  //Here we declared pin 12 of Arduino as an input pin.
  pinMode(Echo, INPUT);

//Test
  // This code activates the Serial Monitor.
  Serial.begin(9600);
}

void loop() 
{
//Values
  // Here we made an integer variable called "Rval" and gave it the values of whatever the analog input pin 0 reads.
  //(bit values of the right line tracking sensor)
  int Rval = analogRead(sensorR);
  // Here we made an integer variable called "Lval" and gave it the values of whatever the analog input pin 2 reads.
  //(bit values of the left line tracking sensor)
  int Lval = analogRead(sensorL);
  // Here we made an integer variable called "midVal" and gave it the values of whatever the analog input pin 1 reads.
  //(bit values of the middle line tracking sensor)
  int midVal = analogRead(sensorMid);

//Millies
  // Here we made an unsigned long variable called "currenTime" and gave it the values of millis command.
  unsigned long currentTime = millis();

//Print
  // IF millis - previous time is bigger than 150 ...
  if(currentTime - previousTime >= prinTime)
  {
    // Make previous time = to millis (reset timer)
    previousTime = currentTime;
    // Serial print the word "L: "
    Serial.print("L: ");
    // Serial print the values of the left line tracking sensor.
    Serial.print(Lval);
    // Serial print 8 spaces(tab x2)
    Serial.print("\t\t");
    // Serial print the word "M: "
    Serial.print("M: ");
    // Serial print the values of the middle line tracking sensor.
    Serial.print(midVal);
    // Serial print 8 spaces(tab x2)
    Serial.print("\t\t");
    // Serial print the word "R: "
    Serial.print("R: ");
    // Serial print the values of the right line tracking sensor.
    Serial.println(Rval);
  }

//Line Tracking
  // Here we called for our ultrasonic sensor sensing function.
  SonicSens();
  // IF the distance that the ultrasonic sensor reads is less than 20cm ...
  if(distance < 20)
  {
    // Stop the car.
    Stop();
  }
  // IF NOT then ...
  else
  {
    // IF the values that the middle sensor, right sensor and left sensor reads are all greater than 500 ... (car picked up)
    if(midVal >= 500 && Rval >= 500 && Lval >= 500)
    {
      // Stop the car.
      Stop();
    }
    // IF only the middle sensor reads above 500 ... (line in middle detected)
    else if(midVal >= 500)
    {
      // Go forward at a speed of 100.
      Forward(100);
    }
    // IF only the right and left sensors at the same time read greater than 500 ... (line detected on the right and left) (not really possible) 
    else if(Rval >= 500 && Lval >= 500)
    {
      // robot nods it's head (prevention of fooling/tricking the robot)
      Nod();
    }
    // IF only the right sensor reads values greater than 500 ... (line detected on the right)
    else if(Rval >= 500)
    {
      // Turn right at a speed of 100.
      Right(100);
    }
    // IF only the left sensor reads values greater than 500 ... (line detected on the left)
    else if(Lval >= 500)
    {
      // Turn left at a speed of 100.
      Left(100);
    }
    // IF none of the sensors read above 500 (on ground)(no line detected)
    else
    {
      /*
      Go into search mode and try to find a line on either the right or left or else U turn back onto the line it came from.
      However, for some reason the car stays stuck in case 0 and only turns right till it U turns. Meaning it won't detect if there is a line on the left.
      The U turn still works nervertheless.
      */
      Search();
    }
  }
}

//Functions
// This is my function for the ultrasonic sensing.
void SonicSens()
{
  // Turn the ultrasonic sensor off for 2 micro seconds. 
  digitalWrite(Trigger, LOW);
  // Freez the program for 2 microseconds.
  delayMicroseconds(2);
  // Turn the ultrasonic sensor on (send pulse of sound) for 10 microseconds.
  digitalWrite(Trigger, HIGH);
  // Freez the program for 10 microseconds.
  delayMicroseconds(10);
  // Turn the sensor off again (send no pulse)
  digitalWrite(Trigger, LOW);

  // Store the time that it took for the sound pulse to reach the receiver (echo) into the duration variable.
  duration = pulseIn(Echo, HIGH);
  // Calculate the distance by multiplying half of the duration by 0.0343. Store the distance value within the distance variable.
  distance = (duration/2) * 0.0343;
}

// This is my function for the forward movement.
void Forward(int speed)
{
//Motors A
  /*
  Here we send voltage to the input connection of the motor driver which is an H bridge.
  This allows the motors to have a certain polarity which affects the direction at which the motors spin.
  In this case we make motors A spin clockwise. (HIGH = input 1 on, input 2 off) The shield inverses the state of the other input so we don't have to.
  So in summary: HIGH/1 = clockwise and LOW/0 = counterclockwise.
  */
  // make motors A spin clockwise.
  digitalWrite(INA, clockWise);
  // control the speed with whatever bit value the operator sets inside the parentheses of function. 
  analogWrite(PWMA, speed);
//Motors B
  // make motors B spin clockwise.
  digitalWrite(INB, clockWise);
  // control the speed with whatever bit value the operator sets inside the parentheses of function.
  analogWrite(PWMB, speed);
//Servo
  // Head(servo) facing straight forward. (95 degrees because for some reason 90 degrees isn't perfectly straight)
  myservo.write(95);
}

// This is my function for the turn right movement.
void Right(int speed)
{
//Motors A
  // make motors A spin clockwise.
  digitalWrite(INA, clockWise);
  // control the speed with whatever bit value the operator sets inside the parentheses of function.  
  analogWrite(PWMA, speed);
// Motors B
  // make motors B spin anticlockwise.
  digitalWrite(INB, antiClockwise);
  // control the speed with whatever bit value the operator sets inside the parentheses of function.
  analogWrite(PWMB, speed);
}

// This is my function for the turn left movement.
void Left(int speed)
{
//Motors A
  // make motors A spin anticlockwise.
  digitalWrite(INA, antiClockwise);
  // control the speed with whatever bit value the operator sets inside the parentheses of function. 
  analogWrite(PWMA, speed);
//Motors B
  // make motors B spin clockwise.
  digitalWrite(INB, clockWise);
  // control the speed with whatever bit value the operator sets inside the parentheses of function.
  analogWrite(PWMB, speed);
}

// This is my function for the stop.
void Stop()
{
//Motors A
  // Here the direction does not really matter but the speed is always set to 0.
  digitalWrite(INA, clockWise);
  // Speed set to 0 (stationary)
  analogWrite(PWMA, 0);
//Motors B
  // Same for motors B.
  digitalWrite(INB, clockWise);
  // Speed set to 0 (stationary)
  analogWrite(PWMB, 0);
}

// This is my function for the nodding action.
void Nod()
{
  // Firstly stop the motors.
  Stop();
  // FOR loop to make position variable reach 180 degrees (left)
  for (position = 0; position <= 180; position += 4) 
  {
    // Make robot turn it's head to the left.
    myservo.write(position);
    // Small delay command for stability.
    delay(15);
  }
  // Another FOR loop to bring position back to 0 (right)
  for (position = 180; position >= 0; position -= 4) 
  {
    // Make robot turn it's head to the right.
    myservo.write(position);
    // Small delay command for stability.
    delay(15);
  }
}

// This is my function for the searching action.
void Search() 
{
  // Here we use switch case for the different steps of the search.
  switch (search) 
  {
    // Case 0 is the 90 degree right turn.
    case 0:
      // Turn the head to the right.
      myservo.write(25);
      // Turn right at a speed of 55.
      Right(55);
      // IF the millis - 0 is bigger than 2 and a half seconds ...
      if (millis() - previousTime > 2500)
      {
        // Go to next step.
        search = 1;
        // Set previous time to millis() (reset timer)
        previousTime = millis();
      }
      // break out of case 0;
      break;

    // Case 1 is the U turn.
    case 1:
      // Turn the head to the left.
      myservo.write(160);
      // Turn left at a speed of 55.
      Left(55);
      // IF millis - 2500 is bigger than 5 and a half seconds ...
      if (millis() - previousTime > 5500)
      {
        // Go to the next step.
        search = 2;
        // Set previous time to millis() (reset timer) 
        previousTime = millis();
      }
      // break out of case 1;
      break;

    // Final case is full stop.
    case 2:
      // Stop motors.
      Stop();
      // Head facing forward.
      myservo.write(95);
  }
}
