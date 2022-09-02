PORT_FILE_NAME = "port.info"


def read_port():
    try:
        with open("port.info", "r") as port_file:
            return int(port_file.readline())
    except FileNotFoundError:
        print("Warning: port.info doesn't exists, using 1234 as the port")
        return 1234


if __name__ == '__main__':
    print("This is the server")
    print(read_port())
