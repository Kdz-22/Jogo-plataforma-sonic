/**
 * @file InimigoSpikes.h
 * @author Prof. Dr. David Buzatto
 * @brief Declarações das funções do Inimigo (Spikes).
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria um novo inimigo (spikes).
 */
InimigoSpikes *criarInimigoSpikes( Rectangle ret, Color cor );

/**
 * @brief Destroi um inimigo (spikes).
 */
void destruirInimigoSpikes( InimigoSpikes *inimigo );

/**
 * @brief Atualiza um inimigo (spikes).
 */
void atualizarInimigoSpikes( InimigoSpikes *inimigo, GameWorld *gw,float delta );

/**
 * @brief Desenha um inimigo (spikes).
 */
void desenharInimigoSpikes( InimigoSpikes *inimigo );

/**
 * @brief Obtém o quadro de animação atual de um inimigo (spikes).
 */
QuadroAnimacao *getQuadroAnimacaoAtualInimigoSpikes( InimigoSpikes *inimigo );