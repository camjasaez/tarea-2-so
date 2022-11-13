#include <mutex>
#include <thread>
#include <iostream>
#include <fstream>
#include <stdlib.h> 
#include <unistd.h> 
#include <omp.h>
#include <vector>
#include <string> 
#include <iomanip>
using namespace std;

// Creacion de la clase Monitorizacion que contendrá los datos a monitorizar de cada IP
class Monitorizacion {      
  public:          
    string ip;
    string paquetesTransmitidos;
    string paquetesRecibidos;
    string paquetesPerdidos;
    bool estado; //(UP (1) = Disponible, DOWN (0)= NO disponible) 
};

// Funcion main que contiene el programa principal
int main(int argc, char *argv[]) {

    // Primera validacion de la cantidad de argumentos de entrada y formato correcto.
    if(argc != 3) {
        cout << "Error: Numero de argumentos incorrecto" << endl;
        cout << "Uso: " << argv[0] << " <nombre-fichero-de-ip>" <<" npaq=<cantidad-de-paquetes>" << endl;
        exit(-1);
    }

    // Segunda validacion de la extension del fichero con las ips
    string nombreArchivo = argv[1];
    if(nombreArchivo.find(".txt") > nombreArchivo.length()) {
        cout << "Error: El fichero de ip no es un fichero de texto" << endl;
        exit(-1);
    }

    //Tercera validacion de la cantidad de paquetes a enviar
    int cantidadPaquetes = atoi(argv[2]);
    if (cantidadPaquetes < 0) {
        cout << "npaq debe ser un numero > 0\n";
        return -1;
    }

    ifstream archivo(nombreArchivo.c_str());
    string linea;

    int cantidadHebras = 0; // Contador de hebras que tendra el programa
    std::vector<std::string> arregloIPs; // Arreglo que contendra las IPs a monitorizar
    // Lectura del fichero de ip y almacenamiento en un vector de cada una de las ip
    while (getline(archivo, linea)) {
        arregloIPs.push_back(linea);
        cantidadHebras++;
    }

    cout << "Cantidad de hebras: ";
    cout << cantidadHebras << endl;

    //Pequeña funcionalidad que nos ayudar a acalcular el ping mas largo, para luego poder mostrarlo con el formato correcto.
    int pingMasLargo= arregloIPs[0].length();; // Variable auxiliar que contendra el tamaño del ping mas largo de todas las hebras, para efectos practicos al mostrar los resultados.
    for(int i = 1; i < cantidadHebras; i++) {
        if(pingMasLargo < arregloIPs[i].length()) {
            pingMasLargo = arregloIPs[i].length();
        }
    }
   

    string ping = "ping -q -c" + std::to_string(cantidadPaquetes) + " "; // String que almacena el comando ping
    archivo.close();
    cout << "Ejecutando por hebras..." << endl;
    cout << "------------------------------------------------" << endl;
    cout << "IP     " << std::setw(pingMasLargo)  << "Trans." << std::setw(8)  << "Rec."<< std::setw(9)  << "Perd."<< std::setw(11) << " Estado\n";
    cout << "================================================\n";
    
    // Instancia de la clase Monitorizacion, que contendrá los datos a monitorizar de cada IP
    Monitorizacion monitorizaciones[cantidadHebras];
    std::mutex myMutex;
    int i = 0;
    #pragma omp parallel for num_threads(cantidadHebras) //Dividimos la ejecucion de nuestro ciclo for en hebras
        for(i = 0; i < cantidadHebras; i++){
            //Creamos los archivos logs para cada ping que leeremos mas adelante.
            string pingMasIP = ping + arregloIPs[i] + " > logs/ping" + std::to_string(i) + ".txt";
             int sysCallResponse = system(pingMasIP.c_str());

            std::string nombreArchivo = "logs/ping"+std::to_string(i)+".txt";
            std::ifstream file(nombreArchivo);

            //  Para verificar que el comando de ping es valido, podemos recibir dos respuestas principales, el 0 que indica 
            //  que el comando se ejecuto correctamente y recibe respuesta del servidor y  
            // que se ejecuta correctamente pero no recibe respuesta del serivdor 256. Si la respuesta es diferente a 0 o 256, 
            // el comando ping ejecutado no es valido, ya sea porque el ping ingresado no existe o a sido ingresado de manera erronea.
            if(sysCallResponse == 0 || sysCallResponse == 256){
            // Bloqueamos el hilo de ejecucion para que no se produzcan errores de lectura y escritura
            myMutex.lock();
            //Leemos el archivo creado y guardamos los datos en la clase Monitorizacion
            while(getline(file, linea)){
                //Buscamos en el archivo ping la ip
                if (linea.find("PING") != std::string::npos){
                    monitorizaciones[i].ip = linea.substr(5, linea.find("(")-5);
                }

                //Buscamos en el archivo ping la cantidad de paquetes transmitidos, recibidos y perdidos
                if(linea.find("packets")!= std::string::npos){
                    monitorizaciones[i].paquetesTransmitidos = linea.substr(0, linea.find(" "));
                
                    monitorizaciones[i].paquetesRecibidos = linea.substr(linea.find(",")+1, linea.find("received")-linea.find(",")-1);
                    
                    monitorizaciones[i].paquetesPerdidos = linea.substr(linea.find("received")+9, linea.find("packet loss")-linea.find("received")-9);
                }
                
            }
            // Verificamos si la ip esta disponible o no
                int paquetes = stoi(monitorizaciones[i].paquetesRecibidos);
                if(paquetes == 0){
                    monitorizaciones[i].estado = false;
                }else{
                    monitorizaciones[i].estado = true;
                }      
            
                      
            // Mostramos por pantalla los datos de cada coamndo ping, que estan almacenados en nuestra estructura de datos
            // Se iran mostrando a medida de que vayan terminando su ejecucion de hebras. 
            cout << monitorizaciones[i].ip << std::setw((pingMasLargo- arregloIPs[i].length())+3) // ajustamos el ping mas largo para que se vea bien en la pantalla.
            << monitorizaciones[i].paquetesTransmitidos << std::setw(10)
            << monitorizaciones[i].paquetesRecibidos << std::setw(10)
            << monitorizaciones[i].paquetesPerdidos << std::setw(10)
            << (monitorizaciones[i].estado ? "UP" : "DOWN") << endl;
            // Desbloqueamos el hilo de ejecucion
            myMutex.unlock();
        
            //Eliminamos los archivos creados para cada ping que ya no utiliaremos.
            string toRemove = "rm logs/ping"+std::to_string(i)+".txt";
            system(toRemove.c_str());
            }
        }
    cout << "------------------------------------------------" << endl;
    cout << "Fin del programa" << endl;
    return 0;
}

