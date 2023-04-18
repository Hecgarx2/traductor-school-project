#ifndef ETIQUETA_H_INCLUDED
#define ETIQUETA_H_INCLUDED
#include <iostream>
#include <string>
#include "definiciones.h"

using namespace std;

class Etiqueta
{
private:
    string nombre;
    string memoria;
public:
    Etiqueta(string nombre);
    Etiqueta();
    ~Etiqueta();
    string getNombre(string etiqueta);
    string getMemoria();
    void setMemoria(string memoria);
};

Etiqueta::Etiqueta(string nombre)
{
    this->nombre = nombre;
}

Etiqueta::Etiqueta()
{}

Etiqueta::~Etiqueta()
{
}

string Etiqueta::getNombre(string etiqueta){
    return this->nombre;
}

string Etiqueta::getMemoria(){
    return this->memoria;
}

void Etiqueta::setMemoria(string memoria){
    this->memoria = memoria;
}

#endif