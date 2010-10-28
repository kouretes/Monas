
ifndef os
   os = LINUX
endif

ifndef arch
   arch = IA32
endif

CXXFLAGS =	  -O3 -g -Wall -fmessage-length=0 -D$(os)  -finline-functions

LDFLAGS =  -lstdc++ -lpthread -lm `pkg-config --cflags --libs opencv`

OBJS =	*.o

TARGET = KLocView


#$(TARGET):	$(OBJS)
#	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)


all:	$(TARGET)


KLocView:   msges
	$(CXX) -o KLocView -I ../../../external/protobuf/src/ KLocView.cpp KfieldGui.cpp  ../../../external/PracticalSocket/PracticalSocket.cpp VisionObservations.pb.o   WorldInfo.pb.o ../../architecture/narukom/cmake/linux/lib/libprotobuf.a   $(LDFLAGS)


clean:
	rm -f $(OBJS) $(TARGET)


msges:
	$(CXX) -c  -I ../../../external/protobuf/src/ ../../messages/VisionObservations.pb.cc ../../messages/WorldInfo.pb.cc
