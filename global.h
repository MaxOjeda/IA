#ifndef externo
int Tamano;
#endif

#ifdef externo
extern int Tamano;
#endif

struct Nodo {
    float x, y;
    float demanda;
    int tipo;
    int id;
};

struct Vehiculo {
    int id;
    float dem_line;
    float dem_back;
    float dist_total;
    vector <int> tour;
};


// Variables de los parametros de entrada
char* arch_entrada;
char* arch_salida;
int debug;
int cantVehiculos;
float capacidad;
float menor_distancia;
int saltos;
int nodoRetorno;
bool cbj_flag;
int stop;


// Arreglos
int **dist;
Nodo *nodos;
Vehiculo *autos;
Vehiculo *mejores;
vector<int> id_nodos;
vector<int> *conflicto;

// Funciones
void leer_archivo(void);
int parametros_de_entrada(int, char **);
int busqueda_de_rutas(int *, int*, int, int);
vector<int> filtrar(vector<int>, int);
