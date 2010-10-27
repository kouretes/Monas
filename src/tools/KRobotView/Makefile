
ifndef os
   os = LINUX
endif

ifndef arch
   arch = IA32
endif

CXXFLAGS =	-O3 -g -Wall -fmessage-length=0 -D$(os)  -finline-functions

LDFLAGS = -L -lstdc++ -lpthread -lm `pkg-config --cflags --libs opencv protobuf`

OBJS =	KRobotView.o

TARGET = KRobotView


#$(TARGET):	$(OBJS)
#	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)


all:	$(TARGET)


%.o: %.cc
	$(C++) $(CCFLAGS) $< -c

KRobotView: KRobotView.o
	$(CXX) -o KRobotView KRobotView.o  ../../../external/PracticalSocket/PracticalSocket.cpp ../../messages/Kimage.pb.cc ../../messages/VisionObservations.pb.cc ../../messages/WorldInfo.pb.cc  $(LDFLAGS)



clean:
	rm -f $(OBJS) $(TARGET)
