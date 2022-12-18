%% %%%%%% Formula CASE calcs %%%%%% %%
clear all
clc

%%% F1 car specs (RB15) %%%

m_F1 = 743; % kg
P_F1 = 671 * 1e3; % W

% Acceleration
t_0to100_F1 = 2.6; % s
t_0to200_F1 = 4.5; % s

% Deceleration
t_100to0_F1 = 1.5;
v_max_F1 = 300 / 3.6; % (?) m/s

% Dimensions
width_F1 = 2; % m
height_F1 = 0.95; % m
length_F1 = 5.4; % m
wheel_diameter_F1 = 0.66; % m, 2021 tyres, 2022: 0.72
back_wheel_width_F1 = 0.405; % m
front_wheel_width_F1 = 0.365; % (?) m
wheel_base_F1 = 3.55; % m


%%% Model specs %%%

m = 0.250; % kg
scaling = 0.045;

width = scaling * width_F1;
height = scaling * height_F1;
length = scaling * length_F1;

wheel_diameter = scaling * wheel_diameter_F1;
wheel_radius = wheel_diameter / 2;
back_wheel_width = scaling * back_wheel_width_F1;
front_wheel_width = scaling * front_wheel_width_F1;
wheel_base = scaling * wheel_base_F1;

v_max = scaling * v_max_F1;

track_width = width - front_wheel_width;

omega_max = v_max / wheel_diameter;

omega_max_RPM = omega_max * 30/pi;

a = v_max / t_0to100_F1;

F = m * a;

T = F / wheel_diameter;

T_wheel = T/4;

P_wheel_max = omega_max * T_wheel







