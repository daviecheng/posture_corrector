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

void getDurationData(){
  for(int i=0;i<samples;i++){
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

void calcFeatures(){
  // Calculating RMS and Mean
  float rms[3] = {0, 0, 0};
  float mean[3] = {0, 0, 0};
  for(int j=0;j<3;j++){
    for(int i=0;i<samples;i++){
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
  for(int i=0;i<samples;i++){
    for(int j=0;j<3;j++){
      Serial.print(sensorVals[i][j]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void printFeatures(){
  for(int i =0; i<6; i++){
    Serial.print(ml_features[i]);
    Serial.print(" ");
  }
  Serial.println();
}
