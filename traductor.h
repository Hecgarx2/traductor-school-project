#ifndef TRADUCTOR_H_INCLUDED
#define TRADUCTOR_H_INCLUDED
#include <iostream>
#include <string>
#include "definiciones.h"
#include "tabcop.h"

using namespace std;

void leerLineas(Tabcop tabcop[], string &cadenaFinal);
void leerCampos(Tabcop tabcop[], string &cadenaFinal, string &linea);
void saltarEtiqueta(string &nombreDato);
int buscarInstruccion(Tabcop tabcop[], string nombreDato);
void buscarDireccionamiento(Tabcop tabcop[], string nombreDat, int indice, string linea);

void leerLineas(Tabcop tabcop[], string &cadenaFinal){
    string linea;
    int posInicialLinea = 0, posFinalLinea, cadenaModificada;
    while (posInicialLinea != cadenaFinal.size()){
        posFinalLinea = cadenaFinal.find_first_of(delimitadorLinea, posInicialLinea);
        linea = cadenaFinal.substr(posInicialLinea, posFinalLinea-posInicialLinea);
        leerCampos(tabcop, cadenaFinal, linea);
        posInicialLinea = posFinalLinea+1;
    }
}

void leerCampos(Tabcop tabcop[], string &cadenaFinal, string &linea){
    string nombreDato;
    bool existeDireccionamiento = false;
    int posInicialDato = 0, posFinalDato, indice, indiceMnemonico;
    while (posInicialDato != linea.size()){
        posFinalDato = linea.find_first_of(delimitadorCampoP, posInicialDato);
        if (posFinalDato == -1){
            posFinalDato = linea.size();
        }
        nombreDato = linea.substr(posInicialDato, posFinalDato);
        saltarEtiqueta(nombreDato);
        if (nombreDato == "ORG"){
            cout<<linea<<endl;
            posFinalDato = linea.size();
        }
        
        if (!existeDireccionamiento){
            indice = buscarInstruccion(tabcop, nombreDato);
        }
        else{
            buscarDireccionamiento(tabcop, nombreDato, indice, linea);
        }
        
        if (indice != -1){
            if(tabcop[indice].getCantidadInstrucciones() > 1){
                indiceMnemonico = indice;
                existeDireccionamiento = true;
            }
            else{
                cout<<linea<<'\t'<<tabcop[indice].getCodigoInstruccion()<<endl;
            }
        }
        
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

int buscarInstruccion(Tabcop tabcop[], string nombreDato){
    for (int i = 0; i < cantidadMnemonicos; i++){
        if (nombreDato == tabcop[i].getMnemonico()){
            return i;
        }
    }
    return -1;  
}

void buscarDireccionamiento(Tabcop tabcop[], string nombreDato, int indice, string linea){
    int limite = indice + tabcop[indice].getCantidadInstrucciones(), codigo;
    if (nombreDato[0] == '#'){
        for (int i = indice; i < limite; i++){
            if ("IMM" == tabcop[i].getDireccionamiento()){
                cout<<linea<<'\t'<<tabcop[i].getCodigoInstruccion()<<endl;
            }
        }
    }
    else{
        codigo = stoi(nombreDato);
        if (codigo < limiteDecimalOrp8){
            for (int i = indice; i < limite; i++){
                if ("DIR" == tabcop[i].getDireccionamiento()){
                    cout<<linea<<'\t'<<tabcop[i].getCodigoInstruccion()<<endl;
                }
            }
        }
        else if (codigo < limiteDecimalOrp16){
            for (int i = indice; i < limite; i++){
                if ("EXT" == tabcop[i].getDireccionamiento()){
                    cout<<linea<<'\t'<<tabcop[i].getCodigoInstruccion()<<endl;
                }
            }
        }
        else{
            cout<<linea<<'\t'<<"FDR"<<endl;
        }
    }

}

#endif