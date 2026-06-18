/**
 * @file Obstaculo.h
 * @author Prof. Dr. David Buzatto
 * @brief Declarações das funções do Item (FlorPreta).
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria um novo item (florpreta).
 */
ItemFlorPreta *criarItemFlorPreta( Rectangle ret, Color cor );

/**
 * @brief Destroi um item (florpreta).
 */
void destruirItemFlorPreta( ItemFlorPreta *item );

/**
 * @brief Atualiza um item (florpreta).
 */
void atualizarItemFlorPreta( ItemFlorPreta *item, float delta );

/**
 * @brief Desenha um item (florpreta).
 */
void desenharItemFlorPreta( ItemFlorPreta *item );

/**
 * @brief Obtém o quadro de animação atual de um item (florpreta).
 */
QuadroAnimacao *getQuadroAnimacaoAtualItemFlorPreta( ItemFlorPreta *item );