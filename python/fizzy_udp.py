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

    def decode(self, data):
        return list(struct.unpack('<qffffffB', data))
    
    def stop(self):
        self.sock.sendto(struct.pack('Bf', 0, 0), (self.ip, self.port))

    def set_motor(self, value):
        self.sock.sendto(struct.pack('Bf', 1, value), (self.ip, self.port))
        try:
            data = self.sock.recv(200)
        except:
            return -1
        return self.decode(data)
    
    def start_downlink(self):
        self.sock.sendto(struct.pack('Bf', 2, 0), (self.ip, self.port))

    def get_data(self):
        self.sock.sendto(struct.pack('Bf', 66, 0), (self.ip, self.port))
        try:
            data = self.sock.recv(200)
        except:
            return -1
        return self.decode(data)
    
    def get_data_downlink(self):
        try:
            data = self.sock.recv(200)
        except:
            return -1
        return self.decode(data)
    
    def get_firmware_version(self):
        self.sock.sendto(struct.pack('Bf', 0xff, 0), (self.ip, self.port))
        try:
            data = self.sock.recv(200)
        except:
            return -1
        return data.decode('utf-8')