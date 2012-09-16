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
	usage: (python) ../../scripts/clean.py IP

		IP: IP of the robots on which the same configuration will be uploaded
	"""
	exit(-1)

#### UPLOAD SCRIPT ####

if (len(sys.argv) != 2):
		usage();
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
	print "\033[1;32m Robot " + ip + " is unreachable\033[1;m (ssh failed)"
	exit(-1)


nao_cmd = ' ssh nao@'+ip + " ' rm -r naoqi/config naoqi/lib' "
os.system(nao_cmd)

