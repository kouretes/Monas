#!/usr/bin/python
# -*- coding: utf-8 -*-
import os;
import subprocess;
import sys;
import string;
import re;
import commands,socket;
from subprocess import call
def newcmommand():
	print("-------------------------------------------------------------------------------------------------------")
def endcmommand():
	print("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")


def is_valid_ipv4(ip):
	"""Validates IPv4 addresses.
	"""
	pattern = re.compile(r"""
			^
			(?:
				# Dotted variants:
				(?:
					# Decimal 1-255 (no leading 0's)
					[3-9]\d?|2(?:5[0-5]|[0-4]?\d)?|1\d{0,2}
				|
					0x0*[0-9a-f]{1,2}	# Hexadecimal 0x0 - 0xFF (possible leading 0's)
				|
					0+[1-3]?[0-7]{0,2} # Octal 0 - 0377 (possible leading 0's)
				)
				(?:									# Repeat 0-3 times, separated by a dot
					\.
					(?:
						[3-9]\d?|2(?:5[0-5]|[0-4]?\d)?|1\d{0,2}
					|
						0x0*[0-9a-f]{1,2}
					|
						0+[1-3]?[0-7]{0,2}
					)
				){0,3}
			|
				0x0*[0-9a-f]{1,8}		# Hexadecimal notation, 0x0 - 0xffffffff
			|
				0+[0-3]?[0-7]{0,10}	# Octal notation, 0 - 037777777777
			|
				# Decimal notation, 1-4294967295:
				429496729[0-5]|42949672[0-8]\d|4294967[01]\d\d|429496[0-6]\d{3}|
				42949[0-5]\d{4}|4294[0-8]\d{5}|429[0-3]\d{6}|42[0-8]\d{7}|
				4[01]\d{8}|[1-3]\d{0,9}|[4-9]\d{0,8}
			)
			$
	""", re.VERBOSE | re.IGNORECASE)
	return pattern.match(ip) is not None


def usage():

	print """
	usage: (python) ../../scripts/upload.py work IP1 IP2 IP3 ...

		IPi: IPs of the robots on which the same configuration will be uploaded
		CAUTION: must run from inside the Monas/make/[build|crossbuild] folder

	usage: (python) ../../scripts/upload.py game SSID IP1 PL1num IP2 PL2num IP3 PL3num ...

		SSID: ssid of the field wifi, the script will look the corresponding network file
		IPi: IPs of the robots on which the same configuration will be uploaded
		PLinum: 1-Goalkeeper, 2-Defender, 3-Midfielder, 4-Attacker
		CAUTION: must run from inside the Monas/make/[build|crossbuild] folder
	"""
	exit(-1)

def Ksystem(command, message, TerminateOnError):
	ret=os.system(command)
	if(ret!=0):
		print '\033[1;31m Unsuccessful '+ message + '\033[1;m'
		if(TerminateOnError):
			exit(-1)
	else:
		print '\033[1;32m '+ message +' Completed Successfully\033[1;m'



playersdef = ['Goalkeeper', 'Defender', 'Midfielder', 'Attacker']
#### UPLOAD SCRIPT ####

if (len(sys.argv) < 2):
		usage();

if string.find(sys.argv[1], "work") > -1:
	if (len(sys.argv) < 3):
		usage();
	robotsIP = sys.argv[2:len(sys.argv)]
	print robotsIP
	game = 0

elif string.find(sys.argv[1] , "game") > -1 :
	if (len(sys.argv) < 5):
		usage();

	SSID = sys.argv[2]
	print SSID

	players = sys.argv[4:len(sys.argv):2]
	print players
	for p in players:
		if(int(p) > 4 or int(p) < 1) :
			print "ERROR: A player number is not valid! Quiting ..."
			exit(-1)

	robotsIP = sys.argv[3:len(sys.argv):2]
	print robotsIP
	if(len(players)!=len(robotsIP)):
		usage()

	game = 1 #
	
else:
	usage()
	exit(-1)


for ip in robotsIP:
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	timeouttime = 2
	sock.settimeout(timeouttime)
	reachable = True
	try:
		print "Trying to connect to " + ip
		sock.connect((ip, 22))
	except socket.error, msg:
		print '\033[1;31m Waited ' + str(timeouttime) +  ' second. Robot with address ' + ip + ' unreachable \033[1;m'
		if(not is_valid_ipv4(ip)):
			print "\033[1;33m Ip address " + ip + " is not a valid IP4 address \033[1;m"
		reachable = False
	sock.close()

	if(reachable):
		print "\033[1;32m Robot " + ip + " reachable\033[1;m (ssh ok)"


#check that your are one level down from the make folder when you execute  the script
pwd = os.getcwd()
pwdfolders =  pwd.split("/")
if(pwdfolders[-2] != "make"):
	print "ERROR: You are trying to execute upload in a folder different than Monas/make/[build|crossbuild]"
	print "Your current path : " + pwd + "\n Try again from the correct path! Quiting ..."
	exit(-1)


print "Working directory: " + pwd

Ksystem("make install", "Compilation", True)

#if(ret!=0):
	#print '\033[1;31m Unsuccessful Compilation \033[1;m'
	#exit(-1)
#else:
	#print "\033[1;32m Compilation Completed Successfully\033[1;m"

#now we hope that we are inside the correct folder so the partial configuration is above
partial_configuration_dir = "../../scripts/PartialConfiguration/"
scripts_dir = "../../scripts/"

#call('aplay -q '+ scripts_dir +'beep.wav', shell=True)
#p = subprocess.Popen('aplay -q '+ scripts_dir +'beep.wav',stdin=None,shell=True)

os.system('aplay -q '+ scripts_dir +'beep.wav &')
#= os.environ["partial_configuration_dir"]
#al_dir = os.environ["AL_DIR"]
binaries_dir = ""
#probable_binaries_path = "/binaries/robot/naoqi_1.6.0_cross/" #under make/buildfolder
naoqi_cross_folder = commands.getoutput("ls ./binaries/robot | grep naoqi")
if(naoqi_cross_folder!=""):
	binaries_dir = "./binaries/robot/" + naoqi_cross_folder +"/"
else:
	print "Can't find any folder naoqi under ./binaries/robot/ Quiting ..."
	exit(-1)

os.system("mkdir -p " + binaries_dir + "/preferences")
os.system("mkdir -p " + binaries_dir + "/bin")
os.system("mkdir -p " + binaries_dir + "/lib")

os.system('rsync -u ' + scripts_dir +'Start.py ' + binaries_dir + "bin/")
os.system('rsync -u ' + scripts_dir +'Stop.py ' + binaries_dir + "bin/")
os.system('rsync -u ' + scripts_dir +'start.sh ' + binaries_dir + "bin/")
os.system('rsync -u ' + scripts_dir +'autostartkrobot ' + binaries_dir + "bin/")
os.system('rsync -u ' + scripts_dir +'beep.wav ' + binaries_dir + "config/")

if(partial_configuration_dir	== "" ):
	print("ERROR:  Please define partial_configuration_dir")
	exit(-1)

playerscounter = 0;
for	ip in robotsIP:

	if game == 1 :
		print "\nStopping NaoQi "
		nao_stop_cmd = 'ssh nao@'+ip + " ' /etc/init.d/naoqi stop ' "
		print nao_stop_cmd
		os.system(nao_stop_cmd)

		player = players[playerscounter]
		print("\nYou are going to create network files for player " + player +" (the " + playersdef[int(player)-1] + ") for the field " + SSID)

		#Copy network configuration file to /config just for backup
		copy_cmd = "cp " + partial_configuration_dir + "/FieldsWlanConnMan/" + SSID    + ".profile  " + binaries_dir +"config/connman.profile"
		os.system(copy_cmd)

		print ("Creating parameters for player " + player )
		copy_cmd = "cp " + partial_configuration_dir + "/team_config_part.xml " +  binaries_dir +"config/team_config.xml"
		os.system(copy_cmd)
		playerconf = open(binaries_dir +"config/team_config.xml", 'a')
		playerconf.write("<player>"+ player+"</player>")
		playerconf.close()

	if game == 0:
		playerstr = raw_input("1-Goalkeeper, 2-Defender, 3-Midfielder, 4-Attacker \n Set player number or press enter to continue: ")
		while(playerstr != ""):
			player = int(playerstr)
			if(player <= 4 and player >=1):
				print "Setting player number " + playerstr
				print ("Creating parameters for player " + playerstr )
				copy_cmd = "cp " + partial_configuration_dir + "/team_config_part.xml " +  binaries_dir +"config/team_config.xml"
				os.system(copy_cmd)
				playerconf = open(binaries_dir +"config/team_config.xml", 'a')
				playerconf.write("<player>"+ playerstr+"</player>")
				playerconf.close()
				break
			else:
				print "Wrong playernum, player"
				playerstr = raw_input("1-Goalkeeper, 2-Defender, 3-Midfielder, 4-Attacker \n Set player number or press enter to continue: ")
				if(playerstr == ""):
					break

		naoqirestart = raw_input("Enter y to Restart NaoQi or press enter to continue: ")
		if(naoqirestart=='y'):
			print( "Stopping NaoQi, will start it again after all binaries have been uploaded")
			nao_stop_cmd = ' ssh nao@'+ip + " ' /etc/init.d/naoqi stop ' "
			os.system(nao_stop_cmd)

	#if(raw_input("Enter y to Change hostname or press enter to continue: ")=='y'):
	#	hostname = raw_input("Set hostname: ")
	#	os.system('ssh root@'+ip + " 'echo "+ hostname+ " '")

  # rsync_cmd = "rync  --rsh=\"sshpass -p myPassword ssh -l t\" "
	#exit(0)
	if(game==1):
		autoload_src = partial_configuration_dir + "autoload.ini_game"
		autoload_dest = binaries_dir +"preferences/autoload.ini"
		autoload_cmd = "cp " + autoload_src +" "+ autoload_dest
		os.system(autoload_cmd)
		print(autoload_cmd)
		rsync_cmd = "rsync -av " + binaries_dir +"bin "+ binaries_dir	+"lib "+ binaries_dir +"config "+ binaries_dir +"preferences "  + " nao@"+ip+ ":/home/nao/naoqi/"
	else:
		if(raw_input("Enter y to upload a clean autoload.ini_work (no krobot) or press enter to continue:  ")=='y'):
			print("Setting autoload.ini")
			autoload_src = partial_configuration_dir + "autoload.ini_work"
			autoload_dest = binaries_dir +"preferences/autoload.ini"
			autoload_cmd = "cp " + autoload_src +" "+ autoload_dest
			os.system(autoload_cmd)
			print(autoload_cmd)
			rsync_cmd = "rsync -av " + binaries_dir +"bin "+ binaries_dir	+"lib "+ binaries_dir +"config "+ binaries_dir +"preferences "  + " nao@"+ip+ ":/home/nao/naoqi/"
		else:
			rsync_cmd = "rsync -av " + binaries_dir +"bin "+ binaries_dir	+"lib "+ binaries_dir +"config " + " nao@"+ip+ ":/home/nao/naoqi/"

	print("Preparing to copy robot from ", binaries_dir)
	print ""

	Ksystem(rsync_cmd, "Uploading ", True);
	print(rsync_cmd)

	print("All necessary files have been upload successfully!");
	print("TREAT THE ROBOT NICELY.... I know this is sooooo difficult ...")

	if game == 1 :
		perm_cmd = 'ssh nao@'+ip+ " 'chmod 777 /home/nao/naoqi/bin/autostartkrobot'"
		print(">>> Sending autostart permission command: "+perm_cmd)
		os.system(perm_cmd)
		nao_start_cmd = 'ssh nao@'+ip+" ' /etc/init.d/naoqi start ' "
		print(">>> Sending NaoQi start command: "+nao_start_cmd)
		os.system(nao_start_cmd)
		wifi_conf_cmd = 'ssh nao@'+ip+" 'su -c \"/etc/init.d/connman restart\" ' "
		print(">>> Sending ConnMan restart command: "+wifi_conf_cmd)
		os.system(wifi_conf_cmd)

	if (game == 0 and naoqirestart=="y") :
		nao_start_cmd = 'ssh nao@'+ip+" ' /etc/init.d/naoqi start ' "
		print(">>> Sending NaoQi start command: "+nao_start_cmd)
		os.system(nao_start_cmd)
