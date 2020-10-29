#define TRUE 1
#define FALSE 0
typedef int bool;

#define WHITE 1
#define BLACK 2
#define RED 3

#define PRO 1
#define SEM 2

typedef struct Adjacent{													//Vértices adjacentes
	int id;                                                                 //Indice do vértice no grafo
	int type;																//Tipo do vértice(processo ou recurso)
	long value; 															//Valor do vértice
	struct Adjacent *next; 													//Próximo vértice adjacente
}Adjacent;

typedef struct Vertex{
	int type;                                                               //Tipo do vértice(processo ou recurso)
	int color;                                                              //Cor do vértice para visitação (encontrar ciclos)
	long value;																//Valor do vértice
	Adjacent *adj;															//Lista de vértices adjacentes
}Vertex;

typedef struct Graph{
	int num_vertex; 														//Número de vértices
	int num_edge;															//Número de arestas
	Vertex *v_list; 														//Lista principal de vértices do grafo
}Graph;

Graph* createGraph();

int insertVertex(Graph *G, long value, int type);

int findVertex(Graph *G, long value);

Adjacent* createAdjacent(int id, long value, int type);

bool findEdge(Graph *G, long iv, long fv);

bool createEdge(Graph *G, long iv, int type_iv, long fv, int type_fv);

bool deleteEdge(Graph *G, long iv, long fv);

void showGraph(Graph *G);

void dfsRec(Graph *G, int id, bool *cycle);

bool isCyclical(Graph *G);
