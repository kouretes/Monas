Requirements:
	libqt4-dev (installation via synaptic)
	libboost-all-dev
	
	
Build:
	- Enter local folder under Monas/make, if there is no folder create one (mkdir local).
	- Use cmake gui for native compile.
		- Change TargetPlatform to linux.
		- Change TargetRobot to none.
	- Execute make install -jk, where k is the number of cores in your system.
	- Execute make KMonitor
	
Execution:
	- Enter folder binaries/linux
	- Run ./KMonitor


General instructions :

1. For UDP Multicasting ip for networking : 

	233.253.124.144

   ,with static ips, execute : 

   sudo route add -net 224.0.0.0/4 dev eth0

   before ./KMonitor


2. For UDP Multicasting ip for networking :

	226.0.x.1

   ,with static ips, execute :

   sudo route add -net 226.0.0.0/8 dev eth0

   before ./KMonitor
 
