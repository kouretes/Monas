message ( STATUS "" )
message ( STATUS " _..::: Configuring Generic Nao :::.._ " )
message ( STATUS "" )

find_package(qibuild)

#Force Build for 32bit
Monas_global_add_definitions ( -m32 )

message ( STATUS " _..::: Configuring Statechart Engine for Nao robot :::.._ " )

#qi_use_lib(statechartEngine ALCOMMON ALVALUE ALMATH LIBCORE TOOLS BOOST  ) WTF IS THIS

add_library ( GenericNao  ${LibBuildType}
			aldebaran-kme.h
			kAlBroker.h
			aldebaran-motion-extra.h
			aldebaran-sensors.h
			earLed.h
			KImageExtractor.h
			NaoCamera.h
			aldebaran-motion.h
			aldebaran-visiondefinitions.h
			NAOKinematics.h
			NAOKinematics.cpp
			KinematicsDefines.h
			headAngleSensor.h
			robot_consts.h
			robot_consts.cpp
			aldebaran-proxy.h
			aldebaran-vision.h
			kAlBroker.cpp

)
qi_use_lib( GenericNao ALCOMMON)

install ( TARGETS GenericNao
        RUNTIME DESTINATION   bin
        LIBRARY DESTINATION   lib
        ARCHIVE DESTINATION   lib/static )

set ( TargetLinkLibraries ${TargetLinkLibraries} GenericNao  CACHE INTERNAL "" )
