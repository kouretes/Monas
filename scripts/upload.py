#!/usr/bin/python
# -*- coding: utf-8 -*-
import os;
import sys;
import string;
import re;
import commands,socket;

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

	#game = string.find(sys.argv[0] , "upload_work.py")


	#if game > -1:
	if (sys.argv[0] == "upload_work.py"):
		if	 len(sys.argv) < 2 :
			print """
		usage: (python) ../../scripts/upload_work.py Ip1 Ip2 Ip3 etc
					Ip1...: Ips of the robots you want to upload the same configuration!!!
		THE SCRIPT MUST BE NAMED AS upload_work.py or upload_game.py to work

		CAUTION must run the script inside Monad/make/yourebuildfolder
			"""
			exit(1)
		else:
			return
	elif(sys.argv[0] == "upload_game.py"):
		print """
		usage: (python) ../../scripts/upload_game.py SSID Ip1 player1num Ip2 player2num Ip3 player2num etc
		num_of_player: 1: Goalkeeper 2: Defender 3: Attacker
		SSID: name of field the script will look into the config directory
		for a directory field_name_of_field to upload the network files
		files must be under the same path as on robot i.e. etc/network/interfaces.cof :)
		Ip1...: Ips of the robots you want to upload the same configuration!!!

		example upload_game.py Field_D 172.18.9.12 1 172.18.9.13 4 172.18.9.87 3

		CAUTION must run the script inside Monad/make/yourebuildfolder
		"""
	else:
		print """
		You cannot run simply this script. Filename must be upload_work.py or upload_game.py
		Just create links.
		CAUTION must run the script inside Monad/make/yourebuildfolder
		For help just run upload_work.py or upload_game.py

		examples:
		usage: (python) ../../scripts/upload_work.py Ip1 Ip2 Ip3 etc
		usage: (python) ../../scripts/upload_game.py SSID Ip1 player1num Ip2 player2num Ip3 player2num etc

		"""
	exit(-1)

playersdef = ['Goalkeeper', 'Defender', 'Attacker']
#### UPLOAD SCRIPT ####

if string.find(sys.argv[0], "upload_work.py") > -1:
	if(len(sys.argv) < 1 ):
		usage();

	robotsIP = sys.argv[1:len(sys.argv)]
	game = 0
elif string.find(sys.argv[0] , "upload_game.py") > -1 :
	if(len(sys.argv) < 3 ):
		usage();

	SSID = sys.argv[1]
	print sys.argv

	players = sys.argv[3:len(sys.argv):2]
	print players
	for p in players:
		if(int(p) > 3 or int(p) < 1) :
			print "ERROR: A Player num is not valid, Quiting "
			exit(-1)

	robotsIP = sys.argv[2:len(sys.argv):2]
	if(len(players)!=len(robotsIP)):
		usage()

	game = 1 #
else:
	print "ERROR: Please check the filename of the script, must be upload_work.py or upload_game.py, Quiting "
	usage()
	exit(-1)

for ip in robotsIP:
	if(not is_valid_ipv4(ip)):
		print "Ip address " + ip + " is not valid "
		exit(-1)
	else:
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		timeouttime= 2
		sock.settimeout(timeouttime)
		reachable = True
		try:
			print "Trying to connect to " + ip
			sock.connect((ip, 22))
		except socket.error, msg:

			print '\033[1;31m Waited ' + str(timeouttime) +  ' second. Robot with ip ' + ip + ' unreachable \033[1;m'
			reachable = False
		sock.close()

		if(reachable):
			print "\033[1;32m Robot " + ip + " reachable\033[1;m (ssh ok)"


#check that your are one level down from the make folder when you execute  the script
pwd = os.getcwd()
pwdfolders =  pwd.split("/")
if(pwdfolders[-2] != "make"):
	print "ERROR: You are trying to execute upload in different folder than under Monad/make/yourebuildfolder"
	print "Your pwd : " + pwd + "\n Try again from the correct path, Quiting"
	exit(-1)

print "Working directory " + pwd
ret=os.system("make install")
if(ret!=0):
	print '\033[1;31m Unsuccessfull compiling \033[1;m'
	exit(-1)
else:
	print "\033[1;32m Compiling Complete Succesfully\033[1;m"

#now we hope that we are inside the correct folder so the partial configuration is above
partial_configuration_dir = "../../scripts/PartialConfiguration/"
scripts_dir = "../../scripts/"


 #= os.environ["partial_configuration_dir"]
#al_dir = os.environ["AL_DIR"]
binaries_dir = ""
#probable_binaries_path = "/binaries/robot/naoqi_1.6.0_cross/" #under make/buildfolder
naoqi_cross_folder = commands.getoutput("ls ./binaries/robot | grep naoqi")
if(naoqi_cross_folder!=""):
	binaries_dir = "./binaries/robot/" + naoqi_cross_folder +"/"
else:
	print "Cant find any folder naoqi under ./binaries/robot/"
	exit(-1)

os.system("mkdir -p " + binaries_dir + "/preferences")
os.system("mkdir -p " + binaries_dir + "/bin")
os.system("mkdir -p " + binaries_dir + "/lib")

os.system('cp ' + scripts_dir +'Start.py ' + binaries_dir + "bin/")
os.system('cp ' + scripts_dir +'Stop.py ' + binaries_dir + "bin/")
os.system('cp ' + scripts_dir +'autostartkrobot ' + binaries_dir + "bin/")

print "Lenght of arguments " + str(len(sys.argv))

if(partial_configuration_dir	== "" ):
	print("ERROR:  Please define partial_configuration_dir")
	exit(-1)

playerscounter = 0;
for	ip in robotsIP:

	if game == 1 :
		print "Stopping naoqi "
		nao_start_stop_cmd = 'ssh nao@'+ip + " ' /etc/init.d/naoqi stop ' "
		os.system(nao_start_stop_cmd)
		print nao_start_stop_cmd

		player = players[playerscounter]
		#print("Good luck with the game")
		print("You are going to create network files for the player " + player +" the " + playersdef[int(player)-1] + " for the field " + SSID)

		#Copy network configuration file to /config just for backup
		copy_cmd = "cp " + partial_configuration_dir + "/FieldsWlan/" + SSID    + "_wpa_supplicant.conf  " + binaries_dir +"config/wpa_supplicant.conf"
		os.system(copy_cmd)

		print ("Creating parameters for player " + player )
		copy_cmd = "cp " + partial_configuration_dir + "/team_config_part.xml " +  binaries_dir +"config/team_config.xml"
		os.system(copy_cmd)
		playerconf = open(binaries_dir +"config/team_config.xml", 'a')
		playerconf.write("<player>"+ player+"</player>")
		playerconf.close()

	if game == 0:
		playerstr = raw_input("1: Goalkeeper 2: Defender 3: Attacker \n	Set player num or press enter to continue: ")
		while(playerstr != ""):
			player = int(playerstr)
			if(player <= 3 and player >=1):
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
				playerstr = raw_input("1: Goalkeeper 2: Defender 3: Attacker \n	Set player num or press enter to continue: ")
				if(playerstr == ""):
					break

		naoqirestart = raw_input("Enter y to Restart Naoqi or press enter to continue: ")
		if(naoqirestart=='y'):
			print( "Stopping naoqi, will start it after binaries upload")
			nao_start_stop_cmd = ' ssh nao@'+ip + " ' /etc/init.d/naoqi stop ' "
			os.system(nao_start_stop_cmd)

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
		if(raw_input("Enter y to upload a clean autoload.ini_work(no krobot)? or press enter to continue:  ")=='y'):
			print("Setting autoload.ini")
			autoload_src = partial_configuration_dir + "autoload.ini_work"
			autoload_dest = binaries_dir +"preferences/autoload.ini"
			autoload_cmd = "cp " + autoload_src +" "+ autoload_dest
			os.system(autoload_cmd)
			print(autoload_cmd)
			rsync_cmd = "rsync -av " + binaries_dir +"bin "+ binaries_dir	+"lib "+ binaries_dir +"config "+ binaries_dir +"preferences "  + " nao@"+ip+ ":/home/nao/naoqi/"
		else:
			rsync_cmd = "rsync -av " + binaries_dir +"bin "+ binaries_dir	+"lib "+ binaries_dir +"config " + " nao@"+ip+ ":/home/nao/naoqi/"

	#~ rsync_cmd = "rsync	-av "+ binaries_dir +"bin	"+ binaries_dir	+"lib "+ binaries_dir +"config	"+ binaries_dir +"preferences	"+" nao@" + ip+ ":naoqi/"

	print("Preparing to copy robot from ",binaries_dir)
	print ""

	os.system(rsync_cmd)
	print(rsync_cmd)

	print("every file needed is copied");
	print("TREAT THE ROBOT NICELY.... I know is sooooo difficult to do that")

	if game == 1 :
		os.system(' ssh nao@'+ip + " 'chmod 777 /home/nao/naoqi/bin/autostartkrobot'")

		print( "Sending naoqi start command")
		nao_start_stop_cmd = ' ssh nao@'+ip + " ' /etc/init.d/naoqi start ' "
		os.system(nao_start_stop_cmd)

		wifi_conf_cp_cmd ='ssh root@'+ip  + " 'cp /home/nao/naoqi/config/wpa_supplicant.conf /etc/wpa_supplicant.conf' "
		os.system(wifi_conf_cp_cmd)
		print wifi_conf_cp_cmd
		os.system('ssh root@'+ip + " '/etc/init.d/wpa_supplicant.sh '")

	if (game == 0 and naoqirestart=="y" ):
		print( "Sending naoqi start command")
		nao_start_stop_cmd = ' ssh nao@'+ip + " ' /etc/init.d/naoqi start ' "
		os.system(nao_start_stop_cmd)
		#~ os.system('ssh root@'+ip + " 'ifdown wlan0 '")
		#~ os.system('ssh root@'+ip + " 'ifup wlan0 '")
