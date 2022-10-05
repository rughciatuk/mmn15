import sqlite3
import uuid

DB_PATH = "server.db"


def get_or_create_db():
    with sqlite3.connect(DB_PATH) as con:
        sqlite3.register_adapter(uuid.UUID, lambda u: u.bytes_le)
        sqlite3.register_converter('GUID', lambda b: uuid.UUID(bytes_le=b))

        # Creating the clients table.
        con.execute("""
            CREATE TABLE IF NOT EXISTS clients(
                ID GUID PRIMARY KEY,
                Name TEXT,
                PublicKey BLOB,
                LastSeen TEXT,
                AESKey BLOB
            );""")

        # Creating the files table.
        con.execute("""
            CREATE TABLE IF NOT EXISTS files(
                ID GUID PRIMARY KEY,
                FileName TEXT,
                PathName TEXT,
                Verified BOOLEAN
            );""")

        con.commit()


def check_user_exists(username: str) -> bool:
    with sqlite3.connect(DB_PATH) as con:
        cursor = con.execute("SELECT COUNT(*) FROM clients WHERE Name = ?", (username,))
        ret_val = cursor.fetchall()[0][0] != 0

        return ret_val


def register_user(username: str) -> bytes:
    with sqlite3.connect(DB_PATH) as con:
        insert_cursor = con.execute("""
        INSERT INTO clients (ID, Name, LastSeen) VALUES (?, ?, DateTime('now'));
        """, (uuid.uuid4(), username))

        con.commit()

        select_cursor = con.execute("""
        SELECT ID FROM clients WHERE Name = ?
        """, (username,))

        new_uuid = select_cursor.fetchall()[0][0]
        return new_uuid


def update_public_key(user_uuid: bytes, public_key: bytes) -> bool:
    with sqlite3.connect(DB_PATH) as con:
        update_cursor = con.execute("""
        UPDATE clients
        SET PublicKey = ?
        WHERE ID = ?
        """, (public_key, user_uuid))

        con.commit()

        return update_cursor.rowcount == 1


def update_AES_key(user_uuid: bytes, aes_key: bytes) -> bool:
    with sqlite3.connect(DB_PATH) as con:
        update_cursor = con.execute("""
            UPDATE clients
            SET AESKey = ?
            WHERE ID = ?
        """, (aes_key, user_uuid))

        con.commit()

        return update_cursor.rowcount == 1


def get_AES_key(user_uuid: bytes) -> bytes:
    with sqlite3.connect(DB_PATH) as con:
        get_cursor = con.execute("""
            SELECT AESKey
            FROM clients
            WHERE ID = ?
        """, (user_uuid,))

        return get_cursor.fetchall()[0][0]


def new_file(file_name: str, path_name: str, verified: bool):
    with sqlite3.connect(DB_PATH) as con:
        con.execute("""
        INSERT INTO files (ID, FileName, PathName, Verified) VALUES (?, ?, ?, ?);
        """, (uuid.uuid4(), file_name, path_name, verified))
        con.commit()


