#include "includes.h"
#include "global.h"


void leer_archivo(void) {
    FILE *arch;

    arch = fopen("Instancias/GF1.txt", "r");
    // Leer tamano de la instancia
    int read = fscanf(arch, "%d", &Tamano);

    // Creacion de arreglo de nodos
    nodos = new Nodo[Tamano];
    
    // Creacion de conjuntos de conflicto (CBJ)
    conflicto = new vector<int>[Tamano-1];

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
    //
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
    // Print de matriz de distnacias
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
    // Opcion de debug
    debug = atoi(argv[1]);

    return 1;
}


void evaluar(){
    /* En esta funcion se evalua si la distancia total obtenida
    sumando la distancia de todos los vehiculos es mejor que la anterior */
    float sum = 0;
    // Sumar las distancias de los vehiculos
    for(int i=0; i<cantVehiculos; i++){
        sum += autos[i].dist_total;
    }
    cout<<"Distancia: "<<sum<<"\n";
    // Si la distancia es menor que la mejor
    if(sum < menor_distancia){
        menor_distancia = sum;
        // Se guarda los datos de los vehiculos con mejor evaluacion
        for(int j=0; j<cantVehiculos; j++){
            mejores[j] = autos[j];
        }
    }
}

void cbj(vector<int> clientes){
    /* En esta funcion se obtiene el nodo al cual hay que retornar
    una vez creado el conjunto de conflictos */ 
    int nodo_conf = clientes.front();
    float dem = nodos[nodo_conf].demanda;
    bool flag = false;
    // Se revisa que nodos realizan conflicto con el nodo_conf
    for(int i=cantVehiculos; i>0; i--){
        if(flag == true) break;
        float demline = 0, demback = 0;
        // Se revisa los nodos que no permitian que el nodo fuese agregado al tour por la restriccion de capacidad
        for(const auto& value: autos[i-1].tour){
            if(nodos[value-1].tipo == 1){
                demline += nodos[value-1].demanda;
                if(demline + dem > capacidad){
                    conflicto[nodo_conf-2].push_back(value);
                    flag = true;
                    break;
                }
            }else{
                demback += nodos[value-1].demanda;
                if(demback + dem > capacidad){
                    conflicto[nodo_conf-2].push_back(value);
                    flag = true;
                    break;
                }
            }
        }
    }
    // Se guarda el nodo al cual hay que retornar en la variable global nodoRetorno
    nodoRetorno = conflicto[nodo_conf-2].front() - 1;
    conflicto[nodo_conf-2] = {};
    return;
}

void busqueda_de_rutas(vector<int> clientes, vector<int> id_nodos, int nVehiculos, int nodoActual){
    // Cantidad de iteraciones antes de terminar
    if(stop == 2500000) return;
    stop++;
    // Se revisa si no quedan nodos
    if (clientes.size() == 0 && nVehiculos == 0){
        cout<<"Solucion encontrada: \n";
        // Mostrar rutas
        float dist_deposito;
        int ultimo_nodo;
        for(int i = 0; i < cantVehiculos; i++){
            ultimo_nodo = autos[i].tour.back() - 1;
            dist_deposito = dist[ultimo_nodo][0];
            //Se suma la distancia del ultimo nodo al deposito y se agrega al tour
            autos[i].dist_total += dist_deposito;
            autos[i].tour.push_back(nodos[0].id);
            
            // Mostrar rutas
            for(const auto& value: autos[i].tour) {
                cout << value << "->";
            }
        cout<<"\n";
        }
        // Se evalua la solucion encontrada
        evaluar();
        cout<<"Menor distancia: "<<menor_distancia<<"\n";
        // Se elimina el desposito de la ruta de los vehiculos y se resta la distancia al mismo
        for(int i = 0; i < cantVehiculos; i++){
            autos[i].tour.pop_back();
            ultimo_nodo = autos[i].tour.back() - 1;
            autos[i].dist_total -= dist[ultimo_nodo][0];
        }
        return;
    
    }else if(clientes.size() > 0 && nVehiculos == 0){
        // Caso en que no queden vehiculos disponibles pero restan nodos sin ocupar
        if(debug) cout<<"Solucion infactible\n";
        // Se busca al nodo al cual retorna con CBJ
        cbj(clientes);
        // Este flag cambiara cuando se vuelva al nodo entregado por el CBJ
        cbj_flag = true;
        return;

    }else{
        int idxVeh = nVehiculos-1;
        // Cuando no queden nodos disponibles para una ruta
        if(clientes.size() == 0){
            // Crear nuevo vector de nodos sin los que ya fueron ocupados
            vector <int> temp = id_nodos;
            for(int i=0; i<autos[idxVeh].tour.size(); i++){
                temp.erase(remove(temp.begin(), temp.end(), autos[idxVeh].tour[i]), temp.end());
            }
            //Cambiar de vehiculo
            nVehiculos -= 1;

            // Llamado recursivo
            busqueda_de_rutas(temp, temp, nVehiculos, 0);
            
        }
        else{
            // Se instancias los nodos uno por uno agregando al tour del vehiculo dicho nodo junto
            // con la distancia y la demanda
            for(int i=0; i < clientes.size(); i++){
                int nodoNuevo = clientes[i]-1;
                float distancia = dist[nodoActual][nodoNuevo];
                float newPeso = nodos[nodoNuevo].demanda;
                // Vector que tendra los clientes que no fueron eliminados por FC
                vector <int> clientesFiltrados = clientes;
                
                // Se asegura que no se agrege un nodo ya existente en la ruta
                if(find(autos[idxVeh].tour.begin(),autos[idxVeh].tour.end(), nodos[nodoNuevo].id) != autos[idxVeh].tour.end() || autos[idxVeh].tour.back() == nodos[nodoNuevo].id){
                    return;
                }
                
                // Cliente tipo Linehaul
                if (nodos[nodoNuevo].tipo == 1) {
                    autos[idxVeh].dem_line += newPeso;
                    autos[idxVeh].dist_total += distancia;
                    autos[idxVeh].tour.push_back(nodos[nodoNuevo].id);

                    // Forward Checking
                    clientesFiltrados.erase(clientesFiltrados.begin() + i);
                    clientesFiltrados = filtrar(clientesFiltrados, idxVeh);
                
                // Clinete tipo backhaul
                }else if(nodos[nodoNuevo].tipo == 2){
                    autos[idxVeh].dem_back += newPeso;
                    autos[idxVeh].dist_total += distancia;
                    autos[idxVeh].tour.push_back(nodos[nodoNuevo].id);
          
                    // Forward Checking
                    clientesFiltrados.erase(clientesFiltrados.begin());
                    clientesFiltrados = filtrar(clientesFiltrados, idxVeh);

                }
                cout<<"\n";
                // Llamado recursivo con los clientesFiltrados
                busqueda_de_rutas(clientesFiltrados, id_nodos, nVehiculos, nodoNuevo);

                // Se elimina el nodo agregado, la distancia a este y su demanda dependieno el tipo
                int ultimo_nodo = autos[idxVeh].tour.back() - 1;
                if(nodos[ultimo_nodo].tipo == 1){
                    autos[idxVeh].dem_line -= nodos[ultimo_nodo].demanda;
                }else{
                    autos[idxVeh].dem_back -= nodos[ultimo_nodo].demanda;
                }
                autos[idxVeh].tour.pop_back();
                autos[idxVeh].dist_total -= distancia;

                // Si hay que realizar cbj y no se ha alcanzado el nodoRetorno, se vuelve al anterior
                if (cbj_flag == true && nodoRetorno != nodoActual){
                    saltos +=1;
                    if(debug) cout<<"Salto: "<<saltos<<" Nodo actual:" <<nodoActual+1<<" Tour size: "<<autos[idxVeh].tour.size()<<"\n";
                    return;}
                cbj_flag = false;
            }
        }
    }
    return;
}

vector<int> filtrar(vector<int> v_nodos, int idxVehiculo){
    /* Funcion de Forward Checking. El vector v_nodos contiene los nodos disponibles.
    Se eliminan los nodos los cuales no cumpliran las restricciones de capacidad y demanda
    para linehaul y backahul. El vector borrar contiene los nodos que no van cumpliendo con las restricciones
    para luego eliminarlos de v_nodos */
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
    // Lectura de parametros de entrada
    if (!parametros_de_entrada(argc,argv)) {
        cout<<"Error al leer los parametros de entrada";
        exit(1);
    }
    // Lectura del archivo de instancia
    leer_archivo();

    // Vector de nodos sin el deposito
    for(int i = 1; i < Tamano; i++)
        id_nodos.push_back(nodos[i].id);

    stop = 0;
    saltos = 0;
    mejores  = autos;
    menor_distancia = MAX;
    
    // Comienzo de busqueda de rutas
    busqueda_de_rutas(id_nodos, id_nodos, cantVehiculos, 0);
    cout<<"Limite alcanzado\n";
    return 0;
}