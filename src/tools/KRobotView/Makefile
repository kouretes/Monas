
ifndef os
   os = LINUX
endif

ifndef arch
   arch = IA32
endif

CXXFLAGS =	  -O3 -g -Wall -fmessage-length=0 -D$(os)  -finline-functions

LDFLAGS =  -lstdc++ -lpthread -lm `pkg-config --cflags --libs opencv` 

OBJS =	KRobotView.o

TARGET = KRobotView


#$(TARGET):	$(OBJS)
#	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)


all:	$(TARGET)


KRobotView:   msges
	$(CXX) -o KRobotView -I ../../../external/protobuf/src/ KRobotView.cpp   ../../../external/PracticalSocket/PracticalSocket.cpp Kimage.pb.o  VisionObservations.pb.o   WorldInfo.pb.o ../../architecture/narukom/cmake/linux/lib/libprotobuf.a   $(LDFLAGS)


clean:
	rm -f $(OBJS) $(TARGET)


msges: Kimage.pb.o  VisionObservations.pb.o   WorldInfo.pb.o
	$(CXX) -c  -I ../../../external/protobuf/src/ ../../messages/Kimage.pb.cc ../../messages/VisionObservations.pb.cc ../../messages/WorldInfo.pb.cc
