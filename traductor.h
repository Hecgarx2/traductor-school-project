#ifndef TRADUCTOR_H_INCLUDED
#define TRADUCTOR_H_INCLUDED
#include <iostream>
#include <string>
#include "definiciones.h"
#include "tabcop.h"

using namespace std;

void leerLineas(Tabcop tabcop[], string &cadenaFinal);
void leerCampos(Tabcop tabcop[], string &cadenaFinal, string linea);
void analizarInstruccion(Tabcop tabcop[], string &cadenaFinal, string nombreDato);
void saltarEtiqueta(string &nombreDato);

void leerLineas(Tabcop tabcop[], string &cadenaFinal){
    string linea;
    int posInicialLinea = 0, posFinalLinea;
    while (posInicialLinea != cadenaFinal.size()){
        posFinalLinea = cadenaFinal.find_first_of(delimitadorLinea, posInicialLinea);
        linea = cadenaFinal.substr(posInicialLinea, posFinalLinea-posInicialLinea);
        leerCampos(tabcop, cadenaFinal, linea);
        posInicialLinea = posFinalLinea+1;
    }
}

void leerCampos(Tabcop tabcop[], string &cadenaFinal, string linea){
    string nombreDato;
    int posInicialDato = 0, posFinalDato;
    while (posInicialDato != linea.size()){
        posFinalDato = linea.find_first_of(delimitadorCampoP, posInicialDato);
        if (posFinalDato == -1){
            posFinalDato = linea.size();
        }
        nombreDato = linea.substr(posInicialDato, posFinalDato);
        saltarEtiqueta(nombreDato);
        analizarInstruccion(tabcop, cadenaFinal, nombreDato);
        if (posFinalDato != linea.size()){
            posInicialDato = posFinalDato+1;
        }
        else{
            posInicialDato = linea.size(); 
        }
    }
}

void saltarEtiqueta(string &nombreDato){
    int posInicial = 0;
    if (nombreDato[posInicial] == '\t'){
        nombreDato = nombreDato.substr(posInicial+1, nombreDato.size()-1 );
    }
}

void analizarInstruccion(Tabcop tabcop[], string &cadenaFinal, string nombreDato){
    for (int i = 0; i < cantidadMnemonicos; i++){
        if (nombreDato == tabcop[i].getMnemonico()){
            cout<<nombreDato<<dobleTab<<tabcop[i].getCantidadInstrucciones()<<endl;
        }
    }  
}

#endif