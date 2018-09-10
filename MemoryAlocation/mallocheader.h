//
//  mallocheader.h
//  SistemasOperacionais
//
//  Created by Caio Hikaru Aguena on 28/08/18.
//  Copyright © 2018 Caio Hikaru Aguena. All rights reserved.
//

#ifndef mallocheader_h
#define mallocheader_h

#include <stddef.h>
#include <stdio.h>

char memory[16024];

// Estrutura da info
struct bloco{
    size_t tamanho; // Tamanho do bloco
    int free; // Se a memória esta sendo alocada ou não. 1 = não alocada
    struct bloco *proximoBloco; // Ponteiro que aponta para o proximo bloco
};

// Ponteiro que aponta para o bloco de memória principal, especificamente para o primeiro endereço da lista linkada de blocos.
struct bloco *blocoLivre = (void*)memory;

int flag = 0;
void init();
void split(struct bloco *espaco,size_t tamanhoRequisitado);
void *aloca(size_t QtdBytes);
void merge();
void libera(void* ponteiroLibera);


#endif 
