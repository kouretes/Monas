#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys, os, time
import re;
from subprocess import Popen, list2cmdline

def exec_commands(cmds):
    ''' Exec commands in parallel in multiple process
    (as much as we have CPU)
    '''
    if not cmds: return # empty list

    def done(p):
        return p.poll() is not None
    def success(p):
        return p.returncode == 0
    def fail():
        sys.exit(1)

    max_task = 4
    processes = []
    while True:
        while cmds and len(processes) < max_task:
            task = cmds.pop()
            #print task
            print list2cmdline(task)
            processes.append(Popen(task))

        for p in processes:
            if done(p):
                if success(p):
                    processes.remove(p)
                else:
                    fail()

        if not processes and not cmds:
            break
        else:
            time.sleep(0.05)


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


if len(sys.argv) < 2 :
	print """
		usage: (python) naoqirestart.py Ip1 Ip2 Ip3 etc
					Ip1...: Ips of the robots you want restart naoqi!!!
		"""
	exit(1)

robotsIP = sys.argv[1:len(sys.argv)]

for ip in robotsIP:
		if(not is_valid_ipv4(ip)):
			print "Ip address " + ip + " is not valid "
			exit(-1)
		else:
			print "info:  Ip addresses are valid, don't know if reachable "


commands = [];
for	ip in robotsIP:
	print "Restarting naoqi "
	nao_start_stop_cmd = 'nao@'+ip + " ' /etc/init.d/naoqi restart ' "
	#os.system(nao_start_stop_cmd)
	commands.append(['ssh', nao_start_stop_cmd])

exec_commands(commands)
