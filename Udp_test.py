import socket
byte_message = bytes("200", "UTF-8")
opened_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
opened_socket.sendto(byte_message, ("192.168.122.1", 4210))
