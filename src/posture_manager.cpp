#include "posture_manager.hpp"

#include <Wire.h>

namespace posture
{

PostureManager::PostureManager() {}

void PostureManager::gather_duration_data()
{
    for (int i = 0; i < number_samples_per_duration; i++)
    {
        read_mpu_accelerometer();
        _mpu_accelerometer_duration_data[i][0] = _mpu_accelerometer_data[0];
        _mpu_accelerometer_duration_data[i][1] = _mpu_accelerometer_data[1];
        _mpu_accelerometer_duration_data[i][2] = _mpu_accelerometer_data[2];
        delay(sample_delay_ms);
    }
}

void PostureManager::print_duration_data()
{
    for (int i = 0; i < number_samples_per_duration; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Serial.print(_mpu_accelerometer_duration_data[i][j]);
            Serial.print(" ");
        }
        Serial.println();
    }
}

void PostureManager::calculate_duration_features()
{
    float acceleration_sum[3]          = {0, 0, 0};
    float acceleration_squared_sum[3]  = {0, 0, 0};

    for (int i = 0; i < number_samples_per_duration; i++)
    {
        // Acceleration sum x, y, z
        acceleration_sum[0] += _mpu_accelerometer_duration_data[i][0];
        acceleration_sum[1] += _mpu_accelerometer_duration_data[i][1];
        acceleration_sum[2] += _mpu_accelerometer_duration_data[i][2];

        // RMS acceleration x, y, z
        acceleration_squared_sum[0] += pow(_mpu_accelerometer_duration_data[i][0], 2);
        acceleration_squared_sum[1] += pow(_mpu_accelerometer_duration_data[i][1], 2);
        acceleration_squared_sum[2] += pow(_mpu_accelerometer_duration_data[i][2], 2);
    }

    for (int i = 0; i < 3; i++)
    {
        // Mean acceleration x, y, z
        _classifier_inputs[i] = acceleration_sum[i] / number_samples_per_duration;

        // RMS acceleration
        _classifier_inputs[i + 3] = sqrt(acceleration_squared_sum[i] / number_samples_per_duration);
    }
}

void PostureManager::print_duration_features()
{
    for (int i = 0; i < 6; i++)
    {
        Serial.print(_classifier_inputs[i]);
        Serial.print("\t");
    }
    Serial.println();
}

void PostureManager::setup_mpu()
{
    Wire.begin();
    Wire.beginTransmission(0x68); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
    Wire.write(0x6B);             //Accessing the register 6B - Power Management (Sec. 4.28)
    Wire.write(0b00000000);       //Setting SLEEP register to 0. (Required; see Note on p. 9)
    Wire.endTransmission();  
    Wire.beginTransmission(0x68); //I2C address of the MPU
    Wire.write(0x1B);             //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
    Wire.write(0x00000000);       //Setting the gyro to full scale +/- 250deg./s 
    Wire.endTransmission(); 
    Wire.beginTransmission(0x68); //I2C address of the MPU
    Wire.write(0x1C);             //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
    Wire.write(0b00000000);       //Setting the accel to +/- 2g
    Wire.endTransmission();

    Serial.println("MPU Setup Complete");
}

void PostureManager::read_mpu_accelerometer()
{
    Wire.beginTransmission(0x68);        //I2C address of the MPU
    Wire.write(0x3B);                    //Starting register for Accel Readings
    Wire.endTransmission();
    Wire.requestFrom(0x68,6);            //Request Accel Registers (3B - 40)
    while(Wire.available() < 6);
    long accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
    long accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
    long accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  
    // accel values in [g]
    _mpu_accelerometer_data[0] = accelX / 16384.0; 
    _mpu_accelerometer_data[1] = accelY / 16384.0; 
    _mpu_accelerometer_data[2] = accelZ / 16384.0;
}

bool PostureManager::predict_slouch_status()
{
    return _classifier.predict(_classifier_inputs);
}

} // namespace posture