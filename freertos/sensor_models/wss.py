# NOTE: python version 2.7.11

import socket, threading, thread
from Queue import Queue, Empty, Full
import ctypes
import json

CTRL_INIT = (1 << 0)
CTRL_USING_SIMULATOR = (1 << 7)
STATUS_WHEEL_SPEED_VALID = (1 << 0)

global g_wheel_speed_q

# Convert rad/s to 1/1000 rad/s
def wheel_speed_to_reg_val(rad_p_sec, value_min=-50.0, value_max=50.0):
    value = max(min(value_max, rad_p_sec), value_min)
    return ctypes.c_uint32(int(value * 1000.0)).value

class SocketServer(socket.socket):
    clients = []

    def __init__(self):
        socket.socket.__init__(self)
        self.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.bind(('0.0.0.0', 44321))
        self.listen(1)

    def run(self):
        try:
            self.accept_clients()
        except Exception as ex:
            print ex
        finally:
            for client in self.clients:
                client.close()
            self.close()

    def accept_clients(self):
        while 1:
            (clientsocket, address) = self.accept()
            self.clients.append(clientsocket)
            self.onopen(clientsocket)
            thread.start_new_thread(self.recieve, (clientsocket,))

    def recieve(self, client):
        while 1:
            data = client.recv(1024)
            if data == '':
                break
            self.onmessage(client, data)
        self.clients.remove(client)
        self.onclose(client)
        client.close()
        thread.exit()

    def onopen(self, client):
        pass

    def onmessage(self, client, message):
        pass

    def onclose(self, client):
        pass

class ImuRelayServer(SocketServer):

    def __init__(self):
        SocketServer.__init__(self)

    def onmessage(self, client, message):
        global g_wheel_speed_q
        data = json.loads(message)
        #print "WSS recvd wheel speed %f" % data['wheel_speed']
        try:
            g_wheel_speed_q.put_nowait(data['wheel_speed'])
        except Full:
            print "WSS queue is full"

    def onopen(self, client):
        print "WSS client Connected"

    def onclose(self, client):
        print "WSS client Disconnected"

def run_server():
    server = ImuRelayServer()
    server.run()

if request.isInit:
    wheelSpeed = 0
    status = 0
    g_wheel_speed_q = Queue(maxsize=10)
    self.NoisyLog("Initializing wheel speed sensor")
elif request.isRead:
    # Set valid wheel speed once we start getting data from the simulator
    if status & STATUS_WHEEL_SPEED_VALID == 0:
        try:
            ws = g_wheel_speed_q.get_nowait()
            wheelSpeed = ws
            status |= STATUS_WHEEL_SPEED_VALID
            self.NoisyLog("Wheel speed valid")
        except Empty:
            pass

    if request.offset == 0:         # CTRL
        request.value = 0
    elif request.offset == 0x4:     # STATUS
        request.value = status
    elif request.offset == 0x8:     # WHEEL_SPEED
        try:
            ws = g_wheel_speed_q.get_nowait()
            wheelSpeed = ws
        except Empty:
            pass
        request.value = wheel_speed_to_reg_val(wheelSpeed)
    else:
        self.NoisyLog("Invalid %s on WSS at 0x%x, value 0x%x" % (str(request.type), request.offset, request.value))
elif request.isWrite:
    if request.offset == 0:         # CTRL
        if request.value & CTRL_INIT != 0:
            status |= STATUS_WHEEL_SPEED_VALID
            # Wait for first IMU msg before being valid
            status &= ~STATUS_WHEEL_SPEED_VALID
        if request.value & CTRL_USING_SIMULATOR != 0:
            self.NoisyLog("Setting up TCP server for gazebo IMU data")
            threading.Thread(target=run_server).start()
    else:
        self.NoisyLog("Invalid %s on WSS at 0x%x, value 0x%x" % (str(request.type), request.offset, request.value))
