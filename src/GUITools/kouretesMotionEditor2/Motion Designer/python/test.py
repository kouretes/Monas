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




editorPort = 50000

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
			#motionProxy.stiffnessInterpolation("Body", 1.0,0.6)
			#motionProxy.setBodyStiffness(0.8,0.6)
		elif "stiffness_off_all" in str(data) :
			print "stiffness off"
			#motionProxy.stiffnessInterpolation("Body", 0.0, 0.6)
			#motionProxy.setBodyStiffness(0.0, 0.6)

		elif "disconnect" in data :
			break
			
		elif "play" in data or "pose" in data :
			
			values = data.split('%')
			#values.pop( 0 ) #delete "play" or "pose"
			#values.pop()	#delete "end"
			#duration = float(values.pop()) # get duration and delete it from the list
			duration = 1.0

			#for index in range(len(values)):
				#pass				
			#	values[index] = float( values[index] )


			if( duration < 0.2 ):		#We love our robot...
				duration = 2.5

			#motionProxy.gotoBodyAngles(values,duration,motion.INTERPOLATION_LINEAR)



		
		
	except KeyboardInterrupt:
		break

	except socket.timeout:

		sendValues = "pose"

		#pose = motionProxy.getBodyAngles()
		#error = motionProxy.getBodyAngleErrors()

		#for index in range(len(pose)):
		#	sendValues += '%' + str( pose[index] ) 

		channel.send('play%0.1%0.11%0.12%0.13%0.14%-0.15%0.17%0.18%-0.19%0.2%0.21%0.22%0.23%0.24%-0.25%0.26%0.27%0.28%0.29%-0.3%-0.31%-0.32%0.33%0.34%0.35%0.36\n\0'+'\0')


kmeSocket.close()
print( "\n\nConnection Closed!")





