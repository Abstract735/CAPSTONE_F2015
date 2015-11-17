#include <SoftwareSerial.h>
//-----------------------------------------------------------------
//--------------------VARIABLE DECLARATION-------------------------
//-----------------------------------------------------------------
int VOLTAGE_SENSOR_BATT = 2;
int CURRENT_SENSOR_BATT = 3;
int CURRENT_SENSOR_PV = 4;
int SPEED_SENSOR_PIN = 12;
int RPM_SENSOR_PIN = 11;
int NOM_CURRENT = 50;
int SAMPLES = 15;
float V_BATT;
float I_BATT;
float V_REF = 2.5;
float V_SENSE_CS;
float KNOTS = 0;
float RPM = 0;
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
  Serial.begin(9600);
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
  V_BATT = analogRead(VOLTAGE_SENSOR_BATT); //Read ADC0
  V_BATT = ((V_BATT * 50) / 1023)*100; //Adjust reading to get actual value
  V_SENSE_CS = analogRead(CURRENT_SENSOR_BATT); // Read ADC1
  V_SENSE_CS = (V_SENSE_CS * 5) / 1023; //Adjust reading to get actual value
  I_BATT = (V_SENSE_CS - V_REF) * NOM_CURRENT * 1.6; //Adjust V_SENSE_CS to appropiate current reading
  Display(I_BATT);
  KNOTS = (SAMPLING(SPEED_SENSOR_PIN, 20) / 4.8)*10;
  RPM = SAMPLING(RPM_SENSOR_PIN, 20);
  setDecimals2(0b00000100); //Set fixed decimal point
  Display2(KNOTS);
  setDecimals3(0b00000010); //Set fixed decimal point
  Display3(V_BATT);
}
float SAMPLING(int SENSOR_PIN, int SAMPLES)
{
  float VALUE = 0;
  float PULSE_VALUE = 0;
  for (int j = 0; j < SAMPLES; j++) //Speed sampling loop
  {
    PULSE_VALUE = 2 * pulseIn(SENSOR_PIN, HIGH, 50000); //Get signal period
    if (PULSE_VALUE != 0) //Verify if there was pulse measured
    {
      VALUE += 1000000 / PULSE_VALUE; //Store measured frequency
    }
    else //No pulse at input
    {
      VALUE = 0; //Set value to 0
      j = SAMPLES; //End sampling loop
    }
  }
  VALUE = VALUE / SAMPLES; //Get average frequency
  return VALUE;
}
void Display(int VALUE_BUFFER)
{
  sprintf(tempString, "%4d", VALUE_BUFFER); //Convert VALUE_BUFFER to char array
  s7s.print(tempString); //Send to display #1
}
void Display2(int VALUE_BUFFER)
{
  sprintf(tempString, "%4d", VALUE_BUFFER); //Convert VALUE_BUFFER to char array
  s7s2.print(tempString); //Send to display #2
}
void Display3(int VALUE_BUFFER)
{
  sprintf(tempString, "%4d", VALUE_BUFFER); //Convert VALUE_BUFFER to char array
  s7s3.print(tempString); //Send to display #3
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
