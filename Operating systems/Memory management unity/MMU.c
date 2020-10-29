#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
typedef struct {
	uint8_t flag_cache:1;
	uint8_t flag_referenciada:1;
	uint8_t flag_modificada:1;
	uint8_t flag_protecao:3;
	uint8_t flag_padding:2; // inutilizado,apenas para preenchimento
	uint16_t offset_moldura:12;
	uint16_t offset_padding:4; // inutilizado, apenas para preenchimento
}map_virtual_t;

typedef struct Item{
	uint64_t pagVirtual;
	struct Item *prox;
	map_virtual_t pagReal;

}itemLista;

typedef struct{
	int tamanho;
	struct Item *head;
}lista;

lista * lista_inicia(){
	lista *p;
	p = (lista*)malloc(sizeof(lista));
	p -> tamanho = 0;
	p -> head = (struct Item *)malloc(sizeof(itemLista));
	p -> head -> prox = NULL;
	return p;
}

void lista_insere(lista * list, map_virtual_t real, uint64_t virtual){
	struct Item *p = (struct Item*)malloc(sizeof(itemLista));
	p -> prox = NULL;
	p -> pagVirtual = virtual;
	p -> pagReal = real;

	struct Item *aux = list -> head;
	while(aux -> prox != NULL){
		aux = aux -> prox;	
	}
	aux -> prox = p;

}

void lista_printa(lista *list){
	struct Item *aux = list -> head -> prox;
	while(aux != NULL){
		printf("%ld, ", aux -> pagVirtual);
		aux = aux -> prox;
	}
}//verificar se a lista ta funcionando

struct Item * lista_pesquisa(lista *list, uint64_t pVirt){
	struct Item *aux = list -> head -> prox;
	while(aux != NULL && aux -> pagVirtual != pVirt){
		aux = aux -> prox;
	}
	return aux;
}

///////////////// IMPLEMENTANDO HASH

typedef struct {
	lista **table;
	unsigned long int tamanho;
}hash;

hash * hash_inicia(unsigned long int tam){
	hash *p = (hash*)malloc(sizeof(hash));
	p -> tamanho = tam;
	p -> table = (lista**)malloc(sizeof(lista*)*tam);
	for(int i = 0; i < tam; i++){
		p -> table[i] = lista_inicia();
	}
	return p;
}

void hash_insere(hash *p, uint64_t virtual_addr, map_virtual_t pReal){
	int pg = virtual_addr / 4000;
	lista_insere(p -> table[pg % p -> tamanho], pReal, virtual_addr);
}

map_virtual_t *busca_map_virtual(hash *p, uint64_t virtual_addr){
	int pg = virtual_addr / 4000;
	struct Item *resultado_busca;
	resultado_busca = lista_pesquisa(p -> table[pg % p -> tamanho], virtual_addr);
	if (resultado_busca == NULL)
		return NULL;
	return &(resultado_busca -> pagReal);
}

int main(){

	srand(time(NULL));	
	hash *p = hash_inicia(64000); 
	map_virtual_t pReal;
	uint64_t end_virtual;

	for(int a = 0; a < 1000; a++){
		pReal.flag_cache = rand() % 2;
		pReal.flag_referenciada = rand() % 2;
		pReal.flag_modificada = rand() % 2;
		pReal.flag_protecao = rand() % 8;
		pReal.flag_padding = rand() % 4;
		pReal.offset_moldura = rand() % 4096;
		pReal.offset_padding = rand() % 16;

		end_virtual = rand() % 512000000; 	

		hash_insere(p, end_virtual, pReal);
	}

	map_virtual_t *retPesquisa;

	for(int a = 0; a < 1000; a++){
		end_virtual = rand() % 512000000; 	
		retPesquisa = busca_map_virtual(p, end_virtual);
		if(retPesquisa == NULL) printf("Page miss \n");
		else
		{
			printf("%" PRIu8 "\n",  retPesquisa->flag_cache);
  			printf("%" PRIu8 "\n",  retPesquisa->flag_referenciada);
  			printf("%" PRIu8 "\n",  retPesquisa->flag_modificada);
 	 		printf("%" PRIu8 "\n",  retPesquisa->flag_protecao);
 		 	printf("%" PRIu8 "\n",  retPesquisa->flag_padding);
 	 		printf("%" PRIu16 "\n", retPesquisa->offset_moldura);
 		  	printf("%" PRIu16 "\n", retPesquisa->offset_padding);
			printf("-----------\n");

		}
	}
}
