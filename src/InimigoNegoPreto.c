/**
 * @file InimigoNegoPreto.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Inimigo (NegoPreto) 
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoNegoPreto.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Tipos.h"

static const bool MOSTRAR_RETANGULOS = true;

static Animacao *getAnimacaoAtualInimigoNegoPreto(InimigoNegoPreto *inimigo)
{
    return inimigo->animacoes[inimigo->estado];
}

static void desenharQuadroAnimacaoInimigoNegoPreto(InimigoNegoPreto *inimigo, QuadroAnimacao *qa, Color tonalidade)
{
    if (qa != NULL)
    {
        DrawTexturePro(
            rm.texturaBadniks, 
            (Rectangle){
                qa->fonte.x,
                qa->fonte.y,
                inimigo->olhandoParaDireita ? -qa->fonte.width : qa->fonte.width, 
                qa->fonte.height
            },
            inimigo->ret,
            (Vector2){0},
            0.0f,
            tonalidade
        );

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

InimigoNegoPreto *criarInimigoNegoPreto(Rectangle ret, Color cor)
{
    InimigoNegoPreto *novoInimigo = (InimigoNegoPreto *)malloc(sizeof(InimigoNegoPreto));

    novoInimigo->ret = ret;
    novoInimigo->vel = (Vector2){0};
    novoInimigo->cor = cor;
    novoInimigo->velVoo = 150.0f;
    novoInimigo->estado = ESTADO_INIMIGO_NEGOPRETO_VOANDO; 
    novoInimigo->ativo = true;
    novoInimigo->olhandoParaDireita = false; 

    int quantidadeAnimacoes = 0;

    novoInimigo->animacaoVoando.quantidadeQuadros = 1; 
    novoInimigo->animacaoVoando.quadroAtual = 0;
    novoInimigo->animacaoVoando.contadorTempoQuadro = 0.0f;
    novoInimigo->animacaoVoando.pararNoUltimoQuadro = false;
    novoInimigo->animacaoVoando.executarUmaVez = false;
    novoInimigo->animacaoVoando.finalizada = false;
    criarQuadrosAnimacao(&novoInimigo->animacaoVoando, novoInimigo->animacaoVoando.quantidadeQuadros);

    inicializarQuadrosAnimacao(
        novoInimigo->animacaoVoando.quadros,
        novoInimigo->animacaoVoando.quantidadeQuadros,
        100,    
        1, 180,   
        128, 128, 
        0,      
        false,
        (Rectangle){0, 0, 128, 128} 
    );

    novoInimigo->animacoes[ESTADO_INIMIGO_NEGOPRETO_VOANDO] = &novoInimigo->animacaoVoando;
    quantidadeAnimacoes++;

    novoInimigo->quantidadeAnimacoes = quantidadeAnimacoes;

    return novoInimigo;
}

void destruirInimigoNegoPreto(InimigoNegoPreto *inimigo)
{
    if (inimigo != NULL)
    {
        for (int i = 0; i < inimigo->quantidadeAnimacoes; i++)
        {
            if (inimigo->animacoes[i] != NULL)
            {
                destruirQuadrosAnimacao(inimigo->animacoes[i]);
            }
        }
        free(inimigo);
    }
}

void atualizarInimigoNegoPreto(InimigoNegoPreto *inimigo, GameWorld *gw, float delta)
{
    if (inimigo->ativo)
    {
        if (inimigo->estado == ESTADO_INIMIGO_NEGOPRETO_VOANDO)
        {
            Animacao *animacaoAtual = getAnimacaoAtualInimigoNegoPreto(inimigo);
            atualizarAnimacao(animacaoAtual, delta);

            if (inimigo->olhandoParaDireita)
            {
                inimigo->vel.x = inimigo->velVoo;
            }
            else
            {
                inimigo->vel.x = -inimigo->velVoo;
            }

            inimigo->ret.x += inimigo->vel.x * delta;

            if (inimigo->ret.x < -200 || inimigo->ret.x > GetScreenWidth() + 200)
            {
                inimigo->ativo = false;
            }
        }
    }
}

void desenharInimigoNegoPreto(InimigoNegoPreto *inimigo)
{
    if (inimigo->ativo)
    {
        if (inimigo->estado == ESTADO_INIMIGO_NEGOPRETO_VOANDO)
        {
            QuadroAnimacao *qa = getQuadroAtualAnimacao(getAnimacaoAtualInimigoNegoPreto(inimigo));
            desenharQuadroAnimacaoInimigoNegoPreto(inimigo, qa, WHITE);
        }

        if (MOSTRAR_RETANGULOS)
        {
            DrawRectangleRec(inimigo->ret, Fade(inimigo->cor, 0.5f));
            DrawRectangleLines(inimigo->ret.x, inimigo->ret.y, inimigo->ret.width, inimigo->ret.height, BLACK);
        }
    }
}

QuadroAnimacao *getQuadroAnimacaoAtualInimigoNegoPreto(InimigoNegoPreto *inimigo)
{
    return getQuadroAtualAnimacao(getAnimacaoAtualInimigoNegoPreto(inimigo));
}