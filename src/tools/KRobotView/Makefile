
ifndef os
   os = LINUX
endif

ifndef arch
   arch = IA32
endif

CXXFLAGS =	-O3 -g -Wall -fmessage-length=0 -D$(os)  -finline-functions

LDFLAGS = -L -lstdc++ -lpthread -lm

OBJS =	KRobotView.o Kimage.pb.o

TARGET = KRobotView


#$(TARGET):	$(OBJS)
#	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)


all:	$(TARGET)


%.o: %.cc
	$(C++) $(CCFLAGS) $< -c

KRobotView: KRobotView.o
	$(CXX) -o KRobotView KRobotView.o  PracticalSocket.cpp Kimage.pb.cc VisionObservations.pb.cc WorldInfo.pb.cc  $(LDFLAGS) `pkg-config --cflags --libs opencv protobuf`



clean:
	rm -f $(OBJS) $(TARGET)
