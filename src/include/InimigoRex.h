/**
 * @file InimigoRex.h
 * @author Prof. Dr. David Buzatto
 * @brief Declarações das funções do Inimigo (Rex).
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria um novo inimigo (rex).
 */
InimigoRex *criarInimigoRex( Rectangle ret, Color cor );

/**
 * @brief Destroi um inimigo (rex).
 */
void destruirInimigoRex( InimigoRex *inimigo );

/**
 * @brief Atualiza um inimigo (rex).
 */
void atualizarInimigoRex( InimigoRex *inimigo, GameWorld *gw,float delta );

/**
 * @brief Desenha um inimigo (rex).
 */
void desenharInimigoRex( InimigoRex *inimigo );

/**
 * @brief Obtém o quadro de animação atual de um inimigo (rex).
 */
QuadroAnimacao *getQuadroAnimacaoAtualInimigoRex( InimigoRex *inimigo );