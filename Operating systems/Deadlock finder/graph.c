#include "graph.h"
#include <stdlib.h>
#include <stdio.h>

Graph* createGraph(){
	Graph *G = (Graph *) malloc(sizeof(Graph));
	G->num_vertex = 0;
	G->num_edge = 0;
	G->v_list = (Vertex *) malloc(sizeof(Vertex));
	G->v_list[0].value = -1;												//Valor inválido => G não tem vértice
	G->v_list[0].color = WHITE;
	G->v_list[0].adj = NULL;
	G->v_list[0].type = -1;
	return(G);
}

int insertVertex(Graph *G, long value, int type){
	int i = G->num_vertex;
	if(i == 0) {															//Se G não tem vértice
		G->v_list[i].value = value;										    //Atualiza o valor
		G->v_list[i].color = WHITE;
		G->v_list[i].type = type;
		G->num_vertex += 1;													//Atualiza a qtd. de vértices
	} else {
		G->v_list = realloc(G->v_list, (++G->num_vertex)*sizeof(Vertex));	//Alocar nova posição em G->v_list
		G->v_list[i].value = value;											//Insere o valor do vértice
		G->v_list[i].color = WHITE;
		G->v_list[i].type = type;
		G->v_list[i].adj = NULL;											//Define a lista de adjacentes como vazia
	}
	return(i);
}

int findVertex(Graph *G, long value){
	if(!G) return(-1); 														//Se o grafo não estiver alocado retorna -1
	for(int i = 0; i < G->num_vertex; i += 1) {								//Para cada vértice na lista do grafo
		if(G->v_list[i].value == value)										//Se o valor do vértice atual for o valor procurado
			return i;														//Retorna o índice do vértice
	}
	return(-1);
}

Adjacent* createAdjacent(int id, long value, int type){
	Adjacent *temp = (Adjacent *) malloc(sizeof(Adjacent));
	temp->id = id;
	temp->type = type;
	temp->value = value;
	temp->next = NULL;
	return(temp);
}

bool findEdge(Graph *G, long iv, long fv){
	int id_iv = findVertex(G, iv);
	int id_fv = findVertex(G, fv);
	if(id_iv == -1 || id_fv == -1) return FALSE;

	Adjacent *temp = G->v_list[id_iv].adj;
	while(temp != NULL){
		if(fv == temp->value) return TRUE;
		temp = temp->next;
	}
	return(FALSE);
}

bool createEdge(Graph *G, long iv, int type_iv, long fv, int type_fv){
	int id_iv, id_fv;
	if(!G) return FALSE; 													//Se o grafo não estiver alocado
	if(findEdge(G, iv, fv)) return TRUE;

	id_iv = findVertex(G, iv);
	if(id_iv == -1){
		id_iv = insertVertex(G, iv, type_iv);
	}

	id_fv = findVertex(G, fv);
	if(id_fv == -1){
		id_fv = insertVertex(G, fv, type_fv);
	}

	G->num_edge += 1;
	Adjacent *new = createAdjacent(id_fv, fv, type_fv);
	new->next = G->v_list[id_iv].adj;
	G->v_list[id_iv].adj = new;
	return(TRUE);
}

bool deleteEdge(Graph *G, long iv, long fv){
	int id_iv, id_fv;
	Adjacent *prev, *curr;
	if(!G) return FALSE; 													//Se o grafo não estiver alocado
	id_iv = findVertex(G, iv);
	id_fv = findVertex(G, fv);
	if(id_iv == -1) return FALSE;
	if(id_fv == -1) return FALSE;

	prev = NULL; 															//Anterior é o iv em G->v_list
	curr = G->v_list[id_iv].adj;
	while(curr != NULL) {
		if(curr->value == fv){
			if(prev == NULL)
				G->v_list[id_iv].adj = curr->next;
			else
				prev->next = curr->next;
			G->num_edge--;
			curr->next = NULL;
			free(curr);
			return TRUE;
		} else {
			if(prev == NULL)
				prev = G->v_list[id_iv].adj;
			else
				prev = prev->next;
			curr = curr->next;
		}
	}
	return(TRUE);
}

 void showGraph(Graph *G){
 	for(int i = 0; i < G->num_vertex; i += 1) {
 		if(G->v_list[i].type == PRO){
			printf("Thread   %d -> ", i);
		}else{
			printf("Semáfaro %d -> ", i);
		}
 		Adjacent *ad = G->v_list[i].adj;
 		while(ad) {
			if(ad->type == PRO){
				printf("Thread   %d ->", ad->id);
			}else{
				printf("Semáforo %d ->", ad->id);
			}
 			ad = ad->next;
 		}
 		printf("\n");
 	}
 }

 void dfsRec(Graph *G, int id, bool *cycle){
	if(*cycle == TRUE)
		return;
	if(G -> v_list[id].color == BLACK){
		*cycle = TRUE;
		return;
	} else {
		G -> v_list[id].color = BLACK;
		Adjacent *v = G -> v_list[id].adj;
		while(v != NULL){
			dfsRec(G, v -> id, cycle);
			v = v -> next;
		}
	}
	G -> v_list[id].color = RED;
}

bool isCyclical(Graph *G){
	bool cycle = FALSE;

	for(int i = 0; i < G -> num_vertex; i += 1){
		dfsRec(G, i, &cycle);
		back_to_white(G);
	}
	return(cycle);
}

void back_to_white(Graph *G){
	for (int i = 0; i < G -> num_vertex; i+=1){
		G -> v_list[i].color = WHITE;
	}
}
