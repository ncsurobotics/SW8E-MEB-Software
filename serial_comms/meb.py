import serial
import time
ser = serial.Serial('/dev/ttyACM0')
print(ser.name)
print(ser.readline())
#WaitFor7E
ser.write('X')
print(ser.readline())
#WaitForFF
ser.write('A')
print(ser.readline())
#Read
ser.write('C')
print(ser.readline())
#Address
ser.write('T')
print(ser.readline())
#read depthVal
#WaitFor7E
#if ser.readline()[0] == "9":
 # stringD = ser.readline()
  #depth = stringD#[1:3]
  #print(depth)
  #print("working")
#if ser.readline()[0] == 3:
#  stringK = ser.readline()
 # kill = stringK[1]
  #print(kill)
while ser.readline()[0] == "9":
  depthString = ser.readline()[1:4]
  killString = ser.readline()[5]
  print("depth: " + depthString)
  if killString == "0":
    killStatus = "operating"
  else: killStatus = "killed"
  print("kill status: " + killStatus)
  

 # make output in arduino start with D: dephtval() and kiill be K: killval()

  
 # depth = ser.readline()
  
#if ser.readline().isdigit() == false:
 # kill = ser.readline()
#while 2 > 1 :
 # print("Depth: %4s | Kill swith: %4s" % (depth, kill))
 # time.sleep(1);
  #print("Depth: "+ser.readline()+"Kill switch: " + ser.readline())


