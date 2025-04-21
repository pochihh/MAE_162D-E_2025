

#define DeadZone 0.09
#define Saturation 0.95
float DriveMotorA(float Power);
float DriveMotorB(float Power);

void DC_MotorInit()
{
  pinMode(DC_MotorA_Enable_Pin, OUTPUT);
  pinMode(DC_MotorA_In1_Pin, OUTPUT);
  pinMode(DC_MotorA_In2_Pin, OUTPUT);
  pinMode(DC_MotorB_Enable_Pin, OUTPUT);
  pinMode(DC_MotorB_In1_Pin, OUTPUT);
  pinMode(DC_MotorB_In2_Pin, OUTPUT);
  DriveMotorA(0);
  DriveMotorB(0);
  TCCR2A = 0b10100001; 
  TCCR2B = 0b00000010; // CS2:0 = 010 : timer speed : 16M/8 = 2M
  // PWM frequency = 2M/256/2 ~= 3.9 kHz
}

float DriveMotorA(float Power)
{
  float ActualInput = Power;
  if(Power <= DeadZone && Power >= -1*DeadZone) // In the dead zone
  { 
    ActualInput = 0;
    digitalWrite(DC_MotorA_In1_Pin, LOW);
    digitalWrite(DC_MotorA_In2_Pin, LOW);
    OCR2A = 0;
  }
  else if (Power > DeadZone && Power < Saturation)
  {
    digitalWrite(DC_MotorA_In1_Pin, LOW);
    digitalWrite(DC_MotorA_In2_Pin, HIGH);
    //analogWrite(DC_MotorA_Enable_Pin, int(255*ActualInput));
    OCR2A = int(255*ActualInput);
  }
  else if (Power >= Saturation) // Saturated
  {
    ActualInput = Saturation;
    digitalWrite(DC_MotorA_In1_Pin, LOW);
    digitalWrite(DC_MotorA_In2_Pin, HIGH);
    //analogWrite(DC_MotorA_Enable_Pin, int(255*ActualInput));
    OCR2A = int(255*ActualInput);
  }
  else if (Power < -1*DeadZone && Power > -1*Saturation)
  {
    digitalWrite(DC_MotorA_In1_Pin, HIGH);
    digitalWrite(DC_MotorA_In2_Pin, LOW);
    //analogWrite(DC_MotorA_Enable_Pin, int(-255*ActualInput));
    OCR2A = int(-255*ActualInput);
  }
  else if (Power <= -1*Saturation) // Saturated
  {
    ActualInput = -1*Saturation;
    digitalWrite(DC_MotorA_In1_Pin, HIGH);
    digitalWrite(DC_MotorA_In2_Pin, LOW);
    //analogWrite(DC_MotorA_Enable_Pin, int(-255*ActualInput));
    OCR2A = int(-255*ActualInput);
  }
  return ActualInput;
}

float DriveMotorB(float Power)
{
  float ActualInput = Power;
  if(Power <= DeadZone && Power >= -1*DeadZone) // In the dead zone
  { 
    ActualInput = 0;
    digitalWrite(DC_MotorB_In1_Pin, LOW);
    digitalWrite(DC_MotorB_In2_Pin, LOW);
    OCR2B = 0;
  }
  else if (Power > DeadZone && Power < Saturation)
  {
    digitalWrite(DC_MotorB_In1_Pin, LOW);
    digitalWrite(DC_MotorB_In2_Pin, HIGH);
    //analogWrite(DC_MotorB_Enable_Pin, int(255*ActualInput));
    OCR2B = int(255*ActualInput);
  }
  else if (Power >= Saturation) // Saturated
  {
    ActualInput = Saturation;
    digitalWrite(DC_MotorB_In1_Pin, LOW);
    digitalWrite(DC_MotorB_In2_Pin, HIGH);
    //analogWrite(DC_MotorB_Enable_Pin, int(255*ActualInput));
    OCR2B = int(255*ActualInput);
  }
  else if (Power < -1*DeadZone && Power > -1*Saturation)
  {
    digitalWrite(DC_MotorB_In1_Pin, HIGH);
    digitalWrite(DC_MotorB_In2_Pin, LOW);
    //analogWrite(DC_MotorB_Enable_Pin, int(-255*ActualInput));
    OCR2B = int(-255*ActualInput);
  }
  else if (Power <= -1*Saturation) // Saturated
  {
    ActualInput = -1*Saturation;
    digitalWrite(DC_MotorB_In1_Pin, HIGH);
    digitalWrite(DC_MotorB_In2_Pin, LOW);
    //analogWrite(DC_MotorB_Enable_Pin, int(-255*ActualInput));
    OCR2B = int(-255*ActualInput);
  }
  return ActualInput;
}
