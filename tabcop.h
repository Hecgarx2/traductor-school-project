#ifndef TABCOP_H_INCLUDED
#define TABCOP_H_INCLUDED
#include <iostream>
#include <string>
#include "definiciones.h"

using namespace std;

class Tabcop
{
private:
    string mnemonico;
    string direccionamiento;
    int longitudInstruccion;
    string codigoInstruccion;
    int cantidadInstrucciones;
public:
    Tabcop(string mnemonico, string direccionamiento, int longInstruccion, string codigoInstruccion);
    Tabcop(string mnemonico);
    Tabcop();
    ~Tabcop();
    void mostrar();
    string getMnemonico();
    string getDireccionamiento();
    int getLongitudInstruccion();
    string getCodigoInstruccion();
    int getCantidadInstrucciones();
};

Tabcop::Tabcop(string mnemonico, string direccionamiento, int longInstruccion, string codigoInstruccion){
    this->mnemonico = mnemonico;
    this->direccionamiento = direccionamiento;
    this->longitudInstruccion = longInstruccion;
    this->codigoInstruccion = codigoInstruccion;
}

Tabcop::Tabcop(string mnemonico){
    string subCadena;
    int posInicial = 0, posFinal, contDelimitador = 0;
    while (posFinal < mnemonico.length()){
        posFinal = mnemonico.find_first_of(delimitadorCampoTABCOP, posInicial);
        subCadena = mnemonico.substr(posInicial, posFinal-posInicial);
        contDelimitador++;
        switch (contDelimitador)
        {
        case 1:
            this->mnemonico = subCadena;
            break;
        case 2:
            this->direccionamiento = subCadena;
            break;
        case 3:
            this->longitudInstruccion = stoi(subCadena);
            break;
        case 4:
            this->codigoInstruccion = subCadena;
            break;
        case 5:
            this->cantidadInstrucciones = stoi(subCadena);
            break;    
        default:
            break;
        }
        posInicial = posFinal+1;
    }
}

Tabcop::Tabcop(){}

Tabcop::~Tabcop()
{
}

void Tabcop::mostrar(){
    cout<<mnemonico<<delimitadorCampoTABCOP<<direccionamiento<<delimitadorCampoTABCOP<<longitudInstruccion
    <<delimitadorCampoTABCOP<<codigoInstruccion<<delimitadorCampoTABCOP<<cantidadInstrucciones<<delimitadorCampoTABCOP<<endl;
}

string Tabcop::getMnemonico(){
    return this->mnemonico;
}

string Tabcop::getDireccionamiento(){
    return this->direccionamiento;
}

int Tabcop::getLongitudInstruccion(){
    return this->longitudInstruccion;
}

string Tabcop::getCodigoInstruccion(){
    return this->codigoInstruccion;
}

int Tabcop::getCantidadInstrucciones(){
    return this->cantidadInstrucciones;
}

#endif