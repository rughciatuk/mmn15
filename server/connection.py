import threading
import socket
import db
import protocol
from Crypto.PublicKey import RSA
from Crypto.Cipher import AES, PKCS1_OAEP
import Crypto.Random


class ClientConnection(threading.Thread):
    def __init__(self, sock: socket.socket, addr: tuple):
        self.sock = sock
        self.addr = addr
        threading.Thread.__init__(self)

    def send_response(self, payload: protocol.ResponsePayload):
        header = protocol.ResponseHeader(version=3, code=payload.response_code(), payload_size=len(payload))
        self.sock.send(header.pack() + payload.pack())

    def run(self) -> None:
        req_mapper = {
            protocol.RequestCode.registration: self.op_registration,
            protocol.RequestCode.public_key: self.op_public_key
        }

        while True:
            # Reading the next request header from the client.
            data = self.sock.recv(protocol.RequestHeader.length())
            next_request = protocol.RequestHeader(data)
            req_mapper[next_request.code](next_request)

        self.sock.close()

    def send_general_error(self, error_message):
        payload = protocol.ResponsePayloadGeneralError(error_message)
        self.send_response(payload)

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
            self.send_response(payload)

    def op_public_key(self, request_header: protocol.RequestHeader):
        print("Getting public key", request_header)

        # Reading the payload from the client.
        public_key_data = protocol.RequestPayloadPublicKey(self.sock.recv(request_header.payload_size))
        print(public_key_data)

        # Loading the public and saving it in the db.
        db.update_public_key(request_header.client_id, public_key_data.public_key)
        client_RSA_enc = PKCS1_OAEP.new(RSA.import_key(public_key_data.public_key))

        # Generating a new AES key.
        new_AES_key = Crypto.Random.get_random_bytes(16)
        print(new_AES_key)
        db.update_AES_key(request_header.client_id, new_AES_key)

        # Encrypt with the public key:
        enc_AES_key = client_RSA_enc.encrypt(new_AES_key)

        payload = protocol.ResponsePayloadPublicKeyReceived(request_header.client_id, enc_AES_key)
        self.send_response(payload)
