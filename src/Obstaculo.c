/**
 * @file Obstaculo.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Obstáculo.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "Obstaculo.h"
#include "Tipos.h"

/**
 * @brief Cria um novo obstáculo.
 */
Obstaculo *criarObstaculo( Rectangle ret, Color cor, Rectangle fonte, Texture2D *textura, bool solido ) {

    Obstaculo *novoObstaculo = (Obstaculo*) malloc( sizeof( Obstaculo ) );

    novoObstaculo->ret = ret;
    novoObstaculo->cor = cor;
    novoObstaculo->fonte = fonte;
    novoObstaculo->textura = textura;
    novoObstaculo->solido = solido;

    return novoObstaculo;

}

/**
 * @brief Destroi um obstáculo.
 */
void destruirObstaculo( Obstaculo *o ) {
    free( o );
}

/**
 * @brief Desenha um obstáculo.
 */
void desenharObstaculo( Obstaculo *o ) {

    if ( o->textura == NULL ) {
        DrawRectangleRec( o->ret, o->cor );
        DrawRectangleLines( o->ret.x, o->ret.y, o->ret.width, o->ret.height, BLACK );
        return;
    }

    DrawTexturePro(
        *o->textura, 
        o->fonte,
        o->ret,
        (Vector2) { 0 },
        0.0f,
        WHITE
    );

}

void atualizarObstaculo( Obstaculo *o, float delta ) {

    if ( o->quebrando && !o->quebrado ) {

        const float duracaoQuadro = 0.08f; // 80ms por quadro
        o->tempoQuadroQuebra += delta;

        if ( o->tempoQuadroQuebra >= duracaoQuadro ) {
            o->tempoQuadroQuebra = 0.0f;
            o->quadroQuebra++;

            if ( o->quadroQuebra >= 3 ) { // 3 frames após o "intacto" na sua spritesheet
                o->quebrado = true;
            }
        }
    }
}