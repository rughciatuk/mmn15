import threading
import socket


class ClientConnection(threading.Thread):
    def __init__(self, sock: socket.socket, addr: tuple):
        self.sock = sock
        self.addr = addr
        threading.Thread.__init__(self)

    def run(self) -> None:
        while True:
            buffer = self.sock.recv(1024)
            if buffer == b"exit\n":
                self.sock.send(b"Exiting\n")
                break
            elif buffer:
                self.sock.send(buffer)

        self.sock.close()

