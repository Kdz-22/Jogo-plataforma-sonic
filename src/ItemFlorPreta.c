/**
 * @file ItemFlorPreta.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Item (FlorPreta).
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "Animacao.h"
#include "ItemFlorPreta.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void desenharQuadroAnimacaoItemFlorPreta(ItemFlorPreta *item, QuadroAnimacao *qa, Color tonalidade);
static Animacao *getAnimacaoAtualItemFlorPreta(ItemFlorPreta *item);

static const bool MOSTRAR_RETANGULOS = false;

/**
 * @brief Cria um novo Item (florpreta).
 */
ItemFlorPreta *criarItemFlorPreta(Rectangle ret, Color cor) {

    ItemFlorPreta *novoItem = (ItemFlorPreta*) malloc(sizeof(ItemFlorPreta));

    novoItem->ret = ret;
    novoItem->cor = cor;
    novoItem->estado = ESTADO_ITEM_FLOR_PRETA_PARADA;
    novoItem->ativo = true;

    int quantidadeAnimacoes = 0;

    // Estado PARADO
    novoItem->animacaoParado.quantidadeQuadros = 4;
    novoItem->animacaoParado.quadroAtual = 0;
    novoItem->animacaoParado.contadorTempoQuadro = 0.0f;
    novoItem->animacaoParado.pararNoUltimoQuadro = false;
    novoItem->animacaoParado.executarUmaVez = false;
    novoItem->animacaoParado.finalizada = false;
    criarQuadrosAnimacao(&novoItem->animacaoParado, novoItem->animacaoParado.quantidadeQuadros);
    inicializarQuadrosAnimacao( 
        novoItem->animacaoParado.quadros,
        novoItem->animacaoParado.quantidadeQuadros,
        100,
        1, 119,
        16, 16,
        1,
        false,
        (Rectangle) {2, 2, 32, 32}
    );

    // Estado SAINDO_DO_BLOCO
    novoItem->animacaoSaindoDoBloco.quantidadeQuadros = 4;
    novoItem->animacaoSaindoDoBloco.quadroAtual = 0;
    novoItem->animacaoSaindoDoBloco.contadorTempoQuadro = 0.0f;
    novoItem->animacaoSaindoDoBloco.pararNoUltimoQuadro = true;
    novoItem->animacaoSaindoDoBloco.executarUmaVez = true;
    novoItem->animacaoSaindoDoBloco.finalizada = false;
    criarQuadrosAnimacao(&novoItem->animacaoSaindoDoBloco, novoItem->animacaoSaindoDoBloco.quantidadeQuadros);
    inicializarQuadrosAnimacao( 
        novoItem->animacaoSaindoDoBloco.quadros,
        novoItem->animacaoSaindoDoBloco.quantidadeQuadros,
        80,
        1, 119,
        16, 16,
        1,
        false,
        (Rectangle) {0, 0, 32, 32}
    );

    // Estado COLETADO (atacando/disparando)
    novoItem->animacaoColetando.quantidadeQuadros = 4;
    novoItem->animacaoColetando.quadroAtual = 0;
    novoItem->animacaoColetando.contadorTempoQuadro = 0.0f;
    novoItem->animacaoColetando.pararNoUltimoQuadro = false;
    novoItem->animacaoColetando.executarUmaVez = false;
    novoItem->animacaoColetando.finalizada = false;
    criarQuadrosAnimacao(&novoItem->animacaoColetando, novoItem->animacaoColetando.quantidadeQuadros);
    inicializarQuadrosAnimacao( 
        novoItem->animacaoColetando.quadros,
        novoItem->animacaoColetando.quantidadeQuadros,
        80,
        1, 9,
        16, 16,
        1,
        false,
        (Rectangle) {0, 0, 32, 32}
    );

    novoItem->animacoes[ESTADO_ITEM_FLOR_PRETA_PARADA] = &novoItem->animacaoParado; 
    quantidadeAnimacoes++;
    novoItem->animacoes[ESTADO_ITEM_FLOR_PRETA_SAINDO_DO_BLOCO] = &novoItem->animacaoSaindoDoBloco; 
    quantidadeAnimacoes++;
    novoItem->animacoes[ESTADO_ITEM_FLOR_PRETA_COLETADA] = &novoItem->animacaoColetando; 
    quantidadeAnimacoes++;
    novoItem->quantidadeAnimacoes = quantidadeAnimacoes;

    return novoItem;
}

/**
 * @brief Destroi um item (florpreta).
 */
void destruirItemFlorPreta(ItemFlorPreta *item) {
    if (item != NULL) {
        for (int i = 0; i < item->quantidadeAnimacoes; i++) {
            destruirQuadrosAnimacao(item->animacoes[i]);
        }
        free(item);
    }
}

/**
 * @brief Atualiza um item (florpreta).
 */
void atualizarItemFlorPreta(ItemFlorPreta *item, float delta) {
    if (item->ativo) {
        Animacao *animacaoAtual = getAnimacaoAtualItemFlorPreta(item);
        atualizarAnimacao(animacaoAtual, delta);
        
        // Transição: SAINDO_DO_BLOCO -> COLETADA (atacando)
        if (item->estado == ESTADO_ITEM_FLOR_PRETA_SAINDO_DO_BLOCO && animacaoAtual->finalizada) {
            item->estado = ESTADO_ITEM_FLOR_PRETA_COLETADA;
        }
    }
}

/**
 * @brief Desenha um item (florpreta).
 */
void desenharItemFlorPreta(ItemFlorPreta *item) {
    if (item->ativo) {
        QuadroAnimacao *qa = getQuadroAnimacaoAtualItemFlorPreta(item);
        desenharQuadroAnimacaoItemFlorPreta(item, qa, WHITE);
        if (MOSTRAR_RETANGULOS) {
            DrawRectangleRec(item->ret, Fade(item->cor, 0.5f));
            DrawRectangleLines(item->ret.x, item->ret.y, item->ret.width, item->ret.height, BLACK);
        }
    }
}

/**
 * @brief Obtém o quadro de animação atual de um item (florpreta).
 */
QuadroAnimacao *getQuadroAnimacaoAtualItemFlorPreta(ItemFlorPreta *item) {
    return getQuadroAtualAnimacao(getAnimacaoAtualItemFlorPreta(item));
}

static void desenharQuadroAnimacaoItemFlorPreta(ItemFlorPreta *item, QuadroAnimacao *qa, Color tonalidade) {

    if (qa != NULL) {

        DrawTexturePro(
            rm.texturaItens,
            qa->fonte,
            item->ret,
            (Vector2) {0},
            0.0f,
            tonalidade
        );

        if (MOSTRAR_RETANGULOS) {
            float xDesenho = item->ret.x + qa->retColisao.x;
            float yDesenho = item->ret.y + qa->retColisao.y;
            DrawRectangle(xDesenho, yDesenho, qa->retColisao.width, qa->retColisao.height, Fade(GREEN, 0.5f));
        }
    }
}

static Animacao *getAnimacaoAtualItemFlorPreta(ItemFlorPreta *item) {
    return item->animacoes[item->estado];
}