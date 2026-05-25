/**
 * @file Obstaculo.h
 * @author Prof. Dr. David Buzatto
 * @brief Declarações das funções do Item (AnelAzul).
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria um novo item (anelazul).
 */
ItemAnelAzul *criarItemAnelAzul( Rectangle ret, Color cor );

/**
 * @brief Destroi um item (anelazul).
 */
void destruirItemAnelAzul( ItemAnelAzul *item );

/**
 * @brief Atualiza um item (anelazul).
 */
void atualizarItemAnelAzul( ItemAnelAzul *item, float delta );

/**
 * @brief Desenha um item (anelazul).
 */
void desenharItemAnelAzul( ItemAnelAzul *item );

/**
 * @brief Obtém o quadro de animação atual de um item (anelazul).
 */
QuadroAnimacao *getQuadroAnimacaoAtualItemAnelAzul( ItemAnelAzul *item );