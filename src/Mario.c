/**
 * @file Mario.c
 * @author Kevin e Rebeca
 * @brief Mario implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib/raylib.h"
#include "raylib/raymath.h"

#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoKoopaRed.h"
#include "InimigoMotobug.h"
#include "InimigoNegoPreto.h"
#include "InimigoRex.h"
#include "InimigoSpikes.h"
#include "InimigoFlorCarnivora.h"

#include "Item.h"
#include "ItemAnel.h"
#include "ItemAnelAzul.h"
#include "ItemCoguVerm.h"
#include "ItemFlorPreta.h"

#include "Jogador.h"
#include "Macros.h"
#include "Mario.h"
#include "Obstaculo.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void desenharQuadroAnimacaoMario(Mario *m, QuadroAnimacao *qa,
                                        Color tonalidade);
static QuadroAnimacao *getQuadroAnimacaoAtualMario(Mario *m);
static Animacao *getAnimacaoAtualMario(Mario *m);

static void resolverColisaoMarioObstaculosMapaX(Mario *m, Mapa *mapa);
static void resolverColisaoMarioObstaculosMapaY(Mario *m, Mapa *mapa);

static void resolverColisaoMarioItensMapa(Mario *m, Personagem *p, Mapa *mapa);
static void resolverColisaoMarioInimigosMapa(Mario *m, Personagem *p,
                                             Mapa *mapa);
static void resolverEntradaCanoMapa(Mario *m, Personagem *p, GameWorld *gw);

static const bool MOSTRAR_RETANGULOS = true;
static PersonagemFuncoes marioFuncoes;

Personagem *criarMario(float x, float y, float w, float h)
{

    Personagem *p = (Personagem *)malloc(sizeof(Personagem));

    Mario *m = (Mario *)malloc(sizeof(Mario));

    m->ret.x = x;
    m->ret.y = y;
    m->ret.width = w;
    m->ret.height = h;
    m->retOriginal.x = x;
    m->retOriginal.y = y;
    m->retOriginal.width = w;
    m->retOriginal.height = h;
    m->vel = (Vector2){0};

    m->cor = BLUE;

    m->velAndando = 200;
    m->velCorrendo = 500;
    m->velPulo = -700;
    m->velMaxQueda = 600;

    m->frenagem = 1800;
    m->aceleracao = 500;
    m->desaceleracao = 500;

    m->quantidadePulos = 0;
    m->quantidadeMaxPulos = 2;

    m->grande = false;
    m->pulandoGirando = false;
    m->invulneravel = false;
    m->tempoInvulnerabilidade = 3.0f;
    m->contadorTempoInvulnerabilidade = 0.0f;
    p->temCogumeloReserva = false;

    m->piscaPisca = false;
    m->tempoPiscaPisca = 0.05f;
    m->contadorTempoPiscaPisca = 0.0f;

    m->freando = false;

    m->estado = ESTADO_MARIO_PARADO;
    m->olhandoParaDireita = false;

    int quantidadeAnimacoes = 0;

    m->animacaoParado.quantidadeQuadros = 1;
    m->animacaoParado.quadroAtual = 0;
    m->animacaoParado.contadorTempoQuadro = 0.0f;
    m->animacaoParado.pararNoUltimoQuadro = false;
    m->animacaoParado.executarUmaVez = false;
    m->animacaoParado.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoParado,
                         m->animacaoParado.quantidadeQuadros); // OK
    inicializarQuadrosAnimacao(
        m->animacaoParado.quadros, m->animacaoParado.quantidadeQuadros,
        1000,   // duração padrão para todos os quadros
        8, 33,  // início
        48, 48, // dimensões
        4,      // separação
        true,   // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            30, 28, 40, 50
            // 18, 20, 54, 76
        });

    m->animacaoAndando.quantidadeQuadros = 2;
    m->animacaoAndando.quadroAtual = 0;
    m->animacaoAndando.contadorTempoQuadro = 0.0f;
    m->animacaoAndando.pararNoUltimoQuadro = false;
    m->animacaoAndando.executarUmaVez = false;
    m->animacaoAndando.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoAndando,
                         m->animacaoAndando.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoAndando.quadros, m->animacaoAndando.quantidadeQuadros,
        150,     // duração padrão para cada quadro
        216, 33, // início
        48, 48,  // dimensões
        4,       // separação
        false,   // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            30, 28, 40, 50
            // 18, 20, 54, 76
        });

    m->animacaoCorrendo.quantidadeQuadros = 2;
    m->animacaoCorrendo.quadroAtual = 0;
    m->animacaoCorrendo.contadorTempoQuadro = 0.0f;
    m->animacaoCorrendo.pararNoUltimoQuadro = false;
    m->animacaoCorrendo.executarUmaVez = false;
    m->animacaoCorrendo.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoCorrendo,
                         m->animacaoCorrendo.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoCorrendo.quadros, m->animacaoCorrendo.quantidadeQuadros,
        50,      // duração padrão para cada quadro
        372, 33, // início
        48, 48,  // dimensões
        4,       // separação
        false,   // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            30, 28, 40, 50
            // 18, 20, 54, 76
        });

    m->animacaoPulando.quantidadeQuadros = 1;
    m->animacaoPulando.quadroAtual = 0;
    m->animacaoPulando.contadorTempoQuadro = 0.0f;
    m->animacaoPulando.pararNoUltimoQuadro = false;
    m->animacaoPulando.executarUmaVez = false;
    m->animacaoPulando.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoPulando,
                         m->animacaoPulando.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoPulando.quadros, m->animacaoPulando.quantidadeQuadros,
        40,     // duração padrão para cada quadro
        60, 97, // início
        48, 48, // dimensões
        4,      // separação
        false,  // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            30, 35, 40, 50
            // 18, 36, 60, 60
        });

    m->animacaoPulandoCorrendo.quantidadeQuadros = 1;
    m->animacaoPulandoCorrendo.quadroAtual = 0;
    m->animacaoPulandoCorrendo.contadorTempoQuadro = 0.0f;
    m->animacaoPulandoCorrendo.pararNoUltimoQuadro = false;
    m->animacaoPulandoCorrendo.executarUmaVez = false;
    m->animacaoPulandoCorrendo.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoPulandoCorrendo,
                         m->animacaoPulandoCorrendo.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoPulandoCorrendo.quadros,
        m->animacaoPulandoCorrendo.quantidadeQuadros,
        15,      // duração padrão para cada quadro
        164, 97, // início
        48, 48,  // dimensões
        4,       // separação
        false,   // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            30, 35, 40, 50
            // 18, 36, 60, 60
        });

    m->animacaoPulandoGirando.quantidadeQuadros = 4;
    m->animacaoPulandoGirando.quadroAtual = 0;
    m->animacaoPulandoGirando.contadorTempoQuadro = 0.0f;
    m->animacaoPulandoGirando.pararNoUltimoQuadro = false;
    m->animacaoPulandoGirando.executarUmaVez = false;
    m->animacaoPulandoGirando.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoPulandoGirando,
                         m->animacaoPulandoGirando.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoPulandoGirando.quadros,
        m->animacaoPulandoGirando.quantidadeQuadros,
        15,      // duração padrão para cada quadro
        216, 97, // início
        48, 48,  // dimensões
        4,       // separação
        false,   // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            30, 35, 40, 50
            // 18, 36, 60, 60
        });

    m->animacaoCaindo.quantidadeQuadros = 1;
    m->animacaoCaindo.quadroAtual = 0;
    m->animacaoCaindo.contadorTempoQuadro = 0.0f;
    m->animacaoCaindo.pararNoUltimoQuadro = false;
    m->animacaoCaindo.executarUmaVez = false;
    m->animacaoCaindo.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoCaindo,
                         m->animacaoCaindo.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoCaindo.quadros, m->animacaoCaindo.quantidadeQuadros,
        15,      // duração padrão para cada quadro
        112, 97, // início
        48, 48,  // dimensões
        4,       // separação
        false,   // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            30, 35, 40, 50
            // 18, 36, 60, 60
        });

    m->animacaoAbaixado.quantidadeQuadros = 1;
    m->animacaoAbaixado.quadroAtual = 0;
    m->animacaoAbaixado.contadorTempoQuadro = 0.0f;
    m->animacaoAbaixado.pararNoUltimoQuadro = false;
    m->animacaoAbaixado.executarUmaVez = false;
    m->animacaoAbaixado.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoAbaixado,
                         m->animacaoAbaixado.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoAbaixado.quadros, m->animacaoAbaixado.quantidadeQuadros,
        15,      // duração padrão para cada quadro
        112, 33, // início
        48, 48,  // dimensões
        4,       // separação
        false,   // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            32, 46, 32, 32
            // 18, 36, 60, 60
        });

    m->animacaoCrescendo.quantidadeQuadros = 3;
    m->animacaoCrescendo.quadroAtual = 0;
    m->animacaoCrescendo.contadorTempoQuadro = 0.0f;
    m->animacaoCrescendo.pararNoUltimoQuadro = true;
    m->animacaoCrescendo.executarUmaVez = true;
    m->animacaoCrescendo.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoCrescendo,
                         m->animacaoCrescendo.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoCrescendo.quadros, m->animacaoCrescendo.quantidadeQuadros,
        50,       // duração padrão para cada quadro
        580, 251, // início
        48, 48,   // dimensões
        4,        // separação
        false,    // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            30, 28, 40, 50
            // 18, 36, 60, 60
        });

    m->animacaoGrandeParado.quantidadeQuadros = 1;
    m->animacaoGrandeParado.quadroAtual = 0;
    m->animacaoGrandeParado.contadorTempoQuadro = 0.0f;
    m->animacaoGrandeParado.pararNoUltimoQuadro = false;
    m->animacaoGrandeParado.executarUmaVez = false;
    m->animacaoGrandeParado.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoGrandeParado,
                         m->animacaoGrandeParado.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoGrandeParado.quadros,
        m->animacaoGrandeParado.quantidadeQuadros,
        15,     // duração padrão para cada quadro
        8, 576, // início
        48, 48, // dimensões
        4,      // separação
        false,  // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            38, 23, 40, 71
            // 18, 36, 60, 60
        });
    m->animacaoGrandeAndando.quantidadeQuadros = 3;
    m->animacaoGrandeAndando.quadroAtual = 0;
    m->animacaoGrandeAndando.contadorTempoQuadro = 0.0f;
    m->animacaoGrandeAndando.pararNoUltimoQuadro = false;
    m->animacaoGrandeAndando.executarUmaVez = false;
    m->animacaoGrandeAndando.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoGrandeAndando,
                         m->animacaoGrandeAndando.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoGrandeAndando.quadros,
        m->animacaoGrandeAndando.quantidadeQuadros,
        15,       // duração padrão para cada quadro
        164, 576, // início
        48, 48,   // dimensões
        4,        // separação
        false,    // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            38, 23, 40, 71
            // 18, 36, 60, 60
        });

    m->animacaoGrandeCorrendo.quantidadeQuadros = 3;
    m->animacaoGrandeCorrendo.quadroAtual = 0;
    m->animacaoGrandeCorrendo.contadorTempoQuadro = 0.0f;
    m->animacaoGrandeCorrendo.pararNoUltimoQuadro = false;
    m->animacaoGrandeCorrendo.executarUmaVez = false;
    m->animacaoGrandeCorrendo.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoGrandeCorrendo,
                         m->animacaoGrandeCorrendo.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoGrandeCorrendo.quadros,
        m->animacaoGrandeCorrendo.quantidadeQuadros,
        15,       // duração padrão para cada quadro
        320, 576, // início
        48, 48,   // dimensões
        4,        // separação
        false,    // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            38, 23, 40, 71
            // 18, 36, 60, 60
        });
    m->animacaoGrandePulando.quantidadeQuadros = 1;
    m->animacaoGrandePulando.quadroAtual = 0;
    m->animacaoGrandePulando.contadorTempoQuadro = 0.0f;
    m->animacaoGrandePulando.pararNoUltimoQuadro = false;
    m->animacaoGrandePulando.executarUmaVez = false;
    m->animacaoGrandePulando.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoGrandePulando,
                         m->animacaoGrandePulando.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoGrandePulando.quadros,
        m->animacaoGrandePulando.quantidadeQuadros,
        15,      // duração padrão para cada quadro
        60, 647, // início
        48, 48,  // dimensões
        4,       // separação
        false,   // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            38, 23, 40, 71
            // 18, 36, 60, 60
        });

    m->animacaoGrandePulandoCorrendo.quantidadeQuadros = 1;
    m->animacaoGrandePulandoCorrendo.quadroAtual = 0;
    m->animacaoGrandePulandoCorrendo.contadorTempoQuadro = 0.0f;
    m->animacaoGrandePulandoCorrendo.pararNoUltimoQuadro = false;
    m->animacaoGrandePulandoCorrendo.executarUmaVez = false;
    m->animacaoGrandePulandoCorrendo.finalizada = false;
    criarQuadrosAnimacao(
        &m->animacaoGrandePulandoCorrendo,
        m->animacaoGrandePulandoCorrendo.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoGrandePulandoCorrendo.quadros,
        m->animacaoGrandePulandoCorrendo.quantidadeQuadros,
        15,       // duração padrão para cada quadro
        164, 647, // início
        48, 48,   // dimensões
        4,        // separação
        false,    // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            38, 23, 40, 71
            // 18, 36, 60, 60
        });
    m->animacaoGrandePulandoGirando.quantidadeQuadros = 4;
    m->animacaoGrandePulandoGirando.quadroAtual = 0;
    m->animacaoGrandePulandoGirando.contadorTempoQuadro = 0.0f;
    m->animacaoGrandePulandoGirando.pararNoUltimoQuadro = false;
    m->animacaoGrandePulandoGirando.executarUmaVez = false;
    m->animacaoGrandePulandoGirando.finalizada = false;
    criarQuadrosAnimacao(
        &m->animacaoGrandePulandoGirando,
        m->animacaoGrandePulandoGirando.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoGrandePulandoGirando.quadros,
        m->animacaoGrandePulandoGirando.quantidadeQuadros,
        40,       // duração padrão para cada quadro
        216, 647, // início
        48, 48,   // dimensões
        4,        // separação
        false,    // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            38, 23, 40, 71
            // 18, 36, 60, 60
        });

    m->animacaoGrandeCaindo.quantidadeQuadros = 1;
    m->animacaoGrandeCaindo.quadroAtual = 0;
    m->animacaoGrandeCaindo.contadorTempoQuadro = 0.0f;
    m->animacaoGrandeCaindo.pararNoUltimoQuadro = false;
    m->animacaoGrandeCaindo.executarUmaVez = false;
    m->animacaoGrandeCaindo.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoGrandeCaindo,
                         m->animacaoGrandeCaindo.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoGrandeCaindo.quadros,
        m->animacaoGrandeCaindo.quantidadeQuadros,
        15,       // duração padrão para cada quadro
        112, 647, // início
        48, 48,   // dimensões
        4,        // separação
        false,    // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            38, 28, 45, 78
            // 18, 36, 60, 60
        });

    m->animacaoGrandeAbaixado.quantidadeQuadros = 1;
    m->animacaoGrandeAbaixado.quadroAtual = 0;
    m->animacaoGrandeAbaixado.contadorTempoQuadro = 0.0f;
    m->animacaoGrandeAbaixado.pararNoUltimoQuadro = false;
    m->animacaoGrandeAbaixado.executarUmaVez = false;
    m->animacaoGrandeAbaixado.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoGrandeAbaixado,
                         m->animacaoGrandeAbaixado.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoGrandeAbaixado.quadros,
        m->animacaoGrandeAbaixado.quantidadeQuadros,
        15,       // duração padrão para cada quadro
        112, 576, // início
        48, 48,   // dimensões
        4,        // separação
        false,    // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            32, 56, 45, 38
            // 18, 36, 60, 60
        });

    m->animacoes[ESTADO_MARIO_PARADO] = &m->animacaoParado;
    quantidadeAnimacoes++;
    m->animacoes[ESTADO_MARIO_ANDANDO] = &m->animacaoAndando;
    quantidadeAnimacoes++;
    m->animacoes[ESTADO_MARIO_CORRENDO] = &m->animacaoCorrendo;
    quantidadeAnimacoes++;
    m->animacoes[ESTADO_MARIO_PULANDO] = &m->animacaoPulando;
    quantidadeAnimacoes++;
    m->animacoes[ESTADO_MARIO_PULANDO_CORRENDO] = &m->animacaoPulandoCorrendo;
    quantidadeAnimacoes++;
    m->animacoes[ESTADO_MARIO_CAINDO] = &m->animacaoCaindo;
    quantidadeAnimacoes++;
    m->animacoes[ESTADO_MARIO_ABAIXADO] = &m->animacaoAbaixado;
    quantidadeAnimacoes++;
    m->animacoes[ESTADO_MARIO_PULANDO_GIRANDO] = &m->animacaoPulandoGirando;
    quantidadeAnimacoes++;
    m->animacoes[ESTADO_MARIO_CRESCENDO] = &m->animacaoCrescendo;
    quantidadeAnimacoes++;
    m->animacoesGrande[ESTADO_MARIO_PARADO] = &m->animacaoGrandeParado;
    quantidadeAnimacoes++;
    m->animacoesGrande[ESTADO_MARIO_ANDANDO] = &m->animacaoGrandeAndando;
    quantidadeAnimacoes++;
    m->animacoesGrande[ESTADO_MARIO_CORRENDO] = &m->animacaoGrandeCorrendo;
    quantidadeAnimacoes++;
    m->animacoesGrande[ESTADO_MARIO_PULANDO] = &m->animacaoGrandePulando;
    quantidadeAnimacoes++;
    m->animacoesGrande[ESTADO_MARIO_PULANDO_CORRENDO] =
        &m->animacaoGrandePulandoCorrendo;
    quantidadeAnimacoes++;
    m->animacoesGrande[ESTADO_MARIO_CAINDO] = &m->animacaoGrandeCaindo;
    quantidadeAnimacoes++;
    m->animacoesGrande[ESTADO_MARIO_ABAIXADO] = &m->animacaoGrandeAbaixado;
    quantidadeAnimacoes++;
    m->animacoesGrande[ESTADO_MARIO_PULANDO_GIRANDO] =
        &m->animacaoGrandePulandoGirando;
    quantidadeAnimacoes++;

    m->quantidadeAnimacoes = quantidadeAnimacoes;

    p->dados = m;
    p->funcoes = &marioFuncoes;
    p->quantidadeAneis = 0;
    p->quantidadeVidas = 3;
    p->score = 0;
    p->comboAereo = 0;
    p->time = 0.0f;

    return p;
}

/**
 * @brief Destrói um objeto Jogador e libera seus recursos.
 */
void destruirMario(void *dados)
{
    Mario *m = (Mario *)dados;
    if (m != NULL)
    {
        for (int i = 0; i < m->quantidadeAnimacoes; i++)
        {
            destruirQuadrosAnimacao(m->animacoes[i]);
        }
        free(m);
    }
}

/**
 * @brief Lê a entrada do usuário e atualiza as velocidades do jogador.
 */
void entradaMario(void *dados, Personagem *p, float delta)
{
    Mario *m = (Mario *)dados;

    // EstadoJogador estadoAnterior = m->estado;

    bool direitaDown = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D) ||
                       (IsGamepadAvailable(0) &&
                        IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT));
    bool esquerdaDown = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) ||
                        (IsGamepadAvailable(0) &&
                         IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT));
    bool pulandoGirandoPressed =
        IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) ||
        (IsGamepadAvailable(0) &&
         IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP));
    bool baixoDown = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S) ||
                     (IsGamepadAvailable(0) &&
                      IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN));
    bool puloPressed = IsKeyPressed(KEY_SPACE) ||
                       (IsGamepadAvailable(0) &&
                        IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN));
    bool shiftDown = IsKeyDown(KEY_LEFT_SHIFT);

    if (direitaDown)
    {

        if (!shiftDown)
        {
            if (m->vel.x < 0)
            {
                m->vel.x += m->frenagem * delta;
                if (!m->freando && m->estado == ESTADO_MARIO_ANDANDO)
                {
                    // PlaySound(rm.somFrenagem);
                    m->freando = true;
                }
                if (m->vel.x > 0)
                {
                    m->vel.x = 0;
                    m->freando = false;
                }
            }
            else
            {
                m->vel.x += m->aceleracao * delta;
                if (m->vel.x > m->velAndando)
                {
                    m->vel.x = m->velAndando;
                }
            }
            m->olhandoParaDireita = true;
        }
        else
        {
            if (m->vel.x < 0)
            {
                m->vel.x += m->frenagem * delta;
                if (!m->freando && m->estado == ESTADO_MARIO_CORRENDO)
                {
                    // PlaySound(rm.somFrenagem);
                    m->freando = true;
                }
                if (m->vel.x > 0)
                {
                    m->vel.x = 0;
                    m->freando = false;
                }
            }
            else
            {
                m->vel.x += m->aceleracao * delta;
                if (m->vel.x > m->velCorrendo)
                {
                    m->vel.x = m->velCorrendo;
                }
            }
            m->olhandoParaDireita = true;
        }
    }
    else if (esquerdaDown)
    {

        if (!shiftDown)
        {
            if (m->vel.x > 0)
            {
                m->vel.x -= m->frenagem * delta;
                if (!m->freando && m->estado == ESTADO_MARIO_ANDANDO)
                {
                    // PlaySound(rm.somFrenagem);
                    m->freando = true;
                }
                if (m->vel.x < 0)
                {
                    m->vel.x = 0;
                    m->freando = false;
                }
            }
            else
            {
                m->vel.x -= m->aceleracao * delta;
                if (m->vel.x < -m->velAndando)
                {
                    m->vel.x = -m->velAndando;
                }
            }
            m->olhandoParaDireita = false;
        }
        else
        { // shiftdown
            if (m->vel.x > 0)
            {
                m->vel.x -= m->frenagem * delta;
                if (!m->freando && m->estado == ESTADO_MARIO_CORRENDO)
                {
                    // PlaySound(rm.somFrenagem);
                    m->freando = true;
                }

                if (m->vel.x < 0)
                {
                    m->vel.x = 0;
                    m->freando = false;
                }
            }
            else
            { // velo > 0
                m->vel.x -= m->aceleracao * delta;
                if (m->vel.x < -m->velCorrendo)
                {
                    m->vel.x = -m->velCorrendo;
                }
            }
            m->olhandoParaDireita = false;
        }
    }
    else
    { // esquerdadown
        if (m->vel.x > 0)
        {
            m->vel.x -= m->desaceleracao * delta;
            if (m->vel.x < 0)
            {
                m->vel.x = 0;
            }
        }
        else if (m->vel.x < 0)
        {
            m->vel.x += m->desaceleracao * delta;
            if (m->vel.x > 0)
            {
                m->vel.x = 0;
            }
        }
    }

    float absVelX = fabsf(m->vel.x);
    if (m->quantidadePulos > 0)
    {
        if (m->pulandoGirando)
        {
            m->estado = ESTADO_MARIO_PULANDO_GIRANDO;
        }
        else if (m->vel.y < 0)
        {
            if (absVelX <= m->velAndando)
            {
                m->estado = ESTADO_MARIO_PULANDO;
            }
            else
            {
                m->estado = ESTADO_MARIO_PULANDO_CORRENDO;
            }
        }
        else
        {
            m->estado = ESTADO_MARIO_CAINDO;
        }
    }
    else if (baixoDown)
    {
        m->estado = ESTADO_MARIO_ABAIXADO;
        m->vel.x = 0;
    }
    else if (absVelX < 1.0f)
    {
        m->estado = ESTADO_MARIO_PARADO;
    }
    else if (absVelX <= m->velAndando)
    {
        m->estado = ESTADO_MARIO_ANDANDO;
    }
    else
    {
        m->estado = ESTADO_MARIO_CORRENDO;
    }

    if ((puloPressed || pulandoGirandoPressed) &&
        m->quantidadePulos < m->quantidadeMaxPulos)
    {
        m->vel.y = m->velPulo;
        m->quantidadePulos++;
        m->pulandoGirando = pulandoGirandoPressed;
        // PlaySound( rm.somPulo );
    }

    // sincronização de animações andando e andando rápido
    /*if ( estadoAnterior == ESTADO_MARIO_ANDANDO && m->estado ==
    ESTADO_MARIO_ANDANDO_RAPIDO ) { sincronizarAnimacao(
    &m->animacaoAndandoRapido, &m->animacaoAndando ); } else if ( estadoAnterior
    == ESTADO_MARIO_ANDANDO_RAPIDO && m->estado == ESTADO_MARIO_ANDANDO ) {
        sincronizarAnimacao( &m->animacaoAndando, &m->animacaoAndandoRapido );
    } */
}

/**
 * @brief Aplica física e resolve colisões do jogador com o mundo.
 */
void atualizarMario(void *dados, Personagem *p, GameWorld *gw, float delta)
{
    Mario *m = (Mario *)dados;

    if (m->estado < ESTADO_MARIO_PULANDO)
    {
        p->comboAereo = 0;
    }
    if (m->estado == ESTADO_MARIO_CRESCENDO &&
        m->animacaoCrescendo.finalizada)
    {
        m->estado = ESTADO_MARIO_PARADO;
        m->invulneravel = false;
        // aqui você redimensiona o ret para o tamanho grande
        m->ret.y -= m->ret.height;
        m->ret.width *= 1.2;
        m->ret.height *= 1.2;
    }

    if (m->invulneravel)
    {

        m->piscaPisca = false;

        m->contadorTempoInvulnerabilidade += delta;
        if (m->contadorTempoInvulnerabilidade >= m->tempoInvulnerabilidade)
        {
            m->contadorTempoInvulnerabilidade = 0.0f;
            m->invulneravel = false;
            m->piscaPisca = false;
        }
    }

    Animacao *animacaoAtual = getAnimacaoAtualMario(m);
    atualizarAnimacao(animacaoAtual, delta);

    // fase X: move horizontalmente e resolve colisões laterais
    m->ret.x += m->vel.x * delta;
    resolverColisaoMarioObstaculosMapaX(m, gw->mapa);

    // fase Y: aplica gravidade, move verticalmente e resolve colisões verticais
    m->vel.y += gw->gravidade * delta;
    if (m->vel.y > m->velMaxQueda)
    {
        m->vel.y = m->velMaxQueda;
    }
    m->ret.y += m->vel.y * delta;
    resolverColisaoMarioObstaculosMapaY(m, gw->mapa);

    resolverColisaoMarioItensMapa(m, p, gw->mapa);
    resolverColisaoMarioInimigosMapa(m, p, gw->mapa);
    resolverEntradaCanoMapa(m, p, gw);
}

/**
 * @brief Desenha o jogador.
 */
void desenharMario(void *dados)
{

    Mario *m = (Mario *)dados;

    if (!m->piscaPisca)
    {
        QuadroAnimacao *qa = getQuadroAnimacaoAtualMario(m);
        desenharQuadroAnimacaoMario(m, qa, WHITE);
    }

    if (MOSTRAR_RETANGULOS)
    {
        DrawRectangleRec(m->ret, Fade(m->cor, 0.5f));
        DrawRectangleLines(m->ret.x, m->ret.y, m->ret.width, m->ret.height,
                           BLACK);
    }
}

void resetarMario(void *dados, Personagem *p)
{
    Mario *m = (Mario *)dados;

    m->invulneravel = false;
    p->temCogumeloReserva = false;
    m->contadorTempoInvulnerabilidade = 0.0f;
    m->contadorTempoPiscaPisca = 0.0f;
    m->piscaPisca = false;
    m->pulandoGirando = false;
    m->estado = ESTADO_MARIO_PARADO;

    // reseta animações pequenas
    for (int i = 0; i < ESTADO_MARIO_CRESCENDO + 1; i++)
    {
        if (m->animacoes[i] != NULL)
        {
            m->animacoes[i]->quadroAtual = 0;
            m->animacoes[i]->contadorTempoQuadro = 0.0f;
            m->animacoes[i]->finalizada = false;
        }
    }

    // reseta animações grandes
    for (int i = 0; i < ESTADO_MARIO_ABAIXADO + 1; i++)
    {
        if (m->animacoesGrande[i] != NULL)
        {
            m->animacoesGrande[i]->quadroAtual = 0;
            m->animacoesGrande[i]->contadorTempoQuadro = 0.0f;
            m->animacoesGrande[i]->finalizada = false;
        }
    }

    // reseta animação de crescendo separadamente
    m->animacaoCrescendo.quadroAtual = 0;
    m->animacaoCrescendo.contadorTempoQuadro = 0.0f;
    m->animacaoCrescendo.finalizada = false;
}

static PersonagemFuncoes marioFuncoes = {
    entradaMario, atualizarMario, desenharMario, destruirMario, resetarMario};

static void desenharQuadroAnimacaoMario(Mario *m, QuadroAnimacao *qa,
                                        Color tonalidade)
{

    if (qa != NULL)
    {

        DrawTexturePro(rm.texturaMario,
                       (Rectangle){qa->fonte.x, qa->fonte.y,
                                   m->olhandoParaDireita ? -qa->fonte.width
                                                         : qa->fonte.width,
                                   qa->fonte.height},
                       m->ret, (Vector2){0}, 0.0f, tonalidade);

        if (MOSTRAR_RETANGULOS)
        {
            float xDesenho = m->olhandoParaDireita
                                 ? m->ret.x + qa->retColisao.x
                                 : m->ret.x + m->ret.width - qa->retColisao.x -
                                       qa->retColisao.width;
            float yDesenho = m->ret.y + qa->retColisao.y;
            DrawRectangle(xDesenho, yDesenho, qa->retColisao.width,
                          qa->retColisao.height, Fade(GREEN, 0.5f));
        }
    }
}

static QuadroAnimacao *getQuadroAnimacaoAtualMario(Mario *m)
{
    return getQuadroAtualAnimacao(getAnimacaoAtualMario(m));
}

static Animacao *getAnimacaoAtualMario(Mario *m)
{
    if (m->estado == ESTADO_MARIO_CRESCENDO)
    {
        return &m->animacaoCrescendo;
    }
    if (m->grande)
    {
        return m->animacoesGrande[m->estado];
    }
    return m->animacoes[m->estado];
}

/**
 * @brief Resolve colisões do jogador com o mapa no eixo X.
 */
static void resolverColisaoMarioObstaculosMapaX(Mario *m, Mapa *mapa)
{

    ElementoMapa *el = mapa->obstaculos;

    while (el != NULL)
    {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualMario(m);

        float deslocamentoX =
            m->olhandoParaDireita
                ? qa->retColisao.x
                : m->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            m->ret.x + deslocamentoX, m->ret.y + deslocamentoY,
            qa->retColisao.width, qa->retColisao.height};

        Obstaculo *o = (Obstaculo *)el->objeto;

        if (!o->solido)
        {
            el = el->proximo;
            continue;
        }
        if (o->tipoColisao != COLISAO_RETANGULO)
        {
            el = el->proximo;
            continue;
        }
        if (checarColisaoComObstaculo(retColCalculado, o))
        {
            if (retColCalculado.x + retColCalculado.width / 2 <
                o->ret.x + o->ret.width / 2)
            {
                m->ret.x = o->ret.x - qa->retColisao.width - deslocamentoX;
            }
            else
            {
                m->ret.x = o->ret.x + o->ret.width - deslocamentoX;
            }
            m->vel.x = 0;
        }

        el = el->proximo;
    }
}

/**
 * @brief Resolve colisões do jogador com o mapa no eixo Y.
 */
static void resolverColisaoMarioObstaculosMapaY(Mario *m, Mapa *mapa)
{

    ElementoMapa *el = mapa->obstaculos;

    while (el != NULL)
    {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualMario(m);

        float deslocamentoX =
            m->olhandoParaDireita
                ? qa->retColisao.x
                : m->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            m->ret.x + deslocamentoX, m->ret.y + deslocamentoY,
            qa->retColisao.width, qa->retColisao.height};

        Obstaculo *o = (Obstaculo *)el->objeto;

        if (!o->solido)
        {
            el = el->proximo;
            continue;
        }

        if (checarColisaoComObstaculo(retColCalculado, o))
        {

            bool vindoDeBaixo =
                retColCalculado.y + retColCalculado.height / 2 >=
                o->ret.y + o->ret.height / 2;

            bool vindoDeCima = retColCalculado.y + retColCalculado.height / 2 <=
                               o->ret.y + o->ret.height / 2;

            if ((o->eBlocoGiratorio && vindoDeBaixo &&
                 m->estado == ESTADO_MARIO_PULANDO_GIRANDO && !o->quebrando) ||
                (o->eBlocoGiratorio && vindoDeCima &&
                 m->estado == ESTADO_MARIO_PULANDO_GIRANDO && !o->quebrando))
            {

                m->vel.y = m->velPulo;
                o->quebrando = true;
                o->solido = false;
                o->quadroQuebra = 0;
                o->tempoQuadroQuebra = 0.0f;

                el = el->proximo;
                continue;
            }

            // tratamento especial para rampas
            if (o->tipoColisao != COLISAO_RETANGULO)
            {
                float xRelativo =
                    (retColCalculado.x + retColCalculado.width / 2) - o->ret.x;
                float proporcao = xRelativo / o->ret.width;
                proporcao = Clamp(proporcao, 0.0f, 1.0f);
                float alturaChao;

                if (o->tipoColisao == COLISAO_RAMPA_CIMA_DIREITA)
                {
                    alturaChao = o->ret.y + (o->ret.height * proporcao);
                }
                else if (o->tipoColisao == COLISAO_RAMPA_BAIXO_DIREITA)
                {
                    alturaChao =
                        o->ret.y + (o->ret.height * (1.0f - proporcao));
                }
                else
                {
                    alturaChao = o->ret.y + (o->ret.height * proporcao);
                }

                float basePersonagem =
                    retColCalculado.y + retColCalculado.height;
                if (basePersonagem >= alturaChao && m->vel.y >= 0)
                {
                    m->ret.y =
                        alturaChao - qa->retColisao.height - deslocamentoY;
                    m->vel.y = 0;
                    m->quantidadePulos = 0;
                    m->pulandoGirando = false;
                }

                el = el->proximo;
                continue;
            }

            if (retColCalculado.y + retColCalculado.height / 2 <
                o->ret.y + o->ret.height / 2)
            {
                m->ret.y = o->ret.y - qa->retColisao.height - deslocamentoY;
                m->quantidadePulos = 0;
                m->pulandoGirando = false;
            }
            else
            {
                m->ret.y = o->ret.y + o->ret.height - deslocamentoY;
            }
            m->vel.y = 0;
        }

        el = el->proximo;
    }
}

static void resolverColisaoMarioItensMapa(Mario *m, Personagem *p, Mapa *mapa)
{

    ElementoMapa *el = mapa->itens;

    while (el != NULL)
    {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualMario(m);

        float deslocamentoX =
            m->olhandoParaDireita
                ? qa->retColisao.x
                : m->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            m->ret.x + deslocamentoX, m->ret.y + deslocamentoY,
            qa->retColisao.width, qa->retColisao.height};

        Item *item = (Item *)el->objeto;

        if (item->tipo == TIPO_ITEM_ANEL)
        {

            ItemAnel *itemAnel = (ItemAnel *)item->objeto;

            if (!itemAnel->ativo ||
                itemAnel->estado == ESTADO_ITEM_ANEL_COLETADO)
            {
                el = el->proximo;
                continue;
            }

            QuadroAnimacao *qaItem = getQuadroAnimacaoAtualItemAnel(itemAnel);

            Rectangle retColItemCalculado = {
                itemAnel->ret.x + qaItem->retColisao.x,
                itemAnel->ret.y + qaItem->retColisao.y,
                qaItem->retColisao.width, qaItem->retColisao.height};

            if (CheckCollisionRecs(retColCalculado, retColItemCalculado))
            {
                itemAnel->estado = ESTADO_ITEM_ANEL_COLETADO;
                p->quantidadeAneis++;
                p->score += 10;
                PlaySound(rm.somAnel);
            }
        }
        else if (item->tipo == TIPO_ITEM_ANEL_AZUL)
        {

            ItemAnelAzul *itemAnelAzul = (ItemAnelAzul *)item->objeto;

            if (!itemAnelAzul->ativo ||
                itemAnelAzul->estado == ESTADO_ITEM_ANEL_AZUL_COLETADO)
            {
                el = el->proximo;
                continue;
            }

            QuadroAnimacao *qaItem =
                getQuadroAnimacaoAtualItemAnelAzul(itemAnelAzul);

            Rectangle retColItemCalculado = {
                itemAnelAzul->ret.x + qaItem->retColisao.x,
                itemAnelAzul->ret.y + qaItem->retColisao.y,
                qaItem->retColisao.width, qaItem->retColisao.height};

            if (CheckCollisionRecs(retColCalculado, retColItemCalculado))
            {
                itemAnelAzul->estado = ESTADO_ITEM_ANEL_AZUL_COLETADO;
                p->quantidadeAneis += 10;
                p->score += 100;
                PlaySound(rm.somAnel);
            }
        }
        else if (item->tipo == TIPO_ITEM_COGUMELO_VERMELHO)
        {

            ItemCogumeloVermelho *itemCogumeloVermelho =
                (ItemCogumeloVermelho *)item->objeto;

            if (!itemCogumeloVermelho->ativo ||
                itemCogumeloVermelho->estado ==
                    ESTADO_ITEM_COGUMELO_VERMELHO_COLETADO)
            {
                el = el->proximo;
                continue;
            }

            QuadroAnimacao *qaItem = getQuadroAnimacaoAtualItemCogumeloVermelho(
                itemCogumeloVermelho);

            Rectangle retColItemCalculado = {
                itemCogumeloVermelho->ret.x + qaItem->retColisao.x,
                itemCogumeloVermelho->ret.y + qaItem->retColisao.y,
                qaItem->retColisao.width, qaItem->retColisao.height};

            if (CheckCollisionRecs(retColCalculado, retColItemCalculado))
            {
                itemCogumeloVermelho->estado =
                    ESTADO_ITEM_COGUMELO_VERMELHO_COLETADO;
                p->quantidadeAneis += 10;
                p->score += 100;
                PlaySound(rm.somAnel);
                printf("Cogumelo vermelho coletado!\n");

                // faz o Mario crescer
                if (!m->grande)
                {
                    m->grande = true;
                    m->estado = ESTADO_MARIO_CRESCENDO;
                    m->invulneravel = true;
                    m->ret.y -= m->ret.height; // sobe para não entrar no chão
                    m->ret.height *= 1.2;
                    m->ret.width *= 1.2;
                }
                else
                {
                    p->temCogumeloReserva = true;
                }
            }
        }
        else if (item->tipo == TIPO_ITEM_FLOR_PRETA)
        {
            ItemFlorPreta *florPreta = (ItemFlorPreta *)item->objeto;

            if (!florPreta->ativo ||
                florPreta->estado == ESTADO_ITEM_FLOR_PRETA_PARADA ||
                florPreta->estado == ESTADO_ITEM_FLOR_PRETA_COLETADA_COMO_MOEDA)
            {
                el = el->proximo;
                continue;
            }

            QuadroAnimacao *qaItem = getQuadroAnimacaoAtualItemFlorPreta(florPreta);

            Rectangle retColItemCalculado = {
                florPreta->ret.x + qaItem->retColisao.x,
                florPreta->ret.y + qaItem->retColisao.y,
                qaItem->retColisao.width,
                qaItem->retColisao.height};

            if (CheckCollisionRecs(retColCalculado, retColItemCalculado))
            {

                // VERIFICA SE O MARIO ESTÁ PULANDO GIRANDO
                if (m->estado == ESTADO_MARIO_PULANDO_GIRANDO)
                {
                    // Mario pulou girando - a flor vira MOEDA
                    florPreta->estado = ESTADO_ITEM_FLOR_PRETA_COLETADA_COMO_MOEDA;
                    p->quantidadeAneis += 5; // Ganha 5 anéis
                    p->score += 200;
                    PlaySound(rm.somAnel);
                    printf("Flor Preta virou moeda! +5 anéis\n");
                    return;
                }

                // VERIFICA SE A FLOR ESTÁ ATACANDO (COLETADA) - CAUSA DANO
                if (florPreta->estado == ESTADO_ITEM_FLOR_PRETA_COLETADA && !m->invulneravel)
                {
                    if (m->grande)
                    {
                        m->grande = false;
                        m->ret.width = m->retOriginal.width;
                        m->ret.height = m->retOriginal.height;
                        m->ret.y -= m->ret.height;
                        PlaySound(rm.somHitComAnel);
                    }
                    else if (p->quantidadeAneis > 0)
                    {
                        p->quantidadeAneis = 0;
                        PlaySound(rm.somHitComAnel);
                    }
                    else
                    {
                        p->quantidadeVidas--;
                        PlaySound(rm.somMorte);
                    }
                    m->invulneravel = true;
                    return;
                }
            }
        }

        el = el->proximo;
    }
}

static void resolverColisaoMarioInimigosMapa(Mario *m, Personagem *p,
                                             Mapa *mapa)
{
    ElementoMapa *el = mapa->inimigos;
    // Max = 7 inimigos mortos em sequencia
    static int tabelaComboAereo[] = {100, 200, 500, 1000, 2000, 5000, 10000};

    while (el != NULL)
    {
        QuadroAnimacao *qa = getQuadroAnimacaoAtualMario(m);

        float deslocamentoX =
            m->olhandoParaDireita
                ? qa->retColisao.x
                : m->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            m->ret.x + deslocamentoX, m->ret.y + deslocamentoY,
            qa->retColisao.width, qa->retColisao.height};

        Inimigo *inimigo = (Inimigo *)el->objeto;

        QuadroAnimacao *qaInimigo = NULL;
        bool *olhandoParaDireita = NULL;
        Rectangle *ret = NULL;

        if (inimigo->tipo == TIPO_INIMIGO_MOTOBUG)
        {
            InimigoMotobug *motobug = (InimigoMotobug *)inimigo->objeto;

            if (!motobug->ativo ||
                motobug->estado == ESTADO_INIMIGO_MOTOBUG_MORRENDO)
            {
                el = el->proximo;
                continue;
            }

            qaInimigo = getQuadroAnimacaoAtualInimigoMotobug(motobug);
            olhandoParaDireita = &motobug->olhandoParaDireita;
            ret = &motobug->ret;

            float deslocamentoX = *olhandoParaDireita
                                      ? ret->width - qaInimigo->retColisao.x -
                                            qaInimigo->retColisao.width
                                      : qaInimigo->retColisao.x;
            float deslocamentoY = qaInimigo->retColisao.y;

            Rectangle retColInimigoCalculado = {
                ret->x + deslocamentoX, ret->y + deslocamentoY,
                qaInimigo->retColisao.width, qaInimigo->retColisao.height};

            if (CheckCollisionRecs(retColCalculado, retColInimigoCalculado))
            {
                if (m->estado == ESTADO_MARIO_PULANDO ||
                    m->estado == ESTADO_MARIO_PULANDO_CORRENDO ||
                    m->estado == ESTADO_MARIO_CAINDO ||
                    m->estado == ESTADO_MARIO_PULANDO_GIRANDO)
                {
                    m->vel.y = m->velPulo;
                    motobug->estado = ESTADO_INIMIGO_MOTOBUG_MORRENDO;
                    PlaySound(rm.somHitInimigo);
                    int idx = p->comboAereo >= 6 ? 6 : p->comboAereo;
                    p->score += tabelaComboAereo[idx];
                    p->comboAereo++;
                }
                else if (!m->invulneravel)
                {
                    if (p->temCogumeloReserva)
                    {
                        p->temCogumeloReserva = false; // consome a reserva
                        m->invulneravel = true;        // não perde vida nem anel
                        PlaySound(rm.somHitComAnel);
                    }
                    else if (m->grande)
                    {
                        // encolhe ao invés de perder vida
                        m->grande = false;
                        m->ret.width = m->retOriginal.width;
                        m->ret.height = m->retOriginal.height;
                        m->invulneravel = true;
                        m->ret.y -= m->ret.height;
                        PlaySound(rm.somHitComAnel); // ou um som de encolher
                    }
                    else if (p->quantidadeAneis > 0)
                    {
                        p->quantidadeAneis = 0;
                        PlaySound(rm.somHitComAnel);
                    }
                    else
                    {
                        p->quantidadeVidas--;
                        PlaySound(rm.somMorte);
                    }
                    m->invulneravel = true;
                }

                return; // um inimigo de cada vez!
            }
        }
        else if (inimigo->tipo == TIPO_INIMIGO_SPIKES)
        {
            InimigoSpikes *spikes = (InimigoSpikes *)inimigo->objeto;

            if (!spikes->ativo ||
                spikes->estado == ESTADO_INIMIGO_SPIKES_MORRENDO)
            {
                el = el->proximo;
                continue;
            }

            qaInimigo = getQuadroAnimacaoAtualInimigoSpikes(spikes);
            olhandoParaDireita = &spikes->olhandoParaDireita;
            ret = &spikes->ret;

            float deslocamentoX = *olhandoParaDireita
                                      ? ret->width - qaInimigo->retColisao.x -
                                            qaInimigo->retColisao.width
                                      : qaInimigo->retColisao.x;
            float deslocamentoY = qaInimigo->retColisao.y;

            Rectangle retColInimigoCalculado = {
                ret->x + deslocamentoX, ret->y + deslocamentoY,
                qaInimigo->retColisao.width, qaInimigo->retColisao.height};

            if (CheckCollisionRecs(retColCalculado, retColInimigoCalculado))
            {
                if (m->estado == ESTADO_MARIO_PULANDO ||
                    m->estado == ESTADO_MARIO_PULANDO_CORRENDO ||
                    m->estado == ESTADO_MARIO_CAINDO ||
                    m->estado == ESTADO_MARIO_PULANDO_GIRANDO)
                {
                    m->vel.y = m->velPulo;
                    spikes->estado = ESTADO_INIMIGO_SPIKES_MORRENDO;
                    PlaySound(rm.somHitInimigo);
                    int idx = p->comboAereo >= 6 ? 6 : p->comboAereo;
                    p->score += tabelaComboAereo[idx];
                    p->comboAereo++;
                }
                else if (!m->invulneravel)
                {
                    if (p->temCogumeloReserva)
                    {
                        p->temCogumeloReserva = false; // consome a reserva
                        m->invulneravel = true;        // não perde vida nem anel
                        PlaySound(rm.somHitComAnel);
                    }
                    else if (m->grande)
                    {
                        // encolhe ao invés de perder vida
                        m->grande = false;
                        m->ret.width = m->retOriginal.width;
                        m->ret.height = m->retOriginal.height;
                        m->invulneravel = true;
                        m->ret.y -= m->ret.height;
                        PlaySound(rm.somHitComAnel); // ou um som de encolher
                    }
                    else if (p->quantidadeAneis > 0)
                    {
                        p->quantidadeAneis = 0;
                        PlaySound(rm.somHitComAnel);
                    }
                    else
                    {
                        p->quantidadeVidas--;
                        PlaySound(rm.somMorte);
                    }
                    m->invulneravel = true;
                }
            }
        }
        else if (inimigo->tipo == TIPO_INIMIGO_KOOPARED)
        {
            InimigoKoopaRed *koopared = (InimigoKoopaRed *)inimigo->objeto;

            if (!koopared->ativo)
            {
                el = el->proximo;
                continue;
            }

            qaInimigo = getQuadroAnimacaoAtualInimigoKoopaRed(koopared);
            olhandoParaDireita = &koopared->olhandoParaDireita;
            ret = &koopared->ret;

            float deslocamentoX = *olhandoParaDireita
                                      ? ret->width - qaInimigo->retColisao.x -
                                            qaInimigo->retColisao.width
                                      : qaInimigo->retColisao.x;
            float deslocamentoY = qaInimigo->retColisao.y;

            Rectangle retColInimigoCalculado = {
                ret->x + deslocamentoX, ret->y + deslocamentoY,
                qaInimigo->retColisao.width, qaInimigo->retColisao.height};

            if (CheckCollisionRecs(retColCalculado, retColInimigoCalculado))
            {
                // Verifica se o Mario está pulando (ataque bem-sucedido)
                if (m->estado == ESTADO_MARIO_PULANDO ||
                    m->estado == ESTADO_MARIO_PULANDO_CORRENDO ||
                    m->estado == ESTADO_MARIO_CAINDO ||
                    m->estado == ESTADO_MARIO_PULANDO_GIRANDO)
                {
                    m->vel.y = m->velPulo;

                    if (koopared->estado == ESTADO_KOOPA_ANDANDO)
                    {
                        // Transforma em casco parado
                        koopared->estado = ESTADO_KOOPA_CASCO_PARADO;
                        koopared->ret.width = 32;
                        koopared->ret.height = 34;
                        koopared->ret.y -= 6;

                        // Ganha pontos por derrubar o Koopa
                        PlaySound(rm.somHitInimigo);
                        int idx = p->comboAereo >= 6 ? 6 : p->comboAereo;
                        p->score += tabelaComboAereo[idx];
                        p->comboAereo++;
                        // Quando pular em cima do casco parado
                    }
                    else if (koopared->estado == ESTADO_KOOPA_CASCO_PARADO)
                    {
                        // Ativa o casco correndo com animação de 4 quadros
                        koopared->estado = ESTADO_KOOPA_CASCO_CORRENDO;
                        koopared->velAndando = 350;

                        // Define a direção baseado na posição do Mario em
                        // relação ao casco
                        if (retColCalculado.x + retColCalculado.width / 2 <
                            koopared->ret.x + koopared->ret.width / 2)
                        {
                            koopared->olhandoParaDireita =
                                false; // Casco vai para a esquerda
                        }
                        else
                        {
                            koopared->olhandoParaDireita =
                                true; // Casco vai para a direita
                        }

                        // Ganha pontos por chutar o casco
                        PlaySound(rm.somHitInimigo);
                        int idx = p->comboAereo >= 6 ? 6 : p->comboAereo;
                        p->score += tabelaComboAereo[idx];
                        p->comboAereo++;
                    }
                    // Se o casco estiver correndo e o Mario pular em cima, ele
                    // quebra/morre
                    else if (koopared->estado == ESTADO_KOOPA_CASCO_CORRENDO)
                    {
                        // Mario destrói o casco correndo
                        koopared->estado = ESTADO_KOOPA_MORRENDO;
                        koopared->ativo =
                            false; // Ou deixa a animação de morte tocar

                        PlaySound(rm.somHitInimigo);
                        int idx = p->comboAereo >= 6 ? 6 : p->comboAereo;
                        p->score += tabelaComboAereo[idx] *
                                    2; // Bônus maior por destruir o casco
                        p->comboAereo++;
                    }
                }
                else
                {

                    // Caso especial: Se o Koopa estiver em casco parado e o
                    // Mario trombar pelas laterais, ele chuta o casco em vez de
                    // tomar dano
                    if (koopared->estado == ESTADO_KOOPA_CASCO_PARADO)
                    {
                        // Mario chuta o casco parado
                        koopared->estado = ESTADO_KOOPA_CASCO_CORRENDO;
                        koopared->velAndando = 350;

                        // Define a direção baseado em qual lado o Mario está
                        if (retColCalculado.x + retColCalculado.width / 2 <
                            koopared->ret.x + koopared->ret.width / 2)
                        {
                            koopared->olhandoParaDireita =
                                false; // Casco vai para a esquerda
                        }
                        else
                        {
                            koopared->olhandoParaDireita =
                                true; // Casco vai para a direita
                        }

                        PlaySound(rm.somHitInimigo);
                        // Não ganha pontos por chutar, apenas evita o dano
                    }
                    else if (!m->invulneravel)
                    {
                        // Rex faz dano no Mario
                        if (p->temCogumeloReserva)
                        {
                            p->temCogumeloReserva = false; // consome a reserva
                            m->invulneravel = true;        // não perde vida nem anel
                            PlaySound(rm.somHitComAnel);
                        }
                        else if (m->grande)
                        {
                            m->grande = false;
                            m->ret.width = m->retOriginal.width;
                            m->ret.height = m->retOriginal.height;
                            m->ret.y -= m->ret.height;
                            PlaySound(rm.somHitComAnel);
                        }
                        else if (p->quantidadeAneis > 0)
                        {
                            p->quantidadeAneis = 0;
                            PlaySound(rm.somHitComAnel);
                        }
                        else
                        {
                            p->quantidadeVidas--;
                            PlaySound(rm.somMorte);
                        }
                        m->invulneravel =
                            true; // Torna invulnerável após tomar dano
                    }
                }
                return; // Processa apenas uma colisão por frame
            }
        }
        else if (inimigo->tipo == TIPO_INIMIGO_REX)
        {

            InimigoRex *rex = (InimigoRex *)inimigo->objeto;

            if (!rex->ativo || rex->estado == ESTADO_INIMIGO_REX_MORRENDO)
            {
                el = el->proximo;
                continue;
            }

            qaInimigo = getQuadroAnimacaoAtualInimigoRex(rex);
            olhandoParaDireita = &rex->olhandoParaDireita;
            ret = &rex->ret;

            float deslocamentoX = *olhandoParaDireita
                                      ? ret->width - qaInimigo->retColisao.x -
                                            qaInimigo->retColisao.width
                                      : qaInimigo->retColisao.x;
            float deslocamentoY = qaInimigo->retColisao.y;

            Rectangle retColInimigoCalculado = {
                ret->x + deslocamentoX, ret->y + deslocamentoY,
                qaInimigo->retColisao.width, qaInimigo->retColisao.height};

            if (CheckCollisionRecs(retColCalculado, retColInimigoCalculado))
            {
                // Verifica se o Mario está pulando/atacando de cima
                if (m->estado == ESTADO_MARIO_PULANDO ||
                    m->estado == ESTADO_MARIO_PULANDO_CORRENDO ||
                    m->estado == ESTADO_MARIO_CAINDO ||
                    m->estado == ESTADO_MARIO_PULANDO_GIRANDO)
                {
                    // Mata o Rex
                    m->vel.y = m->velPulo;
                    rex->estado = ESTADO_INIMIGO_REX_MORRENDO;
                    PlaySound(rm.somHitInimigo);
                    int idx = p->comboAereo >= 6 ? 6 : p->comboAereo;
                    p->score += tabelaComboAereo[idx];
                    p->comboAereo++;
                }
                else if (!m->invulneravel)
                {
                    // Rex faz dano no Mario
                    if (p->temCogumeloReserva)
                    {
                        p->temCogumeloReserva = false; // consome a reserva
                        m->invulneravel = true;        // não perde vida nem anel
                        PlaySound(rm.somHitComAnel);
                    }
                    else if (m->grande)
                    {
                        m->grande = false;
                        m->ret.width = m->retOriginal.width;
                        m->ret.height = m->retOriginal.height;
                        m->ret.y -= m->ret.height;
                        PlaySound(rm.somHitComAnel);
                    }
                    else if (p->quantidadeAneis > 0)
                    {
                        p->quantidadeAneis = 0;
                        PlaySound(rm.somHitComAnel);
                    }
                    else
                    {
                        p->quantidadeVidas--;
                        PlaySound(rm.somMorte);
                    }
                    m->invulneravel =
                        true; // Torna invulnerável após tomar dano
                }
                return;
            }
        }
        else if (inimigo->tipo == TIPO_INIMIGO_NEGOPRETO)
        {
            InimigoNegoPreto *negoPreto = (InimigoNegoPreto *)inimigo->objeto;

            if (!negoPreto->ativo)
            {
                el = el->proximo;
                continue;
            }

            qaInimigo = getQuadroAnimacaoAtualInimigoNegoPreto(negoPreto);
            olhandoParaDireita = &negoPreto->olhandoParaDireita;
            ret = &negoPreto->ret;

            float deslocamentoX = *olhandoParaDireita
                                      ? ret->width - qaInimigo->retColisao.x -
                                            qaInimigo->retColisao.width
                                      : qaInimigo->retColisao.x;

            float deslocamentoY = qaInimigo->retColisao.y;

            Rectangle retColInimigoCalculado = {
                ret->x + deslocamentoX, ret->y + deslocamentoY,
                qaInimigo->retColisao.width, qaInimigo->retColisao.height};

            if (CheckCollisionRecs(retColCalculado, retColInimigoCalculado))
            {
                if (m->estado == ESTADO_MARIO_PULANDO_GIRANDO)
                {
                    // Mario mata o Nego Preto com spin jump
                    m->vel.y = m->velPulo;
                    negoPreto->ativo = false;
                    PlaySound(rm.somHitInimigo);
                    int idx = p->comboAereo >= 6 ? 6 : p->comboAereo;
                    p->score += tabelaComboAereo[idx];
                    p->comboAereo++;

                    return;
                }
                else
                {
                    // REMOVA O "if (!m->invulneravel)" - O NEGO PRETO
                    // SEMPRE CAUSA DANO! Mario toma dano (mesmo se estiver
                    // invulnerável)
                    if (p->temCogumeloReserva)
                    {
                        p->temCogumeloReserva = false; // consome a reserva
                        m->invulneravel = true;        // não perde vida nem anel
                        PlaySound(rm.somHitComAnel);
                    }
                    else if (m->grande)
                    {
                        m->grande = false;
                        m->ret.width = m->retOriginal.width;
                        m->ret.height = m->retOriginal.height;
                        m->invulneravel = true;
                        m->ret.y -= m->ret.height;
                        PlaySound(rm.somHitComAnel);
                    }
                    else if (p->quantidadeAneis > 0)
                    {
                        p->quantidadeAneis = 0;
                        PlaySound(rm.somHitComAnel);
                    }
                    else
                    {
                        p->quantidadeVidas--;
                        PlaySound(rm.somMorte);
                    }
                    m->invulneravel = true;
                }

                return;
            }
        }
        else if (inimigo->tipo == TIPO_INIMIGO_FLORCARNIVORA)
        {
            InimigoFlorCarnivora *flor = (InimigoFlorCarnivora *)inimigo->objeto;

            if (!flor->ativo || flor->estado == ESTADO_FLOR_CARNIVORA_MORRENDO)
            {
                el = el->proximo;
                continue;
            }

            qaInimigo = getQuadroAnimacaoAtualInimigoFlorCarnivora(flor);
            ret = &flor->ret;

            float deslocamentoX = flor->olhandoParaDireita
                                      ? ret->width - qaInimigo->retColisao.x - qaInimigo->retColisao.width
                                      : qaInimigo->retColisao.x;
            float deslocamentoY = qaInimigo->retColisao.y;

            Rectangle retColInimigoCalculado = {
                ret->x + deslocamentoX, ret->y + deslocamentoY,
                qaInimigo->retColisao.width, qaInimigo->retColisao.height};

            if (CheckCollisionRecs(retColCalculado, retColInimigoCalculado))
            {
                // VERIFICA SE O MARIO ESTÁ PULANDO GIRANDO (mata a flor)
                if (m->estado == ESTADO_MARIO_PULANDO_GIRANDO)
                {
                    // Mario mata a flor com pulo girando
                    flor->estado = ESTADO_FLOR_CARNIVORA_MORRENDO;
                    flor->ativo = true;    // Mantém ativo até a animação terminar
                    m->vel.y = m->velPulo; // Quica

                    PlaySound(rm.somHitInimigo);
                    int idx = p->comboAereo >= 6 ? 6 : p->comboAereo;
                    p->score += tabelaComboAereo[idx];
                    p->comboAereo++;
                }
                // VERIFICA SE O MARIO ESTÁ EM OUTROS ESTADOS DE PULO (NÃO MATA, TOMA DANO)
                else if (m->estado == ESTADO_MARIO_PULANDO ||
                         m->estado == ESTADO_MARIO_PULANDO_CORRENDO ||
                         m->estado == ESTADO_MARIO_CAINDO)
                {
                    // Mario toma dano da flor (mesmo que esteja pulando, se não for girando)
                    if (m->grande)
                    {
                        m->grande = false;
                        m->ret.width = m->retOriginal.width;
                        m->ret.height = m->retOriginal.height;
                        m->ret.y -= m->ret.height;
                        PlaySound(rm.somHitComAnel);
                    }
                    else if (p->quantidadeAneis > 0)
                    {
                        p->quantidadeAneis = 0;
                        PlaySound(rm.somHitComAnel);
                    }
                    else
                    {
                        p->quantidadeVidas--;
                        PlaySound(rm.somMorte);
                    }
                    m->invulneravel = true;
                }
                // MARIO NO CHÃO - TOMA DANO
                else
                {
                    if (m->grande)
                    {
                        m->grande = false;
                        m->ret.width = m->retOriginal.width;
                        m->ret.height = m->retOriginal.height;
                        m->ret.y -= m->ret.height;
                        PlaySound(rm.somHitComAnel);
                    }
                    else if (p->quantidadeAneis > 0)
                    {
                        p->quantidadeAneis = 0;
                        PlaySound(rm.somHitComAnel);
                    }
                    else
                    {
                        p->quantidadeVidas--;
                        PlaySound(rm.somMorte);
                    }
                    m->invulneravel = true;
                }

                return;
            }
        }
        el = el->proximo;
    }
}

static void resolverEntradaCanoMapa(Mario *m, Personagem *p, GameWorld *gw)
{

    if (!IsKeyPressed(KEY_DOWN) && !IsKeyPressed(KEY_S))
    {
        return;
    }

    ElementoMapa *el = gw->mapa->obstaculos;

    while (el != NULL)
    {
        Obstaculo *o = (Obstaculo *)el->objeto;

        if (o->eCanoSaida)
        {
            QuadroAnimacao *qa = getQuadroAnimacaoAtualMario(m);

            float deslocamentoX =
                m->olhandoParaDireita
                    ? qa->retColisao.x
                    : m->ret.width - qa->retColisao.x - qa->retColisao.width;
            float deslocamentoY = qa->retColisao.y;

            Rectangle retColCalculado = {
                m->ret.x + deslocamentoX, m->ret.y + deslocamentoY,
                qa->retColisao.width, qa->retColisao.height};

            // checa se o mario está em pé em cima do cano
            Rectangle retTopoSaida = {o->ret.x, o->ret.y - 4, o->ret.width, 8};

            if (CheckCollisionRecs(retColCalculado, retTopoSaida))
            {
                strncpy(gw->proximaFase, o->proximaFase, 255);
                gw->proximaFase[255] = '\0';
                gw->estado = ESTADO_JOGO_PROXIMA_FASE;
                return;
            }
        }

        el = el->proximo;
    }
}