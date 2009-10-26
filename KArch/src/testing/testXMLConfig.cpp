#include <iostream>

#include <string>
#include "tools/XMLConfig.h"


template<class T>
void printer ( T el ) {
    std::cout<<"Element : "<<el<<std::endl;
}

template<class T> 
void printer ( std::pair<const std::string,T> el) {
    std::cout<<"Element : "<<el.first<<" "<<el.second<<std::endl;
}

template<class T> 
void printer ( const std::vector<T> el) {
    int i=0;
    for ( typename std::vector<T>::const_iterator it = el.begin(); it != el.end(); ++it, i++) {
        std::cout<<"Tuple "<<i<<" ";
        printer ( (*it) );
    }
}

template<class Y,class T> 
void printer ( const std::map<Y,T> el) {
    int i=0;
    for ( typename std::map<Y,T>::const_iterator it = el.begin(); it != el.end(); ++it, i++) {
        std::cout<<"Tuple "<<i<<" ";
        printer ( (*it) );
    }
}



int main () {

    XMLConfig config("test.xml");

    /********************************************
                       Setting
    ********************************************/
/*
    //Testing basic types 
    config.SetElement("doulbe",0.23);
    config.SetElement("int",2322);
    config.SetElement("string","astring");
    config.SetElement("string","correct_string");

    //Testing basic types with iteration
    config.SetElement("iter_int",1,true);
    config.SetElement("iter_int",2,true);
    config.SetElement("iter_int",3,true);
    config.SetElement("iter_int",4,true);
    config.SetElement("iter_string","aaa",true);
    config.SetElement("iter_string","bbb",true);
    config.SetElement("iter_string","ccc",true);
    config.SetElement("iter_string","ddd",true);
    config.SetElement("iter_string","eee",true);
  
    //Testing maps
    std::pair<std::string,int> p1 ("TheASDF1",12);
    std::pair<std::string,int> p2 ("TheASDF2",22);
    std::pair<std::string,int> p3 ("TheASDF3",32);
    std::pair<std::string,int> p4 ("TheASDF4",42);

    std::map<std::string,int>  v1;
    v1.insert(p1);
    v1.insert(p2);
    v1.insert(p3);
    v1.insert(p4);

    config.SetElement("map_int", v1);

    std::pair<std::string,float> p11 ("TheASDF1",0.12);
    std::pair<std::string,float> p12 ("TheASDF2",0.22);
    std::pair<std::string,float> p13 ("TheASDF3",0.32);
    std::pair<std::string,float> p14 ("TheASDF4",0.42);

    std::map<std::string,double> v2;
    v2.insert(p11);
    v2.insert(p12);
    v2.insert(p13);
    v2.insert(p14);

    config.SetElement("map_double", v2);

    std::pair<std::string,int> p21 ("correctTheASDF1",12);
    std::pair<std::string,int> p22 ("correctTheASDF2",22);
    std::pair<std::string,int> p23 ("correctTheASDF3",32);
    std::pair<std::string,int> p24 ("correctTheASDF4",42);

    std::map<std::string,int>  v3;
    v3.insert(p21);
    v3.insert(p22);
    v3.insert(p23);
    v3.insert(p24);

    config.SetElement("map_int", v3);

    //Testing Iterative Maps 

    std::pair<std::string,double> p31 ("TheASDF1",0.12);
    std::pair<std::string,double> p32 ("TheASDF2",0.22);
    std::pair<std::string,double> p33 ("TheASDF3",0.32);
    std::pair<std::string,double> p34 ("TheASDF4",0.42);

    std::map<std::string,double>  v4;
    v4.insert(p31);
    v4.insert(p32);
    v4.insert(p33);
    v4.insert(p34);

    config.SetElement("map_double", v4, true);    
    
    config.SaveConfiguration(); */ 

    /********************************
                 Getting
    ********************************/
    
    //Testing basic types

    bool found = false;

    double db;
    found = config.QueryElement("double",db);
    std::cout<< found<< db <<std::endl;

    int aInt;
    found = config.QueryElement("int",aInt);
    std::cout<< found << aInt <<std::endl;

    std::string aString;
    found = config.QueryElement("string", aString);
    std::cout<< found << aString <<std::endl;

    //Testing basic types with iter 

    std::vector<int> intVec;
    found = config.QueryElement("iter_int",intVec);
    std::cout<< found<< "IntVec size : " << intVec.size() <<std::endl;

    printer(intVec);
    
    std::vector<std::string> strVec;
    found = config.QueryElement("iter_string",strVec);
    std::cout<< found << "StrVec size : "<< strVec.size() <<std::endl;

    printer(strVec);

    //Testing Maps

    std::map<std::string,int>  pair1;
    found = config.QueryElement("map_int", pair1);    
    std::cout<< found << "map_int size : "<< pair1.size() <<std::endl;

    printer(pair1);

    std::map<std::string,double>  pair2;
    found = config.QueryElement("map_double", pair2);    
    std::cout<< found << "map_double size : "<< pair2.size() <<std::endl;

    printer(pair2);

    //Testing Iter pairs

    std::vector<std::map<std::string,double> >   pair3;
    found = config.QueryElement("map_double", pair3);    
    std::cout<< found << "map_double size : "<< pair3.size() <<std::endl;

    printer( pair3[0]);






    return 0;

}
