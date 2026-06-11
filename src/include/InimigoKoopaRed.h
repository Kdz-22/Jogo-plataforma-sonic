/**
 * @file Obstaculo.h
 * @author Prof. Dr. David Buzatto
 * @brief Declarações das funções do Inimigo (KoopaRed).
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria um novo inimigo (koopared).
 */
InimigoKoopaRed *criarInimigoKoopaRed( Rectangle ret, Color cor );

/**
 * @brief Destroi um inimigo (koopared).
 */
void destruirInimigoKoopaRed( InimigoKoopaRed *inimigo );

/**
 * @brief Atualiza um inimigo (koopared).
 */
void atualizarInimigoKoopaRed( InimigoKoopaRed *inimigo, GameWorld *gw,float delta );

/**
 * @brief Desenha um inimigo (koopared).
 */
void desenharInimigoKoopaRed( InimigoKoopaRed *inimigo );

/**
 * @brief Obtém o quadro de animação atual de um inimigo (koopared).
 */
QuadroAnimacao *getQuadroAnimacaoAtualInimigoKoopaRed( InimigoKoopaRed *inimigo );