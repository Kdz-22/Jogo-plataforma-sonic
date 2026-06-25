/**
 * @file InimigoFlorCarnivora.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Inimigo (FlorCarnivora)
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"
#include "raylib/raymath.h"

#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoFlorCarnivora.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Tipos.h"

static const bool MOSTRAR_RETANGULOS = false;

static Animacao *getAnimacaoAtualInimigoFlorCarnivora(InimigoFlorCarnivora *flor);
static void desenharQuadroAnimacaoInimigoFlorCarnivora(InimigoFlorCarnivora *flor,
                                                       QuadroAnimacao *qa,
                                                       Color tonalidade);

InimigoFlorCarnivora *criarInimigoFlorCarnivora(Rectangle ret, Color cor)
{
    InimigoFlorCarnivora *flor = (InimigoFlorCarnivora *)malloc(sizeof(InimigoFlorCarnivora));

    // FORÇA O TAMANHO CORRETO DA FLOR
    ret.width = 32;
    ret.height = 42;

    flor->ret = ret;
    flor->vel = (Vector2){0};
    flor->cor = cor;

    flor->alturaMinima = ret.y + 80;
    flor->alturaMaxima = ret.y - 80;
    flor->velocidadeMovimento = 60.0f;
    flor->tempoEstado = 0.0f;
    flor->tempoEspera = 1.5f;
    flor->tempoAtaque = 2.0f;
    flor->jogadorProximo = false;

    flor->estado = ESTADO_FLOR_CARNIVORA_ESPREITANDO;
    flor->ativo = true;
    flor->olhandoParaDireita = false;

    int quantidadeAnimacoes = 0;

    // ===== ANIMAÇÃO ESPREITANDO (1 quadro - boca fechada) =====
    flor->animacaoEspreitando.quantidadeQuadros = 1;
    flor->animacaoEspreitando.quadroAtual = 0;
    flor->animacaoEspreitando.contadorTempoQuadro = 0.0f;
    flor->animacaoEspreitando.pararNoUltimoQuadro = false;
    flor->animacaoEspreitando.executarUmaVez = false;
    flor->animacaoEspreitando.finalizada = false;
    criarQuadrosAnimacao(&flor->animacaoEspreitando,
                         flor->animacaoEspreitando.quantidadeQuadros);
    inicializarQuadrosAnimacao(
        flor->animacaoEspreitando.quadros,
        flor->animacaoEspreitando.quantidadeQuadros,
        500,
        1, 309, 
        16, 21,
        1,
        false,
        (Rectangle){2, 2, 12, 17});

    // ===== ANIMAÇÃO APARECENDO
    flor->animacaoAparecendo.quantidadeQuadros = 2;
    flor->animacaoAparecendo.quadroAtual = 0;
    flor->animacaoAparecendo.contadorTempoQuadro = 0.0f;
    flor->animacaoAparecendo.pararNoUltimoQuadro = false;
    flor->animacaoAparecendo.executarUmaVez = false;
    flor->animacaoAparecendo.finalizada = false;
    criarQuadrosAnimacao(&flor->animacaoAparecendo,
                         flor->animacaoAparecendo.quantidadeQuadros);
    inicializarQuadrosAnimacao(
        flor->animacaoAparecendo.quadros,
        flor->animacaoAparecendo.quantidadeQuadros,
        150,
        1, 309,
        16, 21,
        1,
        false,
        (Rectangle){2, 2, 12, 17});

    // ===== ANIMAÇÃO ATACANDO 
    flor->animacaoAtacando.quantidadeQuadros = 2;
    flor->animacaoAtacando.quadroAtual = 0;
    flor->animacaoAtacando.contadorTempoQuadro = 0.0f;
    flor->animacaoAtacando.pararNoUltimoQuadro = false; // NÃO PARA NO ÚLTIMO QUADRO
    flor->animacaoAtacando.executarUmaVez = false;      // NÃO EXECUTA UMA VEZ SÓ
    flor->animacaoAtacando.finalizada = false;
    criarQuadrosAnimacao(&flor->animacaoAtacando,
                         flor->animacaoAtacando.quantidadeQuadros);

    // Inicializa os 2 quadros da animação de ataque
    // Quadro 0: boca fechada
    flor->animacaoAtacando.quadros[0].fonte = (Rectangle){1, 309, 16, 21};
    flor->animacaoAtacando.quadros[0].duracao = 150; // 150ms
    flor->animacaoAtacando.quadros[0].retColisao = (Rectangle){2, 2, 12, 17};

    // Quadro 1: boca aberta
    flor->animacaoAtacando.quadros[1].fonte = (Rectangle){35, 309, 16, 21};
    flor->animacaoAtacando.quadros[1].duracao = 150; // 150ms
    flor->animacaoAtacando.quadros[1].retColisao = (Rectangle){2, 2, 12, 17};

    // ===== ANIMAÇÃO RECUANDO (2 quadros - descendo) =====
    flor->animacaoRecuando.quantidadeQuadros = 2;
    flor->animacaoRecuando.quadroAtual = 0;
    flor->animacaoRecuando.contadorTempoQuadro = 0.0f;
    flor->animacaoRecuando.pararNoUltimoQuadro = false;
    flor->animacaoRecuando.executarUmaVez = false;
    flor->animacaoRecuando.finalizada = false;
    criarQuadrosAnimacao(&flor->animacaoRecuando,
                         flor->animacaoRecuando.quantidadeQuadros);
    inicializarQuadrosAnimacao(
        flor->animacaoRecuando.quadros,
        flor->animacaoRecuando.quantidadeQuadros,
        150,
        1, 309,
        16, 21,
        1,
        false,
        (Rectangle){2, 2, 12, 17});

    // ===== ANIMAÇÃO MORRENDO (4 quadros) =====
    flor->animacaoMorrendo.quantidadeQuadros = 4;
    flor->animacaoMorrendo.quadroAtual = 0;
    flor->animacaoMorrendo.contadorTempoQuadro = 0.0f;
    flor->animacaoMorrendo.pararNoUltimoQuadro = true;
    flor->animacaoMorrendo.executarUmaVez = true;
    flor->animacaoMorrendo.finalizada = false;
    criarQuadrosAnimacao(&flor->animacaoMorrendo,
                         flor->animacaoMorrendo.quantidadeQuadros);
    inicializarQuadrosAnimacao(
        flor->animacaoMorrendo.quadros,
        flor->animacaoMorrendo.quantidadeQuadros,
        100,
        1, 309,
        16, 21,
        1,
        false,
        (Rectangle){0});

    // ===== REGISTRAR ANIMAÇÕES =====
    flor->animacoes[ESTADO_FLOR_CARNIVORA_ESPREITANDO] = &flor->animacaoEspreitando;
    quantidadeAnimacoes++;
    flor->animacoes[ESTADO_FLOR_CARNIVORA_APARECENDO] = &flor->animacaoAparecendo;
    quantidadeAnimacoes++;
    flor->animacoes[ESTADO_FLOR_CARNIVORA_ATACANDO] = &flor->animacaoAtacando;
    quantidadeAnimacoes++;
    flor->animacoes[ESTADO_FLOR_CARNIVORA_RECUANDO] = &flor->animacaoRecuando;
    quantidadeAnimacoes++;
    flor->animacoes[ESTADO_FLOR_CARNIVORA_MORRENDO] = &flor->animacaoMorrendo;
    quantidadeAnimacoes++;

    flor->quantidadeAnimacoes = quantidadeAnimacoes;

    return flor;
}

void destruirInimigoFlorCarnivora(InimigoFlorCarnivora *flor)
{
    if (flor != NULL)
    {
        destruirQuadrosAnimacao(&flor->animacaoEspreitando);
        destruirQuadrosAnimacao(&flor->animacaoAparecendo);
        destruirQuadrosAnimacao(&flor->animacaoAtacando);
        destruirQuadrosAnimacao(&flor->animacaoRecuando);
        destruirQuadrosAnimacao(&flor->animacaoMorrendo);
        free(flor);
    }
}

void atualizarInimigoFlorCarnivora(InimigoFlorCarnivora *flor, GameWorld *gw, float delta)
{
    if (!flor->ativo)
        return;

    // Atualiza a animação atual
    Animacao *animacaoAtual = getAnimacaoAtualInimigoFlorCarnivora(flor);
    atualizarAnimacao(animacaoAtual, delta);

    flor->tempoEstado += delta;

    // Verifica se o jogador está próximo
    if (gw->personagem != NULL)
    {
        Personagem *jogador = (Personagem *)gw->personagem;
        float posXJogador = 0.0f;

        if (jogador->dados != NULL)
        {
            Mario *mario = (Mario *)jogador->dados;
            posXJogador = mario->ret.x;
        }

        float distancia = fabsf(posXJogador - flor->ret.x);
        flor->jogadorProximo = (distancia < 350.0f);
    }

    switch (flor->estado)
    {
    case ESTADO_FLOR_CARNIVORA_ESPREITANDO:
        flor->vel.y = 0;
        flor->ret.y = flor->alturaMinima;

        if (flor->jogadorProximo && flor->tempoEstado >= flor->tempoEspera)
        {
            flor->estado = ESTADO_FLOR_CARNIVORA_APARECENDO;
            flor->tempoEstado = 0.0f;
            // Reseta a animação de ataque para começar do quadro 0
            flor->animacaoAtacando.quadroAtual = 0;
            flor->animacaoAtacando.contadorTempoQuadro = 0.0f;
        }
        break;

    case ESTADO_FLOR_CARNIVORA_APARECENDO:
        flor->vel.y = -flor->velocidadeMovimento;
        flor->ret.y += flor->vel.y * delta;

        if (flor->ret.y <= flor->alturaMaxima)
        {
            flor->ret.y = flor->alturaMaxima;
            flor->vel.y = 0;
            flor->estado = ESTADO_FLOR_CARNIVORA_ATACANDO;
            flor->tempoEstado = 0.0f;
        }
        break;

    case ESTADO_FLOR_CARNIVORA_ATACANDO:
        flor->vel.y = 0;

        // A animação já está alternando entre os 2 quadros automaticamente
        // via atualizarAnimacao() chamado no início da função

        if (flor->tempoEstado >= flor->tempoAtaque)
        {
            flor->estado = ESTADO_FLOR_CARNIVORA_RECUANDO;
            flor->tempoEstado = 0.0f;
        }
        break;

    case ESTADO_FLOR_CARNIVORA_RECUANDO:
        flor->vel.y = flor->velocidadeMovimento;
        flor->ret.y += flor->vel.y * delta;

        if (flor->ret.y >= flor->alturaMinima)
        {
            flor->ret.y = flor->alturaMinima;
            flor->vel.y = 0;
            flor->estado = ESTADO_FLOR_CARNIVORA_ESPREITANDO;
            flor->tempoEstado = 0.0f;
        }
        break;

    case ESTADO_FLOR_CARNIVORA_MORRENDO:
        if (flor->animacaoMorrendo.finalizada)
        {
            flor->ativo = false;
        }
        break;

    default:
        break;
    }
}

void desenharInimigoFlorCarnivora(InimigoFlorCarnivora *flor)
{
    if (!flor->ativo)
        return;

    QuadroAnimacao *qa = getQuadroAnimacaoAtualInimigoFlorCarnivora(flor);
    desenharQuadroAnimacaoInimigoFlorCarnivora(flor, qa, WHITE);

    if (MOSTRAR_RETANGULOS)
    {
        DrawRectangleRec(flor->ret, Fade(flor->cor, 0.5f));
        DrawRectangleLines(flor->ret.x, flor->ret.y,
                           flor->ret.width, flor->ret.height, BLACK);
    }
}

static void desenharQuadroAnimacaoInimigoFlorCarnivora(InimigoFlorCarnivora *flor,
                                                       QuadroAnimacao *qa,
                                                       Color tonalidade)
{
    if (qa != NULL)
    {
        DrawTexturePro(rm.texturaBadniks,
                       (Rectangle){qa->fonte.x, qa->fonte.y,
                                   flor->olhandoParaDireita ? -qa->fonte.width : qa->fonte.width,
                                   qa->fonte.height},
                       flor->ret,
                       (Vector2){0},
                       0.0f,
                       tonalidade);
    }
}

static Animacao *getAnimacaoAtualInimigoFlorCarnivora(InimigoFlorCarnivora *flor)
{
    if ((int)flor->estado >= 0 && (int)flor->estado < flor->quantidadeAnimacoes)
    {
        return flor->animacoes[flor->estado];
    }
    return NULL;
}

QuadroAnimacao *getQuadroAnimacaoAtualInimigoFlorCarnivora(InimigoFlorCarnivora *flor)
{
    return getQuadroAtualAnimacao(getAnimacaoAtualInimigoFlorCarnivora(flor));
}