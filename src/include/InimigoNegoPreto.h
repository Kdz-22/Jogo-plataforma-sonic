/**
 * @file NegoPreto.h
 * @author Prof. Dr. David Buzatto
 * @brief Declarações das funções do Inimigo (NegoPreto).
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria um novo inimigo (negopreto).
 */
InimigoNegoPreto *criarInimigoNegoPreto( Rectangle ret, Color cor );

/**
 * @brief Destroi um inimigo (negopreto).
 */
void destruirInimigoNegoPreto( InimigoNegoPreto *inimigo );

/**
 * @brief Atualiza um inimigo (negopreto).
 */
void atualizarInimigoNegoPreto( InimigoNegoPreto *inimigo, GameWorld *gw,float delta );

/**
 * @brief Desenha um inimigo (negopreto).
 */
void desenharInimigoNegoPreto( InimigoNegoPreto *inimigo );

/**
 * @brief Obtém o quadro de animação atual de um inimigo (negopreto).
 */
QuadroAnimacao *getQuadroAnimacaoAtualInimigoNegoPreto( InimigoNegoPreto *inimigo );