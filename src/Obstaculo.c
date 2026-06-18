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

    novoObstaculo->eBlocoGiratorio = false;
    novoObstaculo->quebrando = false;
    novoObstaculo->quebrado = false;
    novoObstaculo->quadroQuebra = 0;
    novoObstaculo->tempoQuadroQuebra = 0.0f;

    novoObstaculo->eCanoSaida = false;
    novoObstaculo->proximaFase = NULL;

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
void desenharObstaculo(Obstaculo *o) {

    if (o->quebrado) {
        return; // não desenha nada
    }

    if (o->textura == NULL) {
        DrawRectangleRec(o->ret, o->cor);
        DrawRectangleLines(o->ret.x, o->ret.y, o->ret.width, o->ret.height, BLACK);
        return;
    }

    Rectangle fonteAtual = o->fonte;

    if (o->quebrando && o->eBlocoGiratorio) {
        // cada quadro de quebra está ao lado do frame original na spritesheet
        // ajusta o x conforme o quadro atual da animação
        fonteAtual.x = o->fonte.x + (o->quadroQuebra * (o->fonte.width + 8));
    }

    DrawTexturePro(
        *o->textura,
        fonteAtual,
        o->ret,
        (Vector2){0},
        0.0f,
        WHITE);
}

// Obstaculo.c
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