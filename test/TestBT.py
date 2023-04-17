import socket
import struct
import time

# sensorMACAddress = '78:21:84:82:38:a2'
sensorMACAddress = '80:7d:3a:b9:33:e2'

port = 1

s = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)

# messages corrects
message1 = 0xD0807D3AB933E21905 # distance 400 mm
message2 = 0xC5807D3AB933E20005 # commande bouger corps

# message incorrect
message3 = 0xC5807D3AB933E20007

# Test de connexion
try:
    s.connect((sensorMACAddress,port))
    s.settimeout(0.25)

except TimeoutError:
    print("Erreur de connexion")

else:
    print("Connecte avec succes")
    messages = [message1.to_bytes(9), message2.to_bytes(9), message3.to_bytes(9)]
    
    # Test d'envoi
    for msg in messages:
        try:
            s.sendall(msg)  
        except socket.error as e:
            print("Reconnexion")
            try:
                s.close()
                s = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
                s.connect((sensorMACAddress,port))
                print("Reconnecte")
            except TimeoutError:
                print("Pas connecte")
        
        time.sleep(10)

    # Test de reconnexion
    while(1):
        try:
            s.sendall(message1)  
        except socket.error as e:
            print("Reconnexion")
            try:
                s.close()
                s = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
                s.connect((sensorMACAddress,port))
                print("Reconnecte")
            except TimeoutError:
                print("Pas connecte")
        
        time.sleep(10)
