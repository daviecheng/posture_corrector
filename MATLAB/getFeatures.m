% Getting training data features

% Change the number of training data to be collected on line 60

function [rawData] = getFeatures(~,~)
load rawData.mat

% count the training data samples
c=evalin('base','count');
c=c+1;
assignin('base','count',c);


% get size of rawData collected during sampling time
dimension = size(rawData);
row= dimension(1);   % get rawData row dimension
column=dimension(2); % get rawData column dimension

% Reset values
accelX_sum=0;
accelY_sum=0;
accelZ_sum=0;

% Sum of accel and gyro individual axis
for sampleLine=1:row
    temp=[rawData(sampleLine,1:column)];
    accelX_sum = accelX_sum + temp(1,1);
    accelY_sum = accelY_sum + temp(1,2);
    accelZ_sum = accelZ_sum + temp(1,3);
end

% Get Mean Accel and Gyro in x,y,z directions
accelX_mean = accelX_sum/row;
accelY_mean = accelY_sum/row;
accelZ_mean = accelZ_sum/row;


% Get RMS Accel and Gyro in x,y,z directions
accelX_rms = rms(rawData(1:row,1));
accelY_rms = rms(rawData(1:row,2));
accelZ_rms = rms(rawData(1:row,3));


% save trainingData in workspace
td=[accelX_mean, accelY_mean, accelZ_mean, accelX_rms, accelY_rms, ...
    accelZ_rms];

filename = 'trainingData.mat';
m = matfile(filename,'Writable', true);
trainingData = m.trainingData;
trainingData = [trainingData;td];
save('trainingData.mat','trainingData');
assignin('base','trainingData',trainingData);
clear m;

% Check if the number of training data samples have been reached
td_size=size(trainingData);
td_row=td_size(1);
disp(td_row);
if td_row ~= 50 % gather 50 training samples
    assignin('base','rawData',[]);
    assignin('base','counter',1);
else 
    assignin('base','counter',0);
end
end

