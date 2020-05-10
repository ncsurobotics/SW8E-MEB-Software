import rospy
import mavros
from std_msgs.msg import Float64
from mavros_msgs.msg import OverrideRCIn
import serial
import time

vertical_rate = 0.0
lateral_rate = 0.0
rotation_rate = 0.0

def valmap(value, istart, istop, ostart, ostop):
  return ostart + (ostop - ostart) * ((value - istart) / (istop - istart))

def verticalCallback(data):
	global vertical_rate
	vertical_rate = valmap(data.data, -1.0, 1.0, 1000, 2000)

def lateralCallback(data):
	global lateral_rate
	lateral_rate = valmap(data.data, -1.0, 1.0, 1000, 2000)

def rotationCallback(data):
	global rotation_rate
	rotation_rate = valmap(data.data, -1.0, 1.0, 1000, 2000)


def pixhawk():
	global vertical_rate, lateral_rate, rotation_rate

	rospy.init_node('pixhawk', anonymous=False)
	rate = rospy.Rate(20)
	mavros.set_namespace()

	rospy.Subscriber("wolf_vertical", Float64, verticalCallback)
	rospy.Subscriber("wolf_lateral", Float64, lateralCallback)
	rospy.Subscriber("wolf_rotation", Float64, rotationCallback)
	
	
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
rc = OverrideRCIn()
override_pub = rospy.Publisher(mavros.get_topic("rc", "override"), OverrideRCIn, queue_size=10)
while not rospy.is_shutdown():
		#rc.channels[2] = vertical_rate
		#rc.channels[3] = rotation_rate
		#rc.channels[4] = lateral_rate
		#override_pub.publish(rc)
    while True:
      while ser.readline()[0] == "9":
        depthString = ser.readline()[2:6]
        killString = ser.readline()[6]
        print("depth: " + depthString)
        if killString == "0":
          killStatus = "operating"
        else: killStatus = "killed"
        print("kill status: " + killStatus)

    rate.sleep()

if __name__ == '__main__':
	pixhawk()













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


 # make output in arduino start with D: dephtval() and kiill be K: killval()

  
 # depth = ser.readline()
  
#if ser.readline().isdigit() == false:
 # kill = ser.readline()
#while 2 > 1 :
 # print("Depth: %4s | Kill swith: %4s" % (depth, kill))
 # time.sleep(1);
  #print("Depth: "+ser.readline()+"Kill switch: " + ser.readline())


