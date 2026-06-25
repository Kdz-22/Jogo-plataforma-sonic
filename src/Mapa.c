/**
 * @file Mapa.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Mapa.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "Inimigo.h"
#include "InimigoKoopaRed.h"
#include "InimigoMotobug.h"
#include "InimigoNegoPreto.h"
#include "InimigoRex.h"
#include "InimigoSpikes.h"
#include "InimigoTonTon.h"

#include "Macros.h"
#include "Mapa.h"

#include "Item.h"
#include "ItemAnel.h"
#include "ItemAnelAzul.h"
#include "ItemCoguVerm.h"
#include "ItemFlorPreta.h"

#include "Obstaculo.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void inserirObstaculo(Mapa *mapa, ElementoMapa *obstaculo);
static void inserirItem(Mapa *mapa, ElementoMapa *item);
static void inserirInimigo(Mapa *mapa, ElementoMapa *inimigo);

/**
 * @brief Carrega um mapa a partir de uma arquivo.
 */
Mapa *carregarMapa(const char *caminhoArquivo) {

    // aloca um novo mapa
    Mapa *novoMapa = (Mapa *)malloc(sizeof(Mapa));

    novoMapa->obstaculos = NULL;
    novoMapa->quantidadeObstaculos = 0;

    novoMapa->itens = NULL;
    novoMapa->quantidadeItens = 0;

    novoMapa->inimigos = NULL;
    novoMapa->quantidadeInimigos = 0;

    novoMapa->dimensaoPadraoElementos = 48;
    novoMapa->linhas = 0;
    novoMapa->colunas = 0;

    // carrega dados do arquivo de texto
    char *dadosMapa = LoadFileText(caminhoArquivo);

    // marcadores para processamento do mapa
    char *caractereAtual = dadosMapa;
    int linhaAtual = 0;
    int colunaAtual = 0;

    // caractere atual marca inicialmente a primeira posição de dadosMapa
    // C-strings terminam em '\0', sendo assim, caminhamos caractere por
    // caractere até o fim
    while (*caractereAtual != '\0') {

        char c = *caractereAtual;

        // fim de linha?
        if (c == '\n') {

            linhaAtual++;
            colunaAtual = 0;

            novoMapa->linhas = linhaAtual;
        } else {

            if (c != ' ') {

                ElementoMapa *el = (ElementoMapa *)malloc(sizeof(ElementoMapa));
                el->proximo = NULL;

                if (c >= 'A' && c <= 'Z') {
                    bool solido = true;
                    int deslocamento = c - 'A';

                    if (c == 'A' || c == 'R' || c == 'S' || c == 'U' ||
                        c == 'Q' || c == 'P' || c == 'M' || c == 'F' ||
                        c == 'G') {
                        solido = false;
                    }

                    el->objeto = criarObstaculo(
                        (Rectangle){
                            .x =
                                novoMapa->dimensaoPadraoElementos * colunaAtual,
                            .y = novoMapa->dimensaoPadraoElementos * linhaAtual,
                            .width = novoMapa->dimensaoPadraoElementos,
                            .height = novoMapa->dimensaoPadraoElementos},
                        GRAY,
                        (Rectangle){
                            1 + (novoMapa->dimensaoPadraoElementos + 1) *
                                    deslocamento,
                            1, novoMapa->dimensaoPadraoElementos,
                            novoMapa->dimensaoPadraoElementos},
                        &rm.texturaTerreno, solido);

                    el->tipo = TIPO_ELEMENTO_MAPA_OBSTACULO;

                    inserirObstaculo(novoMapa, el);

                    Obstaculo *obs = (Obstaculo *)el->objeto;
                    if (c == 'N' || c == 'Q' || c == 'R' || c == 'U') {
                        obs->tipoColisao = COLISAO_RAMPA_CIMA_DIREITA;
                    } else if (c == 'O' || c == 'W') {
                        obs->tipoColisao = COLISAO_RAMPA_BAIXO_DIREITA;
                    } else if (c == 'P' || c == 'S' || c == 'Z') {
                        obs->tipoColisao = COLISAO_RAMPA_BAIXO_ESQUERDA;
                    }

                } else if (c == '!' || c == '@' || c == '#' || c == '$' ||
                           c == '%' || c == '&' || c == '*' || c == '(' ||
                           c == ')' || c == '[' || c == ']' || c == '{' ||
                           c == '}' || c == '?' || c == ';' || c == ':' ||
                           c == ',' || c == '.' || c == '<' || c == '>') {
                    // mapeia cada caractere para um tile do cano
                    int deslocamento = 0;
                    if (c == '!') {
                        deslocamento = 0; // topo esquerdo do cano
                    } else if (c == '@') {
                        deslocamento = 1; // topo direito
                    } else if (c == '#') {
                        deslocamento = 2; // corpo esquerdo
                    } else if (c == '$') {
                        deslocamento = 3; // corpo direito
                    } else if (c == '%') {
                        deslocamento = 4; // corpo direito
                    } else if (c == '&') {
                        deslocamento = 5; // corpo direito
                    } else if (c == '*') {
                        deslocamento = 6; // corpo direito
                    } else if (c == '(') {
                        deslocamento = 7; // corpo direito
                    } else if (c == ')') {
                        deslocamento = 8; // corpo direito
                    } else if (c == '[') {
                        deslocamento = 9; // corpo direito
                    } else if (c == ']') {
                        deslocamento = 10; // corpo direito
                    } else if (c == '{') {
                        deslocamento = 11; // corpo direito
                    } else if (c == '}') {
                        deslocamento = 12; // corpo direito
                    } else if (c == '?') {
                        deslocamento = 13; // corpo direito
                    } else if (c == ';') {
                        deslocamento = 14; // corpo direito
                    } else if (c == ':') {
                        deslocamento = 15; // corpo direito
                    } else if (c == ',') {
                        deslocamento = 16; // corpo direito
                    } else if (c == '.') {
                        deslocamento = 17; // corpo direito
                    } else if (c == '<') {
                        deslocamento = 18; // corpo direito
                    } else if (c == '>') {
                        deslocamento = 19; // corpo direito
                    }

                    bool solido = true;
                    el->objeto = criarObstaculo(
                        (Rectangle){
                            .x =
                                novoMapa->dimensaoPadraoElementos * colunaAtual,
                            .y = novoMapa->dimensaoPadraoElementos * linhaAtual,
                            .width = novoMapa->dimensaoPadraoElementos,
                            .height = novoMapa->dimensaoPadraoElementos},
                        GRAY,
                        (Rectangle){
                            1 + (novoMapa->dimensaoPadraoElementos + 1) *
                                    deslocamento,
                            1, novoMapa->dimensaoPadraoElementos,
                            novoMapa->dimensaoPadraoElementos},
                        &rm.texturaCanos, solido);

                    el->tipo = TIPO_ELEMENTO_MAPA_OBSTACULO;
                    inserirObstaculo(novoMapa, el);

                    Obstaculo *obs = (Obstaculo *)el->objeto;
                    if (c == '!' || c == ']') {
                        obs->tipoColisao = COLISAO_RAMPA_BAIXO_DIREITA;
                    } else if (c == '&' || c == '{') {
                        obs->tipoColisao = COLISAO_RAMPA_BAIXO_ESQUERDA;
                    }

                } else if (c == '+') {
                    // pega o deslocamento do tile do cano na spritesheet
                    // use o mesmo deslocamento do topo do cano normal que já
                    // existe
                    int deslocamento =
                        12; // ajusta conforme o tile correto na texturaCanos

                    el->objeto = criarObstaculo(
                        (Rectangle){
                            .x =
                                novoMapa->dimensaoPadraoElementos * colunaAtual,
                            .y = novoMapa->dimensaoPadraoElementos * linhaAtual,
                            .width = novoMapa->dimensaoPadraoElementos,
                            .height = novoMapa->dimensaoPadraoElementos},
                        GRAY,
                        (Rectangle){
                            1 + (novoMapa->dimensaoPadraoElementos + 1) *
                                    deslocamento,
                            1, novoMapa->dimensaoPadraoElementos,
                            novoMapa->dimensaoPadraoElementos},
                        &rm.texturaCanos, true);

                    Obstaculo *obs = (Obstaculo *)el->objeto;
                    obs->eCanoSaida = true;
                    obs->proximaFase =
                        "resources\\mapas\\mapa01.txt"; // troca pelo arquivo da
                                                        // fase destino

                    el->tipo = TIPO_ELEMENTO_MAPA_OBSTACULO;
                    inserirObstaculo(novoMapa, el);
                } else if (c == '-') {
                    el->objeto = criarObstaculo(
                        (Rectangle){
                            .x =
                                novoMapa->dimensaoPadraoElementos * colunaAtual,
                            .y = novoMapa->dimensaoPadraoElementos * linhaAtual,
                            .width = novoMapa->dimensaoPadraoElementos,
                            .height = novoMapa->dimensaoPadraoElementos},
                        YELLOW,
                        (Rectangle){8, 304, 16,
                                    16}, // ajuste pra posição do frame
                                         // "intacto" na spritesheet
                        &rm.texturaItensAnimadosMario, true);

                    Obstaculo *obs = (Obstaculo *)el->objeto;
                    obs->solido = true;
                    obs->eBlocoGiratorio = true;
                    obs->quebrando = false;
                    obs->quebrado = false;
                    obs->quadroQuebra = 0;
                    obs->tempoQuadroQuebra = 1.0f;

                    el->tipo = TIPO_ELEMENTO_MAPA_OBSTACULO;
                    inserirObstaculo(novoMapa, el);
                } else if (c >= 'a' && c <= 'z') {

                    Item *item = NULL;

                    switch (c) {

                    case 'a':

                        item = criarItem(TIPO_ITEM_ANEL);

                        item->objeto = criarItemAnel(
                            (Rectangle){.x = novoMapa->dimensaoPadraoElementos *
                                                 colunaAtual +
                                             8,
                                        .y = novoMapa->dimensaoPadraoElementos *
                                                 linhaAtual +
                                             5,
                                        .width = 32,
                                        .height = 32},
                            YELLOW);

                        el->objeto = item;
                        el->tipo = TIPO_ELEMENTO_MAPA_ITEM;

                        break;

                    case 'b':

                        item = criarItem(TIPO_ITEM_ANEL_AZUL);

                        item->objeto = criarItemAnelAzul(
                            (Rectangle){.x = novoMapa->dimensaoPadraoElementos *
                                                 colunaAtual +
                                             8,
                                        .y = novoMapa->dimensaoPadraoElementos *
                                                 linhaAtual +
                                             5,
                                        .width = 32,
                                        .height = 32},
                            YELLOW);

                        el->objeto = item;
                        el->tipo = TIPO_ELEMENTO_MAPA_ITEM;

                        break;

                    case 'c':

                        item = criarItem(TIPO_ITEM_COGUMELO_VERMELHO);

                        item->objeto = criarItemCogumeloVermelho(
                            (Rectangle){.x = novoMapa->dimensaoPadraoElementos *
                                                 colunaAtual +
                                             8,
                                        .y = novoMapa->dimensaoPadraoElementos *
                                                 linhaAtual +
                                             5,
                                        .width = 32,
                                        .height = 32},
                            YELLOW);

                        el->objeto = item;
                        el->tipo = TIPO_ELEMENTO_MAPA_ITEM;

                        break;
                    case 'd':

                        item = criarItem(TIPO_ITEM_FLOR_PRETA);

                        item->objeto = criarItemFlorPreta(
                            (Rectangle){.x = novoMapa->dimensaoPadraoElementos *
                                                 colunaAtual +
                                             8,
                                        .y = novoMapa->dimensaoPadraoElementos *
                                                 linhaAtual +
                                             5,
                                        .width = 32,
                                        .height = 32},
                            YELLOW);

                        el->objeto = item;
                        el->tipo = TIPO_ELEMENTO_MAPA_ITEM;

                        break;

                    default:
                        TraceLog(LOG_ERROR, "Tipo de item desconhecido.");
                        abort();
                        break;
                    }

                    inserirItem(novoMapa, el);
                } else if (c >= '0' && c <= '9') {

                    Inimigo *inimigo = NULL;

                    switch (c) {

                    case '0':

                        inimigo = criarInimigo(TIPO_INIMIGO_MOTOBUG);

                        inimigo->objeto = criarInimigoMotobug(
                            (Rectangle){.x = novoMapa->dimensaoPadraoElementos *
                                             colunaAtual,
                                        .y = novoMapa->dimensaoPadraoElementos *
                                                 linhaAtual -
                                             12,
                                        .width = 80,
                                        .height = 60},
                            YELLOW);

                        el->objeto = inimigo;
                        el->tipo = TIPO_ELEMENTO_MAPA_INIMIGO;

                        break;

                    case '1':

                        inimigo = criarInimigo(TIPO_INIMIGO_SPIKES);

                        inimigo->objeto = criarInimigoSpikes(
                            (Rectangle){.x = novoMapa->dimensaoPadraoElementos *
                                             colunaAtual,
                                        .y = novoMapa->dimensaoPadraoElementos *
                                                 linhaAtual -
                                             21,
                                        .width = 80,
                                        .height = 60},
                            BLUE);

                        el->objeto = inimigo;
                        el->tipo = TIPO_ELEMENTO_MAPA_INIMIGO;

                        break;
                    case '2':

                        inimigo = criarInimigo(TIPO_INIMIGO_TONTON);

                        inimigo->objeto = criarInimigoTonTon(
                            (Rectangle){.x = novoMapa->dimensaoPadraoElementos *
                                             colunaAtual,
                                        .y = novoMapa->dimensaoPadraoElementos *
                                                 linhaAtual -
                                             21,
                                        .width = 80,
                                        .height = 60},
                            BLUE);

                        el->objeto = inimigo;
                        el->tipo = TIPO_ELEMENTO_MAPA_INIMIGO;

                        break;
                    case '3':

                        inimigo = criarInimigo(TIPO_INIMIGO_KOOPARED);

                        inimigo->objeto = criarInimigoKoopaRed(
                            (Rectangle){.x = novoMapa->dimensaoPadraoElementos *
                                             colunaAtual,
                                        .y = novoMapa->dimensaoPadraoElementos *
                                                 linhaAtual -
                                             21,
                                        .width = 80,
                                        .height = 60},
                            BLUE);

                        el->objeto = inimigo;
                        el->tipo = TIPO_ELEMENTO_MAPA_INIMIGO;

                        break;
                    case '4':

                        inimigo = criarInimigo(TIPO_INIMIGO_REX);

                        inimigo->objeto = criarInimigoRex(
                            (Rectangle){.x = novoMapa->dimensaoPadraoElementos *
                                             colunaAtual,
                                        .y = novoMapa->dimensaoPadraoElementos *
                                                 linhaAtual -
                                             21,
                                        .width = 80,
                                        .height = 60},
                            BLUE);

                        el->objeto = inimigo;
                        el->tipo = TIPO_ELEMENTO_MAPA_INIMIGO;

                        break;

                    case '5':

                        inimigo = criarInimigo(TIPO_INIMIGO_NEGOPRETO);

                        inimigo->objeto = criarInimigoNegoPreto(
                            (Rectangle){.x = novoMapa->dimensaoPadraoElementos *
                                             colunaAtual,
                                        .y = novoMapa->dimensaoPadraoElementos *
                                                 linhaAtual -
                                             21,
                                        .width = 128,
                                        .height = 128},
                            BLUE);

                        el->objeto = inimigo;
                        el->tipo = TIPO_ELEMENTO_MAPA_INIMIGO;

                        break;

                    default:
                        TraceLog(LOG_ERROR, "Tipo de inimigo desconhecido.");
                        abort();
                        break;
                    }

                    inserirInimigo(novoMapa, el);
                } else {
                    TraceLog(LOG_ERROR, "Entidade inválida no mapa.");
                    abort();
                }
            }

            colunaAtual++;

            if (novoMapa->colunas < colunaAtual) {
                novoMapa->colunas = colunaAtual;
            }
        }

        caractereAtual++;
    }

    novoMapa->linhas++;

    // descarrega os dados
    UnloadFileText(dadosMapa);

    return novoMapa;
}

/**
 * @brief Destroi um mapa.
 */
void destruirMapa(Mapa *m) {

    if (m != NULL) {

        ElementoMapa *el = NULL;
        printf("destruindo obstaculos...\n");

        el = m->obstaculos;
        while (el != NULL) {
            printf("  destruindo obstaculo %p\n", (void *)el);

            destruirObstaculo((Obstaculo *)el->objeto);
            ElementoMapa *t = el;
            el = el->proximo;
            free(t);
        }
        printf("destruindo itens...\n");

        el = m->itens;
        while (el != NULL) {
            destruirItem((Item *)el->objeto);
            ElementoMapa *t = el;
            el = el->proximo;
            free(t);
        }
        printf("destruindo inimigos...\n");

        el = m->inimigos;
        while (el != NULL) {
            destruirInimigo((Inimigo *)el->objeto);
            ElementoMapa *t = el;
            el = el->proximo;
            free(t);
        }
        printf("liberando mapa...\n");

        free(m);
    }
}

/**
 * @brief Atualiza um mapa.
 */
void atualizarMapa(Mapa *m, GameWorld *gw, float delta) {

    ElementoMapa *el = NULL;

    el = m->itens;
    while (el != NULL) {
        atualizarItem((Item *)el->objeto, delta);
        el = el->proximo;
    }

    el = m->inimigos;
    while (el != NULL) {
        atualizarInimigo((Inimigo *)el->objeto, gw, delta);
        el = el->proximo;
    }

    el = m->obstaculos;
    while (el != NULL) {
        atualizarObstaculo((Obstaculo *)el->objeto, delta);
        el = el->proximo;
    }
}

/**
 * @brief Desenha um mapa.
 */
void desenharMapa(Mapa *m) {

    ElementoMapa *el = NULL;

    el = m->obstaculos;
    while (el != NULL) {
        desenharObstaculo((Obstaculo *)el->objeto);
        el = el->proximo;
    }

    el = m->itens;
    while (el != NULL) {
        desenharItem((Item *)el->objeto);
        el = el->proximo;
    }

    el = m->inimigos;
    while (el != NULL) {
        desenharInimigo((Inimigo *)el->objeto);
        el = el->proximo;
    }
}

/**
 * @brief Calcula a largura do mapa.
 */
int calcularLarguraMapa(Mapa *m) {
    return (int)(m->dimensaoPadraoElementos * m->colunas);
}

/**
 * @brief Calcula a altura do mapa.
 */
int calcularAlturaMapa(Mapa *m) {
    return (int)(m->dimensaoPadraoElementos * m->linhas);
}

/**
 * @brief Insere um obstáculo na lista de obstáculos.
 */
static void inserirObstaculo(Mapa *mapa, ElementoMapa *obstaculo) {
    if (mapa->obstaculos == NULL) {
        mapa->obstaculos = obstaculo;
    } else {
        obstaculo->proximo = mapa->obstaculos;
        mapa->obstaculos = obstaculo;
    }
    mapa->quantidadeObstaculos++;
}

/**
 * @brief Insere um item na lista de itens.
 */
static void inserirItem(Mapa *mapa, ElementoMapa *item) {
    if (mapa->itens == NULL) {
        mapa->itens = item;
    } else {
        item->proximo = mapa->itens;
        mapa->itens = item;
    }
    mapa->quantidadeItens++;
}

/**
 * @brief Insere um inimigo na lista de inimigos.
 */
static void inserirInimigo(Mapa *mapa, ElementoMapa *inimigo) {
    if (mapa->inimigos == NULL) {
        mapa->inimigos = inimigo;
    } else {
        inimigo->proximo = mapa->inimigos;
        mapa->inimigos = inimigo;
    }
    mapa->quantidadeInimigos++;
}