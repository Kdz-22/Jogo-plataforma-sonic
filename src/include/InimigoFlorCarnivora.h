/**
 * @file FlorCarnivora.h
 * @author Prof. Dr. David Buzatto
 * @brief Declarações das funções do Inimigo (FlorCarnivora).
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Tipos.h"

/**
 * @brief Cria um novo inimigo (florcarnivora).
 */
InimigoFlorCarnivora *criarInimigoFlorCarnivora( Rectangle ret, Color cor );

/**
 * @brief Destroi um inimigo (florcarnivora).
 */
void destruirInimigoFlorCarnivora( InimigoFlorCarnivora *inimigo );

/**
 * @brief Atualiza um inimigo (florcarnivora).
 */
void atualizarInimigoFlorCarnivora( InimigoFlorCarnivora *inimigo, GameWorld *gw,float delta );

/**
 * @brief Desenha um inimigo (florcarnivora).
 */
void desenharInimigoFlorCarnivora( InimigoFlorCarnivora *inimigo );

/**
 * @brief Obtém o quadro de animação atual de um inimigo (florcarnivora).
 */
QuadroAnimacao *getQuadroAnimacaoAtualInimigoFlorCarnivora( InimigoFlorCarnivora *inimigo );