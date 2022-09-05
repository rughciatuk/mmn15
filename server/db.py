import sqlite3


def get_or_create_db(path):
    con = sqlite3.connect(path)

    # Creating the clients table.
    con.execute("""
CREATE TABLE IF NOT EXISTS clients(
    ID BLOB PRIMARY KEY,
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
