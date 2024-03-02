%% Sensor measurements
clear
clc

baudrate = 115200;
COM_PORT = "COM14";

serial_port = serialport(COM_PORT, baudrate);

% First lines, adjust as needed
idn = readline(serial_port);
idn = readline(serial_port);
idn = readline(serial_port);


N = 10000;
disp("Reading sensor values...")
tic;
for i=1:N
    idn = readline(serial_port);
    all_imu = str2double(split(idn));
    imu_a_raw(:,i) = all_imu(1:3);
    imu_g_raw(:,i) = all_imu(4:6);
end
disp(['Done! Runtime: ' num2str(toc) ' s'])
%%
% clc
% % Made with "default" IMU settings, possibly with standard 16-bit values
% save_file = "200s_acc_and_gyr.mat";
% save(save_file, "imu_a_raw", "imu_g_raw");
%%
clc

load("200s_acc_and_gyr.mat")

bit_max = 2^16 / 2 - 1;
bit_min = -2^16/2;

% Scale by bit resolution
imu_a_unit = imu_a_raw.' / bit_max;
imu_g_unit = imu_g_raw.' / bit_max;

% Mean accelerometer norm is supposed to be 1g
IMU_ACC_INT16_TO_G = 1/norm((mean(imu_a_raw.')));

% In '1g'
imu_a = IMU_ACC_INT16_TO_G * imu_a_raw.';

% Acc
sigma_a = std(imu_a);
mu_a = mean(imu_a);    % Table was not ideal, 
                       % don't use as offset


% Figuring out gyro resolution

% Recorded 10 000 at 90 deg/s
IMU_GYR_INT16_TO_DEG_PER_S = 90/10000;

imu_g = IMU_GYR_INT16_TO_DEG_PER_S * imu_g_raw.';

% Gyr
sigma_g = std(imu_g);
mu_g = mean(imu_g);


clf
figure(1)
% Plotting Acc
for k=1:3
    subplot(2,3,k)
    hist = histogram(imu_a(:,k), 'Normalization', 'pdf');
    xline(mu_a(k),'--','LineWidth',1.5)
    xline(mu_a(k)-3*sigma_a(k),'--g','LineWidth',1.5)
    xline(mu_a(k)+3*sigma_a(k),'--g','LineWidth',1.5)
    title(['\mu = ' num2str(mu_a(k)) ' | \sigma = ' num2str(sigma_a(k))])
end
subplot(231)
xlim([-1550 -1225]*IMU_ACC_INT16_TO_G)
subplot(233)
xlim([1.91 1.99]*1e4*IMU_ACC_INT16_TO_G)

% Plotting Gyr
for k=1:3
    subplot(2,3,k+3)
    hist = histogram(imu_g(:,k), 'Normalization', 'pdf');
    xline(mu_g(k),'--','LineWidth',1.5)
    xline(mu_g(k)-3*sigma_g(k),'--g','LineWidth',1.5)
    xline(mu_g(k)+3*sigma_g(k),'--g','LineWidth',1.5)
    title(['\mu = ' num2str(mu_g(k)) ' | \sigma = ' num2str(sigma_g(k))])
end
