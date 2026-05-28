/**
 * @file InimigoTonTon.h
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
InimigoTonTon *criarInimigoTonTon( Rectangle ret, Color cor );

/**
 * @brief Destroi um inimigo (spikes).
 */
void destruirInimigoTonTon( InimigoTonTon *inimigo );

/**
 * @brief Atualiza um inimigo (spikes).
 */
void atualizarInimigoTonTon( InimigoTonTon *inimigo, GameWorld *gw,float delta );

/**
 * @brief Desenha um inimigo (spikes).
 */
void desenharInimigoTonTon( InimigoTonTon *inimigo );

/**
 * @brief Obtém o quadro de animação atual de um inimigo (spikes).
 */
QuadroAnimacao *getQuadroAnimacaoAtualInimigoTonTon( InimigoTonTon *inimigo );