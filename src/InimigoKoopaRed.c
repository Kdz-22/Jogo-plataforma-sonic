/**
 * @file Inimigo.c
 * @author Prof. Dr. David Buzatto (Adaptado para KoopaRed)
 * @brief Implementação do Inimigo (KoopaRed) com mecânica de casco.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoKoopaRed.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void desenharQuadroAnimacaoInimigoKoopaRed(InimigoKoopaRed *inimigo,
                                                  QuadroAnimacao *qa,
                                                  Color tonalidade);
static Animacao *getAnimacaoAtualInimigoKoopaRed(InimigoKoopaRed *inimigo);

static const bool MOSTRAR_RETANGULOS = false;

/**
 * @brief Cria um novo Inimigo (koopared).
 */
InimigoKoopaRed *criarInimigoKoopaRed(Rectangle ret, Color cor)
{
    InimigoKoopaRed *novoInimigo = (InimigoKoopaRed *)malloc(sizeof(InimigoKoopaRed));

    novoInimigo->ret = ret;
    novoInimigo->vel = (Vector2){0};
    novoInimigo->cor = cor;

    novoInimigo->velAndando = 100;
    novoInimigo->velMaxQueda = 600;
    novoInimigo->estado = ESTADO_KOOPA_ANDANDO;
    novoInimigo->ativo = true;
    novoInimigo->olhandoParaDireita = false;

    int quantidadeAnimacoes = 0;

    //ANIMAÇÃO ANDANDO
    novoInimigo->animacaoAndando.quantidadeQuadros = 2;
    novoInimigo->animacaoAndando.quadroAtual = 0;
    novoInimigo->animacaoAndando.contadorTempoQuadro = 0.0f;
    novoInimigo->animacaoAndando.pararNoUltimoQuadro = false;
    novoInimigo->animacaoAndando.executarUmaVez = false;
    novoInimigo->animacaoAndando.finalizada = false;
    criarQuadrosAnimacao(&novoInimigo->animacaoAndando,
                         novoInimigo->animacaoAndando.quantidadeQuadros);
    inicializarQuadrosAnimacao(
        novoInimigo->animacaoAndando.quadros,
        novoInimigo->animacaoAndando.quantidadeQuadros,
        250,
        1, 115,
        16, 28,
        1,
        false,
        (Rectangle){2, 2, 68, 58});

    //ANIMAÇÃO CASCO PARADO
    novoInimigo->animacaoCascoParado.quantidadeQuadros = 1;
    novoInimigo->animacaoCascoParado.quadroAtual = 0;
    novoInimigo->animacaoCascoParado.contadorTempoQuadro = 0.0f;
    novoInimigo->animacaoCascoParado.pararNoUltimoQuadro = true;
    novoInimigo->animacaoCascoParado.executarUmaVez = false;
    novoInimigo->animacaoCascoParado.finalizada = false;
    criarQuadrosAnimacao(&novoInimigo->animacaoCascoParado,
                         novoInimigo->animacaoCascoParado.quantidadeQuadros);
    inicializarQuadrosAnimacao(
        novoInimigo->animacaoCascoParado.quadros,
        novoInimigo->animacaoCascoParado.quantidadeQuadros,
        1000,
        35, 113,
        32, 34,
        0,
        false,
        (Rectangle){2, 2, 32, 34});

    //ANIMAÇÃO CASCO CORRENDO
    novoInimigo->animacaoCascoCorrendo.quantidadeQuadros = 4;
    novoInimigo->animacaoCascoCorrendo.quadroAtual = 0;
    novoInimigo->animacaoCascoCorrendo.contadorTempoQuadro = 0.0f;
    novoInimigo->animacaoCascoCorrendo.pararNoUltimoQuadro = false;
    novoInimigo->animacaoCascoCorrendo.executarUmaVez = false;
    novoInimigo->animacaoCascoCorrendo.finalizada = false;
    criarQuadrosAnimacao(&novoInimigo->animacaoCascoCorrendo,
                         novoInimigo->animacaoCascoCorrendo.quantidadeQuadros);

    // Inicializa os 4 quadros do casco girando
    inicializarQuadrosAnimacao(
        novoInimigo->animacaoCascoCorrendo.quadros,
        novoInimigo->animacaoCascoCorrendo.quantidadeQuadros,
        50,      // duração de cada quadro (mais rápido = mais giro)
        1, 331, // posição inicial do primeiro quadro
        32, 32,  // dimensões de cada quadro
        1,       // separação entre quadros
        false,
        (Rectangle){2, 2, 32, 32});

    // SE OS QUADROS ESTIVEREM EM POSIÇÕES DIFERENTES, USE ISSO:
    /*
    // Quadro 0
    novoInimigo->animacaoCascoCorrendo.quadros[0].fonte = (Rectangle){35, 113, 32, 34};
    novoInimigo->animacaoCascoCorrendo.quadros[0].duracao = 50;
    novoInimigo->animacaoCascoCorrendo.quadros[0].retColisao = (Rectangle){2, 2, 32, 34};

    // Quadro 1
    novoInimigo->animacaoCascoCorrendo.quadros[1].fonte = (Rectangle){67, 113, 32, 34};
    novoInimigo->animacaoCascoCorrendo.quadros[1].duracao = 50;
    novoInimigo->animacaoCascoCorrendo.quadros[1].retColisao = (Rectangle){2, 2, 32, 34};

    // Quadro 2
    novoInimigo->animacaoCascoCorrendo.quadros[2].fonte = (Rectangle){99, 113, 32, 34};
    novoInimigo->animacaoCascoCorrendo.quadros[2].duracao = 50;
    novoInimigo->animacaoCascoCorrendo.quadros[2].retColisao = (Rectangle){2, 2, 32, 34};

    // Quadro 3
    novoInimigo->animacaoCascoCorrendo.quadros[3].fonte = (Rectangle){131, 113, 32, 34};
    novoInimigo->animacaoCascoCorrendo.quadros[3].duracao = 50;
    novoInimigo->animacaoCascoCorrendo.quadros[3].retColisao = (Rectangle){2, 2, 32, 34};
    */

    // ===== ANIMAÇÃO MORRENDO =====
    novoInimigo->animacaoMorrendo.quantidadeQuadros = 4;
    novoInimigo->animacaoMorrendo.quadroAtual = 0;
    novoInimigo->animacaoMorrendo.contadorTempoQuadro = 0.0f;
    novoInimigo->animacaoMorrendo.pararNoUltimoQuadro = true;
    novoInimigo->animacaoMorrendo.executarUmaVez = true;
    novoInimigo->animacaoMorrendo.finalizada = false;
    criarQuadrosAnimacao(&novoInimigo->animacaoMorrendo,
                         novoInimigo->animacaoMorrendo.quantidadeQuadros);
    inicializarQuadrosAnimacao(
        novoInimigo->animacaoMorrendo.quadros,
        novoInimigo->animacaoMorrendo.quantidadeQuadros,
        100,
        67, 113, // posição inicial (ajuste conforme sua sprite sheet)
        32, 34,
        0,
        false,
        (Rectangle){0});

    // ===== REGISTRAR ANIMAÇÕES =====
    novoInimigo->animacoes[ESTADO_KOOPA_ANDANDO] = &novoInimigo->animacaoAndando;
    quantidadeAnimacoes++;

    novoInimigo->animacoes[ESTADO_KOOPA_CASCO_PARADO] = &novoInimigo->animacaoCascoParado;
    quantidadeAnimacoes++;

    novoInimigo->animacoes[ESTADO_KOOPA_CASCO_CORRENDO] = &novoInimigo->animacaoCascoCorrendo;
    quantidadeAnimacoes++;

    novoInimigo->animacoes[ESTADO_KOOPA_MORRENDO] = &novoInimigo->animacaoMorrendo;
    quantidadeAnimacoes++;

    novoInimigo->quantidadeAnimacoes = quantidadeAnimacoes;

    return novoInimigo;
}

/**
 * @brief Destroi um inimigo (koopared).
 */
void destruirInimigoKoopaRed(InimigoKoopaRed *inimigo)
{
    if (inimigo != NULL)
    {
        // Libera todas as animações
        destruirQuadrosAnimacao(&inimigo->animacaoAndando);
        destruirQuadrosAnimacao(&inimigo->animacaoCascoParado);
        destruirQuadrosAnimacao(&inimigo->animacaoCascoCorrendo);
        destruirQuadrosAnimacao(&inimigo->animacaoMorrendo);

        free(inimigo);
    }
}

/**
 * @brief Atualiza um inimigo (koopared).
 */
void atualizarInimigoKoopaRed(InimigoKoopaRed *inimigo, GameWorld *gw,
                              float delta)
{

    if (inimigo->ativo)
    {

        Animacao *animacaoAtual = getAnimacaoAtualInimigoKoopaRed(inimigo);
        atualizarAnimacao(animacaoAtual, delta);

        if (inimigo->estado == ESTADO_KOOPA_ANDANDO)
        {
            // Anda normal para o lado que está olhando
            if (inimigo->olhandoParaDireita)
            {
                inimigo->vel.x = inimigo->velAndando;
            }
            else
            {
                inimigo->vel.x = -inimigo->velAndando;
            }
        }
        else if (inimigo->estado == ESTADO_KOOPA_CASCO_PARADO)
        {
            // Fica completamente parado no chão
            inimigo->vel.x = 0;
        }
        else if (inimigo->estado == ESTADO_KOOPA_CASCO_CORRENDO)
        {
            if (inimigo->olhandoParaDireita)
            {
                inimigo->vel.x = inimigo->velAndando; // velAndando aqui será o valor alto (300/350) definido no chute
            }
            else
            {
                inimigo->vel.x = -inimigo->velAndando;
            }
        }

        inimigo->vel.y += gw->gravidade * delta;
        if (inimigo->vel.y > inimigo->velMaxQueda)
        {
            inimigo->vel.y = inimigo->velMaxQueda;
        }

        // Interface para o resolvedor de colisões genérico do mapa do professor
        Inimigo ini = {.objeto = inimigo, .tipo = TIPO_INIMIGO_KOOPARED};

        // Aplica o movimento e resolve colisão no eixo X
        inimigo->ret.x += inimigo->vel.x * delta;
        resolverColisaoInimigoObstaculosMapaX(&ini, gw->mapa);

        // Aplica o movimento e resolve colisão no eixo Y
        inimigo->ret.y += inimigo->vel.y * delta;
        resolverColisaoInimigoObstaculosMapaY(&ini, gw->mapa);

        if (inimigo->estado == ESTADO_KOOPA_MORRENDO)
        {
            // A animação de morte pode tocar antes de desativar
            if (inimigo->animacaoMorrendo.finalizada)
            {
                inimigo->ativo = false;
            }
        }
    }
}

/**
 * @brief Desenha um inimigo (koopared).
 */
void desenharInimigoKoopaRed(InimigoKoopaRed *inimigo)
{

    if (inimigo->ativo)
    {
        // Pega o quadro da animação baseado no estado atual (Andando ou Casco)
        QuadroAnimacao *qa = getQuadroAnimacaoAtualInimigoKoopaRed(inimigo);
        desenharQuadroAnimacaoInimigoKoopaRed(inimigo, qa, WHITE);

        if (MOSTRAR_RETANGULOS)
        {
            DrawRectangleRec(inimigo->ret, Fade(inimigo->cor, 0.5f));
            DrawRectangleLines(inimigo->ret.x, inimigo->ret.y,
                               inimigo->ret.width, inimigo->ret.height, BLACK);
        }
    }
}

/**
 * @brief Obtém o quadro de animação atual de um inimigo (koopared).
 */
QuadroAnimacao *
getQuadroAnimacaoAtualInimigoKoopaRed(InimigoKoopaRed *inimigo)
{
    return getQuadroAtualAnimacao(getAnimacaoAtualInimigoKoopaRed(inimigo));
}

static void desenharQuadroAnimacaoInimigoKoopaRed(InimigoKoopaRed *inimigo,
                                                  QuadroAnimacao *qa,
                                                  Color tonalidade)
{

    if (qa != NULL)
    {

        DrawTexturePro(rm.texturaBadniks,
                       (Rectangle){qa->fonte.x, qa->fonte.y,
                                   inimigo->olhandoParaDireita
                                       ? -qa->fonte.width
                                       : qa->fonte.width, // Inverte o sprite
                                   qa->fonte.height},
                       inimigo->ret, (Vector2){0}, 0.0f, tonalidade);

        if (MOSTRAR_RETANGULOS)
        {
            float xDesenho = inimigo->olhandoParaDireita
                                 ? inimigo->ret.x + inimigo->ret.width -
                                       qa->retColisao.x - qa->retColisao.width
                                 : inimigo->ret.x + qa->retColisao.x;
            float yDesenho = inimigo->ret.y + qa->retColisao.y;
            DrawRectangle(xDesenho, yDesenho, qa->retColisao.width,
                          qa->retColisao.height, Fade(GREEN, 0.5f));
        }
    }
}

static Animacao *getAnimacaoAtualInimigoKoopaRed(InimigoKoopaRed *inimigo)
{
    return inimigo->animacoes[inimigo->estado];
}