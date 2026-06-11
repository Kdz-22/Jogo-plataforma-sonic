/**
 * @file Obstaculo.h
 * @author Prof. Dr. David Buzatto
 * @brief Declarações das funções do Item (Anel).
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria um novo item (anel).
 */
ItemCogumeloVermelho *criarItemCogumeloVermelho( Rectangle ret, Color cor );

/**
 * @brief Destroi um item (anel).
 */
void destruirItemCogumeloVermelho( ItemCogumeloVermelho *item );

/**
 * @brief Atualiza um item (anel).
 */
void atualizarItemCogumeloVermelho( ItemCogumeloVermelho *item, float delta );

/**
 * @brief Desenha um item (anel).
 */
void desenharItemCogumeloVermelho( ItemCogumeloVermelho *item );

/**
 * @brief Obtém o quadro de animação atual de um item (anel).
 */
QuadroAnimacao *getQuadroAnimacaoAtualItemCogumeloVermelho( ItemCogumeloVermelho *item );