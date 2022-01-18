/* Wearable Posture Corrector Device
 *  1/18/2022   David Cheng
 *  Description: Alarm user when slouching for 7 consecutive seconds
 */

#include "sefr.h"
#include <Wire.h>

// Settings
static const uint16_t sampling_rate = 300;
static const uint8_t samples = 10;
static const uint8_t sample_delay = sampling_rate / samples;

float sensorVals[samples][3];
float ml_features[6];
int ml_label;
int contSlouches;

long accelX, accelY, accelZ;  // raw accel sensor values
float gForceX, gForceY, gForceZ;  // accel converted to [g], g = 9.8 m/s^2

// Functions
void setupMPU();
void recordAccelRegisters_MPU();
void getDurationData();
void calcFeatures();
void printData();
void printFeatures();
Eloquent::ML::Port::SEFR clf;

void setup()
{
  CLKPR = 0x80; // enable CLKPCE
  CLKPR = 0x04; // clock prescaler select
  Serial.begin(115200);  
  Wire.begin();
  setupMPU();
  Serial.println("Setup completed");
}

void loop()
{
  getDurationData();
  calcFeatures();
  ml_label = clf.predict(ml_features);
  
//  Serial.println(ml_label); // debug
  
  // Manage the continuous slouches
  if(ml_label == 1){
    contSlouches+=1;
  }
  else{
    contSlouches=0;
  }

  /*
   * Setting: ATmega328P (3.3V, 8 MHz) 
   * 
   * CLKPR  | Seconds for 10 slouch readings
   * -------------------------------------------------
   * 0x00   | 1.5sec
   * 0x01   | 3 sec
   * 0x02   | 6 sec
   * 0x03   | 12 sec
   * 0x04   | 24 sec
   * 
   * 
   * Using 0x04
   * 
   * 1 slouch = 2.4s
   * 2 = 4.8s
   * 3 = 7.2s -> Choosing this
   * 4 = 9.6s
   */

  // Sound the buzzer for >= 3 continuous slouches  
  if(contSlouches >= 3){
    tone(3, 700, 50);
  }
}
