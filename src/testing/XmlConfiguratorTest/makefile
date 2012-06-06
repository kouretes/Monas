# this is a example of Makefile

#all: lala
#
#lala: XmlConfigurator.o tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o
#	g++ XmlConfigurator.o -o myProg
#
#tinystr.o: tinystr.cpp
#	g++ -c -DTINYXML_USE_STL tinystr.cpp
#tinyxml.o: tinyxml.cpp
#	g++ -c -DTINYXML_USE_STL tinyxml.cpp
#tinyxmlerror.o: tinyxmlerror.cpp
#	g++ -c -DTINYXML_USE_STL tinyxmlerror.cpp
#tinyxmlparser.o: tinyxmlparser.cpp
#	g++ -c -DTINYXML_USE_STL tinyxmlparser.cpp
#	
#XmlConfigurator.o: XmlConfigurator.cpp
#	g++ -c XmlConfigurator.cpp
CC=g++
CFLAGS=-c -DTINYXML_USE_STL
LDFLAGS=
SOURCES=XmlConfigurator.cpp tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=out

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm $(OBJECTS)
