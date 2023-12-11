/*
This is the Infra Red Control program.

Goal: To control the robot car's movement using an IR remote.

Main Theme: finding the numerical values of the remote's buttons using Serial.print() function and making IF statements for the control.
*/

//Libraries
// Here we include the IRremote Library.
#include <IRremote.hpp>
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

//InfraRed
// Arduino pin 9 connected to IR LED.
#define IReceive 9

//Arrows
// numerical value of remote's up arrow button is 70.
#define UP    70
// numerical value of remote's right arrow button is 67.
#define RIGHT 67
// numerical value of remote's left arrow button is 68.
#define LEFT  68
// numerical value of remote's down arrow button is 21.
#define DOWN  21
// numerical value of remote's "ok" button is 64.
#define OK   64

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

  //IRtest
  // This code activates the Serial Monitor.
  Serial.begin(9600);

  //Receiver
  // This activates the IR receiver.
  IrReceiver.begin(IReceive, ENABLE_LED_FEEDBACK);
}

void loop() 
{
//Control
// Here we tell the Arduino that IF the receiver receives a signal and decodes it ...
 if (IrReceiver.decode()) 
 {
    // Print that value on the Serial Monitor.
    Serial.println(IrReceiver.decodedIRData.command);
    // and IF the decoded value received corresponds to the OK button then ...
    if(IrReceiver.decodedIRData.command == OK)
    {
      //Stop the motors.
      Stop();
    }
    // IF NOT then check if the value matches with the up arrow button. IF SO then ...
    else if(IrReceiver.decodedIRData.command == UP)
    {
      // both sets of Motors rotate clockwise at a speed of 100. (advance)
      Forward(100);
      // Head(servo) facing straight forward. (95 degrees because for some reason 90 degrees isn't perfectly straight)
      myservo.write(95);
    }
    // IF NOT then check if the value matches with the right arrow button. IF SO then ...
    else if(IrReceiver.decodedIRData.command == RIGHT)
    {
      // Left motors spin clockwise at speed of 60 and right motors spin counterclockwise at the same speed. (turn right) 
      Right(60);
      // Head(servo) facing to the right.
      myservo.write(25);
    }
    // IF NOT then check if the value matches with the left arrow button. IF SO then ...
    else if(IrReceiver.decodedIRData.command == LEFT)
    {
      // Left motors spin counter clockwise at a speed of 60 and right motors spin in other direction at same speed (turn left)
      Left(60);
      // Head(servo) facing to the left.
      myservo.write(160);
    }
    // IF NOT then check if the value matches with the down arrow button. IF SO then ...
    else if(IrReceiver.decodedIRData.command == DOWN)
    {
      // Both sets of motors spin counterclockwise at a speed of 100. (reverse)
      Backward(100);
      // Head(servo) facing straight forward.
      myservo.write(95);
    }
    // This resumes the reading of IR signals. (resume reading and go back through the loop)
    IrReceiver.resume();
 }
}

//Functions
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
