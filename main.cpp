#include <iostream>
#include <sstream>
#include <fstream>
#include "definiciones.h"
#include "tabcop.h"
#include "traductor.h"

using namespace std;

void cargarTabcop(Tabcop tabcop[]){
    ifstream archivoTabcop;
    string cadena, mn, dir, codInstruc;
    int posInicial = 0, posFinal, longInstruc, i = 0;
    archivoTabcop.open(archivoTABCOP);
    if (archivoTabcop.is_open()){
        while (!archivoTabcop.eof()){  
            getline(archivoTabcop, cadena, delimitadorLinea);
            posFinal = cadena.find_first_of(delimitadorCampoTABCOP, posInicial);
            posFinal = cadena.find_first_of(delimitadorLinea, posInicial);
            if (!cadena.empty()){
                Tabcop mnemonico(cadena);
                tabcop[i] = mnemonico;
            }
            i++;
        }
    }
    else{
        cout<<"No se pudo abrir el archivo"<<endl;
    } 
}

void cargarArchivo(string &cadenaF){
    ifstream archivoP;
    string cadena, nombreMnemonico;
    int posInicial = 0, posFinal;
    archivoP.open(nombreArchivo);
    if (archivoP.is_open()){
        while (!archivoP.eof()){  
            getline(archivoP, cadena, delimitadorLinea);
            posFinal = cadena.find_first_of(delimitadorLinea, posInicial);
            cadenaF += cadena+delimitadorLinea;
            posInicial = posFinal+1;
        }
    }
    else{
        cout<<"No se pudo abrir el archivo"<<endl;
    } 
}

int main(int argc, char const *argv[])
{
    string cadenaFinal;
    Tabcop tabcop[cantidadMnemonicos];

    cargarTabcop(tabcop);
    cargarArchivo(cadenaFinal);
    cout<<"Archivo orginal:\n"<<cadenaFinal;
    leerLineas(tabcop,cadenaFinal);
    cout<<"Archivo modificado:\n"<<cadenaFinal;
    return 0;
}