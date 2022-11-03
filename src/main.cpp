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

struct monitorizacion{             
  string ip;        
  string paquetesTransmitidos;
  string paquetesRecibidos;
  string paquetesPerdidos;
  bool estado; //(UP (1) = Disponible, DOWN (0)= NO disponible)
}; 

void lecturaDeArchivosPingYMostrar(int cantidadDeArchivos){
    std::vector<monitorizacion> monitorizaciones(cantidadDeArchivos);
    std::string linea;
    std::string ip;
    std::string paquetesTransmitidos;
    std::string paquetesRecibidos;
    std::string paquetesPerdidos;
    // std::bool estado;
    for(int i = 0; i<cantidadDeArchivos; i++){
        std::string nombreArchivo = "logs/ping"+std::to_string(i)+".txt";
        std::ifstream file(nombreArchivo);
        
        while(getline(file, linea)){
            // cout << linea << endl;
            if (linea.find("PING") != std::string::npos){
                ip = linea.substr(5, linea.find("(")-5);
                // cout << "ip: " << ip << endl;
                monitorizaciones[i].ip = ip;
            }

            if(linea.find("packets")!= std::string::npos){
                paquetesTransmitidos = linea.substr(0, linea.find(" "));
                // cout << "paquetes transmitidos: " << paquetesTransmitidos << endl;
                monitorizaciones[i].paquetesTransmitidos = paquetesTransmitidos;

                paquetesRecibidos = linea.substr(linea.find(",")+1, linea.find("received")-linea.find(",")-1);
                // cout << "paquetes recibidos: " << paquetesRecibidos << endl;
                monitorizaciones[i].paquetesRecibidos = paquetesRecibidos;

                paquetesPerdidos = linea.substr(linea.find("received")+9, linea.find("packet loss")-linea.find("received")-9);
                // cout << "paquetes perdidos: " << paquetesPerdidos << endl;
                monitorizaciones[i].paquetesPerdidos = paquetesPerdidos;

                if(linea.find("100% packet loss") != std::string::npos){
                    monitorizaciones[i].estado = false;
                }else{
                    monitorizaciones[i].estado = true;
                }
            }
        }
        
    }

    for(int i = 0; i<cantidadDeArchivos; i++){
        cout << "ip: " << monitorizaciones[i].ip << endl;
        cout << "paquetes transmitidos: " << monitorizaciones[i].paquetesTransmitidos<< endl;
        cout << "paquetes recibidos: " << monitorizaciones[i].paquetesRecibidos << endl;
        cout << "paquetes perdidos: " << monitorizaciones[i].paquetesPerdidos << endl;
        if(monitorizaciones[i].estado == true){
            cout << "estado: UP" << endl;
        }else{
            cout << "estado: DOWN" << endl;
        }
        cout << "------------------------------------------" << endl;
    }
}

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
    cout << "Ejecutando por hebras..." << endl;
    cout << "------------------------------------------" << endl;
    int i;
    #pragma omp parallel for private(i) num_threads(cantidadHebras)
        for(int i = 0; i < cantidadHebras; i++){
            string pingMasIP = ping + arr[i] + " > logs/ping" + std::to_string(i) + ".txt";
            int x = system(pingMasIP.c_str());
            
        }
        
    lecturaDeArchivosPingYMostrar(cantidadHebras);
    cout << "Fin del programa" << endl;
    return 0;
}

