#include "includes.h"
#include "global.h"


void leer_archivo(void) {
    FILE *arch;

    arch = fopen(arch_entrada, "r");
    // Leer tamano de la instancia
    int read = fscanf(arch, "%d", &Tamano);

    // Creacion de arreglo de nodos
    nodos = new Nodo[Tamano];
    
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
    float cap;
    // Lectura de cantidad de vehiculos y capacidad
    read =  fscanf(arch, "%d %f\n", &n_vehiculos, &cap);
    // Creacion de lista de los vehiculos con configuraciones iniciales
    cantVehiculos = n_vehiculos;
    capacidad = cap;
    cout<<cantVehiculos<<" "<<capacidad<<endl;
    // Creacion de arreglo de vehiculos
    autos = new Vehiculo[n_vehiculos];
    for(int i = 0; i < n_vehiculos; i++){
       autos[i].id = i+1;
       autos[i].dem_line = 0;
       autos[i].dem_back = 0;
       autos[i].dist_total = 0;
       autos[i].tour = vector<int> {1};
    }
    
    for(int k=0;k<n_vehiculos;k++){
        cout<<autos[k].id<<" "<<autos[k].dem_line<<" "<<autos[k].dist_total<<endl;
    }
    
    int nodeID;
    float demand;
    // Setear la demanda de los nodos (clientes linehaul y backhaul)
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


void evaluar(){
    float sum = 0;
    for(int i=0; i<cantVehiculos; i++){
        sum += autos[i].dist_total;
    }
    cout<<"DISTANCIA: "<<sum<<"\n";
    if(sum < menor_distancia){
        menor_distancia = sum;
        for(int j=0; j<cantVehiculos; j++){
            mejores[j] = autos[j];
        }
    }
}

void busqueda_de_rutas(vector<int> clientes, vector<int> id_nodos, int nVehiculos, int nodoActual){
    // Se revisa si no quedan nodos
    if (clientes.size() == 0 && nVehiculos == 0){
        cout<<"TODAS LAS RUTAS\n";
        // Mostrar rutas
        float dist_deposito;
        int ultimo_nodo;
        for(int i = 0; i < cantVehiculos; i++){
            ultimo_nodo = autos[i].tour.back() - 1;
            dist_deposito = dist[ultimo_nodo][0];
            autos[i].dist_total += dist_deposito;
            autos[i].tour.push_back(nodos[0].id);
            
            for(const auto& value: autos[i].tour) {
            cout << value << "->";
            }   
        cout<<"\n";
        }evaluar();
        cout<<"M. DISTANCIA: "<<menor_distancia<<"\n";
        cout<<"\n";
        for(int i = 0; i < cantVehiculos; i++){
            autos[i].tour.pop_back();
            ultimo_nodo = autos[i].tour.back() - 1;
            autos[i].dist_total -= dist[ultimo_nodo][0];
        }
        
        return;
    
    }else{
        int idxVeh = nVehiculos-1;

        // Cuando no queden nodos disponibles para una ruta
        if(clientes.size() == 0){
            cout<<"Ruta final veh "<<nVehiculos<<"\n";
            for(const auto& value: autos[idxVeh].tour) {
                cout << value << "-->";
            }
            cout<<"\n";
            // Crear nuevo vector de nodos sin los que ya fueron ocupados
            vector <int> temp = id_nodos;
            for(int i=0; i<autos[idxVeh].tour.size(); i++){
                temp.erase(remove(temp.begin(), temp.end(), autos[idxVeh].tour[i]), temp.end());
            }


            //Cambiar de vehiculo
            nVehiculos -= 1;    
            cout<<"ALOOOOO "<<nVehiculos<<" "<<temp.size()<<"\n";
            //autos[idxVeh].tour.pop_back();
            busqueda_de_rutas(temp, temp, nVehiculos, 0);
            
        }
        else{
            if(autos[idxVeh].tour.size() > 2 && autos[idxVeh].tour.back() == 1){
                return;
            }
            for(int i=0; i < clientes.size(); i++){
                int nodoNuevo = clientes[i]-1;
                float distancia = dist[nodoActual][nodoNuevo];
                float newPeso = nodos[nodoNuevo].demanda;
                vector <int> clientesFiltrados = clientes;
                
                // Cliente tipo Linehaul
                if (nodos[nodoNuevo].tipo == 1) {
                    autos[idxVeh].dem_line += newPeso;
                    autos[idxVeh].dist_total += distancia;
                    autos[idxVeh].tour.push_back(nodos[nodoNuevo].id);


                    for(const auto& value: autos[idxVeh].tour) {
                        cout << value << "-->";
                    }
                    cout<<"\n";

                    cout<<"Antes\n";
                    for(int z=0; z<clientesFiltrados.size(); z++){
                        cout<<clientesFiltrados[z]<<"; ";
                    }
                    cout<<"\n";

                    clientesFiltrados.erase(clientesFiltrados.begin() + i);
                    clientesFiltrados = filtrar(clientesFiltrados, idxVeh);
                    
                    cout<<"Despues\n";
                    for(int z=0; z<clientesFiltrados.size(); z++){
                        cout<<clientesFiltrados[z]<<"; ";
                    }
                    cout<<"\n\n";
                    //busqueda_de_rutas(clientesFiltrados, Tamano, nVehiculos, nodoNuevo);

                    
                }else if(nodos[nodoNuevo].tipo == 2){
                    autos[idxVeh].dem_back += newPeso;
                    autos[idxVeh].dist_total += distancia;
                    autos[idxVeh].tour.push_back(nodos[nodoNuevo].id);

                    for(const auto& value: autos[idxVeh].tour) {
                        cout << value << "-->";
                    }
                    cout<<"\n";

                    cout<<"Antes\n";
                    for(int z=0; z<clientesFiltrados.size(); z++){
                        cout<<clientesFiltrados[z]<<"; ";
                    }
                    cout<<"\n";

                    clientesFiltrados.erase(clientesFiltrados.begin());
                    clientesFiltrados = filtrar(clientesFiltrados, idxVeh);

                    cout<<"Despues\n";
                    for(int z=0; z<clientesFiltrados.size(); z++){
                        cout<<clientesFiltrados[z]<<"; ";
                    }
                    cout<<"\n\n";
                    //busqueda_de_rutas(clientesFiltrados, Tamano, nVehiculos, nodoNuevo);
                }
                busqueda_de_rutas(clientesFiltrados, id_nodos, nVehiculos, nodoNuevo);
                int ultimo_nodo = autos[idxVeh].tour.back() - 1;
                if(nodos[ultimo_nodo].tipo == 1){
                    autos[idxVeh].dem_line -= nodos[ultimo_nodo].demanda;
                }else{
                    autos[idxVeh].dem_back -= nodos[ultimo_nodo].demanda;
                }
                autos[idxVeh].tour.pop_back();
                autos[idxVeh].dist_total -= newPeso;
                
            }
        }
    }
    return;
}

vector<int> filtrar(vector<int> v_nodos, int idxVehiculo){
    vector<int> borrar;
    int idxNodo;
    for(int i=0; i<v_nodos.size(); i++){
        idxNodo = nodos[v_nodos[i]-2].id; // Indice del nodo en el vector Nodo *nodos
        if((nodos[idxNodo].tipo == 1) && (autos[idxVehiculo].dem_line + nodos[idxNodo].demanda > capacidad)){
            borrar.push_back(v_nodos[i]);
        }else if((nodos[idxNodo].tipo == 2) && (autos[idxVehiculo].dem_back + nodos[idxNodo].demanda > capacidad)){
            borrar.push_back(v_nodos[i]);
        }
    }
    // Eliminar nodos que no seran comptabiles
    for(int j=0; j<borrar.size(); j++){
        v_nodos.erase(remove(v_nodos.begin(), v_nodos.end(), borrar[j]), v_nodos.end());
    }
    return v_nodos;
}


int main (int argc, char *argv[]){
    if (!parametros_de_entrada(argc,argv)) {
        cout<<"Error al leer los parametros de entrada";
        exit(1);
    }
    cout<<"Y aqui?\n";
    leer_archivo();

    mejores  = autos;
    // Vector de nodos sin el deposito
    for(int i = 1; i < Tamano; i++)
        id_nodos.push_back(nodos[i].id);
    
    menor_distancia = MAX;
    busqueda_de_rutas(id_nodos, id_nodos, cantVehiculos, 0);

    return 0;
}