
from fizzy_udp_1 import Fizzy
import sys
import time
from typing import Any
from pyjoystick.sdl2 import Key, Joystick, run_event_loop
from threading import Thread
import numpy as np
import matplotlib.pyplot as plt


## State Machine states with xbox controller: ##
# B = exit
# A = neutral (motor torque 0)
# X = big wiggle
# Y = vibrating
# Left joystick press = speed control (controlled with the left joystick back and forth)
# Right joystick press = directional control (controlled with the right joystick gives direction and magnitude)
# Left button (for index finger) = Position control on downward position
# Right button (for index finger) = Position control (controlled with the left joystick back and forth)


class XboxThread(Thread):   # creates a class that recieves xboc controller data on an eventloop bases and returns those values
    def __init__(self) -> None:
        super().__init__()
        self.__value = 0
        self.x_axis_right = 0
        self.y_axis_right = 0
        self.x_axis_left = 0
        self.y_axis_left = 0
        self.state = 0
        self.terminate = 0

    def print_remove(self, key):
        print(f'Removed device: {key}')
        
    def print_add(self, key):
        print(f'Added device: {key}')

    def run(self):
        run_event_loop(self.print_add, self.print_remove, self.key_received)

    def key_received(self, key: Key):
        ### DOCUMENTATION:
        # Key.AXIS:         # 0 = x-axis left joystick, Left (-1) and Right (1)
                            # 1 = y-axis left joystick, up (-1) and down (1)
                            # 2 = LT, values between 0 and 1
                            # 3 = x-axis right joystick, 
                            # 4 = y-axis right joystick 
                            # 5 = RT, values between 0 and 1 

        # Key.BUTTON:       # 0 = A, 
                            # 1 = B, 
                            # 2 = X,
                            # 3 = Y,
                            # 4 = LB,
                            # 5 = RB,
                            # 6 = from the 3 small buttons in the middle the left one
                            # 7 = ?from the 3 small buttons in the middle the right one
                            # 8 = pressing Left joystick,
                            # 9 = pressing Right joystick,

        # Key.HAT:          
                            #....
                            #....
                            #....
        ### ----------------------------------------------------------------------

        # right joystick
        if key.keytype == Key.AXIS and key.number == 3: # key number 3 is Right Joystick Left (-1) and Right (1)
            self.x_axis_right = key.value
        elif key.keytype == Key.AXIS and key.number == 4: # key number 4 is Right Joystick up (-1) and down (1)
            self.y_axis_right = key.value

        # left joystick
        elif key.keytype == Key.AXIS and key.number == 0: # key number 0 is Right Joystick left (-1) and right (1)
            self.x_axis_left = key.value
        elif key.keytype == Key.AXIS and key.number == 1: # key number 1 is Right Joystick up (-1) and down (1)
            self.y_axis_left = key.value  
        
        # levers left and right (LT and RT)
        elif key.keytype == Key.AXIS and key.number == 2: # key number 2 LT, values between 0 and 1
            self.y_axis_left = -key.value - 0.05        # off set due to difference in resistance in direction
        elif key.keytype == Key.AXIS and key.number == 5: # key number 5 RT, values between 0 and 1
            self.y_axis_left = key.value 
            

        # termination key B
        elif key.keytype == Key.BUTTON and key.number == 1 and key.value == 1: # key number 1 is 'B BUTTON'
            self.terminate = 1

        # other keys
        elif key.keytype == Key.BUTTON and key.number == 0 and key.value ==1: # key number 0 is 'A BUTTON'
            self.state = 1
        elif key.keytype == Key.BUTTON and key.number == 2 and key.value ==1: # key number 2 is 'X BUTTON'
            self.state = 2
        elif key.keytype == Key.BUTTON and key.number == 3 and key.value ==1: # key number 3 is 'Y BUTTON'
            self.state = 3
        elif key.keytype == Key.BUTTON and key.number == 4 and key.value ==1: # key number 4 is 'LB BUTTON'
            self.state = 4
        elif key.keytype == Key.BUTTON and key.number == 5 and key.value ==1: # key number 5 is 'RB BUTTON'
            self.state = 5
       
        elif key.keytype == Key.BUTTON and key.number == 8 and key.value ==1: # key number 8 is 'pressing Left joystick'
            self.state = 8 
        elif key.keytype == Key.BUTTON and key.number == 9 and key.value ==1: # key number 9 is 'pressing Right joystick'
            self.state = 9 
        

        key.ALL_KEYTYPES

    def exit(self):
        return self.terminate
    
    def case(self):
        return self.state

    def get_value(self) -> float:
        return self.__value
    
    def get_y_axis_left(self) -> float:
        return self.y_axis_left

    def get_x_axis_right(self) -> float:
        return self.x_axis_right
    
    def get_y_axis_right(self) -> float:
        return self.y_axis_right
    
    def reset(self):
        self.__value = 0
        self.x_axis_right = 0
        self.y_axis_right = 0
        self.x_axis_left = 0
        self.y_axis_left = 0
        self.state = 0
        
    

thread = XboxThread()
thread.daemon = True # cleaning the receive buffer of the socket
thread.start()

# # ----------------------------------------------------------------------------------------

##Quaternions to earler angels function

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
    yaw = np.arctan2(t3, t4) + np.pi

    return roll, pitch, yaw

# # ----------------------------------------------------------------------------------------

# Initialize comunication
fizzy = Fizzy()


# Get starting time
try:
    data = fizzy.get_data()
except:
    print("dropout")

start_time_on_PCB = data[0]/1_000_000 # in seconds

# # ----------------------------------------------------------------------------------------

# Parameters

plotting_data = False

limitpower = 0.95
satpower = 0
rawpower = 0


# Ossilating movement parameters
T1 = 0.4  # Period for the first and third segments, in seconds, limit: both period on 0.03 with 0.4 amplitude
T2 = 0.8  # Period for the second segment, in seconds

A2 = 0.7         # Amplitude of second segment   choose value between 0 and 0.5, Limit 0.8 with period 0.1
A1 = -0.5*A2         # Amplitude of first segment, (needs to be in relation with the second one)


cycle_duration= T1 + T2 + T1 # cycle duration, in seconds, of back-and-forth flapping the motor to generate some yaw. Sum of all the different flapping frequencies

offset1 = -A1   # Offset to shift the range
offset2 = 0   # Offset to shift the range
offset3 = A1  # Offset to shift the range

# vibrating
T1v = 0.05  # Period for the first and third segments, in seconds, limit: both period on 0.03 with 0.4 amplitude
T2v = 0.05  # Period for the second segment, in seconds

A2v = 0.5         # Amplitude of second segment   choose value between 0 and 0.5, Limit 0.8 with period 0.1
A1v = -0.5*A2v         # Amplitude of first segment, (needs to be in relation with the second one)

cycle_duration_vibration= T1v + T2v + T1v # cycle duration, in seconds, of back-and-forth flapping the motor to generate some yaw. Sum of all the different flapping frequencies

offset1v = -A1v   # Offset to shift the range
offset2v = 0   # Offset to shift the range
offset3v = A1v  # Offset to shift the range


# zero stand
ref_roll = 0
K_P = 0.8           #gain


## for Virtual Model Control (VMC) ##
# set constant parameters:
g = 9.81  # acceleration of gravity in m/s^2
    
# geometrical parameters of the ball (needed for virtual model control):
r = 0.025  # distance from axle to center of mass in m, should be equal to distance from sphere center to axle.
Delta = 0.01366 # distance from axle center to block center of mass in x-direction
R = .1    # radius of ball in m

# virtual model control parameters:
k_virtual = 40 # 30 # rigid version   # virtual spring constant, in percent per m, not the same as in simulation where it is in N/m
dist_error = 1      # m, just a carrot always 1 m ahead
dir_ref = 0         # rad, reference direction (track or heading) in absolute coordinates. Note the hub re-initializes yaw each time to zero!
# So this direction is not really absolute. Alternative setting could e.g. be dir_ref= -pi/2

'''
# to get out of singularities, the ball will do two things:

# 1. modify the reference direction, to get a sort of zigzag motion:
amplitude_zigzag= np.pi/10     # amplitude in rad, zigzagging the reference yaw direction to get out of singularities
freq_zigzag=2               # frequency in Hz, zigzagging reference direction to get the ball out of singularities

# 2. flap the block back and forth around the "down" configuration, to generate torques due to the dynamic imbalance:
cycle_duration=4.5  # cycle duration, in seconds, of back-and-forth flapping the motor to generate some yaw. Must be larger than duration_fast
duration_fast=2     # duration of the fast part of the cycle, in seconds (the rest remains for the slow movement)
K_p_high=0.5        # motor feedback gain to enforce fast motion in first part of cycle, in percent/rad
K_p_low=0.5          # motor feedback gain for slow motion in second part of cycle, percent/rad
'''


desired_minimal_cycle_time = 0.01  # this value is used to limit really fast cylcle times 


if plotting_data == True:
    # Set up plots
    plt.ion()  # Turn on interactive mode
    fig, axs = plt.subplots()
    data_plotting = 10
    # Deques to store the last 100 data points for plotting for each signal
    x_data_plot = []
    y_data_plot = []
    line, = axs.plot(x_data_plot, y_data_plot)

    # Function to update plot
    def update_plot():
        line.set_xdata(x_data_plot)
        line.set_ydata(y_data_plot)
        axs.relim()
        axs.autoscale_view()
        fig.canvas.draw()
        fig.canvas.flush_events()





# # ----------------------------------------------------------------------------------------

## Mail loop with Joystick input as changing factor for the different states ##
while True:
    start = time.time()

    fizzy.set_motor(satpower)
    # fizzy.set_motor(0)          # idle motor
    
    try:
        data = fizzy.get_data()
        
    except:
        print("dropout")

    roll, pitch, yaw = extract_euler_from_packet(data)
    angles_deg = np.degrees([roll, pitch, yaw]) # transforms to degree
    
    # yaw 0deg to 360deg 
    # pitch pedulum along -/+ z axis (hanging down and up) measures 0deg, horizontal one side 90deg and the other -90deg
    # roll pedulum along - z axis (hanging down) 0deg and along + z axis 180deg or -180deg
 
    time_on_PCB = data[0]/1_000_000
    timer_precise_on_PCB = time_on_PCB-start_time_on_PCB
    


    ## Different states: ##
    # ----------------------------------------------------------------------------------
    # exit
    if thread.exit() == 1:
        fizzy.stop()
        print('Bye ...')
        sys.exit()

    # -----------------------------------------------------------------------------------
    # neutral (idle motor)
    elif thread.case() == 1:
        fizzy.stop()
        print('neutral')
        rawpower = 0
        thread.reset()

    

    # -----------------------------------------------------------------------------------
    # wiggle big
    elif thread.case() == 2:
        print('X')
        
        cycle_portion = (timer_precise_on_PCB % cycle_duration) # modulo operator to see in which part of the cycle we are

        if cycle_portion < T1:#for the first part of the cycle, move fast:     
            phitot_ref = A1 *  np.cos(np.pi * (cycle_portion) / T1) + offset1 # sinusoidal input
        
        elif T1 <= cycle_portion < (T1 + T2): #for the second part of the cycle:
            phitot_ref = A2 * np.cos(np.pi *  (cycle_portion-T1) / T2) + offset2 # sinusoidal input

        else: # move back to nutral position:
            phitot_ref = A1 * np.cos(np.pi * (cycle_portion-(T1+T2))/ T1) + offset3 # try to get the motor on average in the down position for max leverage
        
        rawpower = phitot_ref


        ## proportional control wiggle ##
        # K_P_wiggle = 0.2
        # refrollangle = phitot_ref # amplitude is linked to the reference angle
        # roll_converted = np.sin(roll*np.pi/180)
        # rollerror = refrollangle-roll
        # rawpower = float(K_P_wiggle*-rollerror)

    # -----------------------------------------------------------------------------------
    # vibrate
    elif thread.case() == 3:
        print('Y')
    
        cycle_portion = (timer_precise_on_PCB % cycle_duration_vibration) # modulo operator to see in which part of the cycle we are

        if cycle_portion < T1v:#for the first part of the cycle, move fast:     
            phitot_ref = A1v *  np.cos(np.pi * (cycle_portion) / T1v) + offset1v # sinusoidal input
        
        elif T1v <= cycle_portion < (T1v + T2v): #for the second part of the cycle:
            phitot_ref = A2v * np.cos(np.pi *  (cycle_portion-T1v) / T2v) + offset2v # sinusoidal input

        else: # move back to nutral position:
            phitot_ref = A1v * np.cos(np.pi * (cycle_portion-(T1v+T2v))/ T1v) + offset3v # try to get the motor on average in the down position for max leverage
        
        rawpower = phitot_ref

    # -----------------------------------------------------------------------------------
    # zero stand (LB)
    elif thread.case() == 4: 
        # roll = roll_measurement-90 #  when shifted 90 degrees calibrate the IMU data to the orientation of the drivetrain
        roll_converted = np.sin(roll) # 180 and -180 at the bottom and 0 when standing upright, calibrate the IMU data to the orientation of the drivetrain 
        rollerror = ref_roll-roll_converted

        rawpower = float(K_P*rollerror) 
        print(roll_converted)
    
        
         


    # -----------------------------------------------------------------------------------
    # position control (RB)
    elif thread.case() == 5:
        ref_roll = thread.get_y_axis_left()*1.2
        roll_converted = np.sin(roll) # orginal output: 180 and -180 at the bottom and 0 when standing upright, calibrate the IMU data to the orientation of the drivetrain 
        rollerror = ref_roll-roll_converted
        
        rawpower = float(K_P*rollerror) 
        print(rollerror)
        

    # -----------------------------------------------------------------------------------
    # speed control 
    elif thread.case() == 8:
        # print('Left Joystick press')
               
        rawpower =  thread.get_y_axis_left()

    # -----------------------------------------------------------------------------------
    # directional control
    elif thread.case() == 9:
    
        # variables as used in Matlab simulation (converted to rad and slight change in frame):
        psi = yaw               # yaw angle
        theta = pitch           # pitch angle. 
        phitot = roll           # roll angle. Block triad K of simulation slightly different, just rotated by 90 degrees
        

        # make conversion from x-y joystick motion to direction and amplitude
        x_value = thread.get_x_axis_right()
        y_value = -thread.get_y_axis_right()
        dir_ref = np.arctan2(y_value, x_value) + np.pi # adding pi to correct for the range of arctan2,  which is -pi till pi
        

        # calculating the amplitude of signal
        Amplitude = np.sqrt(x_value**2+y_value**2)*1   # adding a gain factor (0.7) on the error to make it react slower

        # reference for yaw:
        dir_ref_rel = dir_ref-psi;#yaw error (relative to the reference) this angle is negative
        

        # the virtual model controller:
        rawpower_vmc = k_virtual*r*(dist_error*np.cos(phitot)*np.sin(dir_ref_rel) - R*np.cos(theta)*np.sin(phitot) + dist_error*np.cos(dir_ref_rel)*np.sin(phitot)*np.sin(theta))
        # simple analytical solution of the virtual spring (Simplification was possible by letting the target height always be just distance R below P)
    
        # # Only vitual model controller, without flapping
        # rawpower = rawpower_vmc
        

        singularity_index = (1000*abs(R*np.sin(theta) + dist_error*np.cos(dir_ref_rel)*np.cos(theta)))/(1000*np.sqrt(R*R + dist_error*dist_error) + 1);
        # # the singularity index value is 1 for aligned axle (worst case) and zero if the axle is orthogonal (best case)


        ## flapping option
        cycle_portion = (timer_precise_on_PCB % cycle_duration) # modulo operator to see in which part of the cycle we are

        if cycle_portion < T1:#for the first part of the cycle, move fast:     
            phitot_ref = A1 *  np.cos(np.pi * (cycle_portion) / T1) + offset1 # sinusoidal input
        
        elif T1 <= cycle_portion < (T1 + T2): #for the second part of the cycle:
            phitot_ref = A2 * np.cos(np.pi *  (cycle_portion-T1) / T2) + offset2 # sinusoidal input

        else: # move back to nutral position:
            phitot_ref = A1 * np.cos(np.pi * (cycle_portion-(T1+T2))/ T1) + offset3 # try to get the motor on average in the down position for max leverage
        
        rawpower_yaw_flapping = phitot_ref

        rawpower=Amplitude*(singularity_index*rawpower_yaw_flapping + rawpower_vmc) #blend between controllers.

        

        '''
        #------------------------
        #Escape strategy part 1, modifying the reference direction to a sort of zig-zag around the actual direction:
        #------------------------
        timer_precise=(time.time()-timer_init) # precise timer, in s
        # dir_ref_rel=dir_ref_rel+singularity_index*amplitude_zigzag *np.sin(2*np.pi*freq_zigzag*timer_precise)
        # print(dir_ref_rel)
        
        #------------------------
        #Escape strategy part 2, alternative movement to generate flapping, needed when in singularity:
        #------------------------
        # move fast for part of the cycle (Achieved by high gain), slow for the other part
        cycle_portion = (timer_precise_on_PCB % cycle_duration)#modulo operator to see in which part of the cycle we are
        # print(cycle_portion)

        if cycle_portion < T1:#for the first part of the cycle, move fast:
            # phitot_ref=-np.pi-np.pi/4 # try to get the motor on average in the down position for max leverage
            # phitot_ref = 1.5 # 0 is at 90 degrees (drivetrain horizontal)
            
            phitot_ref = A1 *  np.cos(np.pi * (cycle_portion) / T1) + offset1 # sinusoidal input
            K_p = K_p_low #high gain
            state = 1

        elif T1 <= cycle_portion < (T1 + T2): #for the second part of the cycle:
            phitot_ref = A2 * np.cos(np.pi *  (cycle_portion-T1) / T2) + offset2 # sinusoidal input
            K_p = K_p_high #high gain 
            state = 2

        else: #move back to nutral position:
            phitot_ref = A1 * np.cos(np.pi * (cycle_portion-(T1+T2))/ T1) + offset3 # try to get the motor on average in the down position for max leverage
            K_p = K_p_low #low gain
            state = 3
        # print(cycle_portion)
        # print(K_p)
        # print(phitot_ref)
        

         rawpower_yaw_flapping = phitot_ref                 

        # rawpower = singularity_index*rawpower_yaw_flapping + rawpower_vmc #blend between controllers. #The closer to the singularity, the more flapping back and forth of the block
        # rawpower = rawpower_yaw_flapping # try only flapping
        # rawpower = rawpower_vmc # Without flapping.
        # print(rawpower)
        '''



    # saturate the motor:
    if rawpower>limitpower:
        satpower=limitpower
    elif rawpower<-limitpower:
        satpower=-limitpower
    else:
        satpower=rawpower
    # print(satpower)

     # plotting desired data 
    if plotting_data == True:
   
        y_data_plot.append(satpower)

        x_data_plot.append(timer_precise_on_PCB)
        update_plot()
        axs.set_xlabel('X Axis')
        axs.set_ylabel('Y Axis')
        axs.set_title('Signal ')
        plt.pause(0.01)


    # Making sure that the cycle time is running with a minimum cycle time.
    endtime = time.time()
    cycle_time = endtime-start
    
    if endtime-start < desired_minimal_cycle_time:
        # print('processing time', endtime-start)
        time.sleep(desired_minimal_cycle_time-(endtime-start))