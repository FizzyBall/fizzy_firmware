import socket
import struct

class Fizzy:
    def __init__(self, ip = '192.168.4.1', port = 4711, timeout = 2):
        self.port = port
        self.ip = ip
        self.timeout = timeout
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(("0.0.0.0", port))
        self.sock.settimeout(timeout)

    def two2dec(self, val):
        if (0x8000 & val):
            val = (0x7FFF &val) - 0x8000
        return val
    
    def stop(self):
        self.sock.sendto(struct.pack('Bf', 0, 0), (self.ip, self.port))

    def set_motor(self, value):
        self.sock.sendto(struct.pack('Bf', 1, value), (self.ip, self.port))
        try:
            data = self.sock.recv(200)
        except:
            return -1
        return list(struct.unpack('q'+ 2*'f', data))
    
    def get_imu_raw(self):
        self.sock.sendto(struct.pack('Bf', 66, 0), (self.ip, self.port))
        try:
            data = self.sock.recv(200)
        except:
            return -1
        raw_data = [self.two2dec(v) for v in struct.unpack('<'+6*'H', data)]
        acc_scale = 2*8/0xFFFF
        gyro_scale = 2*250/0xFFFF
        return [raw_data[0]*gyro_scale, raw_data[1]*gyro_scale, raw_data[2]*gyro_scale, raw_data[3]*acc_scale, raw_data[4]*acc_scale, raw_data[5]*acc_scale]
    
    def get_firmware_version(self):
        self.sock.sendto(struct.pack('Bf', 0xff, 0), (self.ip, self.port))
        try:
            data = self.sock.recv(200)
        except:
            return -1
        return data.decode('utf-8')
