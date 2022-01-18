# Wearable_Posture_Corrector

## Summary
> Wearable device that notifies the user when they are slouching for 7 seconds or more.

- Uses SEFR machine learning algorithm
- 600 training data samples (300 for slouch, 300 for non-slouch)
- Features of SEFR classifier include 
  - Mean x, y, and z acceleration
  - Mean x, y, and z RMS acceleration
- Piezo to notify user (can be replaced with vibration motor for less noise).
- 300 ms sampling interval

# Setup
## Get Training Data
1. Upload 'mpu' Arduino code to Uno board.
2. In MATLAB, have 'getTrainingData.m' and 'getFeatures.m' in the environment.
> Setup the circuit as shown below.
<img src="https://user-images.githubusercontent.com/84931559/120691398-697d4580-c474-11eb-9fd0-e62b2ad93697.png" width="700">
3. Run 'getTrainingData.m'
4. The collected training data is stored in your workspace variable 'trainingData'. Copy the training data to a CSV file and manually input the label as 1 for slouching and 0 for non-slouching in the last column. My training data is included for reference.
5. Continue to run 'getTrainingData.m' to gather training data.

## Export SEFR to C++
1. Load 'sefr_ml' Python program.
2. Replace the path with your CSV path that contains your training data.
3. Run the Python script and the output is your SEFR classifier code in C++. 
4. Save the output code as a .h file.

## Deploy Classifier to Project
1. Upload "posture" Arduino code to Arduino IDE.
2. Include the SEFR .h file into the same folder as the "posture" Arduino code.
3. Setup the circuit as shown below; then upload the code to Arduino Pro Mini board.
<img src="https://user-images.githubusercontent.com/84931559/120695492-76e8fe80-c479-11eb-82a4-1e4a17a5788e.png">
