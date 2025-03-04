#include "posture_manager.hpp"

posture::PostureManager posture_manager;

static const int max_number_slouches = 3;
int consecutive_slouches = 0;

void setup()
{
    Serial.begin(115200);
    posture_manager.setup_mpu();
}

void loop()
{
  /*
   * Setting: ATmega328P (3.3V, 8 MHz) 
   * 
   * Test measurement table showing the duration of each prediction interval based on the
   * clock frequency setting. Each prediction interval consists of 10 data samples. As the
   * clock frequency is divided, the prediction interval duration doubles with each setting.
   * 
   * CLKPR  | Time Taken for 1 Prediction Interval
   * -------------------------------------------------
   * 0x00   | 0.15 seconds
   * 0x01   | 0.30 seconds
   * 0x02   | 0.60 seconds
   * 0x03   | 1.20 seconds
   * 0x04   | 2.40 seconds
   * 
   * The CLKPR=0x04 setting was chosen to maximize battery life.
   * A single interval may not accurately reflect slouching due to brief, unexpected movements.
   * Considering at least 3 consecutive prediction intervals to determine slouching.
   * 
   */

    posture_manager.gather_duration_data();
    posture_manager.calculate_duration_features();
    bool is_slouching = posture_manager.predict_slouch_status();

    if (is_slouching)
    {
        consecutive_slouches += 1;
    }
    else
    {
        consecutive_slouches = 0;
    }

    if (consecutive_slouches >= max_number_slouches)
    {
        tone(3, 700, 50);
    }
}