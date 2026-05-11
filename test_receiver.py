import socket, struct, datetime

MAGIC = 0xDEADBEEF

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind(('127.0.0.1', 12345))
    s.listen(1)
    print("Listening...")
    conn, addr = s.accept()
    with conn:
        while True:
            data = conn.recv(1024)
            if not data:
                break

            if len(data) >= 16:
                magic, size, = struct.unpack_from('<II', data)
                if magic == MAGIC and size == 8:
                    ts, = struct.unpack_from('<q', data, 8)
                    print(f"Received timestamp: {datetime.datetime.utcfromtimestamp(ts/1000)}")