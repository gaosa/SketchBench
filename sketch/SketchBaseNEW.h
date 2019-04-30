#ifndef SKETCH_BASE_H
#define SKETCH_BASE_H
#define REGISTER(className)                                             \
    className* objectCreator##className(){                              \
        return new className;                                           \
    }                                                                   \
    RegisterAction g_creatorRegister##className(                        \
        #className,(PTRCreateObject)objectCreator##className)

#include <cstdio>
#include <string>
#include <map>
#include <iostream>
using namespace std;

typedef void* (*PTRCreateObject)(void);

class ClassFactory{
private:
    map<string, PTRCreateObject> m_classMap ;
    ClassFactory(){}; //构造函数私有化

public:
    void* getClassByName(string className);
    void registClass(string name, PTRCreateObject method) ;
    static ClassFactory& getInstance() ;
};

void* ClassFactory::getClassByName(string className){
    map<string, PTRCreateObject>::const_iterator iter;
    iter = m_classMap.find(className) ;
    if ( iter == m_classMap.end() )
        return NULL ;
    else
        return iter->second() ;
}

void ClassFactory::registClass(string name, PTRCreateObject method){
    m_classMap.insert(pair<string, PTRCreateObject>(name, method)) ;
}

ClassFactory& ClassFactory::getInstance(){
    static ClassFactory sLo_factory;
    return sLo_factory ;
}

class RegisterAction{
public:
    RegisterAction(string className,PTRCreateObject ptrCreateFn){
        ClassFactory::getInstance().registClass(className,ptrCreateFn);
    }
};

class SketchBase
{
    public:
        std::string sketch_name;// store sketch name
        virtual ~SketchBase(){}
        virtual void Insert(const char *str, const int & len) = 0;
        virtual int frequencyQuery(const char *str, const int & len) = 0;
        virtual int topkQuery(const int & k) = 0;
        virtual void parameterSet(const std::string& parameterName, double & parameterValue)=0;
        virtual void init() = 0;
        virtual void reset() = 0;//reset sketch to the initial state
};
#endif