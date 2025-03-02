#include <Arduino.h>
#include <Wire.h>
#include <sefr/sefr.h>

// Settings
static const uint16_t sampling_rate = 300;
static const uint8_t number_samples = 10;
static const uint8_t sample_delay = sampling_rate / number_samples;

float sensorVals[number_samples][3];
float ml_features[6];
int ml_label;
int contSlouches;

long accelX, accelY, accelZ;  // raw accel sensor values
float gForceX, gForceY, gForceZ;  // accel converted to [g], g = 9.8 m/s^2

// Function Declarations
void setupMPU();
void recordAccelRegisters_MPU();
void getDurationData();
void calcFeatures();
void printData();
void printFeatures();
Eloquent::ML::Port::SEFR clf;

void setup() {
  /* Uncomment the below lines to reduce clock frequency and to save battery life.
     Note: Using these clock prescalar lines will disrupt the serial monitor as the baud rate will change.
  */
  // CLKPR = 0x80; // enable CLKPCE
  // CLKPR = 0x04; // clock prescaler select


  Serial.begin(115200);  
  Wire.begin();
  setupMPU();
  Serial.println("Setup completed");
}

void loop() {
  getDurationData();
  calcFeatures();
  printFeatures();


  // ml_label = clf.predict(ml_features);
  
//  Serial.println(ml_label); // debug
  
  // Manage the continuous slouches
  // if(ml_label == 1){
  //   contSlouches+=1;
  // }
  // else{
  //   contSlouches=0;
  // }

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
  // if(contSlouches >= 3){
  //   tone(3, 700, 50);
  // }
}

void setupMPU(){
  Wire.beginTransmission(0x68); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();  
  Wire.beginTransmission(0x68); //I2C address of the MPU
  Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
  Wire.endTransmission(); 
  Wire.beginTransmission(0x68); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission();
}

/// @brief Records the MPU accelerometer data
///
/// Using I2C, we get the gForceX, gForceY, and gForceZ values from the MPU sensor.
///
void recordAccelRegisters_MPU() {
  Wire.beginTransmission(0x68); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0x68,6); //Request Accel Registers (3B - 40)
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ

  // accel values in [g]
  gForceX = accelX / 16384.0; 
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
}

/// @brief Gets the duration MPU data
///
/// For each sample, we populate the gForceX, gForceY, gForceZ.
///
void getDurationData(){
  for(int i=0;i<number_samples;i++){
    recordAccelRegisters_MPU();
    for(int j=0;j<3;j++){
      if(j == 0){
        sensorVals[i][j]=gForceX;
      }
      else if(j ==1){
        sensorVals[i][j]=gForceY;      
      }
      else if(j ==2){
        sensorVals[i][j]=gForceZ;
      }        
    }
    delay(sample_delay);
  }
}

/// @brief Calculates the features for classification in the ML model
void calcFeatures(){
  // Calculating RMS and Mean
  float rms[3] = {0, 0, 0};
  float mean[3] = {0, 0, 0};
  for(int j=0;j<3;j++){
    for(int i=0;i<number_samples;i++){
      rms[j]+=pow(sensorVals[i][j], 2);
      mean[j]+=sensorVals[i][j];
    }
    
    // accel mean (0-2)
    ml_features[j] = mean[j]/10;

    // accel rms (3-5)
    ml_features[j+3] = sqrt(rms[j]/10);
  }
}

void printData() {
  for(int i=0;i<number_samples;i++){
    for(int j=0;j<3;j++){
      Serial.print(sensorVals[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void printFeatures(){
  Serial.println("ml_features");
  for(int i =0; i<6; i++){
    Serial.print(ml_features[i]);
    Serial.print("\t");
  }
  Serial.println();
}
