%% %%%%%% Formula CASE calcs %%%%%% %%
clear all
clc
format shortG
%%% F1 car specs (RB15) %%%

m_F1 = 743; % kg
P_F1 = 671 * 1e3; % W

% Acceleration
t_0to100 = 2.6; % s
t_0to200 = 4.5; % s

% Deceleration
t_100to0_F1 = 1.5;
F1_v_max = 300 / 3.6; % (?) m/s

% Dimensions
F1_width = 2; % m
F1_height = 0.95; % m
F1_length = 5.4; % m
F1_wheel_diameter = 0.66; % m, 2021 tyres, 2022: 0.72
F1_back_wheel_width = 0.405; % m
F1_front_wheel_width = 0.365; % (?) m
F1_wheel_base = 3.55; % m


%%% Model specs %%%

m = 0.250; % kg
scaling = 0.045;

width = scaling * F1_width;
height = scaling * F1_height;
length = scaling * F1_length;

wheel_diameter = scaling * F1_wheel_diameter;
wheel_radius = wheel_diameter / 2;
back_wheel_width = scaling * F1_back_wheel_width;
front_wheel_width = scaling * F1_front_wheel_width;
wheel_base = scaling * F1_wheel_base;

v_max = scaling * F1_v_max;

track_width = width - front_wheel_width;

omega_max = v_max / wheel_diameter;

omega_max_RPM = omega_max * 30/pi

% 0 to "100 km/h", but scaled
a = ((100/3.6) * scaling) / t_0to100;

F = m * a;

T = F / wheel_diameter;

T_wheel = T/4;

%P_wheel_max = omega_max * T_wheel

t_0to100 * a * F





