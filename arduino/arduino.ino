#define DC_MotorA_Enable_Pin 10 // Don't change this one
#define DC_MotorA_In1_Pin 40    // You can swape this pin with In2 if the motor rotation direction is not what you want
#define DC_MotorA_In2_Pin 41    // You can swape this pin with In1 if the motor rotation direction is not what you want
#define DC_MotorB_Enable_Pin 9  // Don't change this one
#define DC_MotorB_In1_Pin 42    // You can swape this pin with In2 if the motor rotation direction is not what you want
#define DC_MotorB_In2_Pin 43    // You can swape this pin with In1 if the motor rotation direction is not what you want

#define DC_MotorA_Encoder_PinA 14  // PCINT10  DON'T CHANGE THIS ONE unless you set your pin change interrupt yourself
#define DC_MotorA_Encoder_PinB 15  // PCINT9   DON'T CHANGE THIS ONE unless you set your pin change interrupt yourself
#define DC_MotorB_Encoder_PinA A14 // PCINT22  DON'T CHANGE THIS ONE unless you set your pin change interrupt yourself
#define DC_MotorB_Encoder_PinB A15 // PCINT23  DON'T CHANGE THIS ONE unless you set your pin change interrupt yourself

#define ACCEL_BUTTON 22
#define OFF_BUTTON 23
#define PAUSE_BUTTON 24
#define ON_BUTTON 25
#define DECEL_BUTTON 26

#define ServoMotorA_Pin 11 // Don't change it unless you know how to play with registers
#define ServoMotorB_Pin 12 // Don't change it unless you know how to play with registers
#define ServoMotorC_Pin 2  // Don't change it unless you know how to play with registers
#define ServoMotorD_Pin 3  // Don't change it unless you know how to play with registers
#define ServoMotorE_Pin 5  // Don't change it unless you know how to play with registers
// These pins are set to be used for servo motors specifically
// If you want to use them for other purpose, or you need more servo motors
// Arduino Mega has more pins to be utilized, plaese contact TAs for details

// #define USE_ULTRASONIC_SENSOR
// #define USE_1D_LIDAR
#define USE_RASPBERRY_PI
#define __DEBUG__

#include "InterruptSetup.h" // Include the timer setting strucure provided by TAs
#include "DC_Motor.h"       // Include the DC_Motor setting strucure provided by TAs
extern "C"
{
#include "src/tlvcodec.h"
}
#include "MessageCenter.h" // Include the message center setting strucure provided by TAs

#include "src/SparkFun_Qwiic_Ultrasonic_Arduino_Library.h"
QwiicUltrasonic myUltrasonic;                           // Create an ultrasonic sensor object
uint8_t deviceAddress = kQwiicUltrasonicDefaultAddress; // Defalut Ultrasonic sensor address :0x2F

#include "src/LIDARLite_v4LED.h"
LIDARLite_v4LED myLIDAR; // Click here to get the library: http://librarymanager/All#SparkFun_LIDARLitev4 by SparkFun

extern "C"
{ // Include the simulink produced files for "control loop"
#include "src/ControlLoop_ert_rtw/ControlLoop.h"
#include "src/ControlLoop_ert_rtw/ControlLoop_private.h"
#include "src/ControlLoop_ert_rtw/ControlLoop_types.h"
  // Use ControlLoop_U.XXXX to access all your input signals (XXXX is your varible names)
  // Use ControlLoop_Y.ZZZZ to access all your output signals (ZZZZ is your varible names)
}
extern "C"
{ // Include the simulink produced files for "StateflowBlock"
#include "src/StateflowBlock_ert_rtw/StateflowBlock.h"
#include "src/StateflowBlock_ert_rtw/StateflowBlock_private.h"
#include "src/StateflowBlock_ert_rtw/StateflowBlock_types.h"
  // Use StateflowBlock_U.XXXX to access all your input signals (XXXX is your varible names)
  // Use StateflowBlock_Y.ZZZZ to access all your output signals (ZZZZ is your varible names)
}

float DC_MotorA_SpeedCommand = 0;
float DC_MotorB_SpeedCommand = 0;
float DC_MotorA_SpeedFeedback, DC_MotorB_SpeedFeedback;
int state;
uint16_t UltrasoundDistanceA = 0;
uint16_t UltrasoundDistanceA_O = 0;
uint16_t DistanceA_command;
boolean KeepDistanceA_Enable_Bool = false;
boolean ServoA_Sweeping_Enable_Bool = false;
float ServoMotorA_Angle, ServoMotorB_Angle, ServoMotorC_Angle, ServoMotorD_Angle, ServoMotorE_Angle;
double LidarDistanceA = 0;
double RoverGlobalDirection, RoverGlobalCoordX, RoverGlobalCoordY;
double PurePursuitVel, PurePursuitOmega;
boolean PurePursuitEnableBool;
double DesiredRoverGlobalDirection = 0;
double TargetDirection = 0;
MessageCenter RoverGlobalMsg;
// boolean NewSweeping = 0; // Indicate that the scanned data is new.

#define ledPin 13

void setup()
{
  // put your setup code here, to run once:

  /////
  pinMode(ledPin, OUTPUT);
  StateflowBlock_U.accel_button_in = 0;
  StateflowBlock_U.off_button_in = 0;
  StateflowBlock_U.pause_button_in = 0;
  StateflowBlock_U.on_button_in = 0;
  StateflowBlock_U.decel_button_in = 0;
  pinMode(ACCEL_BUTTON, INPUT_PULLUP);
  pinMode(OFF_BUTTON, INPUT_PULLUP);
  pinMode(PAUSE_BUTTON, INPUT_PULLUP);
  pinMode(ON_BUTTON, INPUT_PULLUP);
  pinMode(DECEL_BUTTON, INPUT_PULLUP);
  pinMode(ServoMotorA_Pin, OUTPUT);
  pinMode(ServoMotorB_Pin, OUTPUT);
  pinMode(ServoMotorC_Pin, OUTPUT);
  pinMode(ServoMotorD_Pin, OUTPUT);
  pinMode(ServoMotorE_Pin, OUTPUT);

  DC_MotorA_SpeedCommand = 0;
  DC_MotorB_SpeedCommand = 0;
  Serial.begin(115200);
  Wire.begin();
#ifdef USE_ULTRASONIC_SENSOR
  while (myUltrasonic.begin(deviceAddress) == false)
  {
    Serial.println("Ultrasonic sensor not connected, check your wiring and I2C address!");
    delay(2000);
  }
#endif
#ifdef USE_1D_LIDAR
  if (myLIDAR.begin() == false)
  {
    Serial.println("Lidar not connected, check your wiring and I2C address!");
    while (1)
      ;
  }
#endif
  for (int i = 0; i < 10; i++)
  {
    digitalWrite(ledPin, !digitalRead(ledPin));
    delay(100);
  }
  TimerAndInterruptSetup();
  Serial.println("Initializing DC Motor...");
  DC_MotorInit();
  Serial.println("Initializing Message Center...");
  RoverGlobalMsg.init();
}

void loop() // Nonreal time loop, keep running but will be interrupted anytime
{
// put your main code here, to run repeatedly:

// Serial.print("\n");
// Serial.print("Time:\t");
// Serial.print(millis());
/*
Serial.print("\t State:\t");
Serial.print(state);
//Serial.print(",");
Serial.print("\t ,MA_Cmd:\t");
Serial.print(DC_MotorA_SpeedCommand);
//Serial.print(",");
Serial.print("\t ,MA_Fb:\t");
Serial.print(DC_MotorA_SpeedFeedback);
//Serial.print(",");
Serial.print("\t ,MA_PWM:\t");
Serial.print(ControlLoop_Y.DC_MotorA_PWM);
//Serial.print(",");
Serial.print("\t ,MB_Cmd:\t");
Serial.print(DC_MotorB_SpeedCommand);
//Serial.print(",");
Serial.print("\t ,MB_Fb:\t");
Serial.print(DC_MotorB_SpeedFeedback);
//Serial.print(",");
Serial.print("\t ,MB_PWM:\t");
Serial.print(ControlLoop_Y.DC_MotorB_PWM);
*/
// Serial.print("\t ,DistA:\t");
// Serial.print(UltrasoundDistanceA);
// Serial.print("\t ,ServoA_O:\t");
// Serial.print(ServoMotorA_Angle);

/*
Serial.print("DistA:");
Serial.print(UltrasoundDistanceA);
Serial.print("DistB:");
Serial.print(LidarDistanceA);
Serial.print(",ServoA_O:");
Serial.print(ServoMotorA_Angle);
*/

// Serial.print("\t, State:");
// Serial.print(state);

// Serial.print("\t,CurrentAngle:");
// Serial.print(RoverGlobalDirection * 180 / 3.14159);

// Serial.print("\t,Current Location, X:");
// Serial.print(RoverGlobalCoordX);

// Serial.print(", Y:");
// Serial.print(RoverGlobalCoordY);

// Serial.print("\t, Target X:");
// Serial.print(StateflowBlock_Y.PointX);

// Serial.print(", Y:");
// Serial.print(StateflowBlock_Y.PointY);

// for(int i=0;i<180;i++)
//{
//   Serial.print("\t");
//   Serial.print(ForwardDistanceArray[i]);
// }

////============Print a message if either timer timeout (couldn't finish the code before next interrupt)//
//  100% means the interrupt function will NOT have enough time to execute
// PrintIfTimerTimeout();   // You can comment out this one during your test. Just print this from time to time to make sure your code doesn't overload the micro processor
//================================================================================//
#ifdef USE_RASPBERRY_PI
  RoverGlobalMsg.processingTick();
#endif
}

// Try not to print anything in the below function,
// use the non real time loop above to print stuff because the Serail.print() takes a long time

void InterruptFunction1() // Higher Priority, Called by Timer 1   // Could be used for hardware control
{                         // This function runs at 200 Hz, used for motor control loop

  ControlLoop_U.DC_MotorA_SpeedCommand = DC_MotorA_SpeedCommand;   // Send the desired motor speed to the control loop
  ControlLoop_U.DC_MotorB_SpeedCommand = DC_MotorB_SpeedCommand;   // Send the desired motor speed to the control loop
  ControlLoop_U.DC_MotorA_Encoder_Counter = MotorA_EncoderCounter; // Send the motor encoder counter feedback
  ControlLoop_U.DC_MotorB_Encoder_Counter = MotorB_EncoderCounter; // Send the motor encoder counter feedback
  ControlLoop_U.UltrasoundDistanceA = UltrasoundDistanceA;
  ControlLoop_U.DistanceA_command = DistanceA_command;
  ControlLoop_U.KeepDistanceA_Enable_Bool = KeepDistanceA_Enable_Bool;
  // ControlLoop_U.ServoA_Sweeping_Enable_Bool = ServoA_Sweeping_Enable_Bool;
  ControlLoop_U.DesiredRoverGlobalDirection = DesiredRoverGlobalDirection;
  ControlLoop_U.PurePursuitVel = PurePursuitVel;
  ControlLoop_U.PurePursuitOmega = PurePursuitOmega;
  ControlLoop_U.PurePursuitEnableBool = PurePursuitEnableBool;

  ControlLoop_step(); // This runs your "ControlLoop" simulink loop

  DriveMotorA(ControlLoop_Y.DC_MotorA_PWM);                        // output the calculated value from -1 to 1 to the motor
  DriveMotorB(ControlLoop_Y.DC_MotorB_PWM);                        // output the calculated value from -1 to 1 to the motor
  DC_MotorA_SpeedFeedback = ControlLoop_Y.DC_MotorA_SpeedFeedback; // Acquire the motor speed feedback
  DC_MotorB_SpeedFeedback = ControlLoop_Y.DC_MotorB_SpeedFeedback; // Acquire the motor speed feedback
  // ServoMotorA_Angle = ControlLoop_Y.ServoMotorA_Angle;
  RoverGlobalDirection = ControlLoop_Y.RoverGlobalDirection;
  RoverGlobalCoordX = ControlLoop_Y.RoverGlobalCoordX;
  RoverGlobalCoordY = ControlLoop_Y.RoverGlobalCoordY;
  ///// Output the servo motor command PWM signals
  OutputServoPWM_Timer1(ServoMotorA_Pin, ServoMotorA_Angle); // Used when Servo pin is 11, 12, or 13
  OutputServoPWM_Timer1(ServoMotorB_Pin, ServoMotorB_Angle); // Used when Servo pin is 11, 12, or 13
  OutputServoPWM_Timer3(ServoMotorC_Pin, ServoMotorC_Angle); // Used when Serno pin is 2, 3, or 5
  OutputServoPWM_Timer3(ServoMotorD_Pin, ServoMotorD_Angle); // Used when Serno pin is 2, 3, or 5
  OutputServoPWM_Timer3(ServoMotorE_Pin, ServoMotorE_Angle); // Used when Serno pin is 2, 3, or 5
  return;
}
void InterruptFunction2() // Middle Priority, Called by Timer 3
{                         // This function runs at 20 Hz, used for sensor feedback and communication

  ///////////Reading the Ultrasonic sensor
  uint16_t CurrentUltrasoundDistanceA = 0;
#define LOWPASS_Const 0.8 // The less the more it filter out, slower respond
#ifdef USE_ULTRASONIC_SENSOR
  myUltrasonic.triggerAndRead(CurrentUltrasoundDistanceA);
  UltrasoundDistanceA_O = CurrentUltrasoundDistanceA;
  if (CurrentUltrasoundDistanceA == 0)
  {
    // UltrasoundDistanceA = CurrentUltrasoundDistanceA; // If the reading is not zero, update it
    //  Add low pass filter for the ultrasonic sensor reading
    CurrentUltrasoundDistanceA = 1500;
  }
  UltrasoundDistanceA = (UltrasoundDistanceA + CurrentUltrasoundDistanceA * LOWPASS_Const) / (1 + LOWPASS_Const); // If the reading is not zero, update it
#else
  UltrasoundDistanceA = 0;
#endif
  ////////////////////////////////////End of reading Ultrasonic sensor

  /////////Reading Lidar
#ifdef USE_1D_LIDAR
  if (!myLIDAR.getBusyFlag())
  {
    LidarDistanceA = 10 * myLIDAR.readDistance(); // Read and make it mm
    myLIDAR.takeRange();
  }
  // LidarDistanceA = myLIDAR.getDistance();
#endif

  // #ifdef USE_RASPBERRY_PI
  //   RoverGlobalMsg.processingTick();
  // #endif
  return;
}
void InterruptFunction3() // Lower Priority, Called by Timer 4  // Could be used for state machine, navigation logic, etc
{                         // This function runs at 10 Hz, used for strategy, navigation...
  // delay(60);
  //========================Feed the input signals==================================//
  StateflowBlock_U.accel_button_in = !digitalRead(ACCEL_BUTTON);
  StateflowBlock_U.off_button_in = !digitalRead(OFF_BUTTON);
  StateflowBlock_U.pause_button_in = !digitalRead(PAUSE_BUTTON);
  StateflowBlock_U.on_button_in = !digitalRead(ON_BUTTON);
  StateflowBlock_U.decel_button_in = !digitalRead(DECEL_BUTTON);
  StateflowBlock_U.UltrasoundDistanceA = UltrasoundDistanceA;
  // StateflowBlock_U.UltrasoundDistanceA = LidarDistanceA;
  StateflowBlock_U.DC_MotorA_SpeedFeedback = DC_MotorA_SpeedFeedback;
  StateflowBlock_U.DC_MotorB_SpeedFeedback = DC_MotorB_SpeedFeedback;
  StateflowBlock_U.RoverGlobalDirection = RoverGlobalDirection;
  StateflowBlock_U.RoverGlobalCoordX = RoverGlobalCoordX;
  StateflowBlock_U.RoverGlobalCoordY = RoverGlobalCoordY;

  //================================================================================//

  //==========================Run the simulink program=============================//
  StateflowBlock_step();
  //================================================================================//

  //===========================Take out the output signals==========================//
  // put the output desired motor speed to global variables that can be accessed from other funtion
  DC_MotorA_SpeedCommand = StateflowBlock_Y.DC_MotorA_SpeedCommand;
  DC_MotorB_SpeedCommand = StateflowBlock_Y.DC_MotorB_SpeedCommand;
  state = StateflowBlock_Y.state;
  DistanceA_command = StateflowBlock_Y.DistanceA_command;
  KeepDistanceA_Enable_Bool = StateflowBlock_Y.KeepDistanceA_Enable_Bool;
  // ServoA_Sweeping_Enable_Bool = StateflowBlock_Y.ServoA_Sweeping_Enable_Bool;
  DesiredRoverGlobalDirection = StateflowBlock_Y.DesiredRoverGlobalDirection;
  TargetDirection = StateflowBlock_Y.TargetDirection;
  ServoMotorA_Angle = StateflowBlock_Y.ServoMotorA_Angle;
  PurePursuitVel = StateflowBlock_Y.PurePursuitVel;
  PurePursuitOmega = StateflowBlock_Y.PurePursuitOmega;
  PurePursuitEnableBool = StateflowBlock_Y.PurePursuitEnableBool;
  //================================================================================//

  return;
}
