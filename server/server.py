import threading
import socket
import connection


class Server(threading.Thread):
    def __init__(self, port, host="0.0.0.0"):
        self.addr = (host, port)
        threading.Thread.__init__(self)

    def run(self):

        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind(self.addr)
        server_socket.listen(5)

        while True:
            print("Waiting for a new connection")
            client_sock, client_addr = server_socket.accept()
            print("A new connection from", client_addr)
            connection.ClientConnection(client_sock, client_addr).start()

        print("Closing")
        server_socket.close()
