#ifndef externo
int Tamano;
#endif

#ifdef externo
extern int Tamano;
#endif

struct nodo {
    float x, y;
    float demanda;
    int tipo;
    int id;
};

struct Vehiculo {
    int id;
    float peso;
    float dist_total;
    vector <int> tour;
};


// Variables de los parametros de entrada
char* arch_entrada;
char* arch_salida;
int debug;


int **dist;
nodo *nodos;
Vehiculo *autos;

void leer_archivo(void);
int parametros_de_entrada(int, char **);