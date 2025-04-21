
#define Priority0 0
#define Priority1 1
#define Priority2 2
#define Priority3 3
#define Priority4 4
#define Priority5 5
#define Priority6 6
#define Priority7 7

#define ServoMotorPulseLowerLimit 500 // micro second
#define ServoMotorPulseUpperLimit 2450 // micro second
#define ServoMotorRotateAngleLimit 180 // micro second

//#define PRINT_TIMER_FREQUENCY
//#define PRINT_TIMER_STATUS
#define PRINT_TIMER_LOADING

unsigned char InterruptRunningFlag = 0b00000000; // All eight flags are clear, higher bit has higher priority
// When running flag is set, means that interrupt is currently running

int TimerTimeout = 0; // if not zero, means that certain timer is up before being able to finish the code.
void InterruptFunction1();
void InterruptFunction2();
void InterruptFunction3();
unsigned long Interrupt1Period, Interrupt1SpentTime, TimerTemp1A, TimerTemp1B;
unsigned long Interrupt2Period, Interrupt2SpentTime, TimerTemp2A, TimerTemp2B;
unsigned long Interrupt3Period, Interrupt3SpentTime, TimerTemp3A, TimerTemp3B;
boolean CheckBit = 0;

void TimerAndInterruptSetup()
{
  //Timer 1 setup    // Control pin 11, 12, 13
  TCCR1A = 0b10100010;
  TCCR1B = 0b00011010; // Set WGM3:0 to be 0b1110: Fast PWM mode; CS2:0 to be 0b010, Prescaler = 8 -> timer ticks at 16Mhz/8 = 2Mhz
  ICR1 = 10000; // Timer counter overflow at 2Mhz/10000 = 200 Hz
  //ICR1 = 2000; // Timer counter overflow at 2Mhz/2000 = 1000 Hz
  TIMSK1 = 0b00000001; // Enable Timer Interrupt
  //End of Timer 1 setup
  
  //Timer 3 setup  //  control pin 2, 3, 5
  TCCR3A = 0b10101010;
  TCCR3B = 0b00011011; // Set WGM3:0 to be 0b1110: Fast PWM mode; CS2:0 to be 0b011, Prescaler = 64 -> timer ticks at 16Mhz/64 = 250kHz
  //ICR3 = 2500;  // 100Hz 
  ICR3 = 12500;   // 20Hz
  //ICR3 = 5000;   // 50Hz
  TIMSK3 = 0b00000001; // Enable Timer Interrupt
  //End of Timer 3 setup
  
  //Timer 4 setup
  TCCR4A = 0b00000010;
  TCCR4B = 0b00011101; // Set WGM3:0 to be 0b1110: Fast PWM mode; CS2:0 to be 0b101, Prescaler = 1024 -> timer ticks at 16Mhz/1024 = 15625Hz
  //ICR4 = 31250; // Timer counter overflow at 15625/31250 = 0.5Hz
  ICR4 = 1562; // Timer counter overflow at 15625/1562 ~= 10Hz
  TIMSK4 = 0b00000001; // Enable Timer Interrupt
  //End of Timer 4 setup

  // Setup Pin Change Interrupt
  PCICR = 0b0000110; // Set PCIE1 = HIGH, PCIE2 = HIGH
  PCMSK1 = 0b00000110; // Set PCINT10 and PCINT11 = HIGH 
  PCMSK2 = 0b11000000; // Set PCINT22 and PCINT23 = HIGH 
}

void PrintIfTimerTimeout()
{
#ifdef  PRINT_TIMER_FREQUENCY
  Serial.print("Interrupt 1 frequency: ");
  Serial.print(1/(Interrupt1Period *1e-6));
  Serial.print(" Hz  ");
  Serial.print("Interrupt 2 frequency: ");
  Serial.print(1/(Interrupt2Period *1e-6));
  Serial.print(" Hz  ");
  Serial.print("Interrupt 3 frequency: ");
  Serial.print(1/(Interrupt3Period *1e-6));
  Serial.println(" Hz");
#endif

#ifdef PRINT_TIMER_STATUS
  Serial.print("Timer 1 period: ");
  Serial.print(Interrupt1Period);
  Serial.print(",spent: ");
  Serial.print(Interrupt1SpentTime);
  Serial.print(", Timer 2 period: ");
  Serial.print(Interrupt2Period);
  Serial.print(",spent: ");
  Serial.print(Interrupt2SpentTime);
  Serial.print(", Timer 3 period: ");
  Serial.print(Interrupt3Period);
  Serial.print(",spent: ");
  Serial.println(Interrupt3SpentTime);
  delay(500);
#endif

#ifdef PRINT_TIMER_LOADING
  Serial.print("\t");
  Serial.print("Timer 1 Load: ");
  Serial.print(100*Interrupt1SpentTime/Interrupt1Period);
  Serial.print("%  ");
  Serial.print("Timer 2 Load: ");
  Serial.print(100*Interrupt2SpentTime/Interrupt2Period);
  Serial.print("%  ");
  Serial.print("Timer 3 Load: ");
  Serial.print(100*Interrupt3SpentTime/Interrupt3Period);
  Serial.print("%");
  Serial.print("\t");
  delay(500);
#endif
if(CheckBit)
{
  Serial.println("Check");
}
}


ISR(TIMER1_OVF_vect) // Timer1 overflow will automatically trigger this function
{
  sei(); // Allow this function to be interrupted
  TimerTemp1A = micros();
  if(InterruptRunningFlag>>Priority2) 
  {
    TCNT1 = ICR1 - 1; // Set the timer counter very close to the TOP so it could be triggered as soon as possible once the higher priority function is done
    //Serial.println(TCNT1);
    //CheckBit = 1;
    return 0; //if there are higher or same priority interrupt not done yet
  }
  else // No higher or same prioirity interrupt running, we can proceed.
  {
    InterruptRunningFlag |= (1<<Priority2); // Set the running flag for bit 2
    InterruptFunction1();
    InterruptRunningFlag &= ~(1<<Priority2); // clear the running flag for bit 2
  }
  Interrupt1SpentTime = micros() - TimerTemp1A;
  Interrupt1Period = micros() - TimerTemp1B;
  TimerTemp1B = micros();
  return 1;
}
ISR(TIMER3_OVF_vect) // Timer3 overflow will automatically trigger this function
{
  //TCNT3 = 34826; // has to be declared in the code
  sei(); // Allow this function to be interrupted
  TimerTemp2A = micros();
  if(InterruptRunningFlag>>Priority1) 
  {
    TCNT3 = ICR3 - 1; // Set the timer counter very close to the TOP so it could be triggered as soon as possible once the higher priority function is done
    return 0; //if there are higher or same priority interrupt not done yet
  }
  else // No higher or same prioirity interrupt running, we can proceed.
  {
    InterruptRunningFlag |= (1<<Priority1); // Set the running flag for bit 1
    InterruptFunction2();
    InterruptRunningFlag &= ~(1<<Priority1); // clear the running flag for bit 1
  }
  Interrupt2SpentTime = micros() - TimerTemp2A;
  Interrupt2Period = micros() - TimerTemp2B;
  TimerTemp2B = micros();
  return 1;
}
ISR(TIMER4_OVF_vect) // Timer4 overflow will automatically trigger this function
{
  sei(); // Allow this function to be interrupted
  TimerTemp3A = micros();
  if(InterruptRunningFlag>>Priority0) 
  {
    //CheckBit = 1;
    TCNT4 = ICR4 - 1; // Set the timer counter very close to the TOP so it could be triggered as soon as possible once the higher priority function is done
    return 0; //if there are higher or same priority interrupt not done yet
  }
  else // No higher or same prioirity interrupt running, we can proceed.
  {
    InterruptRunningFlag |= (1<<Priority0); // Set the running flag for bit 0
    InterruptFunction3();
    InterruptRunningFlag &= ~(1<<Priority0); // clear the running flag for bit 0
  }
  Interrupt3SpentTime = micros() - TimerTemp3A;
  Interrupt3Period = micros() - TimerTemp3B;
  TimerTemp3B = micros();
  return 1;
}


// Encoder Reading
long MotorA_EncoderCounter = 0;
long MotorB_EncoderCounter = 0;
unsigned char LastMotorAEncoderState = 0;//0~3;
unsigned char LastMotorBEncoderState = 0;//0~3;


ISR(PCINT1_vect)  // Motor A encoder Pin change is detected
{
  unsigned char PA = digitalRead(DC_MotorA_Encoder_PinA);
  unsigned char PB = digitalRead(DC_MotorA_Encoder_PinB);
  unsigned char LastEncoderState = LastMotorAEncoderState;
  unsigned char EncoderState = PA<<1 + PB;
  int Diff = 0;
  switch(LastEncoderState)
  {// 013201320132
    case 0:
      if(EncoderState==1) Diff = 1;
      else if(EncoderState==2) Diff = -1;
      break;
    case 1:   
      if(EncoderState==3) Diff = 1;
      else if(EncoderState==0) Diff = -1;
      break;
    case 2:
      if(EncoderState==0) Diff = 1;
      else if(EncoderState==3) Diff = -1;
      break;
    case 3:
      if(EncoderState==2) Diff = 1;
      else if(EncoderState==1) Diff = -1;
      break;
    default:
      break;
  }
  LastMotorAEncoderState = EncoderState;
  MotorA_EncoderCounter += Diff;
}


ISR(PCINT2_vect)  // Motor B encoder Pin change is detected
{
  unsigned char PA = digitalRead(DC_MotorB_Encoder_PinA);
  unsigned char PB = digitalRead(DC_MotorB_Encoder_PinB);
  unsigned char LastEncoderState = LastMotorBEncoderState;
  unsigned char EncoderState = PA<<1 + PB;
  int Diff = 0;
  switch(LastEncoderState)
  {// 013201320132
    case 0:
      if(EncoderState==1) Diff = 1;
      else if(EncoderState==2) Diff = -1;
      break;
    case 1:   
      if(EncoderState==3) Diff = 1;
      else if(EncoderState==0) Diff = -1;
      break;
    case 2:
      if(EncoderState==0) Diff = 1;
      else if(EncoderState==3) Diff = -1;
      break;
    case 3:
      if(EncoderState==2) Diff = 1;
      else if(EncoderState==1) Diff = -1;
      break;
    default:
      break;
  }
  LastMotorBEncoderState = EncoderState;
  MotorB_EncoderCounter += Diff;
}

void OutputServoPWM_Timer1(int PinNumber, float Angle) // PinNumber has to be 11, 12 or 13 , Angle from 0 to 180
{
  // Timer 1 frequency is 2Mhz, every ticking is 1/2M = 0.5 us
  // To generate PWM pulse width between lower limit, first we calculate how much the compare register should be
  int LowerLimit = ServoMotorPulseLowerLimit/0.5;
  int UpperLimit = ServoMotorPulseUpperLimit/0.5;
  if (Angle > ServoMotorRotateAngleLimit) Angle = ServoMotorRotateAngleLimit;
  else if (Angle < 0) Angle = 0;
  int reges = (Angle/ServoMotorRotateAngleLimit)*(UpperLimit-LowerLimit) + LowerLimit;
  switch(PinNumber)
  {
    case 11:
      OCR1A = reges;
      break;
    case 12:
      OCR1B = reges;
      break;
    case 13:
      OCR1C = reges;
      break;
    default:
      break;
  }
  return;
}


void OutputServoPWM_Timer3(int PinNumber, float Angle) // PinNumber has to be 2, 3 or 5 , Angle from 0 to 180
{
  // Timer 1 frequency is 250kHz, every ticking is 1/250k = 4 us
  // To generate PWM pulse width between lower limit, first we calculate how much the compare register should be
  int LowerLimit = ServoMotorPulseLowerLimit/4;
  int UpperLimit = ServoMotorPulseUpperLimit/4;
  if (Angle > ServoMotorRotateAngleLimit) Angle = ServoMotorRotateAngleLimit;
  else if (Angle < 0) Angle = 0;
  int reges = (Angle/ServoMotorRotateAngleLimit)*(UpperLimit-LowerLimit) + LowerLimit;
  switch(PinNumber)
  {
    case 5:
      OCR3A = reges;
      break;
    case 2:
      OCR3B = reges;
      break;
    case 3:
      OCR3C = reges;
      break;
    default:
      break;
  }
  return;
}
