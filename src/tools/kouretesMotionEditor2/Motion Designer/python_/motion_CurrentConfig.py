import os
import sys
import time

path = `os.environ.get("AL_DIR")`
home = `os.environ.get("HOME")`

# import NaoQi lib
if path == "None":
  print "the environnement variable AL_DIR is not set, aborting..."
  sys.exit(1)
else:
  alPath = path + "/extern/python/aldebaran"
  alPath = alPath.replace("~", home)
  alPath = alPath.replace("'", "")
  sys.path.append(alPath)
  import naoqi
  from naoqi import ALBroker
  from naoqi import ALModule
  from naoqi import ALProxy
  from naoqi import ALBehavior

  import motion




IP = "192.168.212.21"  # Put here the IP address of your robot
#~ IP = "10.0.252.170"  # Put here the IP address of your robot
#IP = "nao2.local"
PORT = 9559

if (IP == ""):
  print "IP address not defined, aborting"
  print "Please define it in " + __name__  
  exit(1)



# ======================= PRINT  ==========================
print "CurrentConfig: Using IP: " + str(IP) + " and port: "  + str(PORT)

