#ifndef TRADUCTOR_H_INCLUDED
#define TRADUCTOR_H_INCLUDED
#include <iostream>
#include <string>
#include <bitset>
#include "definiciones.h"
#include "tabcop.h"
#include "etiqueta.h"
#include "conversor.h"

using namespace std;

Etiqueta etiquetas[cantidadEtiquetas];
int contLinea = 0, idLineaEtiqueta[cantidadEtiquetas], contLineasEtiquetas = 0;

void leerLineas(Tabcop tabcop[], string &cadenaFinal);
void leerCampos(Tabcop tabcop[], string &cadenaModificada, string &linea, string &memoria);
bool existeEtiqueta(string &subCadena, string linea);
void escribirTABSIM(string etiqueta);
int buscarInstruccion(Tabcop tabcop[], string subCadena);
void buscarDireccionamiento(Tabcop tabcop[], string &subCadena, int indice, string linea, string &cadenaModificada, string &dirMemoria, bool &espMemoria, bool etiqueta);
void verficarDirectivas(string subCadena, string &dirMemoria, string &cadenaModificada, string linea, int &posInicialDato, bool &espMemoria, bool etiqueta);
bool validarSistemaNumeracionOEtiqueta(string &subCadena, string &cadenaModificada, string &numCodigo);
bool validarIndexado(string &subCadena, string &cadenaModificada, string &numCodigo);
void encontrarFormulaIndexado(Tabcop tabcop[], string &subCadena, int &indice,int limite, string linea, string &cadenaModificada, string &dirMemoria);
void formula1Indexado(string &subCadena,int valorIdx, string &cadenaModificada);
void formula2Indexado(string &subCadena,int valorIdx, string &cadenaModificada);
void formula3Indexado(string &subCadena,int valorIdx, string &cadenaModificada);
void escribrirCodigoInstruccion(Tabcop tabcop[], int indice, string &cadenaModificada, string numCodigo);
void rellenarCeros(string &dirMemoria, int espacios);

void leerLineas(Tabcop tabcop[], string &cadenaFinal){
    string linea, cadenaModificada, memoria = "0000";
    int posInicialLinea = 0, posInicialLineaAux, posFinalLinea, posFinalCodigo;
    while (posInicialLinea != cadenaFinal.size()){                  //Primera lectura, reserva de memoria
        posFinalLinea = cadenaFinal.find_first_of(delimitadorLinea, posInicialLinea);
        linea = cadenaFinal.substr(posInicialLinea, posFinalLinea-posInicialLinea);
        leerCampos(tabcop, cadenaModificada, linea, memoria);
        posInicialLinea = posFinalLinea+1;
    }
    cadenaFinal = cadenaModificada;
    posInicialLinea = 0;
    int lineaActual = 0, indice;
    string cadenaMitadInicio, cadenaMitadFinal, cadenaAux;
    while (posInicialLinea != cadenaModificada.size()){   //Segunda lectura, valor de codigo   
        posFinalLinea = cadenaModificada.find_first_of(delimitadorLinea, posInicialLinea);
        linea = cadenaModificada.substr(posInicialLinea, posFinalLinea-posInicialLinea);
        lineaActual++;
        for (int i = 0; i < contEtiquetas; i++){
            if (lineaActual == idLineaEtiqueta[i]){
                posFinalCodigo = cadenaModificada.find_first_of(delimitadorCampoP, posInicialLinea);
                posInicialLinea++;
                cadenaAux = cadenaModificada.substr(posInicialLinea, posFinalCodigo-posInicialLinea);
                indice = buscarInstruccion(tabcop, cadenaAux);
                if (tabcop[indice].getDireccionamiento() != "REL"){
                    posFinalCodigo = cadenaModificada.find_first_of(delimitadorLinea, posInicialLinea);
                    cadenaMitadInicio = cadenaModificada.substr(0,posFinalCodigo);                          //Partir cadena a la mitad para agregar codigo de etiqueta
                    cadenaMitadFinal = cadenaModificada.substr(posFinalCodigo, cadenaModificada.size());    //
                    posInicialLinea = cadenaModificada.find_first_of(delimitadorCampoP, posInicialLinea);
                    posFinalCodigo = cadenaModificada.find_first_of(delimitadorEtiqueta,posInicialLinea);
                    posInicialLinea++;
                    cadenaAux = cadenaModificada.substr(posInicialLinea, posFinalCodigo - posInicialLinea);
                    for (int j = 0; j < contEtiquetas; j++){
                        if (cadenaAux == etiquetas[j].getNombre()){
                            cadenaModificada = cadenaMitadInicio + ' ' +etiquetas[j].getMemoria() + cadenaMitadFinal;
                            posFinalLinea = cadenaMitadInicio.size() + etiquetas[j].getMemoria().size() +1;
                            break;
                        }
                    }
                }
                else{ //ERROR AL CORTAR LA CADENA FINAL
                    string direccion, numBinario, codRelativo;
                    int dirEtiqueta, dirSiguiente, resultadoRelativo;
                    posFinalCodigo = cadenaModificada.find_first_of(delimitadorLinea, posInicialLinea);
                    cadenaMitadInicio = cadenaModificada.substr(0,posFinalCodigo);                          //Partir cadena a la mitad para agregar codigo de etiqueta
                    cadenaMitadFinal = cadenaModificada.substr(posFinalCodigo, cadenaModificada.size());    //
                    posInicialLinea = cadenaModificada.find_first_of(delimitadorCampoP, posInicialLinea);
                    posFinalCodigo = cadenaModificada.find_first_of(delimitadorEtiqueta,posInicialLinea);
                    posInicialLinea++;
                    cadenaAux = cadenaModificada.substr(posInicialLinea, posFinalCodigo - posInicialLinea); //Encuentro etiqueta
                    for (int j = 0; j < contEtiquetas; j++){                                                //Busco direccion de memoria de la etiqueta
                        if (cadenaAux == etiquetas[j].getNombre()){
                            cadenaAux = etiquetas[j].getMemoria();
                            break;
                        }
                    }
                    posInicialLinea = posFinalCodigo+1;
                    posFinalCodigo = cadenaModificada.find_first_of(delimitadorCampoP, posInicialLinea);
                    direccion = cadenaModificada.substr(posInicialLinea, posFinalCodigo - posInicialLinea);
                    dirEtiqueta = stoi(cadenaAux,0,16);
                    dirSiguiente = stoi(direccion,0,16);
                    dirSiguiente = dirSiguiente + tabcop[indice].getLongitudInstruccion();
                    resultadoRelativo = dirEtiqueta -  dirSiguiente;
                    bitset<16> bs1(resultadoRelativo);                                                 //Tranformo resultado a bin Complemento 2
                    numBinario = bs1.to_string();
                    resultadoRelativo = stoi(numBinario,0, 2);
                    codRelativo = decimalAHexa(resultadoRelativo);
                    cadenaModificada = cadenaMitadInicio + ' ' +codRelativo + cadenaMitadFinal;
                    posFinalLinea = cadenaMitadInicio.size() + codRelativo.size() +1;
                }
                break;
            }
        }
        posInicialLinea = posFinalLinea+1;
    }
    cadenaFinal = cadenaModificada;
}

void leerCampos(Tabcop tabcop[], string &cadenaModificada, string &linea, string &memoria){
    string subCadena;
    bool etiqueta, espMemoria;
    int posInicialDato = 0, posFinalDato, indice, indiceMnemonico, dirMemoria;
    contLinea++;
    posFinalDato = linea.find_first_of(delimitadorCampoP, posInicialDato);
    if (posFinalDato == -1){
        posFinalDato = linea.size();
    }
    subCadena = linea.substr(posInicialDato, posFinalDato);
    
    etiqueta = existeEtiqueta(subCadena, linea);
    if (etiqueta){       
        espMemoria = false;
    }
    
    verficarDirectivas(subCadena, memoria, cadenaModificada, linea, posInicialDato, espMemoria,etiqueta);
    
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

bool existeEtiqueta(string &subCadena, string linea){
    int posInicial = 0, posFinal; 
    string nomEtiqueta;
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
    if (tabcop[indice].getCantidadInstrucciones() == 1){
        memoria = stoi(dirMemoria);
        dirMemoria = decimalAHexa(memoria);
        if (dirMemoria.size() < 4){
            rellenarCeros(dirMemoria,4);  
        }
        if ("INH" == tabcop[indice].getDireccionamiento()){
            cadenaModificada += linea +'\t'+ dirMemoria +" "+ tabcop[indice].getCodigoInstruccion()+'\n';
        }
        else{
            if(!validarSistemaNumeracionOEtiqueta(subCadena, cadenaModificada, numCodigo)){
                cadenaModificada += linea +'\t'+ dirMemoria +" "+ tabcop[indice].getCodigoInstruccion()+' ';
                escribrirCodigoInstruccion(tabcop, indice, cadenaModificada, numCodigo);
            }
            else{
                cadenaModificada += linea +'\t'+ dirMemoria +" "+ tabcop[indice].getCodigoInstruccion()+'\n';
            }
        }
        if (!espMemoria && etiqueta){
            dirMemoria += '\n';
            escribirTABSIM(dirMemoria);
        }
        memoria = memoria + tabcop[indice].getLongitudInstruccion();
        dirMemoria = to_string(memoria);
    }
    else{
        if ("INH" == tabcop[indice].getDireccionamiento()){
            memoria = stoi(dirMemoria);
            dirMemoria = decimalAHexa(memoria);
            if (dirMemoria.size() < 4){
                rellenarCeros(dirMemoria,4);  
            }
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
                    if (dirMemoria.size() < 4){
                        rellenarCeros(dirMemoria,4);  
                    }
                    cadenaModificada += linea+'\t'+dirMemoria+" "+tabcop[i].getCodigoInstruccion()+' ';
                    if (dirMemoria.size() < 4){
                        rellenarCeros(dirMemoria,4);  
                    }
                    if (!espMemoria && etiqueta){
                        dirMemoria += '\n';
                        escribirTABSIM(dirMemoria);
                    }
                    memoria = stoi(dirMemoria,0,16);
                    memoria = memoria + tabcop[i].getLongitudInstruccion();
                    dirMemoria = to_string(memoria);
                    posFinal = subCadena.size();
                    subCadena = subCadena.substr(posInicial+1, posFinal);
                    validarSistemaNumeracionOEtiqueta(subCadena, cadenaModificada, numCodigo);
                    escribrirCodigoInstruccion(tabcop, i, cadenaModificada, numCodigo);
                }
            }
        }
        else{
            validarSistemaNumeracionOEtiqueta(subCadena, cadenaModificada, numCodigo);
            if (numCodigo == "IDX"){
                if (tabcop[indice].getDireccionamiento() == "IDX"){
                    memoria = stoi(dirMemoria);
                    dirMemoria = decimalAHexa(memoria);
                    if (dirMemoria.size() < 4){
                        rellenarCeros(dirMemoria,4);  
                    }
                    encontrarFormulaIndexado(tabcop,subCadena,indice,limite,linea,cadenaModificada,dirMemoria);
                    memoria = memoria + tabcop[indice].getLongitudInstruccion();
                    dirMemoria = to_string(memoria);
                    if (!espMemoria && etiqueta){
                        dirMemoria += '\n';
                        escribirTABSIM(dirMemoria);
                    }
                    memoria = stoi(dirMemoria,0,16);
                }
                else{
                    cadenaModificada += linea+'\t'+dirMemoria+" "+"FDR\n";
                }
            }
            else{
                codigo = stoi(subCadena);
                if (codigo < limiteDecimalOrp8){
                    for (int i = indice; i < limite; i++){
                        if ("DIR" == tabcop[i].getDireccionamiento()){
                            memoria = stoi(dirMemoria);
                            dirMemoria = decimalAHexa(memoria);
                            if (dirMemoria.size() < 4){
                                rellenarCeros(dirMemoria,4);  
                            }
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
                            if (dirMemoria.size() < 4){
                                rellenarCeros(dirMemoria,4);  
                            }
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
    }
}

bool isNumber(const string subCadena)
{
    for (char const &c : subCadena) {
        if (isdigit(c) == 0) return false;
    }
    return true;
}

void encontrarFormulaIndexado(Tabcop tabcop[], string &subCadena, int &indice, int limite, string linea, string &cadenaModificada, string &dirMemoria){
    int posFinalDato, posIniDato = 0, numIdx;
    string valorIdx, valorFormIdx;
    if (subCadena[0] == '['){
        for (int i = indice; i < limite; i++){
            if (tabcop[i].getDireccionamiento() == "IDX3"){
                cadenaModificada += linea+'\t'+dirMemoria+" "+tabcop[i].getCodigoInstruccion()+' ';
                posFinalDato = subCadena.find_first_of(delimitadorComa,0);
                valorIdx = subCadena.substr(1,posFinalDato-1);
                posIniDato = posFinalDato+1;
                posFinalDato = subCadena.size();
                valorFormIdx = subCadena.substr(posIniDato, posFinalDato -  posIniDato-1);
                numIdx = stoi(valorIdx);
                if (numIdx < 0){
                    cadenaModificada += " FDR\n";
                }
                else{
                    formula3Indexado(valorFormIdx,numIdx,cadenaModificada);
                }
                indice = i;
                break;
            }
        }
    }
    else if (subCadena[0] == delimitadorComa){
        for (int i = indice; i < limite; i++){
            if (tabcop[i].getDireccionamiento() == "IDX"){
                cadenaModificada += linea+'\t'+dirMemoria+" "+tabcop[i].getCodigoInstruccion()+' ';
                numIdx = 0;
                valorFormIdx = subCadena.substr(1,subCadena.size()-1);
                formula1Indexado(valorFormIdx,numIdx,cadenaModificada);
                indice = i;
                break;
            }
        }
    }
    else{
        posFinalDato = subCadena.find_first_of(delimitadorComa, 0);
        valorIdx = subCadena.substr(0,posFinalDato);
        numIdx = stoi(valorIdx);
        if (numIdx <= 15 && numIdx >= -16){
            for (int i = indice; i < limite; i++){
                if (tabcop[i].getDireccionamiento() == "IDX"){
                    cadenaModificada += linea+'\t'+dirMemoria+" "+tabcop[i].getCodigoInstruccion()+' ';
                    posIniDato = posFinalDato+1;
                    valorFormIdx = subCadena.substr(posIniDato, subCadena.size()-posIniDato);
                    formula1Indexado(valorFormIdx,numIdx,cadenaModificada);
                    indice = i;
                    break;
                }
            }   
        }
        else if (numIdx <= 255 && numIdx >= -256){
            for (int i = indice; i < limite; i++){
                if (tabcop[i].getDireccionamiento() == "IDX1"){
                    cadenaModificada += linea+'\t'+dirMemoria+" "+tabcop[i].getCodigoInstruccion()+' ';
                    posIniDato = posFinalDato+1;
                    valorFormIdx = subCadena.substr(posIniDato, subCadena.size()-posIniDato);
                    formula2Indexado(valorFormIdx,numIdx,cadenaModificada);
                    indice = i;
                    break;
                }
            }   
        }
        else{
            for (int i = indice; i < limite; i++){
                if (tabcop[i].getDireccionamiento() == "IDX2"){
                    cadenaModificada += linea+'\t'+dirMemoria+" "+tabcop[i].getCodigoInstruccion()+' ';
                    posIniDato = posFinalDato+1;
                    valorFormIdx = subCadena.substr(posIniDato, subCadena.size()-posIniDato);
                    formula2Indexado(valorFormIdx,numIdx,cadenaModificada);
                    indice = i;
                    break;
                }
            }
        }
    }
}

void formula1Indexado(string &subCadena,int valorIdx, string &cadenaModificada){
    string resultIndexado, numBinario;
    int resultDecimal;
    if (subCadena == "X"){
        resultIndexado = "000";
        bitset<5> bs1(valorIdx);                                                 
        numBinario = bs1.to_string();
        resultIndexado += numBinario;
        resultDecimal = stoi(resultIndexado,0,2);
        resultIndexado = decimalAHexa(resultDecimal);
        cadenaModificada += resultIndexado + "\tFORMULA 1\n";
    }
    else if (subCadena == "Y"){
        resultIndexado = "010";
        bitset<5> bs1(valorIdx);                                                 
        numBinario = bs1.to_string();
        resultIndexado += numBinario;
        resultDecimal = stoi(resultIndexado,0,2);
        resultIndexado = decimalAHexa(resultDecimal);
        cadenaModificada += resultIndexado + "\tFORMULA 1\n";
    }
    else if (subCadena == "SP"){
        resultIndexado = "100";
        bitset<5> bs1(valorIdx);                                                 
        numBinario = bs1.to_string();
        resultIndexado += numBinario;
        resultDecimal = stoi(resultIndexado,0,2);
        resultIndexado = decimalAHexa(resultDecimal);
        cadenaModificada += resultIndexado + "\tFORMULA 1\n";   
    }
    else if (subCadena == "PC"){
        resultIndexado = "110";
        bitset<5> bs1(valorIdx);                                                 
        numBinario = bs1.to_string();
        resultIndexado += numBinario;
        resultDecimal = stoi(resultIndexado,0,2);
        resultIndexado = decimalAHexa(resultDecimal);
        cadenaModificada += resultIndexado + "\tFORMULA 1\n";  
    }
}
void formula2Indexado(string &subCadena,int valorIdx, string &cadenaModificada){
    string resultIndexado, numeroIdx, binCom2;
    int resultDecimal;
    if (subCadena == "X"){
        resultIndexado = "111000";
        if(valorIdx <= 255 && valorIdx >= -256){
            resultIndexado += '0';
            numeroIdx = decimalAHexa(valorIdx);
        }
        else{
            resultIndexado += '1';
            numeroIdx = decimalAHexa(valorIdx);
            rellenarCeros(numeroIdx, 4);
        }
        if (valorIdx < 0){
            resultIndexado += '1';
            bitset<8> bs1(valorIdx);
            binCom2 = bs1.to_string();
            resultDecimal = stoi(binCom2,0,2);
            resultIndexado = decimalAHexa(resultDecimal);
        }
        else{
            resultIndexado += '0';
        }
        resultDecimal = stoi(resultIndexado,0,2);
        resultIndexado = decimalAHexa(resultDecimal);
        cadenaModificada += resultIndexado +  + "\tFORMULA 2\n";
    }
    else if (subCadena == "Y"){
        resultIndexado = "111010";
        if(valorIdx <= 255 && valorIdx >= -256){
            resultIndexado += '0';
            numeroIdx = decimalAHexa(valorIdx);
        }
        else{
            resultIndexado += '1';
            numeroIdx = decimalAHexa(valorIdx);
            rellenarCeros(numeroIdx, 4);
        }
        if (valorIdx < 0){
            resultIndexado += '1';
            bitset<8> bs1(valorIdx);
            binCom2 = bs1.to_string();
            resultDecimal = stoi(binCom2,0,2);
            resultIndexado = decimalAHexa(resultDecimal);
        }
        else{
            resultIndexado += '0';
        }
        resultDecimal = stoi(resultIndexado,0,2);
        resultIndexado = decimalAHexa(resultDecimal);
        cadenaModificada += resultIndexado +  + "\tFORMULA 2\n";
    }
    else if (subCadena == "SP"){
        resultIndexado = "111100";
        if(valorIdx <= 255 && valorIdx >= -256){
            resultIndexado += '0';
            numeroIdx = decimalAHexa(valorIdx);
        }
        else{
            resultIndexado += '1';
            numeroIdx = decimalAHexa(valorIdx);
            rellenarCeros(numeroIdx, 4);
        }
        if (valorIdx < 0){
            resultIndexado += '1';
            bitset<8> bs1(valorIdx);
            binCom2 = bs1.to_string();
            resultDecimal = stoi(binCom2,0,2);
            resultIndexado = decimalAHexa(resultDecimal);
        }
        else{
            resultIndexado += '0';
        }
        resultDecimal = stoi(resultIndexado,0,2);
        resultIndexado = decimalAHexa(resultDecimal);
        cadenaModificada += resultIndexado +  + "\tFORMULA 2\n";  
    }
    else if (subCadena == "PC"){
        resultIndexado = "111110";
        if(valorIdx <= 255 && valorIdx >= -256){
            resultIndexado += '0';
            numeroIdx = decimalAHexa(valorIdx);
        }
        else{
            resultIndexado += '1';
            numeroIdx = decimalAHexa(valorIdx);
            rellenarCeros(numeroIdx, 4);
        }
        if (valorIdx < 0){
            resultIndexado += '1';
            bitset<8> bs1(valorIdx);
            binCom2 = bs1.to_string();
            resultDecimal = stoi(binCom2,0,2);
            numeroIdx = decimalAHexa(resultDecimal);
        }
        else{
            resultIndexado += '0';
        }
        resultDecimal = stoi(resultIndexado,0,2);
        resultIndexado = decimalAHexa(resultDecimal);
        cadenaModificada += resultIndexado + ' ' + numeroIdx + "\tFORMULA 2\n"; 
    }
}

void formula3Indexado(string &subCadena,int valorIdx, string &cadenaModificada){
    string resultIndexado, numeroIdx;
    int resultDecimal;
    if (subCadena == "X"){
        resultIndexado = "11100011";
        resultDecimal = stoi(resultIndexado,0,2);
        resultIndexado = decimalAHexa(resultDecimal);
        numeroIdx = decimalAHexa(valorIdx);
        rellenarCeros(numeroIdx,4);
        cadenaModificada += resultIndexado + ' ' + numeroIdx + "\tFORMULA 3\n"; 
    }
    else if (subCadena == "Y"){
        resultIndexado = "11101011";
        resultDecimal = stoi(resultIndexado,0,2);
        resultIndexado = decimalAHexa(resultDecimal);
        numeroIdx = decimalAHexa(valorIdx);
        rellenarCeros(numeroIdx,4);
        cadenaModificada += resultIndexado + ' ' + numeroIdx + "\tFORMULA 3\n"; 
    }
    else if (subCadena == "SP"){
        resultIndexado = "11110011";
        resultDecimal = stoi(resultIndexado,0,2);
        resultIndexado = decimalAHexa(resultDecimal);
        numeroIdx = decimalAHexa(valorIdx);
        rellenarCeros(numeroIdx,4);
        cadenaModificada += resultIndexado + ' ' + numeroIdx + "\tFORMULA 3\n";  
    }
    else if (subCadena == "PC"){
        resultIndexado = "11111011";
        resultDecimal = stoi(resultIndexado,0,2);
        resultIndexado = decimalAHexa(resultDecimal);
        numeroIdx = decimalAHexa(valorIdx);
        rellenarCeros(numeroIdx,4);
        cadenaModificada += resultIndexado + ' ' + numeroIdx + "\tFORMULA 3\n"; 
    }
}

bool validarSistemaNumeracionOEtiqueta(string &subCadena, string &cadenaModificada, string &numCodigo){
    int posInicial = 0, posFinal, numero, decimal, numIdx;
    numIdx = subCadena.find_first_of(',',0);
    if (numIdx != -1){
        numCodigo = "IDX";
        return false;
    }
    else{
        if (subCadena[0] == simboloHexadecimal){
            posFinal = subCadena.size();
            subCadena = subCadena.substr(posInicial+1, posFinal);
            numCodigo = subCadena;
            return false;
        }
        else if (subCadena[0] == simboloOctal){
            posFinal = subCadena.size();
            subCadena = subCadena.substr(posInicial+1, posFinal);
            decimal = stoi(subCadena,0,8);
            numCodigo = decimalAHexa(decimal);
            return false;
        }
        else if (subCadena[0] == simboloBinario){
            posFinal = subCadena.size();
            subCadena = subCadena.substr(posInicial+1, posFinal);
            decimal = stoi(subCadena,0,2);  //Traduce de binario a decimal
            numCodigo = decimalAHexa(decimal);
            return false;
        }
        else{
            if (isNumber(subCadena)){
                numero = stoi(subCadena);
                numCodigo = decimalAHexa(numero);
                return false;
            }
            else{
                idLineaEtiqueta[contLineasEtiquetas] = contLinea;
                contLineasEtiquetas++;
                return true;
            }
        }
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

void verficarDirectivas(string subCadena, string &dirMemoria, string &cadenaModificada, string linea, int &posInicialLinea, bool &espMemoria, bool etiqueta){
    int posIniDato = 0, posFinalDato = linea.find_first_of(simboloHexadecimal, posInicialLinea),memoria, numeroAux;
    string numCodigo;
    if (subCadena == "ORG"){
        cadenaModificada += linea+delimitadorLinea;
        posIniDato = posFinalDato+1;
        posFinalDato = linea.size();
        subCadena = linea.substr(posIniDato, posFinalDato);
        memoria = stoi(subCadena,0,16);
        dirMemoria = to_string(memoria);
        if (dirMemoria.size() < 4){
            rellenarCeros(dirMemoria,4);  
        }
        posInicialLinea = linea.size();
        if (!espMemoria && etiqueta){
            etiquetas[contEtiquetas].setMemoria(dirMemoria);
            dirMemoria += '\n';
            escribirTABSIM(dirMemoria);
        }
    }
    else if (subCadena == "END"){
        memoria = stoi(dirMemoria);
        dirMemoria = decimalAHexa(memoria);
        if (dirMemoria.size() < 4){
            rellenarCeros(dirMemoria,4);  
        }
        cadenaModificada += linea+" \t"+dirMemoria+'\n';
        if (!espMemoria && etiqueta){
            etiquetas[contEtiquetas].setMemoria(dirMemoria);
            dirMemoria += '\n';
            escribirTABSIM(dirMemoria);
        }
    }
    else if (subCadena == "START"){
        dirMemoria = "0000";
        cadenaModificada += linea+ '\n';
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
        validarSistemaNumeracionOEtiqueta(subCadena, cadenaModificada, numCodigo);
        etiquetas[contEtiquetas].setMemoria(numCodigo);
        numCodigo += '\n';
        escribirTABSIM(numCodigo);
        espMemoria = true;
    }
    else if (subCadena == "DC.B"){
        memoria = stoi(dirMemoria);
        dirMemoria = decimalAHexa(memoria);
        if (dirMemoria.size() < 4){
            rellenarCeros(dirMemoria,4);  
        }
        cadenaModificada += linea +'\t'+ dirMemoria + ' ';
        posIniDato = linea.find_first_of(delimitadorCampoP, posInicialLinea);
        posFinalDato = linea.size();
        if (posIniDato == -1){
            cadenaModificada += " 00\n";
            memoria ++;
            dirMemoria = to_string(memoria);
        }
        else{
            subCadena = linea.substr(posIniDato+1, posFinalDato);
            posFinalDato = subCadena.find_first_of(delimitadorComa, posInicialLinea);
            if (posFinalDato != -1){
                posIniDato = 0;
                while (posIniDato < subCadena.size()){
                    numCodigo = subCadena.substr(posIniDato, posFinalDato - posIniDato);
                    validarSistemaNumeracionOEtiqueta(numCodigo, cadenaModificada, numCodigo);
                    cadenaModificada += numCodigo + ' ';
                    memoria ++;
                    dirMemoria = to_string(memoria);
                    posIniDato = posFinalDato+1;
                    posFinalDato = subCadena.find_first_of(delimitadorComa, posIniDato);
                    if (posFinalDato == -1){
                        posFinalDato = subCadena.size();
                    }
                }
                cadenaModificada += '\n';
            }
            else{
                validarSistemaNumeracionOEtiqueta(subCadena, cadenaModificada, numCodigo);
                cadenaModificada += numCodigo + '\n';
                memoria ++;
                dirMemoria = to_string(memoria);
            }
        }
    }
    else if (subCadena == "DC.W"){
        memoria = stoi(dirMemoria);
        dirMemoria = decimalAHexa(memoria);
        if (dirMemoria.size() < 4){
            rellenarCeros(dirMemoria,4);  
        }
        cadenaModificada += linea +'\t'+ dirMemoria + ' ';
        posIniDato = linea.find_first_of(delimitadorCampoP, posInicialLinea);
        posFinalDato = linea.size();
        if (posIniDato == -1){
            cadenaModificada += " 0000\n";
            memoria += 2;
            dirMemoria = to_string(memoria);
        }
        else{
            subCadena = linea.substr(posIniDato+1, posFinalDato);
            posFinalDato = subCadena.find_first_of(delimitadorComa, posInicialLinea);
            if (posFinalDato != -1){
                posIniDato = 0;
                while (posIniDato < subCadena.size()){
                    numCodigo = subCadena.substr(posIniDato, posFinalDato - posIniDato);
                    validarSistemaNumeracionOEtiqueta(numCodigo, cadenaModificada, numCodigo);
                    rellenarCeros(numCodigo,4);
                    cadenaModificada += numCodigo + ' ';
                    memoria += 2;
                    dirMemoria = to_string(memoria);
                    posIniDato = posFinalDato+1;
                    posFinalDato = subCadena.find_first_of(delimitadorComa, posIniDato);
                    if (posFinalDato == -1){
                        posFinalDato = subCadena.size();
                    }
                }
                cadenaModificada += '\n';
            }
            else{
                validarSistemaNumeracionOEtiqueta(subCadena, cadenaModificada, numCodigo);
                rellenarCeros(numCodigo,4);
                cadenaModificada += numCodigo + '\n';
                memoria += 2;
                dirMemoria = to_string(memoria);
            }
        }
    }
    else if (subCadena == "BSZ" || subCadena == "ZMB"){
        memoria = stoi(dirMemoria);
        dirMemoria = decimalAHexa(memoria);
        if (dirMemoria.size() < 4){
            rellenarCeros(dirMemoria,4);  
        }
        cadenaModificada += linea +'\t'+ dirMemoria + ' ';
        posIniDato = linea.find_first_of(' ',0);
        posFinalDato = linea.size();
        subCadena = linea.substr(posIniDato+1, posFinalDato);
        validarSistemaNumeracionOEtiqueta(subCadena, cadenaModificada, numCodigo);
        numeroAux = stoi(numCodigo,0,16);
        for (int i = 0; i < numeroAux; i++){
            cadenaModificada += "00 ";
            memoria++;
        }
        dirMemoria = to_string(memoria);
        cadenaModificada += '\n';
    }
    else if (subCadena == "FCB"){
        memoria = stoi(dirMemoria);
        dirMemoria = decimalAHexa(memoria);
        if (dirMemoria.size() < 4){
            rellenarCeros(dirMemoria,4);  
        }
        cadenaModificada += linea +'\t'+ dirMemoria + ' ';
        posIniDato = linea.find_first_of(' ',0);
        posFinalDato = linea.size();
        subCadena = linea.substr(posIniDato+1, posFinalDato);
        validarSistemaNumeracionOEtiqueta(subCadena, cadenaModificada, numCodigo);
        if (numCodigo.size() > 2){
            cadenaModificada += "Fuera De Rango\n";
            dirMemoria = to_string(memoria);
        }
        else{
            rellenarCeros(numCodigo,2);
            cadenaModificada += numCodigo + '\n';
            memoria++;
            dirMemoria = to_string(memoria);
        }
    }
    else if (subCadena == "FCC"){
        char charActual;
        int asciiCod, i = 0;
        memoria = stoi(dirMemoria);
        dirMemoria = decimalAHexa(memoria);
        if (dirMemoria.size() < 4){
            rellenarCeros(dirMemoria,4);  
        }
        cadenaModificada += linea +'\t'+ dirMemoria + ' ';
        posIniDato = linea.find_first_of('/',0);
        posFinalDato = linea.size();
        subCadena = linea.substr(posIniDato+1, posFinalDato);
        charActual = subCadena[i];
        while (subCadena[i] != '/'){
            charActual = subCadena[i];
            asciiCod = charActual;
            numCodigo = decimalAHexa(asciiCod);
            cadenaModificada += numCodigo + ' ';
            memoria++;
            i++;
        }
        cadenaModificada+= '\n';
        dirMemoria = to_string(memoria);
    }
    else if (subCadena == "FILL"){
        memoria = stoi(dirMemoria);
        dirMemoria = decimalAHexa(memoria);
        if (dirMemoria.size() < 4){
            rellenarCeros(dirMemoria,4);  
        }
        cadenaModificada += linea +'\t'+ dirMemoria + ' ';
        posIniDato = linea.size() - subCadena.size();
        posFinalDato = linea.size();
        subCadena = linea.substr(posIniDato+1, posFinalDato);
        posFinalDato = subCadena.find_first_of(delimitadorComa, 0);
        numCodigo = linea.substr(posIniDato+1, posFinalDato);
        validarSistemaNumeracionOEtiqueta(numCodigo, cadenaModificada, numCodigo);
        posIniDato = posFinalDato+1;
        subCadena = subCadena.substr(posIniDato,subCadena.size());
        numeroAux = stoi(subCadena);
        for (int i = 0; i < numeroAux; i++){
            cadenaModificada += numCodigo + ' ';
            memoria++;
        }
        dirMemoria = to_string(memoria);
        cadenaModificada += '\n';
    }
}

void rellenarCeros(string &dirMemoria, int espacios){
    int longCodigo = dirMemoria.size();
    espacios =  espacios - longCodigo;
    string ceros = "";
    while (espacios > 0){
        ceros += '0';
        espacios--;
    }
    dirMemoria = ceros + dirMemoria;
}

#endif