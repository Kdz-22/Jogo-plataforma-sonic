/**
 * @file Inimigo.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Inimigo (TonTon).
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoTonTon.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void desenharQuadroAnimacaoInimigoTonTon( InimigoTonTon *inimigo, QuadroAnimacao *qa, Color tonalidade );
static void desenharQuadroAnimacaoInimigoTonTonMorrendo( InimigoTonTon *inimigo, QuadroAnimacao *qa, float escala, Color tonalidade );
static void desenharBolaTonTon( BolaTonTon *bola ); 
static Animacao *getAnimacaoAtualInimigoTonTon( InimigoTonTon *inimigo );

static const bool MOSTRAR_RETANGULOS = true;

/**
 * @brief Cria um novo Inimigo (tonton).
 */
InimigoTonTon *criarInimigoTonTon( Rectangle ret, Color cor ) {

    InimigoTonTon *novoInimigo = (InimigoTonTon*) malloc( sizeof( InimigoTonTon ) );

    novoInimigo->ret = ret;
    novoInimigo->vel = (Vector2) { 0 };
    novoInimigo->cor = cor;

    novoInimigo->velAndando = 100;
    novoInimigo->velMaxQueda = 600;

    novoInimigo->contadorTiroBola = 0.0f;

    // Inicializar as 5 bolas
    for ( int i = 0; i < 5; i++ ) {
        novoInimigo->bola[i].ativo = false;
        novoInimigo->bola[i].ret = (Rectangle){ 0 };
        novoInimigo->bola[i].vel = (Vector2){ 0 };
        
        novoInimigo->bola[i].animacao.quantidadeQuadros = 2;
        novoInimigo->bola[i].animacao.quadroAtual = 0;
        novoInimigo->bola[i].animacao.contadorTempoQuadro = 0.0f;
        novoInimigo->bola[i].animacao.pararNoUltimoQuadro = false;
        novoInimigo->bola[i].animacao.executarUmaVez = false;
        novoInimigo->bola[i].animacao.finalizada = false;
        
        criarQuadrosAnimacao( &novoInimigo->bola[i].animacao, novoInimigo->bola[i].animacao.quantidadeQuadros );
        inicializarQuadrosAnimacao( 
            novoInimigo->bola[i].animacao.quadros,
            novoInimigo->bola[i].animacao.quantidadeQuadros,
            150,            // duração de cada quadro
            101, 72,        // posição inicial na spritesheet
            16, 16,         // dimensões da bola
            1,              // separação
            false,          // de trás para frente
            (Rectangle) { 0, 0, 16, 16 }  // retângulo de colisão
        );
    }

    novoInimigo->estado = ESTADO_INIMIGO_TONTON_ANDANDO;
    novoInimigo->ativo = true;
    novoInimigo->olhandoParaDireita = false;

    int quantidadeAnimacoes = 0;

    novoInimigo->animacaoAndando.quantidadeQuadros = 3;
    novoInimigo->animacaoAndando.quadroAtual = 0;
    novoInimigo->animacaoAndando.contadorTempoQuadro = 0.0f;
    novoInimigo->animacaoAndando.pararNoUltimoQuadro = false;
    novoInimigo->animacaoAndando.executarUmaVez = false;
    novoInimigo->animacaoAndando.finalizada = false;
    criarQuadrosAnimacao( &novoInimigo->animacaoAndando, novoInimigo->animacaoAndando.quantidadeQuadros );
    inicializarQuadrosAnimacao( 
        novoInimigo->animacaoAndando.quadros,
        novoInimigo->animacaoAndando.quantidadeQuadros,
        250,             // duração padrão para todos os quadros
        1, 72,           // início
        24, 40,          // dimensões
        1,               // separação
        false,           // de trás para frente
        (Rectangle) {    // retângulo de colisão padrão para cada quadro
            10, 20, 60, 40
        }
    );

    novoInimigo->animacaoMorrendo.quantidadeQuadros = 4;
    novoInimigo->animacaoMorrendo.quadroAtual = 0;
    novoInimigo->animacaoMorrendo.contadorTempoQuadro = 0.0f;
    novoInimigo->animacaoMorrendo.pararNoUltimoQuadro = false;
    novoInimigo->animacaoMorrendo.executarUmaVez = true;
    novoInimigo->animacaoMorrendo.finalizada = false;
    criarQuadrosAnimacao( &novoInimigo->animacaoMorrendo, novoInimigo->animacaoMorrendo.quantidadeQuadros );
    inicializarQuadrosAnimacao( 
        novoInimigo->animacaoMorrendo.quadros,
        novoInimigo->animacaoMorrendo.quantidadeQuadros,
        100,              // duração padrão para todos os quadros
        169, 1,           // início
        32, 32,           // dimensões
        1,                // separação
        false,            // de trás para frente
        (Rectangle) { 0 } // retângulo de colisão padrão para cada quadro
    );

    novoInimigo->animacoes[ESTADO_INIMIGO_TONTON_ANDANDO] = &novoInimigo->animacaoAndando; quantidadeAnimacoes++;
    novoInimigo->animacoes[ESTADO_INIMIGO_TONTON_MORRENDO] = &novoInimigo->animacaoMorrendo; quantidadeAnimacoes++;
    novoInimigo->quantidadeAnimacoes = quantidadeAnimacoes;

    return novoInimigo;

}

/**
 * @brief Destroi um inimigo (tonton).
 */
void destruirInimigoTonTon( InimigoTonTon *inimigo ) {
    if ( inimigo != NULL ) {
        for ( int i = 0; i < inimigo->quantidadeAnimacoes; i++ ) {
            destruirQuadrosAnimacao( inimigo->animacoes[i] );
        }

        // Destroi as bolas
        for ( int i = 0; i < 5; i++ ) {
            destruirQuadrosAnimacao( &inimigo->bola[i].animacao );
        }
        free( inimigo );
    }
}

/**
 * @brief Atualiza um inimigo (tonton).
 */
void atualizarInimigoTonTon( InimigoTonTon *inimigo, GameWorld *gw, float delta ) {

    if ( inimigo->ativo ) {

        if ( inimigo->estado == ESTADO_INIMIGO_TONTON_ANDANDO ) {

            Animacao *animacaoAtual = getAnimacaoAtualInimigoTonTon( inimigo );
            atualizarAnimacao( animacaoAtual, delta );

            // Incrementar contador de tiro de bola
            inimigo->contadorTiroBola += delta;

            // Lógica para disparar bola
            if ( inimigo->contadorTiroBola >= 2.0f ) {
                printf( "TONTON ATIROU!\n" );
                
                // Encontrar uma bola inativa para disparar
                for ( int i = 0; i < 5; i++ ) {
                    if ( !inimigo->bola[i].ativo ) {
                        // Criar bola na posição do TonTon
                        inimigo->bola[i].ret = (Rectangle){
                            inimigo->ret.x + inimigo->ret.width / 2 - 8,
                            inimigo->ret.y + inimigo->ret.height / 2 - 8,
                            16,  // largura da bola
                            16   // altura da bola
                        };
                        
                        // Velocidade da bola
                        if ( inimigo->olhandoParaDireita ) {
                            inimigo->bola[i].vel = (Vector2){ 200.0f, 0.0f };  // para direita
                        } else {
                            inimigo->bola[i].vel = (Vector2){ -200.0f, 0.0f }; // para esquerda
                        }
                        
                        // reset da animação
                        inimigo->bola[i].animacao.quadroAtual = 0;
                        inimigo->bola[i].animacao.contadorTempoQuadro = 0.0f;
                        inimigo->bola[i].animacao.finalizada = false;
                        
                        inimigo->bola[i].ativo = true;
                        break;
                    }
                }
                
                inimigo->contadorTiroBola = 0.0f;  // reset
            }

            // atualiza bolas ativas
            for ( int i = 0; i < 5; i++ ) {
                if ( inimigo->bola[i].ativo ) {
                    
                    atualizarAnimacao( &inimigo->bola[i].animacao, delta );
                    
                    inimigo->bola[i].vel.y += gw->gravidade * delta;

                    // velocidade de queda
                    if ( inimigo->bola[i].vel.y > 600.0f ) {
                        inimigo->bola[i].vel.y = 600.0f;
                    }

                    // atualiza posição
                    inimigo->bola[i].ret.x += inimigo->bola[i].vel.x * delta;
                    inimigo->bola[i].ret.y += inimigo->bola[i].vel.y * delta;

                    // bola desativada se passar do limite da tela
                    if ( inimigo->bola[i].ret.y > gw->mapa->linhas * gw->mapa->dimensaoPadraoElementos ) {
                        inimigo->bola[i].ativo = false;
                    }
                }
            }

            Inimigo ini = {
                .objeto = inimigo,
                .tipo = TIPO_INIMIGO_TONTON
            };

            if ( inimigo->olhandoParaDireita ) {
                inimigo->vel.x = inimigo->velAndando;
            } else {
                inimigo->vel.x = -inimigo->velAndando;
            }

            // fase X
            inimigo->ret.x += inimigo->vel.x * delta;
            resolverColisaoInimigoObstaculosMapaX( &ini, gw->mapa );

            inimigo->vel.y += gw->gravidade * delta;
            if ( inimigo->vel.y > inimigo->velMaxQueda ) {
                inimigo->vel.y = inimigo->velMaxQueda;
            }

            // fase Y
            inimigo->ret.y += inimigo->vel.y * delta;
            resolverColisaoInimigoObstaculosMapaY( &ini, gw->mapa );

        } else if ( inimigo->estado == ESTADO_INIMIGO_TONTON_MORRENDO ) {

            atualizarAnimacao( &inimigo->animacaoMorrendo, delta );

            if ( inimigo->animacaoMorrendo.finalizada ) {
                inimigo->ativo = false;
            }

        }

    }

}

/**
 * @brief Desenha um inimigo (tonton).
 */
void desenharInimigoTonTon( InimigoTonTon *inimigo ) {

    if ( inimigo->ativo ) {

        if ( inimigo->estado == ESTADO_INIMIGO_TONTON_ANDANDO ) {
            QuadroAnimacao *qa = getQuadroAnimacaoAtualInimigoTonTon( inimigo );
            desenharQuadroAnimacaoInimigoTonTon( inimigo, qa, WHITE );
        } else if ( inimigo->estado == ESTADO_INIMIGO_TONTON_MORRENDO ) {
            desenharQuadroAnimacaoInimigoTonTonMorrendo( inimigo, getQuadroAtualAnimacao( &inimigo->animacaoMorrendo ), 2.0f, WHITE );
        }

        // Desenhar as bolas ativas
        for ( int i = 0; i < 5; i++ ) {
            if ( inimigo->bola[i].ativo ) {
                desenharBolaTonTon( &inimigo->bola[i] );
            }
        }

        if ( MOSTRAR_RETANGULOS ) {
            DrawRectangleRec( inimigo->ret, Fade( inimigo->cor, 0.5f ) );
            DrawRectangleLines( inimigo->ret.x, inimigo->ret.y, inimigo->ret.width, inimigo->ret.height, BLACK );
        }

    }

}

/**
 * @brief Obtém o quadro de animação atual de um inimigo (tonton).
 */
QuadroAnimacao *getQuadroAnimacaoAtualInimigoTonTon( InimigoTonTon *inimigo ) {
    return getQuadroAtualAnimacao( getAnimacaoAtualInimigoTonTon( inimigo ) );
}

static void desenharQuadroAnimacaoInimigoTonTon( InimigoTonTon *inimigo, QuadroAnimacao *qa, Color tonalidade ) {

    if ( qa != NULL ) {
        
        DrawTexturePro(
            rm.texturaBadniks,
            (Rectangle) {
                qa->fonte.x,
                qa->fonte.y,
                inimigo->olhandoParaDireita ? -qa->fonte.width : qa->fonte.width,
                qa->fonte.height
            },
            inimigo->ret,
            (Vector2) { 0 },
            0.0f,
            tonalidade
        );

        if ( MOSTRAR_RETANGULOS ) {
            float xDesenho = inimigo->olhandoParaDireita
                ? inimigo->ret.x + inimigo->ret.width - qa->retColisao.x - qa->retColisao.width
                : inimigo->ret.x + qa->retColisao.x;
            float yDesenho = inimigo->ret.y + qa->retColisao.y;
            DrawRectangle( xDesenho, yDesenho, qa->retColisao.width, qa->retColisao.height, Fade( GREEN, 0.5f ) );
        }

    }

}

static void desenharQuadroAnimacaoInimigoTonTonMorrendo( InimigoTonTon *inimigo, QuadroAnimacao *qa, float escala, Color tonalidade ) {

    if ( qa != NULL ) {
        
        DrawTexturePro(
            rm.texturaBadniks,
            qa->fonte,
            (Rectangle) {
                inimigo->ret.x,
                inimigo->ret.y,
                qa->fonte.width * escala,
                qa->fonte.height * escala
            },
            (Vector2) { 0 },
            0.0f,
            tonalidade
        );

    }

}

/**
 * @brief Desenha uma bola do TonTon com animação.
 */
static void desenharBolaTonTon( BolaTonTon *bola ) {
    
    if ( bola != NULL ) {
        desenharComAnimacao( &bola->animacao, rm.texturaBadniks, bola->ret, WHITE );
        
        if ( MOSTRAR_RETANGULOS ) {
            DrawRectangleLines( bola->ret.x, bola->ret.y, bola->ret.width, bola->ret.height, RED );
        }
    }
}

static Animacao *getAnimacaoAtualInimigoTonTon( InimigoTonTon *inimigo ) {
    return inimigo->animacoes[inimigo->estado];
}