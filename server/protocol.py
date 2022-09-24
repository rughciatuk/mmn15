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
    def pack(self):
        pass


@dataclass
class ResponsePayloadGeneralError(ResponsePayload):
    error_text: str

    def __len__(self):
        return len(self.error_text) + 1  # For the null terminator

    def pack(self) -> bytes:
        return self.error_text.encode("ascii") + b'\x00'  # Adding the null terminator


# successful
@dataclass
class ResponsePayloadSuccessfulRegistration(ResponsePayload):
    client_id: bytes

    def __len__(self):
        return len(self.client_id)

    def pack(self) -> bytes:
        return self.client_id
