#!/usr/bin/python
# -*- coding: utf-8 -*-
import os;
import subprocess;
import sys;
import string;
import re;
import commands,socket;
from subprocess import call
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
	usage: (python) initializeStick.py IP

		IP: IP of the robot
		Must be run from the scripts folder

	"""
	exit(-1)

if (len(sys.argv) != 2):
	usage();
	exit(-1);
else:
	ip = sys.argv[1]

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
else:
	exit(-1);


pwd = os.getcwd()
print "Working directory: " + pwd


print "Changing /etc/profile"
ssh_mkdir = "ssh nao@" + ip + " 'mkdir ./.ssh'"
ssh_transfer0 = "scp ./PartialConfiguration/authorized_keys nao@" + ip + ':/home/nao/.ssh/authorized_keys'
ssh_transfer1 = "scp ./PartialConfiguration/autoload.ini_etc nao@" + ip + ':/home/nao/autoload.ini'
ssh_transfer2 = "scp connman nao@" + ip + ':/home/nao/connman'
ssh_transfer3 = "scp enable_krobot.sh disable_krobot.sh nao@" + ip + ':~/'
ssh_transfer4 = "scp ./PartialConfiguration/empty_autodiagnostic_behavior.xar nao@" + ip + ':/home/nao/behavior.xar'
ssh_transfer5 = "scp ./PartialConfiguration/autoload.ini nao@" + ip + ':/home/nao/naoqi/preferences/autoload.ini'
print ssh_mkdir
os.system(ssh_mkdir)
print ssh_transfer0
os.system(ssh_transfer0)
print ssh_transfer1
os.system(ssh_transfer1)
print ssh_transfer2
os.system(ssh_transfer2)
print ssh_transfer3
os.system(ssh_transfer3)
print ssh_transfer4
os.system(ssh_transfer4)
print ssh_transfer5
os.system(ssh_transfer5)

echo_command1 = "echo export LD_LIBRARY_PATH=\\\"$\\\"LD_LIBRARY_PATH:/home/nao/naoqi/lib/ >> /etc/profile"
echo_command2 = "echo export LD_LIBRARY_PATH=\\\"$\\\"LD_LIBRARY_PATH:/home/nao/naoqi/lib/ >> /etc/profile.d/dbus-session.sh"
echo_command3 = "echo UseDNS no >> /etc/ssh.conf/sshd_config"
mv_command1 = "mv /home/nao/connman /etc/init.d/connman"
mv_command2 = "mv /home/nao/autoload.ini /etc/naoqi/autoload.ini"
mv_command3 = "mv /home/nao/behavior.xar /usr/share/diagnostic/aldebaran_auto_diagnostic/behavior.xar"
ssh_profile = 'ssh -t nao@'+ip+ " 'su -c \"if [ ! -e \\\"/.init\\\" ]; then " + echo_command1 + ";" + echo_command2 + ";"+ echo_command3 + ";"  + mv_command1 + ";" + mv_command2 + ";" + mv_command3 + "; chmod a+s /sbin/shutdown; chmod a+s /etc/init.d/naoqi; echo initialized > /.init; else rm /home/nao/connman /home/nao/autoload.ini; fi\"'"
print ssh_profile
os.system(ssh_profile)








