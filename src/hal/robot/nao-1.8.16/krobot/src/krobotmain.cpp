/**
 * \mainpage
 * \section Author
 * @author Cbm
 *
 * \section Copyright
 * Version : $Id$
 *
 * \section Description
 *
 * This file was generated by Aldebaran Robotics ModuleGenerator
 */

#include <signal.h>
#include <alcore/alptr.h>
#include <alcommon/albroker.h>
#include <alcommon/almodule.h>
#include <alcommon/albrokermanager.h>
#include <alcommon/altoolsmain.h>

#include "mainmodule.h"
#include "button_listener_module.h"



#ifdef KROBOT_IS_REMOTE_OFF

#ifdef _WIN32
    #define ALCALL __declspec(dllexport)
#else
    #define ALCALL
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif


ALCALL int _createModule( AL::ALPtr<AL::ALBroker> pBroker )
{      
  // init broker with the main broker inctance 
  // from the parent executable
  AL::ALBrokerManager::setInstance(pBroker->fBrokerManager.lock());
  AL::ALBrokerManager::getInstance()->addBroker(pBroker);

  AL::ALModule::createModule<mainModule>( pBroker, "mainModule" );
#ifndef WEBOTS
  AL::ALModule::createModule<ButtonListener>(pBroker,"ButtonListener");
#else
  cout << "Webots build" << endl;
#endif
  return 0;
}

ALCALL int _closeModule(  )
{
  return 0;
}

# ifdef __cplusplus
}
# endif

#ifdef KROBOT_IS_REMOTE_ON

int main( int argc, char *argv[] )
{
  // pointer to createModule
  TMainType sig;
  sig = &_createModule;

  // call main
  ALTools::mainFunction("Krobot",argc, argv,sig);
}
#endif
