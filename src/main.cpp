#include <thread>
#include <iostream>
#include <fstream>
using namespace std;
#include <stdlib.h> // atoi
#include <unistd.h> // sleep
#include <omp.h>
#include <stdio.h>
#include <vector>
#include <string> 


int main(int argc, char *argv[]) {
    string nombreArchivo = "archivo_listado_ips.txt";
    ifstream archivo(nombreArchivo.c_str());
    string linea;
    int cantidadHebras = 0;

    std::vector<std::string> arr;
    while (getline(archivo, linea)) {
        arr.push_back(linea);
        cantidadHebras++;
    }
    cout << "Cantidad de hebras: ";
    cout << cantidadHebras << endl;

     if (atoi(argv[1]) <= 0) {
        cout << "numero paquetes debe ser > 0\n";
        return -1;
    }

    int cantidadPaquetes = atoi(argv[1]);

    std::string cantidadPaquetesString = std::to_string(cantidadPaquetes);
    string ping = "ping -q -c" + cantidadPaquetesString + " ";
    #pragma omp parallel for num_threads(cantidadHebras)
        for(int i = 0; i < cantidadHebras; i++){
            string pingMasIP = ping + arr[i];
            int x = system(pingMasIP.c_str());
        }
        
}

