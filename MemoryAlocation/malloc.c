//
//  malloc.c
//  SistemasOperacionais
//
//  Created by Caio Hikaru Aguena on 02/09/18.
//  Copyright © 2018 Caio Hikaru Aguena. All rights reserved.
//

#include <stddef.h>
#include <stdio.h>
#include "mallocheader.h"

// Inicializa o bloco para uso
void init(){
    blocoLivre->tamanho = 16024-sizeof(struct bloco); // 16k é o tamanho do blocão que usaremos, subtraido do tamanho do bloco que contém a info
    blocoLivre->free = 1; // Obviamente, se inicializado tem q estar livre
    blocoLivre->proximoBloco = NULL; // Como só tem um bloco, aponta para null
    
}

// Divide um bloco caso o espaço a ser alocado seje menor que o bloco:
void split(struct bloco *espaco, size_t tamanhoRequisitado){
    struct bloco *novoBloco = (void*)((void*)espaco+tamanhoRequisitado+sizeof(struct bloco) ); // Cria um novo ponteiro para o novo bloco, onde ficará o espaço vazio, leigamente falando o pornteiro fica "Afrente" do bloco espaco que encontramos.
    novoBloco->tamanho = (espaco->tamanho) - tamanhoRequisitado - sizeof(struct bloco); // como o bloco será dividido, espaco->tamanho = representa o tamanho original do bloco, tamanhoRequisitado = o quanto queremos alocar, sizeof(struck bloco) = bytes com a info do novo bloco.
    novoBloco->free = 1; // Novo bloco, começará livre
    novoBloco->proximoBloco = espaco->proximoBloco; // Como o novo bloco ficará "afrente", esse novo bloco terá como próximo bloco o bloco que o bloco espaco apontava.
    
    // Agora trataremos o bloco espaco, onde ficará a memória.
    espaco->tamanho = tamanhoRequisitado; // mudamos o tamanho dele para o tamanho requisitado
    espaco->free = 0; // Agora o bloco estará alocado.
    espaco->proximoBloco = novoBloco; // Agora o proximo bloco será o bloco que acabamos de criar
    
    // Para uma representação visual de como isso funciona, veja o sufite com os desenhos.
}

// Malloc simulator
void *aloca(size_t QtdBytes){
    struct bloco *atual, *anterior; // Usado para percorrer a lista
    void *resultado; // Usado para retornar o endereço inicial de um bloco da memoria
    
    // Inicializa a memória caso não foi feito anteriormente.
    if(!(blocoLivre->tamanho)){
        init();
        printf("Memoria inicializada\n");
    }
    
    printf("\nTamanho que eu quero alocar: %zu \n", QtdBytes);
    atual = blocoLivre;
    
    // Percorre todos os blocos procurando um espaço disponivel, lembrando que o bloco deve ter o tamanho requisitado no minimo e não deve estar alocado.
    do{
        // Percorrendo a lista...
        // Condição pra quando achamos um bloco do exato tamanho que precisavamos
        if( ((atual->tamanho) == QtdBytes) && (atual->free == 1) ){
            printf("Tamanho exato do bloco para ser alocado\n");
            printf("Memoria alocada - ponteiro %p\n", atual);
            atual->free = 0; // Agora o bloco está ocupado
            resultado = (void*)(atual);
            return  resultado; // Retorna o endereço inicial do bloco que foi alocado
        }
        
        // Condição pra caso o bloco encontrado ter tamanho maior que o requisitado, lembrando que deve ser maior que a QtdBytes + o tamanho da struct, precisamos desse espaço pois iremos criar outro bloco utilizando o split, assim precisando de espaço para criar uma info nova.
        // Sim, ele aloca no primeiro bloco que tem espaço o suficiente.
        else if( ((atual->tamanho) > (QtdBytes+sizeof(struct bloco))) && (atual->free == 1) ){
            printf("Dividido bloco em 2\n");
            split(atual, QtdBytes); // Divide o bloco para se alocar somente o espaço que queremos
            printf("Memoria alocada - ponteiro %p\n", atual);
            resultado = (void*)(atual);
            return  resultado; // Retorna o endereço do bloco que foi alocado
        }
        
        anterior = atual;
        atual = atual->proximoBloco;
    }while(atual);
    
    // Caso não encontre um bloco com memória disponivel.
    printf("Memoria insuficiente.\n");
    resultado = NULL;
    return resultado;
    
}

// Função que faça um merge de 2 blocos consecutivos que estejam free
void merge(){
    struct bloco *anterior, *atual; // Usado como comparação
    atual = blocoLivre;
    
    // Agora percorreremos todos os blocos, procurando sempre se 2 blocos consecutivos não estão alocado
    while (atual && (atual->proximoBloco)) {
        // Condição em que os 2 blocos não estão alocados
        if ( (atual->free) &&  (atual->proximoBloco->free) ){
            atual->tamanho += (atual->proximoBloco->tamanho) + sizeof(struct bloco); // Atualiza o tamanho do bloco atual, adicionando o tamanho do proximo bloco + header
            atual->proximoBloco = atual->proximoBloco->proximoBloco; // Atualizamos o proximo bloco, já que tecnicamente o bloco que foi agregado não existe mais
            atual = blocoLivre;
            flag = 1;
        }
        
        // Se foi realizado um merge, tem que começar do começo
        if(flag == 1){
            flag = 0;
        }else{
            anterior = atual;
            atual = atual->proximoBloco;
        }
       
    }
}

// Função libera, que libera memória alocada previamente
void libera(void* ponteiroLibera){
    // Condicional para saber se o ponteiro está "dentro" do vetor de memória
    if( ((void*)memory <= ponteiroLibera) && (ponteiroLibera <= (void*)(memory + 16024) ) ){
        struct bloco *atual = ponteiroLibera;
        //--atual;
        printf("\nMemoria liberada - ponteiro %p\n", atual);
        atual->free = 1;
        merge(); // Verifica e realiza o merge, necessário para identificar se o free deixou uma sequencia de blocos desalocados.
    }else {
        printf("Não está no range \n");
    }
}

void apresentaMemoria(){
    struct bloco *atual;
    atual = blocoLivre;
    printf("\nPercorrendo a memória:\n");
    while(atual){
        printf("Tamanho do bloco: %zu | Alocado: %d | Endereco: %p \n", atual->tamanho, atual->free, atual);
        atual = atual->proximoBloco;
    }
}

int main(){
    printf("Tamanho da struct: %lu \n", sizeof(struct bloco));
    char *p = aloca(1000*sizeof(char));
    char *p1 = aloca(1000*sizeof(char));
    char *p2 = aloca(1000*sizeof(char));
    char *p3 = aloca(1000*sizeof(char));

    libera(p);
//    libera(p1);
    //libera(p2);
  //  libera(p3);
    apresentaMemoria();
}
