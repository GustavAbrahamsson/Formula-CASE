# %%
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import serial

# %%
# Open serial port
ser = serial.Serial('COM16', 115200, timeout=1)

# Read data from serial port
lines = []
while True:
    try:
        line = ser.readline()
        line = line.decode('utf-8')
        line = line.strip()
        lines.append(line)
    except KeyboardInterrupt:
        break

# Close serial port
ser.close()

# %%
print(lines[0:10])

# %%
# Convert data to pandas dataframe
df = pd.DataFrame(lines)
df = df[0].str.split(',', expand=True)
df.columns = ['micros', 'voltage']
# Remove empty rows
df = df.dropna()

df['micros'].astype('int64')
df['voltage'].astype('float64')
# numeric
df = df.apply(pd.to_numeric)

df['seconds'] = df['micros'] / 1000000

def voltage_to_current(voltage):
    k = 0.0961
    m = -0.109
    return voltage*k +m

df['current'] = voltage_to_current(df['voltage'])
df['milliampere'] = df['current'] * 1000

# write to csv
df.to_csv('data.csv', index=False)

# %% plot miliampere
df.plot(x='seconds', y='milliampere', figsize=(10, 5))
plt.xlabel('Time (s)')
plt.ylabel('Current (mA)')

df['milliampere_rolling'] = df['milliampere'].rolling(100).mean()
plt.plot(df['seconds'], df['milliampere_rolling'], color='red', linewidth=2)

plt.legend(['Current', 'Current (rolling mean 100)'])
plt.title('Current vs Time. Opamp + Arduino uno')
# Annotation
# plt.annotate('Motor starts spinning', xy=(6.8, 50), xytext=(8, 55), arrowprops=dict(facecolor='black', shrink=0.01))
# plt.annotate('Motor stops spinning', xy=(22.5, 28), xytext=(24, 33), arrowprops=dict(facecolor='black', shrink=0.01))
# plt.text(26, 20, 'Motor voltage used \nis between 0 and ~1V')

plt.xlim(10,11)
# %%
# Mean dt
df['dt'] = df['micros'].diff()
print(f'Mean frequency: {1e6 / df["dt"].mean():.2f} Hz')

# Mean current
print(f'Mean current: {df["milliampere"].mean():.2f} mA')