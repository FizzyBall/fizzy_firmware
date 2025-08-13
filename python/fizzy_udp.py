import socket
import struct

class Fizzy:
    """
    This is a class to for communication with Fizzy.
    It handles the actual set of functions available on Fizzy.
    In general there are to way to communicate:
    1. polling data on request
    2. downlink, which can be triggered to initiate Fizzy to send data-frames as soon as they are available. The Downlink frequency is about 104Hz.

    :param ip: IP address of Fizzy, defaults to '192.168.4.1'
    :type ip: str, optional
    :param port: UDP port of Fizzy, defaults to 4711
    :type port: int, optional
    :param timeout: timeout for UDP requests in seconds, defaults to 2
    :type timeout: int, optional
    """
    
    def __init__(self, ip = '192.168.4.1', port = 4711, timeout = 2):
        """
        Constructor method
        """
        self.port = port
        self.ip = ip
        self.timeout = timeout
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(("0.0.0.0", port))
        self.sock.settimeout(timeout)

    def decode(self, data):
        """
        Decode raw udp dataframe to list.

        :param data: UDP dataframe 
        :type data: byte[33]
        :return: [timestamp, motor-speed, v_bat, q1, q2, q3, q4, quality_mag]
        :rtype: [int64_t, float, float, float, float, float, float, uint8_t]
        """
        return list(struct.unpack('<qffffffB', data))
    
    def stop(self):
        """
        1. stops the motor.
        2. stops data downlink (if running)
        """
        self.sock.sendto(struct.pack('Bf', 0, 0), (self.ip, self.port))

    def set_motor(self, value):
        """
        Set new motor speed (Range -1.0 to 1.0).
        Returns actual data-frame.

        :param value: speed [-1.0 to 1.0]
        :type value: float
        :return: [timestamp, motor-speed, v_bat, q1, q2, q3, q4, quality_mag]
        :rtype: [int64_t, float, float, float, float, float, float, uint8_t]
        """
        self.sock.sendto(struct.pack('Bf', 1, value), (self.ip, self.port))
        try:
            data = self.sock.recv(200)
        except:
            return -1
        return self.decode(data)
    
    def start_downlink(self):
        """
        Starts data-downlink.
        Forces Fizzy to fire data-frames at ~104Hz.
        To stop data-downlink use the "stop()" command.

        Note: Data-frames will pile up in the UDP-receive buffer (FIFO) on the local computer.
        Take care to pull them out or flush the buffer frequently. 
        """
        self.sock.sendto(struct.pack('Bf', 2, 0), (self.ip, self.port))

    def get_data(self):
        """
        Get data-frame from Fizzy in non-downlink mode.

        :return: [timestamp, motor-speed, v_bat, q1, q2, q3, q4, quality_mag]
        :rtype: [int64_t, float, float, float, float, float, float, uint8_t]
        """
        self.sock.sendto(struct.pack('Bf', 66, 0), (self.ip, self.port))
        try:
            data = self.sock.recv(200)
        except:
            return -1
        return self.decode(data)
    
    def get_data_downlink(self):
        """
        Get data-frame from Fizzy in downlink mode.

        Note: v_bat will be 0 in downlink mode.

        :return: [timestamp, motor-speed, v_bat, q1, q2, q3, q4, quality_mag]
        :rtype: [int64_t, float, float, float, float, float, float, uint8_t]
        """
        try:
            data = self.sock.recv(200)
        except:
            return -1
        return self.decode(data)
    
    def get_firmware_version(self):
        """
        Requests the actual firmware version of the connected Fizzy.
        
        :return: git-version (FW)
        :rtype: string 
        """
        self.sock.sendto(struct.pack('Bf', 0xff, 0), (self.ip, self.port))
        try:
            data = self.sock.recv(200)
        except:
            return -1
        return data.decode('utf-8')

    def flush_receive_buffer(self):
        """
        Flushes the UDP-receive buffer on the local computer.

        Note: this will take at least the time of UDP timeout (>2s)

        :return: number of data-frames that has been deleted
        :rtype: int
        """
        cnt = 0
        while(isinstance(self.get_data_downlink(), list)):
            cnt +=1
        return cnt