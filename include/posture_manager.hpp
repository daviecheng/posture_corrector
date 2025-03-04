#ifndef POSTURE_MANAGER_HPP
#define POSTURE_MANAGER_HPP

#include <Arduino.h>
#include <sefr.h>

namespace posture
{

static const uint16_t sampling_rate_ms = 300;
static const uint8_t number_samples_per_duration = 10;
static const uint8_t sample_delay_ms = sampling_rate_ms / number_samples_per_duration;

/// @brief Orchestrates the posture data gathering and feature extraction for the ML classifier
/// It also handles the prediction events and logic triggered after receiving the classifier's prediction.
class PostureManager
{
public:

    /// @brief Constructor
    PostureManager();

    /// @brief Default destructor
    ~PostureManager() = default;

    /// @brief Configures the MPU sensor via I2C
    void setup_mpu();

    /// @brief Gets the motion duration data
    void gather_duration_data();

    /// @brief Prints the motion duration data
    void print_duration_data();

    /// @brief Calculates the motion features for the ML classifier
    void calculate_duration_features();

    /// @brief Prints the motion duration features
    void print_duration_features();

    /// @brief Predicts the duration slouch status
    /// @return True if sluch was detected, false otherwise
    bool predict_slouch_status();

private:
    float _mpu_accelerometer_data[3]; // x, y, z acceleration data
    float _mpu_accelerometer_duration_data[number_samples_per_duration][3]; // x, y, z acceleration
    float _classifier_inputs[6]; // acceleration mean (0-2) and acceleration rms (3-5)

    Eloquent::ML::Port::SEFR _classifier;

    /// @brief Reads the MPU accelerometer data
    void read_mpu_accelerometer();
};

} // namespace posture

#endif // POSTURE_MANAGER_HPP