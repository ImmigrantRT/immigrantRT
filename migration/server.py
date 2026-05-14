# first of all import the socket library
import socket
from socket import AF_INET, SOCK_DGRAM
import os
import yaml

absolute_path = lambda path : os.path.join(os.path.dirname(os.path.realpath(__file__)), path)

with open(absolute_path('../launch/config.yaml'), 'r') as f:
    data = f.read().replace('\t', '  ')
    config = yaml.load(data, yaml.SafeLoader)
    user = config['user']

# next create a socket object
#s = socket.socket(AF_INET,SOCK_DGRAM)
s = socket.socket()
print ("Socket successfully created")

# reserve a port on your computer in our
# case it is 12345 but it can be anything
port = 12345

# Next bind to the port
# we have not typed any ip in the ip field
# instead we have inputted an empty string
# this makes the server listen to requests
# coming from other computers on the network
s.bind(('', port))
print ("socket binded to %s" %(port))

# put the socket into listening mode
s.listen(5)
print ("socket is listening")

# a forever loop until we interrupt it or
# an error occurs
while True:

# Establish connection with client.
  c, addr = s.accept()
  print ('Got connection from', addr )

  while True:
      rec_data = c.recv(1024).decode()
      cmd = rec_data.split(',')
      print(cmd)
      if cmd[0]=='0': # Copy pre-checkpoint data
          copy_cmd = "rsync -aqz " + user + "@" + addr[0] + ":" + \
            absolute_path('../data/checkpoints/pre_' + cmd[1] + '.tar.gz') + " " + \
            absolute_path('../data/checkpoints/')
          print(copy_cmd)
          os.system(copy_cmd)
          c.send('0'.encode())
      elif cmd[0]=='1': # Copy checkpoint data
          ret = os.system("rsync -aqz " + user + "@" + addr[0] + ":" + 
                          absolute_path('../data/checkpoints/' + cmd[1] + '.tar.gz') + " " + 
                          absolute_path('../data/checkpoints/'))
          if ret==0:
              c.send('1'.encode())
          else:
              ret=0
      elif cmd[0]=='2': # Restore container
          os.system("sudo podman --runtime runc container restore --tcp-established" + 
                    " --import-previous=" + absolute_path('../data/checkpoints/pre_' + cmd[1] + '.tar.gz') + 
                    " --import=" + absolute_path('../data/checkpoints/' + cmd[1] + '.tar.gz'))
          os.system("sudo podman start -l")
          os.system('bash ' + absolute_path('../launch/run_container_monitor.sh') + ' ' + cmd[1]) # launch container monitor
          c.send('2'.encode())
      else:
          #c.send('Invalid command'.encode())
          break

  #os.system(rec_data)
  # send a thank you message to the client. encoding to send byte type.
  #c.send('Thank you for connecting'.encode())

  # Close the connection with the client
  c.close()

  # Breaking once connection closed
  #break
