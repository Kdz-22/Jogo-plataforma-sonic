/**
 * @file ItemFlorPreta.h
 * @author Prof. Dr. David Buzatto
 * @brief Declarações das funções do Item (FlorPreta).
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria um novo item (florPreta).
 */
ItemFlorPreta *criarItemFlorPreta( Rectangle ret, Color cor );

/**
 * @brief Destroi um item (florPreta).
 */
void destruirItemFlorPreta( ItemFlorPreta *item );

/**
 * @brief Atualiza um item (florPreta).
 */
void atualizarItemFlorPreta( ItemFlorPreta *item, float delta );

/**
 * @brief Desenha um item (florPreta).
 */
void desenharItemFlorPreta( ItemFlorPreta *item );

/**
 * @brief Obtém o quadro de animação atual de um item (florPreta).
 */
QuadroAnimacao *getQuadroAnimacaoAtualItemFlorPreta( ItemFlorPreta *item );