import time
from fizzy_udp import Fizzy
from pynput import keyboard

## use up and down keys to conrol the speed. Max values 1 and -1. 
# Speed in one direction is always a bit delayed compared to the other (starts rotating at 0.15 instead of 0.05)

# Initialize motor
motor = Fizzy()

# Initial speed
speed = 0.0
step = 0.05
running = True

def clamp(val, min_val, max_val):
    return max(min(val, max_val), min_val)

def on_press(key):
    global speed, running

    try:
        if key == keyboard.Key.up:
            speed = clamp(speed + step, -1.0, 1.0)
            motor.set_motor(speed)
            # print(f"↑ Increased speed to {speed:.2f}")
            print(speed)
        elif key == keyboard.Key.down:
            speed = clamp(speed - step, -1.0, 1.0)
            motor.set_motor(speed)
            # print(f"↓ Decreased speed to {speed:.2f}")
            print(speed)
        elif key == keyboard.Key.esc:
            print("Exiting...")
            running = False
            motor.stop()
            return False  # Stop the listener
    except Exception as e:
        print("Error:", e)

def on_release(key):
    pass  # Do nothing on release

# Listener
with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
    print("Use ↑ and ↓ to control motor speed. Press Esc to stop.")
    while running:
        time.sleep(0.01)