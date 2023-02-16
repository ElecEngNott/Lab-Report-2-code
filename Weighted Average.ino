#include <Wire.h>
#define I2C_SLAVE_ADDR 0x04

int IR1 = 27;  //Set up variables for each sensor to each pin on ESP32
int IR2 = 26;
int IR3 = 25;
int IR4 = 33;
int IR5 = 32;
int IR6 = 35;

void setup() {
  Serial.begin(9600);
  Wire.begin();  
}

void loop() {
  int SteeringAngle;
  float Xpk;
  Xpk = WeightedAverage();
  SteeringAngle = 90 - (55 * Xpk);  // 55 represents range of degrees in a full sweep
  Serial.println(Xpk); //print Xpk to check error whilst car is moving 
  if (SteeringAngle >= 120) { SteeringAngle = 120; }  //Make sure there is no damage to car
  if (SteeringAngle <= 60) { SteeringAngle = 65; }    //Make sure there is no damage to car
  ChangeDirection(90, 90, SteeringAngle);
}

void ChangeDirection(int leftMotor, int rightMotor, int SteeringAngle) {

  Wire.beginTransmission(I2C_SLAVE_ADDR); 
  Wire.write((byte)((leftMotor & 0x0000FF00) >> 8));   
  Wire.write((byte)(leftMotor & 0x000000FF));          
  Wire.write((byte)((rightMotor & 0x0000FF00) >> 8)); 
  Wire.write((byte)(rightMotor & 0x000000FF));         
  Wire.write((byte)((SteeringAngle & 0x0000FF00) >> 8));
  Wire.write((byte)(SteeringAngle & 0x000000FF));
  Wire.endTransmission();
}

float WeightedAverage() {
  int I1, I2, I3, I4, I5, I6;
  float XPK, sumtop, sumbottom;

  I1 = analogRead(IR1);
  I2 = analogRead(IR2);
  I3 = analogRead(IR3);
  I4 = analogRead(IR4);
  I5 = analogRead(IR5);
  I6 = analogRead(IR6);

  sumtop = (-3 * (I1 - I6)) + (-2 * (I2 - I5)) + (-1 * (I3 - I4));
  sumbottom = (I1 + I2 + I3 + I4 + I5 + I6);
  XPK = (sumtop / sumbottom);
  return (XPK);
}

