#include "includes.h"
#include "global.h"


void leer_archivo(void) {
    FILE *arch;

    arch = fopen(arch_entrada, "r");
    // Leer tamano de la instancia
    int read = fscanf(arch, "%d", &Tamano);

    // Creacion de arreglo de nodos
    nodos = new nodo[Tamano];
    // Arreglo de distancias
    dist = new int*[Tamano];
    for (int i = 0; i < Tamano; i++){
        dist[i] = new int [Tamano];
    }
    int id, t;
    float x, y;
    int cont = 0;
    // Guardar informacion de cada nodo
    for(int j = 0; j < Tamano; j++){
        int read =  fscanf(arch, "%d %d %f %f\n", &t, &id, &x, &y);
        nodos[id-1].x = x;
        nodos[id-1].y = y;
        nodos[id-1].tipo = t;
        nodos[id-1].id = id;
        nodos[id-1].demanda = 0;
    }

    // Matriz de distancias
    for(int i = 0; i<(Tamano-1); i++){
        for(int j = i+1; j<Tamano; j++){
            // Distancia euclidiana
            dist[i][j] = round(sqrt(pow((nodos[i].x - nodos[j].x),2) + pow((nodos[i].y - nodos[j].y),2)));
            dist[j][i] = dist[i][j];
        }
    }

    if(debug){
        for(int i=0;i<Tamano;i++)
            for(int j=0;j<Tamano;j++)
                cout<<dist[i][j]<<",";
            cout<<endl;  
    }

    int n_vehiculos;
    float capacidad;
    read =  fscanf(arch, "%d %f\n", &n_vehiculos, &capacidad);
    Vehiculo autos[n_vehiculos];
    for(int i = 0; i < n_vehiculos; i++){
       autos[i].id = i+1;
       autos[i].peso = 0;
       autos[i].dist_total = MAX;
       autos[i].tour = vector<int>(Tamano,1);
    }
    
    int nodeID;
    float demand;
    for(int j = 0; j < (Tamano-1); j++){
        int read = fscanf(arch, "%d %f\n", &nodeID, &demand);
        nodos[nodeID-1].demanda += demand;
    }

    if(debug){
        cout<<"NodoID | Tipo | Demanda"<<endl;
        for(int i=1; i < (Tamano); i++){
            cout<<"  "<<nodos[i].id<<"\t"<<nodos[i].tipo<<" \t"<<nodos[i].demanda<<endl;
        }
    }

    fclose(arch);
    return;
}

int parametros_de_entrada(int argc, char **argv) {
    // Archivo de entrada
    arch_entrada = (char*)(argv[1]);

    // Archivo de salida
    arch_salida = (char*)(argv[2]);

    // Opcion de debug
    debug = atoi(argv[3]);

    return 1;
}






int main (int argc, char *argv[]){
    if (!parametros_de_entrada(argc,argv)) {
        cout<<"Error al leer los parametros de entrada";
        exit(1);
    }

    leer_archivo();

    return 0;
}