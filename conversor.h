#ifndef CONVERSOR_H_INCLUDED
#define CONVERSOR_H_INCLUDED
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

string decimalAHexa(int decimal);

string decimalAHexa(int decimal){
    string numHexa = "";
    int residuo, resultado;
    do{
        residuo = decimal % 16;
        resultado = decimal / 16;
        if (residuo >= 10 && residuo <= 15){
            switch(residuo){
                case 10:    numHexa +='A';  break;
                case 11:    numHexa +='B';  break;
                case 12:    numHexa +='C';  break;
                case 13:    numHexa +='D';  break;
                case 14:    numHexa +='E';  break;
                case 15:    numHexa +='F';  break;
            }
        }
        else{
            numHexa += to_string(residuo);
        }       
        decimal = resultado;
    }while (resultado > 15);
    if (resultado >= 10 && resultado <= 15){
        switch(residuo){
            case 10:    numHexa +='A';  break;
            case 11:    numHexa +='B';  break;
            case 12:    numHexa +='C';  break;
            case 13:    numHexa +='D';  break;
            case 14:    numHexa +='E';  break;
            case 15:    numHexa +='F';  break;
        }
    }
    else{
        numHexa += to_string(resultado);
    }
    reverse(numHexa.begin(), numHexa.end());
    return numHexa;
}

#endif 