#include <SoftwareSerial.h>
//-----------------------------------------------------------------
//--------------------VARIABLE DECLARATION-------------------------
//-----------------------------------------------------------------
int VOLTAGE_SENSOR_BATT = 0;
int CURRENT_SENSOR_BATT = 1;
int SPEED_SENSOR_PIN = 5;
int RPM_SENSOR_PIN = 6;
int VALUE_BUFFER;
int NOM_CURRENT = 50;
int SAMPLES = 15;
float V_BATT;
float I_BATT;
float KNOTS = 0;
float V_REF = 2.5;
float RPM = 0;
float PULSE_SAMPLE;
float PULSE_KNOTS;
float PULSE_RPM;
float V_SENSE_CS;
char tempString[10];  //Will be used with sprintf to create strings
//-----------------------------------------------------------------
//---------------------SOFTWARE SERIAL 1---------------------------
//-----------------------------------------------------------------
const int SWTx1 = 8; //Define Tx port
const int SWRx1 = 51; //Define Rx port
SoftwareSerial s7s(SWRx1 , SWTx1); //Function for Serial 7 Segment Display1
//-----------------------------------------------------------------
//---------------------SOFTWARE SERIAL 2---------------------------
//-----------------------------------------------------------------
const int SWTx2 = 9; //Define Tx port
const int SWRx2 = 52; //Define Rx port
SoftwareSerial s7s2(SWRx2, SWTx2); //Function for Seria 7 Segment Display 2
//-----------------------------------------------------------------
//---------------------SOFTWARE SERIAL 3---------------------------
//-----------------------------------------------------------------
const int SWTx3 = 10; //Define Tx port
const int SWRx3 = 53; //Define Rx port
SoftwareSerial s7s3(SWRx3, SWTx3); //Function for Seria 7 Segment Display
//-----------------------------------------------------------------
//-------------------PORTS&PERIPHERALS SETUP-----------------------
//-----------------------------------------------------------------
void setup()
{
  //-----------------------------------------------------------------
  //------------------------PIN SETUP--------------------------------
  //-----------------------------------------------------------------
  pinMode(SPEED_SENSOR_PIN, INPUT); //Set digital pin to INPUT
  pinMode(RPM_SENSOR_PIN, INPUT); //Set digital pin to INPUT
  //-----------------------------------------------------------------
  //-----------------------SERIAL SETUP------------------------------
  //-----------------------------------------------------------------
  s7s.begin(9600); //Start serial peripheral
  s7s2.begin(9600); //Start serial peripheral
  s7s3.begin(9600); //Start serial peripheral
  //-----------------------------------------------------------------
  //-----------------------DISPLAY SETUP-----------------------------
  //-----------------------------------------------------------------
  clearDisplay();  //Clears display, resets cursor
  setBrightness(255);  //Max brightness
  delay(1500);
  clearDisplay(); //Clear the display before jumping into loop
}
//-----------------------------------------------------------------
//----------------------------MAIN LOOP----------------------------
//-----------------------------------------------------------------
void loop()
{
  //-----------------------------------------------------------------
  //-----------------------ANALOG READING----------------------------
  //-----------------------------------------------------------------
  V_BATT = analogRead(VOLTAGE_SENSOR_BATT); //Read ADC0
  V_BATT = (V_BATT * 5) / 1023; //Adjust reading to get actual value
  V_SENSE_CS = analogRead(CURRENT_SENSOR_BATT); // Read ADC1
  V_SENSE_CS = (V_SENSE_CS * 5) / 1023; //Adjust reading to get actual value
  I_BATT = (V_SENSE_CS - V_REF) * NOM_CURRENT * 1.6; //Adjust V_SENSE_CS to appropiate current reading
  //-----------------------------------------------------------------
  //-------------------V_BATT DISPLAY ALGORITHM----------------------
  //-----------------------------------------------------------------
  VALUE_BUFFER = V_BATT * 1000; //Convert V_BATT from float to int data type
  sprintf(tempString, "%4d", VALUE_BUFFER); //Convert VALUE_BUFFER to char array
  s7s.print(tempString); //Display V_BATT
  setDecimals1(0b00000001); //Set fixed decimal point
  //-----------------------------------------------------------------
  //-------------------I_BATT DISPLAY ALGORITHM----------------------
  //-----------------------------------------------------------------
  VALUE_BUFFER = I_BATT; //Convert I_BATT from float to int data type
  sprintf(tempString, "%4d", VALUE_BUFFER); //Convert VALUE_BUFFER to char array
  s7s2.print(tempString); //Display I_BATT
  //-----------------------------------------------------------------
  //-------------------SPEED SAMPLING LOOP---------------------------
  //-----------------------------------------------------------------
  for (int j = 0; j < SAMPLES; j++) //Speed sampling loop 
  {
    PULSE_KNOTS = 2 * pulseIn(SPEED_SENSOR_PIN, HIGH, 50000); //Get signal period
    if (PULSE_KNOTS != 0) //Verify if there was pulse measured
    {
      KNOTS += 1000000 / PULSE_KNOTS; //Store measured frequency
    }
    else //No pulse at input
    {
      KNOTS = 0; //Set speed value to 0
      j = SAMPLES; //End sampling loop
    }
  }
  KNOTS = KNOTS / SAMPLES; //Get average frequency

  //-----------------------------------------------------------------
  //---------------------RPM SAMPLING LOOP---------------------------
  //-----------------------------------------------------------------
  for (int i = 0; i < SAMPLES; i++) //RPM sampling loop
  {
    PULSE_RPM = 2 * pulseIn(RPM_SENSOR_PIN, HIGH, 50000); //Get signal period
    if (PULSE_RPM != 0) //Verify if there was pulse measured
    {
      RPM += 1000000 / PULSE_RPM; //Store mezsured frequency
    }
    else //No pulse at input
    {
      RPM = 0; //Set RPM value to 0
      i = SAMPLES; //End sampling loop
    }
  }
  RPM = RPM / SAMPLES; //Get average frequency
  //-----------------------------------------------------------------
  //--------------------KNOTS DISPLAY ALGORITHM----------------------
  //-----------------------------------------------------------------
  VALUE_BUFFER = KNOTS / 4.8; //Convert KNOTS from float to int data type
  sprintf(tempString, "%4d", VALUE_BUFFER); //Convert VALUE_BUFFER to char array
  s7s3.print(tempString); //Display KNOTS
}
void setBrightness(byte value)
{
  s7s.write(0x7A);  // Set brightness command byte
  s7s.write(value);  // brightness data byte
  s7s2.write(0x7A);  // Set brightness command byte
  s7s2.write(value);  // brightness data byte
  s7s3.write(0x7A);  // Set brightness command byte
  s7s3.write(value);  // brightness data byte
}
void clearDisplay()
{
  s7s.write(0x76);  // Clear display command
  s7s2.write(0x76);  // Clear display command
  s7s3.write(0x76);  // Clear display command
}
void setDecimals1(byte decimals)
{
  s7s.write(0x77); //Clear display command
  s7s.write(decimals); //Set decimal point
}
void setDecimals2(byte decimals)
{
  s7s2.write(0x77);  //Clear display command
  s7s2.write(decimals);  //Set decimal point
}
void setDecimals3(byte decimals)
{
  s7s3.write(0x77);  //Clear display command
  s7s3.write(decimals);  //Set decimal point
}
