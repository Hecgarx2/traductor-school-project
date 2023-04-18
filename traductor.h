#ifndef TRADUCTOR_H_INCLUDED
#define TRADUCTOR_H_INCLUDED
#include <iostream>
#include <string>
#include "definiciones.h"
#include "tabcop.h"
#include "etiqueta.h"
#include "conversor.h"

using namespace std;

Etiqueta etiquetas[cantidadEtiquetas];

void leerLineas(Tabcop tabcop[], string &cadenaFinal);
void leerCampos(Tabcop tabcop[], string &cadenaModificada, string &linea, string &memoria);
bool existeEtiqueta(string &subCadena, string dirMemoria);
void escribirTABSIM(string etiqueta);
int buscarInstruccion(Tabcop tabcop[], string subCadena);
void buscarDireccionamiento(Tabcop tabcop[], string &subCadena, int indice, string linea, string &cadenaModificada, string &dirMemoria, bool &espMemoria, bool etiqueta);
void verificarPalabraReservada(string subCadena, string &dirMemoria, string &cadenaModificada, string linea, int &posInicialDato, bool &espMemoria, bool etiqueta);
void validarSistemaNumeracion(string &subCadena, string &cadenaModificada, string &numCodigo);
void escribrirCodigoInstruccion(Tabcop tabcop[], int indice, string &cadenaModificada, string numCodigo);

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
    string subCadena;
    bool etiqueta, espMemoria;
    int posInicialDato = 0, posFinalDato, indice, indiceMnemonico, dirMemoria;
    posFinalDato = linea.find_first_of(delimitadorCampoP, posInicialDato);
    if (posFinalDato == -1){
        posFinalDato = linea.size();
    }
    subCadena = linea.substr(posInicialDato, posFinalDato);
    
    etiqueta = existeEtiqueta(subCadena, memoria);
    if (etiqueta){       
        espMemoria = false;
    }
    
    verificarPalabraReservada(subCadena, memoria, cadenaModificada, linea, posInicialDato, espMemoria,etiqueta);
    
    indice = buscarInstruccion(tabcop, subCadena);
    if (indice != -1){
        if (posFinalDato+1 < linea.length()){
            posInicialDato = posFinalDato+1;
            posFinalDato = linea.find_first_of(delimitadorCampoP, posInicialDato);
            subCadena = linea.substr(posInicialDato, posFinalDato);
            buscarDireccionamiento(tabcop, subCadena, indice, linea, cadenaModificada, memoria, espMemoria, etiqueta);
        }
        else{
            buscarDireccionamiento(tabcop, subCadena, indice, linea, cadenaModificada, memoria, espMemoria, etiqueta);
        }
    }
    if (etiqueta){       
        contEtiquetas++;
    }
}

bool existeEtiqueta(string &subCadena, string dirMemoria){
    int posInicial = 0, posFinal, memoria; 
    string nomEtiqueta, direccion;
    if (subCadena[posInicial] == '\t'){
        subCadena = subCadena.substr(posInicial+1, subCadena.size()-1 );
        return false;
    }
    else{
        posFinal = subCadena.find_first_of(delimitadorEtiqueta, posInicial);
        nomEtiqueta = subCadena.substr(posInicial, posFinal);
        Etiqueta etiquetaAux(nomEtiqueta);
        etiquetas[contEtiquetas] = etiquetaAux;
        escribirTABSIM(nomEtiqueta+' ');
        posInicial = posFinal+1;
        posFinal = subCadena.size();
        subCadena = subCadena.substr(posInicial, posFinal);
        return true;
    }
}

void verificarPalabraReservada(string subCadena, string &dirMemoria, string &cadenaModificada, string linea, int &posInicialDato, bool &espMemoria, bool etiqueta){
    int posIniDato = 0, posFinalDato = linea.find_first_of(simboloHexadecimal, posInicialDato),dirMemory;
    string numCodigo;
    if (subCadena == "ORG"){
        cadenaModificada += linea+delimitadorLinea;
        posIniDato = posFinalDato+1;
        posFinalDato = linea.size();
        subCadena = linea.substr(posIniDato, posFinalDato);
        dirMemory = stoi(subCadena,0,16);
        dirMemoria = to_string(dirMemory);
        posInicialDato = linea.size();
        if (!espMemoria && etiqueta){
            etiquetas[contEtiquetas].setMemoria(dirMemoria);
            dirMemoria += '\n';
            escribirTABSIM(dirMemoria);
        }
    }
    else if (subCadena == "END"){
        dirMemory = stoi(dirMemoria);
        dirMemoria = decimalAHexa(dirMemory);
        cadenaModificada += linea+'\t'+dirMemoria;
        if (!espMemoria && etiqueta){
            etiquetas[contEtiquetas].setMemoria(dirMemoria);
            dirMemoria += '\n';
            escribirTABSIM(dirMemoria);
        }
    }
    else if (subCadena == "EQU"){
        cadenaModificada += linea+delimitadorLinea;
        posIniDato = linea.size() - subCadena.size();
        posFinalDato = linea.size();
        subCadena = linea.substr(posIniDato+1, posFinalDato);
        validarSistemaNumeracion(subCadena, cadenaModificada, subCadena);
        etiquetas[contEtiquetas].setMemoria(subCadena);
        subCadena += '\n';
        escribirTABSIM(subCadena);
        espMemoria = true;
    }
}

void escribirTABSIM(string etiqueta){
    ofstream archivoTabsim;
    archivoTabsim.open(archivoTABSIM, ios::app);
    if (archivoTabsim.is_open()){
        archivoTabsim<<etiqueta;
        archivoTabsim.close();
    }
}

int buscarInstruccion(Tabcop tabcop[], string subCadena){
    for (int i = 0; i < cantidadMnemonicos; i++){
        if (subCadena == tabcop[i].getMnemonico()){
            return i;
        }
    }
    return -1;  
}

void buscarDireccionamiento(Tabcop tabcop[], string &subCadena, int indice, string linea, string &cadenaModificada, string &dirMemoria, bool &espMemoria, bool etiqueta){
    int limite = indice + tabcop[indice].getCantidadInstrucciones(), codigo, memoria, posInicial = 0, posFinal;
    string numCodigo;
    if ("INH" == tabcop[indice].getDireccionamiento()){
        memoria = stoi(dirMemoria);
        dirMemoria = decimalAHexa(memoria);
        cadenaModificada += linea +'\t'+ dirMemoria +" "+ tabcop[indice].getCodigoInstruccion()+'\n';
        if (!espMemoria && etiqueta){
            dirMemoria += '\n';
            escribirTABSIM(dirMemoria);
        }
        memoria = memoria + tabcop[indice].getLongitudInstruccion();
        dirMemoria = to_string(memoria);   
    }
    else if (subCadena[0] == '#'){
        for (int i = indice; i < limite; i++){
            if ("IMM" == tabcop[i].getDireccionamiento()){
                memoria = stoi(dirMemoria);
                dirMemoria = decimalAHexa(memoria);
                cadenaModificada += linea+'\t'+dirMemoria+" "+tabcop[i].getCodigoInstruccion()+' ';
                if (!espMemoria && etiqueta){
                    dirMemoria += '\n';
                    escribirTABSIM(dirMemoria);
                }
                memoria = stoi(dirMemoria,0,16);
                memoria = memoria + tabcop[i].getLongitudInstruccion();
                dirMemoria = to_string(memoria);
                posFinal = subCadena.size();
                subCadena = subCadena.substr(posInicial+1, posFinal);
                validarSistemaNumeracion(subCadena, cadenaModificada, numCodigo);
                escribrirCodigoInstruccion(tabcop, i, cadenaModificada, numCodigo);
            }
        }
    }
    else{
        validarSistemaNumeracion(subCadena, cadenaModificada, numCodigo);
        codigo = stoi(subCadena);
        if (codigo < limiteDecimalOrp8){
            for (int i = indice; i < limite; i++){
                if ("DIR" == tabcop[i].getDireccionamiento()){
                    memoria = stoi(dirMemoria);
                    dirMemoria = decimalAHexa(memoria);
                    cadenaModificada += linea+'\t'+dirMemoria+" "+tabcop[i].getCodigoInstruccion()+' ';
                    if (!espMemoria && etiqueta){
                        dirMemoria += '\n';
                        escribirTABSIM(dirMemoria);
                    }
                    memoria = stoi(dirMemoria,0,16);
                    memoria = memoria + tabcop[i].getLongitudInstruccion();
                    dirMemoria = to_string(memoria);
                    posFinal = subCadena.size();
                    subCadena = subCadena.substr(posInicial+1, posFinal);
                    escribrirCodigoInstruccion(tabcop, i, cadenaModificada, numCodigo);
                }
            }
        }
        else if (codigo < limiteDecimalOrp16){
            for (int i = indice; i < limite; i++){
                if ("EXT" == tabcop[i].getDireccionamiento()){
                    memoria = stoi(dirMemoria);
                    dirMemoria = decimalAHexa(memoria);
                    cadenaModificada += linea+'\t'+dirMemoria+" "+tabcop[i].getCodigoInstruccion()+' ';
                    if (!espMemoria && etiqueta){
                        dirMemoria += '\n';
                        escribirTABSIM(dirMemoria);
                    }
                    memoria = stoi(dirMemoria,0,16);
                    memoria = memoria + tabcop[i].getLongitudInstruccion();
                    dirMemoria = to_string(memoria);
                    posFinal = subCadena.size();
                    subCadena = subCadena.substr(posInicial+1, posFinal);
                    escribrirCodigoInstruccion(tabcop, i, cadenaModificada, numCodigo);
                }
            }
        }
        else{
            cadenaModificada += linea+'\t'+"FDR"+'\n';
        }
    }
}

void validarSistemaNumeracion(string &subCadena, string &cadenaModificada, string &numCodigo){
    int posInicial = 0, posFinal, numero, decimal;
    if (subCadena[0] == simboloHexadecimal){
        posFinal = subCadena.size();
        subCadena = subCadena.substr(posInicial+1, posFinal);

    }
    else if (subCadena[0] == simboloOctal){
        posFinal = subCadena.size();
        subCadena = subCadena.substr(posInicial+1, posFinal);
        decimal = stoi(subCadena,0,8);
        numCodigo = decimalAHexa(decimal);
    }
    else if (subCadena[0] == simboloBinario){
        posFinal = subCadena.size();
        subCadena = subCadena.substr(posInicial+1, posFinal);
        decimal = stoi(subCadena,0,2);  //Traduce de binario a decimal
        numCodigo = decimalAHexa(decimal);
    }
    else{
        numero = stoi(subCadena);
        numCodigo = decimalAHexa(numero);
    }
}

void escribrirCodigoInstruccion(Tabcop tabcop[], int indice, string &cadenaModificada, string numCodigo){
    int espacios = tabcop[indice].getLongitudInstruccion() * 2;
    int longCodigo = tabcop[indice].getCodigoInstruccion().length();
    int longNumCod = numCodigo.length();
    espacios =  espacios - longCodigo - longNumCod;
    if (espacios < 0){
        cadenaModificada += '\t'+"FDR"+'\n';
    }
    else{
        while (espacios > 0){
            cadenaModificada += '0';
            espacios--;
        }
        cadenaModificada += numCodigo + '\n';
    }
}

#endif