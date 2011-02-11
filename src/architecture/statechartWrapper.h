#ifndef STATECHARTWRAPPER_H
#define STATECHARTWRAPPER_H 1

#include "architecture/statechartEngine/Statechart.h"

class StatechartWrapper {

  public:

    StatechartWrapper () {;}

    virtual void Start ()=0;

    virtual void Stop ()=0;

};

typedef GenericFactory < StatechartWrapper, std::string,StatechartWrapper* (*)(Narukom*),Narukom* >  StatechartFactory;

template<class T>
struct StatechartRegistrar {
  typedef Registrar<StatechartFactory,StatechartWrapper,std::string,T,Narukom*> Type;
};

#endif // STATECHARTWRAPPER_H