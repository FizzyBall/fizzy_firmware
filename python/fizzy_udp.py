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
    
    def stop(self):
        self.sock.sendto(struct.pack('Bf', 0, 0), (self.ip, self.port))

    def set_motor(self, value):
        self.sock.sendto(struct.pack('Bf', 1, value), (self.ip, self.port))
        try:
            data = self.sock.recv(200)
        except:
            return -1
        return list(struct.unpack('q'+'f', data))