import socket
from werkzeug.security import check_password_hash

HOST = "127.0.0.1"
PORT = 5432

class Responses:
    ok = b"OK"
    auth = b"AUTH"
    err = b"ERR"

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    while True:
        conn, addr = s.accept()
        with conn:
            while True:
                try:
                    data = conn.recv(1000000)
                    if not data:
                        break
                    if len(data) < 16:
                        conn.sendall(Responses.err)
                        continue
                    auth = data[:16]
                    msg = data[16:]
                    # c0189790ab65aa39
                    # if not check_password_hash("pbkdf2:sha256:600000$H8mLDmYd5wLWRTcn$5826063ba19b12390b86881fc07782214da0a3e2039a2c2c6b2ae93f6123136b", auth.decode()):
                    #     conn.sendall(Responses.auth)
                    #     continue
                    print(f"revd {len(msg)} bytes:")
                    conn.sendall(Responses.ok)
                except:
                    conn.sendall(Responses.err)
                    continue
