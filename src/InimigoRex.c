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
#include "InimigoRex.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void desenharQuadroAnimacaoInimigoRex(InimigoRex *inimigo, QuadroAnimacao *qa, Color tonalidade);
static Animacao *getAnimacaoAtualInimigoRex(InimigoRex *inimigo);

static const bool MOSTRAR_RETANGULOS = false;

/**
 * @brief Cria um novo Inimigo Rex.
 */
InimigoRex *criarInimigoRex(Rectangle ret, Color cor)
{
    InimigoRex *novoInimigo = (InimigoRex *)malloc(sizeof(InimigoRex));

    novoInimigo->ret = ret;
    novoInimigo->vel = (Vector2){0};
    novoInimigo->cor = cor;

    novoInimigo->velAndando = 100;
    novoInimigo->velMaxQueda = 600;
    novoInimigo->estado = ESTADO_REX_ANDANDO; // Inicializa andando em pé
    novoInimigo->ativo = true;
    novoInimigo->olhandoParaDireita = false;

    int quantidadeAnimacoes = 0;

    // REX ANDANDO
    novoInimigo->animacaoAndando.quantidadeQuadros = 2;
    novoInimigo->animacaoAndando.quadroAtual = 0;
    novoInimigo->animacaoAndando.contadorTempoQuadro = 0.0f;
    novoInimigo->animacaoAndando.pararNoUltimoQuadro = false;
    novoInimigo->animacaoAndando.executarUmaVez = false;
    novoInimigo->animacaoAndando.finalizada = false;

    criarQuadrosAnimacao(&novoInimigo->animacaoAndando, novoInimigo->animacaoAndando.quantidadeQuadros);

    inicializarQuadrosAnimacao(
        novoInimigo->animacaoAndando.quadros,
        novoInimigo->animacaoAndando.quantidadeQuadros,
        250,
        1, 146, // Início X, Y na sua Texture
        20, 33, // Dimensões do sprite em pé
        1,      // 1 pixel de espaçamento
        false,
        (Rectangle){0, 0, 20, 33});

    // REX ACHATADO
    novoInimigo->animacaoMorrendo.quantidadeQuadros = 1;
    novoInimigo->animacaoMorrendo.quadroAtual = 0;
    novoInimigo->animacaoMorrendo.contadorTempoQuadro = 0.0f;
    novoInimigo->animacaoMorrendo.pararNoUltimoQuadro = true;
    novoInimigo->animacaoMorrendo.executarUmaVez = false;
    novoInimigo->animacaoMorrendo.finalizada = false;

    criarQuadrosAnimacao(&novoInimigo->animacaoMorrendo, novoInimigo->animacaoMorrendo.quantidadeQuadros);

    inicializarQuadrosAnimacao(
        novoInimigo->animacaoMorrendo.quadros,
        novoInimigo->animacaoMorrendo.quantidadeQuadros,
        1000,
        99, 71, // X=68, Y=0 (Último frame achatado)
        16, 8,  // Dimensões dele achatado
        0,
        false,
        (Rectangle){0, 0, 16, 8});

    // Mapeando os estados do seu enum nas posições do array de animações
    novoInimigo->animacoes[ESTADO_REX_ANDANDO] = &novoInimigo->animacaoAndando;
    quantidadeAnimacoes++;

    // Como o array na sua struct só possui tamanho [2], vamos apontar ambos os estados
    // achatados para a mesma animação de frame único!
    novoInimigo->animacoes[ESTADO_REX_ACHATADO_PARADO] = &novoInimigo->animacaoMorrendo;
    quantidadeAnimacoes++;

    novoInimigo->quantidadeAnimacoes = quantidadeAnimacoes;

    return novoInimigo;
}

/**
 * @brief Destroi um inimigo Rex.
 */
void destruirInimigoRex(InimigoRex *inimigo)
{
    if (inimigo != NULL)
    {
        for (int i = 0; i < inimigo->quantidadeAnimacoes; i++)
        {
            destruirQuadrosAnimacao(inimigo->animacoes[i]);
        }
        free(inimigo);
    }
}

/**
 * @brief Atualiza um inimigo Rex.
 */
void atualizarInimigoRex(InimigoRex *inimigo, GameWorld *gw, float delta)
{
    if (inimigo->ativo)
    {
        Animacao *animacaoAtual = getAnimacaoAtualInimigoRex(inimigo);
        atualizarAnimacao(animacaoAtual, delta);

        // Atualizando os movimentos com os nomes do seu novo Enum
        if (inimigo->estado == ESTADO_REX_ANDANDO)
        {
            if (inimigo->olhandoParaDireita)
            {
                inimigo->vel.x = inimigo->velAndando;
            }
            else
            {
                inimigo->vel.x = -inimigo->velAndando;
            }
        }
        else if (inimigo->estado == ESTADO_REX_ACHATADO_PARADO)
        {
            inimigo->vel.x = 0; // Parado
        }
        else if (inimigo->estado == ESTADO_REX_ACHATADO_CORRENDO)
        {
            if (inimigo->olhandoParaDireita)
            {
                inimigo->vel.x = inimigo->velAndando; // Correndo rápido ajustado na colisão
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

        Inimigo ini = {
            .objeto = inimigo,
            .tipo = TIPO_INIMIGO_REX};

        inimigo->ret.x += inimigo->vel.x * delta;
        resolverColisaoInimigoObstaculosMapaX(&ini, gw->mapa);

        inimigo->ret.y += inimigo->vel.y * delta;
        resolverColisaoInimigoObstaculosMapaY(&ini, gw->mapa);
    }
}

/**
 * @brief Desenha um inimigo Rex.
 */
void desenharInimigoRex(InimigoRex *inimigo)
{
    if (inimigo->ativo)
    {
        QuadroAnimacao *qa = getQuadroAnimacaoAtualInimigoRex(inimigo);
        desenharQuadroAnimacaoInimigoRex(inimigo, qa, WHITE);

        if (MOSTRAR_RETANGULOS)
        {
            DrawRectangleRec(inimigo->ret, Fade(inimigo->cor, 0.5f));
            DrawRectangleLines(inimigo->ret.x, inimigo->ret.y, inimigo->ret.width, inimigo->ret.height, BLACK);
        }
    }
}

/**
 * @brief Obtém o quadro de animação atual de um inimigo Rex.
 */
QuadroAnimacao *getQuadroAnimacaoAtualInimigoRex(InimigoRex *inimigo)
{
    return getQuadroAtualAnimacao(getAnimacaoAtualInimigoRex(inimigo));
}

static void desenharQuadroAnimacaoInimigoRex(InimigoRex *inimigo, QuadroAnimacao *qa, Color tonalidade)
{
    if (qa != NULL)
    {
        DrawTexturePro(
            rm.texturaBadniks,
            (Rectangle){
                qa->fonte.x,
                qa->fonte.y,
                inimigo->olhandoParaDireita ? -qa->fonte.width : qa->fonte.width,
                qa->fonte.height},
            inimigo->ret,
            (Vector2){0},
            0.0f,
            tonalidade);

        if (MOSTRAR_RETANGULOS)
        {
            float xDesenho = inimigo->olhandoParaDireita
                                 ? inimigo->ret.x + inimigo->ret.width - qa->retColisao.x - qa->retColisao.width
                                 : inimigo->ret.x + qa->retColisao.x;
            float yDesenho = inimigo->ret.y + qa->retColisao.y;
            DrawRectangle(xDesenho, yDesenho, qa->retColisao.width, qa->retColisao.height, Fade(GREEN, 0.5f));
        }
    }
}

static Animacao *getAnimacaoAtualInimigoRex(InimigoRex *inimigo)
{
    // Proteção para o estado correndo não estourar o índice mapeado [2]
    if (inimigo->estado == ESTADO_REX_ACHATADO_CORRENDO)
    {
        return inimigo->animacoes[ESTADO_REX_ACHATADO_PARADO];
    }
    return inimigo->animacoes[inimigo->estado];
}