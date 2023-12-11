/*
This is the Obstacle Avoidance Program for the robot car.

Goal: Avoid obstacles.

Main Theme: Ultrasonic sensor. IF statements within IF statements.
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

//Ultrasonic
// Here we define pin 13 of Arduino as the trigger pin of ultrasonic sensor.
#define Trigger 13
// Here we define pin 12 of Arduino as the echo pin of ultrasonic sensor.
#define Echo 12
// Here we made two float variables named "duration" and "distance"
float duration, distance;

//Line Trackers
// Here we define analog input pin 1 of Arduino as the middle line tracking sensor.
#define sensorMid A1
// Here we define analog input pin 0 of Arduino as the right line tracking sensor.
#define sensorR A0
// Here we define analog input pin 2 of Arduino as the left line tracking sensor.
#define sensorL A2

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

  // IF all three line tracking sensors read higher than 500 (robot picked up) then ...
  if(midVal >= 500 && Rval >= 500 && Lval >= 500)
  {
    // Stop the car.
    Stop();
  }
  // IF NOT then ...
  else
  {
    // Here we made sure that at the start, the ultrasonic is facing straight ahead.
    myservo.write(95);
    // We sens for an obstacle using the ultrasonic.
    SonicSens();
    // IF the value we read is less than 20cm (obstacle detected) then ...
    if (distance < 20)
    {
      // Stop the car.
      Stop();
      // Turn ultrasonic sensor to the right.
      myservo.write(0);
      // Wait 2 and a half seconds.
      delay(2500);
      // Sens again for an obstacle.
      SonicSens();
      // IF the value we read is less than 20cm (obstacle detected on the right) then ...
      if(distance < 20)
      {
        // Stop the car.
        Stop();
        // Turn ultrasonic sensor to the left.
        myservo.write(180);
        // Wait 2 and a half seconds.
        delay(2500);
        // Sens again for an obstacle.
        SonicSens();
        // IF the value we read is less than 20cm (obstacle detected on the left) then ... 
        if(distance < 20)
        {
          // Stop the car.
          Stop();
          // Reverse the car at a speed of 100.
          Backward(100);
          // For one second.
          delay(1000);
          // Turn head to the right (not necessary just for added detail)
          myservo.write(0);
          // Turn right at a speed of 50.
          Right(50);
          // For 2 and a half seconds.
          delay(2500);
        }
        // IF there was no obstacle on the left then ...
        else
        {
          // Turn left at a speed of 50.
          Left(50);
          // for 1 second.
          delay(1250);
          // Make the ultrasonic face forward again.
          myservo.write(95);
          // Small delay to allow proper sensing.
          delay(250);
        }
      }
      // IF there was no obstacle on the right then ...
      else
      {
        // Turn right at a speed of 50.
        Right(50);
        // for 1 second.
        delay(1250);
        // Make the ultrasonic face forward again.
        myservo.write(95);
        // Small delay to allow proper sensing.
        delay(250);
      }
    }
    // IF there were no obstacle ahead to begin with then ...
    else
    {
      // Continue forwards
      Forward(100);
      // Head facing forward.
      myservo.write(95);
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

// This is my function for the reverse movement.
void Backward(int speed)
{
//Motors A
  // make motors A spin anticlockwise.
  digitalWrite(INA, antiClockwise);
  // control the speed with whatever bit value the operator sets inside the parentheses of function.
  analogWrite(PWMA, speed);
//Motors B
  // make motors B spin anticlockwise.
  digitalWrite(INB, antiClockwise);
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
