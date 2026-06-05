/**
 * @file GameWorld.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do GameWorld.
 *
 * @copyright Copyright (c) 2026
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "GameWorld.h"
#include "Macros.h"
#include "Mapa.h"
#include "Obstaculo.h"
#include "Tipos.h"
#include "ResourceManager.h"
#include "sonic.h"
#include "mario.h"

#include "raylib/raylib.h"
#include "raylib/raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raylib/raygui.h"       // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

static void desenharFundo( GameWorld *gw );
static void atualizarCamera( GameWorld *gw );

static void inicializar( GameWorld *gw );
static void reiniciar( GameWorld *gw );

static void desenharPontuacao( Texture2D hud, int valor, Vector2 posicao );
static void desenharTempo( Texture2D hud, int tempo, Vector2 posicao );
static void desenharTexto( Texture2D hud, const char *texto, Vector2 posicao, float escala );

/**
 * @brief Cria uma instância alocada dinamicamente da struct GameWorld.
 */
GameWorld *createGameWorld( void ) {
    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );
    inicializar( gw );

    gw->estado = ESTADO_JOGO_JOGANDO;

    return gw;
}

/**
 * @brief Destrói um objeto GameWorld e suas dependências.
 */
void destroyGameWorld( GameWorld *gw ) {
    if ( gw != NULL ) {
        destruirMapa( gw->mapa );
        gw->personagem->funcoes->destruir( gw->personagem->dados );
        free( gw->personagem );
        free( gw );
    }
}

/**
 * @brief Lê a entrada do usuário e atualiza o estado do jogo.
 */
void updateGameWorld( GameWorld *gw, float delta ) {

    if ( !IsMusicStreamPlaying( rm.musicaFase01 ) ) {
        PlayMusicStream( rm.musicaFase01 );
    } else {
        UpdateMusicStream( rm.musicaFase01 );
    }
    if ( IsMusicStreamPlaying( rm.musicaFase01 ) ) {

        if ( IsKeyDown(KEY_MINUS) || IsKeyDown(KEY_KP_SUBTRACT) ) {
            rm.volumeMusicaFase01 = Clamp( rm.volumeMusicaFase01 - 0.01f, 0.0f, 1.0f );
            SetMusicVolume( rm.musicaFase01, rm.volumeMusicaFase01 );
        } else if ( IsKeyDown(KEY_EQUAL) || IsKeyDown(KEY_KP_ADD) ) {
            rm.volumeMusicaFase01 = Clamp( rm.volumeMusicaFase01 + 0.01f, 0.0f, 1.0f );
            SetMusicVolume( rm.musicaFase01, rm.volumeMusicaFase01 );
        }

        
    }

    if( gw->estado == ESTADO_JOGO_JOGANDO ) {
        if ( IsKeyPressed( KEY_R ) ) {
                reiniciar( gw );
                return;
            }

        
        atualizarMapa( gw->mapa, gw, delta );
        gw->personagem->funcoes->entrada( gw->personagem->dados, gw->personagem, delta );
        gw->personagem->funcoes->atualizar( gw->personagem->dados, gw->personagem, gw, delta );

        gw->personagem->time += delta;

        atualizarCamera( gw );

        //GAMEOVER
        if( gw->personagem->quantidadeVidas < 0 ) {
            gw->estado = ESTADO_JOGO_GAMEOVER;
        }
    } else if( gw->estado == ESTADO_JOGO_GAMEOVER ) {
        
        //Se GAMEOVER apertar R pp/reiniciar
        if ( IsKeyPressed( KEY_R ) ) {
            reiniciar( gw );
            
            //reset do jogo
            gw->personagem->quantidadeVidas = 3;
            gw->personagem->quantidadeAneis = 0;
            gw->personagem->score = 0;
            gw->personagem->comboAereo = 0;
            gw->personagem->time = 0.0f;
            
            gw->personagem->funcoes->resetar( gw->personagem->dados );
            
            gw->estado = ESTADO_JOGO_JOGANDO;
        }
    }
}

/**
 * @brief Desenha o estado do jogo.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();

    
    
    if( gw->estado == ESTADO_JOGO_JOGANDO ) {
        ClearBackground( (Color) { 36, 0, 180, 255 } );
        
        BeginMode2D( gw->camera );
        desenharFundo( gw );
        desenharMapa( gw->mapa );
        gw->personagem->funcoes->desenhar( gw->personagem->dados );
        EndMode2D();
        
        //DrawText( "VOL", 160, 48, 16, WHITE );
        desenharPontuacao( rm.texturaHUD, (int)(rm.volumeMusicaFase01 * 100), (Vector2){ 700, 45 } );  
        desenharTexto( rm.texturaHUD2, "VOL", (Vector2){ 660, 48 }, 2.0f );      
        
        //Score
        DrawTexturePro(
            rm.texturaHUD,
            (Rectangle) { 24, 432, 40, 16 },
            (Rectangle) { 20, 15, 80, 32 },
            (Vector2) { 0, 0 },
            0.0f,
            WHITE
        );
        //Time
        DrawTexturePro(
            rm.texturaHUD,
            (Rectangle) { 24, 456, 32, 16 },
            (Rectangle) { 20, 45, 64, 32 },
            (Vector2) { 0, 0 },
            0.0f,
            WHITE
        );
        //Rings
        DrawTexturePro(
            rm.texturaHUD,
            (Rectangle) { 24, 480, 40, 16 },
            (Rectangle) { 20, 75, 80, 32 },
            (Vector2) { 0, 0 },
            0.0f,
            WHITE
        );

        Vector2 posVidas = { 20, 380 }; // muda só aqui para mover tudo junto

        // Life (fotinha do Sonic)
        DrawTexturePro(
            rm.texturaHUD,
            (Rectangle) { 40, 400, 16, 16 },
            (Rectangle) { posVidas.x, posVidas.y, 32, 32 },
            (Vector2) { 0, 0 }, 0.0f, WHITE
        );
        // Texto "Sonic"
        DrawTexturePro(
            rm.texturaHUD,
            (Rectangle) { 57, 401, 31, 7 },
            (Rectangle) { posVidas.x + 40, posVidas.y, 80, 16 },
            (Vector2) { 0, 0 }, 0.0f, WHITE
        );
        // Multiplicador "X"
        DrawTexturePro(
            rm.texturaHUD,
            (Rectangle) { 62, 410, 8, 8 },
            (Rectangle) { posVidas.x + 36, posVidas.y + 18, 16, 16 },
            (Vector2) { 0, 0 }, 0.0f, WHITE
        );
        // Número de vidas
        desenharPontuacao( rm.texturaHUD, gw->personagem->quantidadeVidas, 
            (Vector2){ posVidas.x + 50, posVidas.y + 16 } );

        desenharPontuacao(rm.texturaHUD, gw->personagem->score, (Vector2){ 110, 15 });
        desenharTempo(rm.texturaHUD, (int)gw->personagem->time, (Vector2){ 94, 45 });
        desenharPontuacao(rm.texturaHUD, gw->personagem->quantidadeAneis, (Vector2){ 110, 75 });
        
        //DrawFPS( 700, 15 );
    } else if ( gw->estado == ESTADO_JOGO_GAMEOVER ) {
        
        ClearBackground( BLACK );

        int larguraTela = GetScreenWidth();
        int alturaTela = GetScreenHeight();

        int tamTexto1 = MeasureText( "GAME OVER", 40 );
        DrawText( "GAME OVER", larguraTela / 2 - tamTexto1 / 2, alturaTela / 2 - 40, 40, RED );
        
        int tamTexto2 = MeasureText( "PRESSIONE 'R' PARA REINICIAR", 20 );
        DrawText( "PRESSIONE 'R' PARA REINICIAR", larguraTela / 2 - tamTexto2 / 2, alturaTela / 2 + 20, 20, LIGHTGRAY );
    }
    EndDrawing();
}

static void desenharFundo( GameWorld *gw ) {

    int larguraFundo = rm.texturaFundo.width;
    int larguraMapa = calcularLarguraMapa( gw->mapa );
    int alturaMapa = calcularAlturaMapa( gw->mapa );
    int repeticoes = larguraMapa / larguraFundo;

    int deslocamentoParallax = (int) ( ( gw->camera.target.x / (float) larguraMapa ) * 200 );

    for ( int i = 0; i <= repeticoes; i++ ) {
        DrawTexture( rm.texturaFundo, larguraFundo * i - deslocamentoParallax, alturaMapa - rm.texturaFundo.height, WHITE );
    }

}

static void atualizarCamera( GameWorld *gw ) {

    Jogador *j = (Jogador*) gw->personagem->dados;
    Camera2D *c = &gw->camera;

    c->offset.x = GetScreenWidth() / 2;
    c->offset.y = GetScreenHeight() / 2;

    // O target é arredondado para o inteiro mais próximo para garantir que a
    // translação da câmera ocorra sempre em posições inteiras de pixel. Sem esse
    // arredondamento, o valor float contínuo de ret.x faz os tiles serem
    // renderizados em posições subpixel, causando frestas visíveis entre eles.
    c->target.x = roundf( j->ret.x + j->ret.width / 2.0f );
    c->target.y = roundf( j->ret.y + j->ret.height / 2.0f );

    int minX = GetScreenWidth() / 2;
    int maxX = calcularLarguraMapa( gw->mapa ) - GetScreenWidth() / 2;
    int maxY = calcularAlturaMapa( gw->mapa ) - GetScreenHeight() / 2;

    if ( c->target.x < minX ) {
        c->target.x = minX;
    } else if ( c->target.x > maxX ) {
        c->target.x = maxX;
    }

    if ( c->target.y > maxY ) {
        c->target.y = maxY;
    }

}

static void inicializar( GameWorld *gw ) {

    //gw->mapa = carregarMapa( "resources/mapas/mapaTeste.txt" );
    gw->mapa = carregarMapa( "resources/mapas/mapa01.txt" );
    gw->personagem = criarMario( GetScreenWidth() / 2 + 144, calcularAlturaMapa( gw->mapa ) - 196, 96, 96 );

    gw->camera = (Camera2D) {
        .offset = { 0 },    // deslocamento relativo da câmera em relação ao alvo
        .target = { 0 },    // o alvo da câmera, ou seja, a coordenada em que ela está centralizada
        .rotation = 0.0f,   // rotação da câmera em graus. o pivô é o alvo.
        .zoom = 1.0f        // zoom da câmera. 1.0f significa sem escala
    };

    gw->gravidade = 900;

}

static void reiniciar( GameWorld *gw ) {

    destruirMapa( gw->mapa );
    gw->personagem->funcoes->destruir( gw->personagem->dados );

    if ( IsMusicStreamPlaying( rm.musicaFase01 ) ) {
        StopMusicStream( rm.musicaFase01 );
    }

    inicializar( gw );

}

static void desenharPontuacao( Texture2D hud, int valor, Vector2 posicao ) {

    char buffer[16];
    sprintf( buffer, "%d", valor );

    int digitoW = 8;
    int digitoH = 16;
    int inicioY = 432;
    int espacamento = 8;

    for (int i = 0; buffer[i] != '\0'; i++) {
        int digito = buffer[i] - '0';

        Rectangle origem = {
            (digito * (digitoW + espacamento) ) + 72,
            inicioY,             
            digitoW,
            digitoH
        };

        Rectangle dest = {
            posicao.x + i * (digitoW * 2),
            posicao.y,
            digitoW * 2,
            digitoH * 2
        };

        DrawTexturePro(hud, origem, dest, (Vector2){0, 0}, 0.0f, WHITE);
    }
}

static void desenharTempo( Texture2D hud, int tempo, Vector2 posicao ) {

    int minutos = tempo / 60;
    int segundos = tempo % 60;

    char buffer[16];
    sprintf( buffer, "%d:%02d", minutos, segundos );
    //printf("%s\n", buffer);

    int digitoW = 8;
    int digitoH = 16;
    int inicioY = 432;
    int espacamento = 8;

    for( int i = 0; buffer[i] != '\0'; i++ ) {

        if( buffer[i] == ':' ) {
            DrawRectangle(
                posicao.x + i * (digitoW * 2) + 6,
                posicao.y + 8,
                4,
                4,
                WHITE
            );

            DrawRectangle(
                posicao.x + i * (digitoW * 2) + 6,
                posicao.y + 20,
                4,
                4,
                WHITE
            );

            continue;

        }

        int digito = buffer[i] - '0';

        Rectangle origem = {
            (digito * (digitoW + espacamento)) + 72,
            inicioY,
            digitoW,
            digitoH
        };

        Rectangle dest = {
            posicao.x + i * (digitoW * 2),
            posicao.y,
            digitoW * 2,
            digitoH * 2
        };

        DrawTexturePro(
            hud,
            origem,
            dest,
            (Vector2){0, 0},
            0.0f,
            WHITE
        );

    }

}

static void desenharTexto( Texture2D hud, const char *texto, Vector2 posicao, float escala ) {

    int charW  = 7;
    int charH  = 11;
    int inicioX = 9;
    int inicioY = 82;
    int espacamento = 2;

    for ( int i = 0; texto[i] != '\0'; i++ ) {

        char c = texto[i];
        int idx = -1;

        if ( c >= 'A' && c <= 'Z' ) {
            idx = c - 'A';
        } else if ( c == ' ' ) {
            continue;
        }

        if ( idx < 0 ) {
            continue;
        }
        //(digito * (digitoW + espacamento) ) + 72,
        Rectangle origem = { 
            inicioX + (idx * charW),
            inicioY,
            charW,
            charH
        };

        Rectangle dest = {
            posicao.x + i * (charW * escala),
            posicao.y,
            charW * escala,
            charH * escala
        };

        DrawTexturePro( hud, origem, dest, (Vector2){0,0}, 0.0f, WHITE );
    }
}