# Kouretes Motion Editor Server
# Author : Pierris Georgios  gpierris@isc.tuc.gr
# Year : 2008
# Feel free to e-mail me if you have any questions or
# in case you don't have anything better to do...
#


import socket
import select
import time
import sys
import string

from motion_CurrentConfig import *

naoPort = 9559
editorPort = 50000


if( sys.argv[0] != "" ):
	IP = sys.argv[1]

####
# Create python broker
try:
	broker = ALBroker("pythonBroker","127.0.0.1",9999,IP, PORT)
except RuntimeError,e:
	print("cannot connect")
	exit(1)

####
# Create motion proxy
print "Creating motion proxy"
try:
	motionProxy = ALProxy("ALMotion")
except Exception,e:
	print "Error when creating motion proxy:"
	print str(e)
	exit(1)


####
# Create memory proxy
print "Creating ALMemory proxy"
try:
	memoryProxy = ALProxy("ALMemory")
except Exception,e:
	print "Error when creating ALMemory proxy:"
	print str(e)
	exit(1)



####
# Create tts proxy
print "Creating textToSpeech proxy"
try:
	pass#ttsProxy = ALProxy("ALTextToSpeech")
except Exception,e:
	print "Error when creating textToSpeech proxy:"
	print str(e)
	exit(1)

####
# Create alsentinel proxy
print "Creating ALSentinel proxy"
try:
	alsentinel = ALProxy("ALSentinel")
except Exception,e:
	print "Error when creating ALSentinel proxy:"
	print str(e)
	exit(1)

# ==================== Socket Connection ==========================


kmeSocket = socket.socket ( socket.AF_INET, socket.SOCK_STREAM )



for i in range(10):
	try:
		kmeSocket.bind ( ( '', editorPort ) )
		break
	except Exception:
		editorPort += 1

print( "\nWaiting connection at Port :" + str( editorPort ) )

try:
	kmeSocket.listen( 1 )
except KeyboardInterrupt,e:
	print( "\nExitting...")
	print( str( e ) )	
	exit(1)

channel, details = kmeSocket.accept()
print( "We have opened a connection with " + str( details ))
channel.setblocking(0)
channel.settimeout(1)



while True:


	try:

		data = channel.recv(1024)
		print data

		if "stiffness_on_all" in str(data) :
			print "stiffness on"
			motionProxy.stiffnessInterpolation("Body", 1.0,0.6)
			#motionProxy.setBodyStiffness(0.8,0.6)
		elif "stiffness_off_all" in str(data) :
			print "stiffness off"
			motionProxy.stiffnessInterpolation("Body", 0.0, 0.6)
			#motionProxy.setBodyStiffness(0.0, 0.6)

		elif "talk" in data :   # PROFANWS DEN PERIMENEIS NA DOULEPSEI AYTO AKOMA...!!!!
			
			valuesTalk = received.split('_')

			ttsProxy.setVolume( int(float(valuesTalk[3])) )
			ttsProxy.setParam( valuesTalk[4], float(valuesTalk[5]) )
			ttsProxy.setParam( valuesTalk[6], float(valuesTalk[7]) )
			ttsProxy.setParam( valuesTalk[8], float(valuesTalk[9]) )
			ttsProxy.setParam( valuesTalk[10], float(valuesTalk[11]) )

			
			if( int(valuesTalk[13]) == 0 ):
				ttsProxy.say( valuesTalk[1] )
			else:
				name = ""
				for i in range(0,5):
					name +=str( (time.localtime())[i] ) 
				
				ttsProxy.sayToFile( valuesTalk[1], name+".raw" )
				ttsProxy.say("File saved!")

		elif "disconnect" in data :
			break
			
		elif "play" in data or "pose" in data :
			
			# EDW PAIRNEIS TO MYNHMA
			values = data.split('%')
			values.pop( 0 ) #delete "play" or "pose"
			
			duration = float(values.pop()) # get duration and delete it from the list
			

			for index in range(len(values)):
				values[index] = float( values[index] )


			if( duration < 0.2 ):		#We love our robot...
				duration = 1.0

			#motionProxy.gotoBodyAngles(values,duration,motion.INTERPOLATION_LINEAR)



		
		
	except KeyboardInterrupt:
		break

	except socket.timeout:

		# EDW STELNEIS TO POSE SOU AFOU DEN EXEIS PAREI KATI APO TO KME
		sendValues = "pose"

		pose = motionProxy.getBodyAngles()
		#error = motionProxy.getBodyAngleErrors()

		for index in range(len(pose)):
			sendValues += '%' + str( pose[index] ) 

		channel.send(sendValues+'\n\0')


kmeSocket.close()
print( "\n\nConnection Closed!")





