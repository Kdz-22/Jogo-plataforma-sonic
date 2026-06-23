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

    InimigoKoopaRed *novoInimigo =
        (InimigoKoopaRed *)malloc(sizeof(InimigoKoopaRed));

    novoInimigo->ret = ret;
    novoInimigo->vel = (Vector2){0};
    novoInimigo->cor = cor;

    novoInimigo->velAndando = 100;
    novoInimigo->velMaxQueda = 600;
    novoInimigo->estado = ESTADO_KOOPA_ANDANDO; // Inicializa andando
    novoInimigo->ativo = true;
    novoInimigo->olhandoParaDireita = false;

    int quantidadeAnimacoes = 0;

    // No tamanho original de SNES, os Koopas andando usam 2 quadros
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
        250,                             // duração de cada frame (ms)
        1, 115,                          // Início X, Y na Texture
        16, 28,                          // Dimensões do sprite (Largura, Altura de SNES)
        1,                               // Separação de pixels entre frames
        false, (Rectangle){2, 2, 68, 58} // Hitbox interna relativa ao sprite
    );

    novoInimigo->animacaoCasco.quantidadeQuadros = 1;
    novoInimigo->animacaoCasco.quadroAtual = 0;
    novoInimigo->animacaoCasco.contadorTempoQuadro = 0.0f;
    novoInimigo->animacaoCasco.pararNoUltimoQuadro = true;
    novoInimigo->animacaoCasco.executarUmaVez = false;
    novoInimigo->animacaoCasco.finalizada = false;

    criarQuadrosAnimacao(&novoInimigo->animacaoCasco,
                         novoInimigo->animacaoCasco.quantidadeQuadros);

    inicializarQuadrosAnimacao(
        novoInimigo->animacaoCasco.quadros,
        novoInimigo->animacaoCasco.quantidadeQuadros,
        1000,                     // duração padrão para o quadro estático (1 segundo)
        35, 113,                  // início (coordenadas X, Y do editor)
        32, 34,                   // dimensões reais do novo casco (Largura, Altura)
        0,                        // separação (não há outros frames colados)
        false,                    // de trás para frente
        (Rectangle){2, 2, 32, 34} // retângulo de colisão cobrindo o casco inteiro
    );

    novoInimigo->animacoes[ESTADO_KOOPA_ANDANDO] =
        &novoInimigo->animacaoAndando;

    novoInimigo->animacoes[ESTADO_KOOPA_CASCO_PARADO] =
        &novoInimigo->animacaoCasco;

    novoInimigo->animacoes[ESTADO_KOOPA_CASCO_CORRENDO] =
        &novoInimigo->animacaoCasco; // Usa o mesmo casco para correr por enquanto

    novoInimigo->animacoes[ESTADO_KOOPA_MORRENDO] =
        &novoInimigo->animacaoCasco;
    // Usa o mesmo casco para o estado morrendo
    // Atualiza a quantidade total de animações mapeadas no array
    novoInimigo->quantidadeAnimacoes = 4;

    return novoInimigo;
}

/**
 * @brief Destroi um inimigo (koopared).
 */
void destruirInimigoKoopaRed(InimigoKoopaRed *inimigo)
{
    if (inimigo != NULL)
    {
        printf("    Destruindo structs reais de animacao...\n");

        // Libera apenas as duas estruturas reais de memória criadas
        destruirQuadrosAnimacao(&inimigo->animacaoAndando);
        destruirQuadrosAnimacao(&inimigo->animacaoCasco);

        printf("    liberando inimigo...\n");
        free(inimigo);
        printf("    liberado!\n");
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
            inimigo->ativo = false;
            // O gerenciador de listas do jogo agora sabe que pode chamar o
            // destruirInimigoKoopaRed de forma totalmente segura fora dos loops!
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