import server
import db

PORT_FILE_NAME = "port.info"


def read_port():
    try:
        with open("port.info", "r") as port_file:
            return int(port_file.readline())
    except FileNotFoundError:
        print("Warning: port.info doesn't exists, using 1234 as the port")
        return 1234


def main():
    db.get_or_create_db()
    port = read_port()
    print("Listing on port: ", port)
    main_server = server.Server(port)

    main_server.start()


if __name__ == '__main__':
    main()