from fizzy_udp import Fizzy
import numpy as np
import matplotlib.pyplot as plt
import time
from collections import deque

## Plots the IMU angles. 
# for exit use the keys: control + C 

# The IMU gives Quaternions which are transformed into eurler angles with the following function 
def extract_euler_from_packet(packet):
    """
    Given a data packet with the specified format, extract Euler angles.
    Input packet: [timestamp, motor_speed, battery_voltage, qx, qy, qz, qw, mag_cal_level]
    Returns: (roll, pitch, yaw) in radians
    """
    qx, qy, qz, qw = packet[3], packet[4], packet[5], packet[6]
    w, x, y, z = qw, qx, qy, qz

    t0 = +2.0 * (w * x + y * z)
    t1 = +1.0 - 2.0 * (x * x + y * y)
    roll = np.arctan2(t0, t1)

    t2 = +2.0 * (w * y - z * x)
    t2 = np.clip(t2, -1.0, 1.0)
    pitch = np.arcsin(t2)

    t3 = +2.0 * (w * z + x * y)
    t4 = +1.0 - 2.0 * (y * y + z * z)
    yaw = np.arctan2(t3, t4)

    return roll, pitch, yaw

# Initialize fizzy
fizzy = Fizzy()

# Setup live plot
plt.ion()
fig, axs = plt.subplots(3, 1, figsize=(8, 6))
labels = ['Roll (°)', 'Pitch (°)', 'Yaw (°)']
data_plotting = 100

x_data = [deque(maxlen=data_plotting) for _ in range(3)]
y_data = [deque(maxlen=data_plotting) for _ in range(3)]

# Main loop
try:
    start_time = time.time()
    while True:
        loop_start = time.time()

        data = fizzy.get_data()
        timestamp = data[0] / 1_000_000  # in seconds
        roll, pitch, yaw = extract_euler_from_packet(data)
        angles_deg = np.degrees([roll, pitch, yaw]) # transforms to degree
        # print(angles_deg)

        # Update data for plots
        for i, angle in enumerate(angles_deg):
            x_data[i].append(timestamp - start_time)
            y_data[i].append(angle)

            axs[i].clear()
            axs[i].plot(x_data[i], y_data[i])
            axs[i].set_ylabel(labels[i])
            axs[i].set_xlabel("Time (s)")
            axs[i].grid(True)

        plt.tight_layout()
        plt.pause(0.01)
        
        # # For printing Magnetometer calibration level
        # print(data[-1])

        # Control loop speed
        loop_time = time.time() - loop_start
        minimal_cycle_time = 0.01
        if loop_time < minimal_cycle_time:
            time.sleep(minimal_cycle_time - loop_time)

except KeyboardInterrupt:
    print("Measurement interrupted by user.")
