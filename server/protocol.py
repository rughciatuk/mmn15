import struct
import enum
from dataclasses import dataclass
import abc


class RequestCode(enum.IntEnum):
    registration = 1100
    public_key = 1101
    send_file = 1103
    CRC_correct = 1104
    CRC_failed = 1105
    CRC_final_failed = 1106


class ResponseCode(enum.IntEnum):
    successful_registration = 2100
    public_key_received = 2102
    file_correct_CRC = 2103
    message_received = 2104
    server_general_error = 3100


@dataclass
class RequestHeader:
    client_id: bytes
    version: int
    code: RequestCode
    payload_size: int

    def __init__(self, data: bytes):
        (self.client_id, self.version, self.code, self.payload_size) = struct.unpack("<16sBHI", data)

    @staticmethod
    def length() -> int:
        return 23


@dataclass
class RequestPayloadRegistration:
    name: str

    def __init__(self, data: bytes):
        (self.name) = data.split(b'\x00')[0].decode("ascii")


@dataclass
class RequestPayloadPublicKey:
    name: str
    public_key: bytes

    def __init__(self, data: bytes):
        (temp_name, self.public_key) = struct.unpack("<255s160s", data)
        self.name = data.split(b'\x00')[0].decode("ascii")


@dataclass
class RequestPayloadSendFile:
    client_id: bytes
    content_size: int
    file_name: str

    def __init__(self, data: bytes):
        print(data)
        (self.client_id, self.content_size, temp_name) = struct.unpack("<16sI255s", data)
        self.file_name = temp_name.split(b'\x00')[0].decode("ascii")


# -------------------------------------------------------------------
#                           Responses
# -------------------------------------------------------------------

@dataclass
class ResponseHeader:
    version: int
    code: ResponseCode
    payload_size: int

    def pack(self) -> bytes:
        return struct.pack("<BHI", self.version, self.code, self.payload_size)

    @staticmethod
    def length():
        return 7


class ResponsePayload(abc.ABC):
    @abc.abstractmethod
    def __len__(self):
        pass

    @abc.abstractmethod
    def pack(self) -> bytes:
        pass

    @abc.abstractmethod
    def response_code(self) -> ResponseCode:
        pass


@dataclass
class ResponsePayloadGeneralError(ResponsePayload):
    error_text: str

    def __len__(self):
        return len(self.error_text) + 1  # For the null terminator

    def response_code(self) -> ResponseCode:
        return ResponseCode.server_general_error

    def pack(self) -> bytes:
        return self.error_text.encode("ascii") + b'\x00'  # Adding the null terminator


@dataclass
class ResponsePayloadSuccessfulRegistration(ResponsePayload):
    client_id: bytes

    def __len__(self):
        return len(self.client_id)

    def response_code(self) -> ResponseCode:
        return ResponseCode.successful_registration

    def pack(self) -> bytes:
        return self.client_id


@dataclass
class ResponsePayloadPublicKeyReceived(ResponsePayload):
    client_id: bytes
    enc_AES_key: bytes

    def __len__(self):
        return len(self.client_id) + len(self.enc_AES_key)

    def response_code(self) -> ResponseCode:
        return ResponseCode.public_key_received

    def pack(self):
        return self.client_id + self.enc_AES_key


@dataclass
class ResponsePayloadFileReceived(ResponsePayload):
    client_id: bytes
    content_size: int
    file_name: bytes
    cksum: int

    def __len__(self):
        return 16 + 4 + 255 + 4

    def response_code(self) -> ResponseCode:
        return ResponseCode.file_correct_CRC

    def pack(self):
        content_size = struct.pack("<I", self.content_size)
        file_name = bytes((self.file_name + ("\x00" * 255))[:255], "ascii")
        cksum = struct.pack("<I", self.cksum)

        return self.client_id + content_size + file_name + cksum


@dataclass
class ResponsePayloadMessageReceived(ResponsePayload):
    def __len__(self):
        return 0

    def response_code(self) -> ResponseCode:
        return ResponseCode.message_received

    def pack(self) -> bytes:
        return b''
