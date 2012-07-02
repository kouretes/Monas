#/bin/sh
sed -e 's/\(#\)\([.|a-z|/]*\)krobot/\2krobot/' -e 's/[#]*naoqi-autolaunch/#naoqi-autolaunch/' naoqi/preferences/autoload.ini > naoqi/preferences/autoload_temp.ini
mv naoqi/preferences/autoload_temp.ini naoqi/preferences/autoload.ini 
