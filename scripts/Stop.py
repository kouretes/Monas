#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
"""
  This is a simple python script to test the autogenerated module

"""
print '        START TESTING mainModule MODULE'

from naoqi import ALProxy
import sys

IP = sys.argv[1]
PORT = 9559

#________________________________
# Generic Proxy creation
#________________________________

try:
  mainModule_Proxy = ALProxy("mainModule",IP,PORT)
except Exception,e:
  print "Error when creating STMOBJECT_NAME  proxy:"
  print str(e)
  exit(1)

#________________________________
# Remote procedure call
#________________________________
inParam = ["test", 1, 45, [123, 456, "789"]]

try:
  ret = mainModule_Proxy.Stop(  )
except Exception,e:
  print "STMOBJECT_NAME test Failed"
  exit(1)

exit(0)
