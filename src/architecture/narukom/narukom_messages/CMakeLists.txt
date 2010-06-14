message(STATUS "Narukom Messages")

add_library(narukom_messages Envelope.pb.cc)

install ( TARGETS narukom_messages
RUNTIME DESTINATION   bin
LIBRARY DESTINATION   lib
ARCHIVE DESTINATION   lib/static )
