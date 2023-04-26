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
bool existeEtiqueta(string &subCadena, string linea);
void escribirTABSIM(string etiqueta);
int buscarInstruccion(Tabcop tabcop[], string subCadena);
void buscarDireccionamiento(Tabcop tabcop[], string &subCadena, int indice, string linea, string &cadenaModificada, string &dirMemoria, bool &espMemoria, bool etiqueta);
void verficarDirectivas(string subCadena, string &dirMemoria, string &cadenaModificada, string linea, int &posInicialDato, bool &espMemoria, bool etiqueta);
void validarSistemaNumeracionOEtiqueta(string &subCadena, string &cadenaModificada, string &numCodigo);
void escribrirCodigoInstruccion(Tabcop tabcop[], int indice, string &cadenaModificada, string numCodigo);
void rellenarCeros(string &dirMemoria);

void leerLineas(Tabcop tabcop[], string &cadenaFinal){
    string linea, cadenaAux, cadenaModificada, memoria = "0000";
    int posInicialLinea = 0, posInicialLineaAux, posFinalLinea, posFinalCodigo;
    while (posInicialLinea != cadenaFinal.size()){                  //Primera lectura, reserva de memoria
        posFinalLinea = cadenaFinal.find_first_of(delimitadorLinea, posInicialLinea);
        linea = cadenaFinal.substr(posInicialLinea, posFinalLinea-posInicialLinea);
        leerCampos(tabcop, cadenaModificada, linea, memoria);
        posInicialLinea = posFinalLinea+1;
    }
    posInicialLinea = 0;
    while (posInicialLinea != cadenaModificada.size()){             //Segunda lectura, valor de codigo
        posFinalLinea = cadenaModificada.find_first_of(delimitadorLinea, posInicialLinea);
        linea = cadenaModificada.substr(posInicialLinea, posFinalLinea-posInicialLinea);
        posInicialLinea = cadenaModificada.find_first_of(delimitadorCampoP, posInicialLinea);
        posInicialLinea++;
        cadenaAux = cadenaModificada.substr(posInicialLinea, posFinalLinea-posInicialLinea);
        posFinalCodigo = cadenaAux.find_first_of(delimitadorEtiqueta);
        if (posFinalCodigo != -1){
            cadenaAux = cadenaAux.substr(0, posFinalCodigo);
        }
        
        for (int i = 0; i < contEtiquetas; i++){
            if (cadenaAux == etiquetas[i].getNombre()){
                cout<<etiquetas[i].getMemoria()<<endl;
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
            rellenarCeros(dirMemoria);  
        }
        cadenaModificada += linea +'\t'+ dirMemoria +" "+ tabcop[indice].getCodigoInstruccion()+'\n';
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
                rellenarCeros(dirMemoria);  
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
                        rellenarCeros(dirMemoria);  
                    }
                    cadenaModificada += linea+'\t'+dirMemoria+" "+tabcop[i].getCodigoInstruccion()+' ';
                    if (dirMemoria.size() < 4){
                        rellenarCeros(dirMemoria);  
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
            codigo = stoi(subCadena);
            if (codigo < limiteDecimalOrp8){
                for (int i = indice; i < limite; i++){
                    if ("DIR" == tabcop[i].getDireccionamiento()){
                        memoria = stoi(dirMemoria);
                        dirMemoria = decimalAHexa(memoria);
                        if (dirMemoria.size() < 4){
                            rellenarCeros(dirMemoria);  
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
                            rellenarCeros(dirMemoria);  
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

void validarSistemaNumeracionOEtiqueta(string &subCadena, string &cadenaModificada, string &numCodigo){
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
            rellenarCeros(dirMemoria);  
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
            rellenarCeros(dirMemoria);  
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
            rellenarCeros(dirMemoria);  
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
            posFinalDato = subCadena.find_first_of(delimitadorDCB, posInicialLinea);
            if (posFinalDato != -1){
                posIniDato = 0;
                while (posIniDato < subCadena.size()){
                    numCodigo = subCadena.substr(posIniDato, posFinalDato);
                    validarSistemaNumeracionOEtiqueta(numCodigo, cadenaModificada, numCodigo);
                    cadenaModificada += numCodigo + ' ';
                    memoria ++;
                    dirMemoria = to_string(memoria);
                    posIniDato = posFinalDato+1;
                    posFinalDato = subCadena.find_first_of(delimitadorDCB, posIniDato);
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
            rellenarCeros(dirMemoria);  
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
            posFinalDato = subCadena.find_first_of(delimitadorDCB, posInicialLinea);
            if (posFinalDato != -1){
                posIniDato = 0;
                while (posIniDato < subCadena.size()){
                    numCodigo = subCadena.substr(posIniDato, posFinalDato);
                    validarSistemaNumeracionOEtiqueta(numCodigo, cadenaModificada, numCodigo);
                    rellenarCeros(numCodigo);
                    cadenaModificada += numCodigo + ' ';
                    memoria += 2;
                    dirMemoria = to_string(memoria);
                    posIniDato = posFinalDato+1;
                    posFinalDato = subCadena.find_first_of(delimitadorDCB, posIniDato);
                    if (posFinalDato == -1){
                        posFinalDato = subCadena.size();
                    }
                }
                cadenaModificada += '\n';
            }
            else{
                validarSistemaNumeracionOEtiqueta(subCadena, cadenaModificada, numCodigo);
                rellenarCeros(numCodigo);
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
            rellenarCeros(dirMemoria);  
        }
        cadenaModificada += linea +'\t'+ dirMemoria + ' ';
        posIniDato = linea.size() - subCadena.size();
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
            rellenarCeros(dirMemoria);  
        }
        cadenaModificada += linea +'\t'+ dirMemoria + ' ';
        posIniDato = linea.size() - subCadena.size();
        posFinalDato = linea.size();
        subCadena = linea.substr(posIniDato+1, posFinalDato);
        validarSistemaNumeracionOEtiqueta(subCadena, cadenaModificada, numCodigo);
        cadenaModificada += numCodigo + '\n';
        memoria++;
        dirMemoria = to_string(memoria);
    }
    else if (subCadena == "FCC"){
        char charActual;
        int asciiCod, i = 0;
        memoria = stoi(dirMemoria);
        dirMemoria = decimalAHexa(memoria);
        if (dirMemoria.size() < 4){
            rellenarCeros(dirMemoria);  
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
            rellenarCeros(dirMemoria);  
        }
        cadenaModificada += linea +'\t'+ dirMemoria + ' ';
        posIniDato = linea.size() - subCadena.size();
        posFinalDato = linea.size();
        subCadena = linea.substr(posIniDato+1, posFinalDato);
        posFinalDato = subCadena.find_first_of(delimitadorDCB, 0);
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

void rellenarCeros(string &dirMemoria){
    int espacios = 4;
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