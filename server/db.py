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
                ID BLOB PRIMARY KEY,
                FileName TEXT,
                PathName TEXT,
                Verified BOOLEAN
            );""")


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

        select_cursor = con.execute("""
        SELECT ID FROM clients WHERE Name = ?
        """, (username,))

        new_uuid = select_cursor.fetchall()[0][0]
        return new_uuid
