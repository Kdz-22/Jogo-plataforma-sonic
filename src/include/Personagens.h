/**
 * @file Personagens.h
 * @author Kevin e Rebeca
 * @brief Declarações das funções dos Personagens.
 *
 * @copyright Copyright (c) 2026
 */

#pragma once

#include "Tipos.h"
#include "Sonic.h"
#include "Mario.h"

// assinatura de criação de personagem
typedef Personagem *(*FuncaoCriarPersonagem)(float x, float y);

typedef struct InfoPersonagem {
    const char *nome;
    FuncaoCriarPersonagem criar;
} InfoPersonagem;

// lista dos personagens do jogo
static InfoPersonagem personagensDisponiveis[] = {
    { "Sonic", criarSonic },
    { "Mario", criarMario },
};

static int totalPersonagens = 2;