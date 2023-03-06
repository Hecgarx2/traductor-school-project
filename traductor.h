#ifndef TRADUCTOR_H_INCLUDED
#define TRADUCTOR_H_INCLUDED
#include <iostream>
#include <string>
#include "definiciones.h"
#include "tabcop.h"
#include "conversor.h"

using namespace std;

void leerLineas(Tabcop tabcop[], string &cadenaFinal);
void leerCampos(Tabcop tabcop[], string &cadenaModificada, string &linea, string &memoria);
void saltarEtiqueta(string &nombreDato, string dirMemoria);
void escribirTABSIM(string etiqueta);
string leerTABSIM(Tabcop tabcop[], int indice, bool existeInstruccion);
int buscarInstruccion(Tabcop tabcop[], string nombreDato);
void buscarDireccionamiento(Tabcop tabcop[], string &nombreDato, int indice, string linea, string &cadenaModificada, string &dirMemoria);
void verificarPalabraReservada(string nombreDato, string &dirMemoria, string &cadenaModificada, string linea);
void validarSistemaNumeracion(string &nombreDato, string &cadenaModificada, string &numCodigo);

void leerLineas(Tabcop tabcop[], string &cadenaFinal){
    string linea, cadenaModificada, memoria = "0000";
    int posInicialLinea = 0, posFinalLinea;
    while (posInicialLinea != cadenaFinal.size()){
        posFinalLinea = cadenaFinal.find_first_of(delimitadorLinea, posInicialLinea);
        linea = cadenaFinal.substr(posInicialLinea, posFinalLinea-posInicialLinea);
        leerCampos(tabcop, cadenaModificada, linea, memoria);
        posInicialLinea = posFinalLinea+1;
    }
    cadenaFinal = cadenaModificada;
}

void leerCampos(Tabcop tabcop[], string &cadenaModificada, string &linea, string &memoria){
    string nombreDato;
    bool existeDireccionamiento = false;
    int posInicialDato = 0, posFinalDato, indice, indiceMnemonico, dirMemoria;
    while (posInicialDato != linea.size()){
        posFinalDato = linea.find_first_of(delimitadorCampoP, posInicialDato);
        if (posFinalDato == -1){
            posFinalDato = linea.size();
        }
        nombreDato = linea.substr(posInicialDato, posFinalDato);
        if (posInicialDato == 0){
            saltarEtiqueta(nombreDato, memoria);
        }
        verificarPalabraReservada(nombreDato, memoria, cadenaModificada, linea);
        
        if (!existeDireccionamiento){
            indice = buscarInstruccion(tabcop, nombreDato);
        }
        else{
            buscarDireccionamiento(tabcop, nombreDato, indice, linea, cadenaModificada, memoria);
        }
        
        if (indice != -1){
            if(tabcop[indice].getCantidadInstrucciones() > 1){
                indiceMnemonico = indice;
                existeDireccionamiento = true;
            }
            else{
                cadenaModificada += linea+'\t'+memoria+" "+tabcop[indice].getCodigoInstruccion()+'\n';
                dirMemoria = stoi(memoria);
                dirMemoria = dirMemoria + tabcop[indice].getLongitudInstruccion();
                memoria = to_string(dirMemoria);
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

void saltarEtiqueta(string &nombreDato, string dirMemoria){
    int posInicial = 0, posFinal;
    string etiqueta;
    if (nombreDato[posInicial] == '\t'){
        nombreDato = nombreDato.substr(posInicial+1, nombreDato.size()-1 );
    }
    else{
        posFinal = nombreDato.find_first_of(delimitadorEtiqueta, posInicial);
        etiqueta = nombreDato.substr(posInicial, posFinal);
        etiqueta += " "+dirMemoria;
        escribirTABSIM(etiqueta);
        posInicial = posFinal+1;
        posFinal = nombreDato.size();
        nombreDato = nombreDato.substr(posInicial, posFinal);
    }
}

void verificarPalabraReservada(string nombreDato, string &dirMemoria, string &cadenaModificada, string linea){
    int posInicialDato = 0, posFinalDato = linea.find_first_of(simboloHexadecimal, posInicialDato);
    if (nombreDato == "ORG"){
            cadenaModificada += linea+delimitadorLinea;
            posInicialDato = posFinalDato+1;
            posFinalDato = linea.size();
            nombreDato = linea.substr(posInicialDato, posFinalDato);
            dirMemoria = nombreDato;
    }
    else if (nombreDato == "END"){
        cadenaModificada += linea+'\t'+dirMemoria;
    }
}

void escribirTABSIM(string etiqueta){
    ofstream archivoTabsim;
    archivoTabsim.open(archivoTABSIM, ios::app);
    if (archivoTabsim.is_open()){
        archivoTabsim<<etiqueta<<endl;
        archivoTabsim.close();
    }
}

string leerTABSIM(Tabcop tabcop[], int indice, bool existeInstruccion){
    ifstream archivoTabsim;
    string texto, cadenaMemoria, cadena;
    int memoria;
    archivoTabsim.open(archivoTABSIM, ios::in);
    if (archivoTabsim.is_open()){
        while (!archivoTabsim.eof()){
            getline(archivoTabsim,cadena);
            texto += cadena;
        }
        texto = texto.substr(texto.size() - 4, texto.size());
        if (existeInstruccion){
            memoria = stoi(texto);
            memoria = memoria + tabcop[indice].getLongitudInstruccion();
            cadenaMemoria = to_string(memoria);
            escribirTABSIM(cadenaMemoria);
        }
        archivoTabsim.close();
        return texto;
    }
    return "nel"; 
}

int buscarInstruccion(Tabcop tabcop[], string nombreDato){
    for (int i = 0; i < cantidadMnemonicos; i++){
        if (nombreDato == tabcop[i].getMnemonico()){
            return i;
        }
    }
    return -1;  
}

void buscarDireccionamiento(Tabcop tabcop[], string &nombreDato, int indice, string linea, string &cadenaModificada, string &dirMemoria){
    int limite = indice + tabcop[indice].getCantidadInstrucciones(), codigo, memoria, posInicial = 0, posFinal;
    string numCodigo;
    if (nombreDato[0] == '#'){
        for (int i = indice; i < limite; i++){
            if ("IMM" == tabcop[i].getDireccionamiento()){
                cadenaModificada += linea+'\t'+dirMemoria+" "+tabcop[i].getCodigoInstruccion()+' ';
                memoria = stoi(dirMemoria);
                memoria = memoria + tabcop[i].getLongitudInstruccion();
                dirMemoria = to_string(memoria);

                posFinal = nombreDato.size();
                nombreDato = nombreDato.substr(posInicial+1, posFinal);
                validarSistemaNumeracion(nombreDato, cadenaModificada, numCodigo);
                cadenaModificada += numCodigo+'\n';
            }
        }
    }
    else{
        validarSistemaNumeracion(nombreDato, cadenaModificada, numCodigo);
        codigo = stoi(nombreDato);
        if (codigo < limiteDecimalOrp8){
            for (int i = indice; i < limite; i++){
                if ("DIR" == tabcop[i].getDireccionamiento()){
                    cadenaModificada += linea+'\t'+dirMemoria+" "+tabcop[i].getCodigoInstruccion()+' ';
                    memoria = stoi(dirMemoria);
                    memoria = memoria + tabcop[i].getLongitudInstruccion();
                    dirMemoria = to_string(memoria);
                    cadenaModificada += numCodigo+'\n';
                }
            }
        }
        else if (codigo < limiteDecimalOrp16){
            for (int i = indice; i < limite; i++){
                if ("EXT" == tabcop[i].getDireccionamiento()){
                    cadenaModificada += linea+'\t'+dirMemoria+" "+tabcop[i].getCodigoInstruccion()+' ';
                    memoria = stoi(dirMemoria);
                    memoria = memoria + tabcop[i].getLongitudInstruccion();
                    dirMemoria = to_string(memoria);
                    cadenaModificada += numCodigo+'\n';
                }
            }
        }
        else{
            cadenaModificada += linea+'\t'+"FDR"+'\n';
        }
    }

}

void validarSistemaNumeracion(string &nombreDato, string &cadenaModificada, string &numCodigo){
    int posInicial = 0, posFinal, numero;
    if (nombreDato[0] == simboloHexadecimal){
        posFinal = nombreDato.size();
        nombreDato = nombreDato.substr(posInicial+1, posFinal);

    }
    else if (nombreDato[0] == simboloOctal){
        cadenaModificada += "Octal\n";
        
        posFinal = nombreDato.size();
        nombreDato = nombreDato.substr(posInicial+1, posFinal);
    }
    else if (nombreDato[0] == simboloBinario){
        cadenaModificada += "Binario\n";
        
        posFinal = nombreDato.size();
        nombreDato = nombreDato.substr(posInicial+1, posFinal);
    }
    else{
        numero = stoi(nombreDato);
        numCodigo = decimalAHexa(numero);
    }
}

#endif