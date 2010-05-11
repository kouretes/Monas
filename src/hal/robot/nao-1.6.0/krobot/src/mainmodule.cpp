/**
 * @author Cbm
 *
 * Version : $Id$
 * This file was generated by Aldebaran Robotics ModuleGenerator
 */
 
#include <alproxy.h>
#include <alvalue.h>
#include <alptr.h>
#include <albroker.h>
#include <almodule.h>
#include "mainmodule.h"

#include "hal/robot/generic_nao/kAlBroker.h"


//______________________________________________
// constructor
//______________________________________________
mainModule::mainModule(AL::ALPtr<AL::ALBroker> broker, const std::string& name ): AL::ALModule(broker, name )
{
  setModuleDescription( "This is an autogenerated module, this descriptio needs to be updated. " );

  functionName( "DummyFunction", "mainModule" ,  "A autogenerated dummyFunction." );
  addParam( "msg", "A string describing a message: the module will print this message when this method is called" );
  addParam( "foo", "Another parameter example" );
  setReturn( "return","Returns the foo param" );
  BIND_METHOD( mainModule::dummyFunction );
  
  functionName( "Start", "mainModule" ,  "Method to start Talws" );
  BIND_METHOD( mainModule::Start );
  
  functionName( "Stop", "mainModule" ,  "Method to stop Talws" );
  BIND_METHOD( mainModule::Stop );

  KAlBroker::Instance().SetBroker ( &(*broker) );

  tal = new Talws();
}

//______________________________________________
// destructor
//______________________________________________
mainModule::~mainModule()
{
    delete tal;
}


/**
 * dummy Function
 * @param pMsg Message to show on screen
 * @param pNawak the function will return this parameter
 * @return an alvalue
 */
AL::ALValue mainModule::dummyFunction( const std::string& pMsg, const AL::ALValue& pFoo )
{
  std::cout << "mainModule say : " << pMsg << std::endl;
  if( pFoo.isValid() )
    std::cout << "Return value: " << pFoo.toString( AL::VerbosityMini ) << std::endl;
  else
    std::cout << "The value you send me is invalid" << std::endl;
    
  return pFoo;
}

void mainModule::Start() {
    tal->Start();
}

void mainModule::Stop() {
    tal->Stop();
}
