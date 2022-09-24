import struct
import threading
import socket
import db
import binascii
import protocol


class ClientConnection(threading.Thread):
    def __init__(self, sock: socket.socket, addr: tuple):
        self.sock = sock
        self.addr = addr
        threading.Thread.__init__(self)

    def send_response(self, code: protocol.ResponseCode, payload: protocol.ResponsePayload):
        header = protocol.ResponseHeader(version=3, code=code, payload_size=len(payload))
        self.sock.send(header.pack() + payload.pack())

    def run(self) -> None:
        req_mapper = {
            protocol.RequestCode.registration: self.op_registration
        }

        while True:
            # Reading the next request header from the client.
            data = self.sock.recv(protocol.RequestHeader.length())
            next_request = protocol.RequestHeader(data)
            req_mapper[next_request.code](next_request)

        self.sock.close()

    def send_general_error(self, error_message):
        payload = protocol.ResponsePayloadGeneralError(error_message)
        self.send_response(protocol.ResponseCode.server_general_error, payload)

    def op_registration(self, request_header: protocol.RequestHeader):
        print("register_user", request_header)

        # Reading the payload from the client.
        registration_data = protocol.RequestPayloadRegistration(self.sock.recv(request_header.payload_size))

        print(registration_data)

        if db.check_user_exists(registration_data.name):
            print("User exists: ", registration_data.name)
            self.send_general_error("User exists: " + registration_data.name)
        else:
            # We need to register the user
            print("Registering new user: ", registration_data.name)
            new_uuid = db.register_user(registration_data.name)

            print("New uuid: ", new_uuid)
            # Creating the response
            payload = protocol.ResponsePayloadSuccessfulRegistration(new_uuid)
            self.send_response(protocol.ResponseCode.successful_registration, payload)
