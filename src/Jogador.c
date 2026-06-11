/**
 * @file Jogador.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do Jogador.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoMotobug.h"
#include "InimigoSpikes.h"
#include "InimigoTonTon.h"
#include "InimigoKoopaRed.h"
#include "Item.h"
#include "ItemAnel.h"
#include "ItemAnelAzul.h"
#include "Macros.h"
#include "Jogador.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void desenharQuadroAnimacaoJogador(Jogador *j, QuadroAnimacao *qa, Color tonalidade);
static QuadroAnimacao *getQuadroAnimacaoAtualJogador(Jogador *j);
static Animacao *getAnimacaoAtualJogador(Jogador *j);

static void resolverColisaoJogadorObstaculosMapaX(Jogador *j, Mapa *mapa);
static void resolverColisaoJogadorObstaculosMapaY(Jogador *j, Mapa *mapa);

static void resolverColisaoJogadorItensMapa(Jogador *j, Personagem *p, Mapa *mapa);
static void resolverColisaoJogadorInimigosMapa(Jogador *j, Personagem *p, Mapa *mapa);

static const bool MOSTRAR_RETANGULOS = false;

/**
 * @brief Cria uma instância alocada dinamicamente da struct Jogador.
 */
Jogador *criarJogador(float x, float y, float w, float h)
{

    Jogador *novoJogador = (Jogador *)malloc(sizeof(Jogador));

    novoJogador->ret.x = x;
    novoJogador->ret.y = y;
    novoJogador->ret.width = w;
    novoJogador->ret.height = h;
    novoJogador->vel = (Vector2){0};

    novoJogador->cor = BLUE;

    novoJogador->velAndando = 500;
    novoJogador->velAndandoRapido = 1200;
    novoJogador->velCorrendo = 5000;
    novoJogador->velPulo = -550;
    novoJogador->velMaxQueda = 600;

    novoJogador->aceleracao = 500;
    novoJogador->desaceleracao = 400;
    novoJogador->frenagem = 1800;

    novoJogador->quantidadePulos = 0;
    novoJogador->quantidadeMaxPulos = 5;

    novoJogador->invulneravel = false;
    novoJogador->tempoInvulnerabilidade = 3.0f;
    novoJogador->contadorTempoInvulnerabilidade = 0.0f;

    novoJogador->piscaPisca = false;
    novoJogador->tempoPiscaPisca = 0.05f;
    novoJogador->contadorTempoPiscaPisca = 0.0f;

    novoJogador->freando = false;

    novoJogador->estado = ESTADO_JOGADOR_PARADO;
    novoJogador->olhandoParaDireita = true;

    int quantidadeAnimacoes = 0;

    novoJogador->animacaoParado.quantidadeQuadros = 1;
    novoJogador->animacaoParado.quadroAtual = 0;
    novoJogador->animacaoParado.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoParado.pararNoUltimoQuadro = false;
    novoJogador->animacaoParado.executarUmaVez = false;
    novoJogador->animacaoParado.finalizada = false;
    criarQuadrosAnimacao(&novoJogador->animacaoParado, novoJogador->animacaoParado.quantidadeQuadros);
    inicializarQuadrosAnimacao(
        novoJogador->animacaoParado.quadros,
        novoJogador->animacaoParado.quantidadeQuadros,
        1000,    // duração padrão para todos os quadros
        24, 251, // início
        48, 48,  // dimensões
        4,       // separação
        false,   // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            32, 20, 42, 76
            // 18, 20, 54, 76
        });

    novoJogador->animacaoAndando.quantidadeQuadros = 8;
    novoJogador->animacaoAndando.quadroAtual = 0;
    novoJogador->animacaoAndando.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoAndando.pararNoUltimoQuadro = false;
    novoJogador->animacaoAndando.executarUmaVez = false;
    novoJogador->animacaoAndando.finalizada = false;
    criarQuadrosAnimacao(&novoJogador->animacaoAndando, novoJogador->animacaoAndando.quantidadeQuadros);
    inicializarQuadrosAnimacao(
        novoJogador->animacaoAndando.quadros,
        novoJogador->animacaoAndando.quantidadeQuadros,
        80,       // duração padrão para cada quadro
        664, 324, // início
        48, 48,   // dimensões
        4,        // separação
        false,    // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            32, 20, 42, 76
            // 18, 20, 54, 76
        });

    novoJogador->animacaoAndandoRapido.quantidadeQuadros = 8;
    novoJogador->animacaoAndandoRapido.quadroAtual = 0;
    novoJogador->animacaoAndandoRapido.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoAndandoRapido.pararNoUltimoQuadro = false;
    novoJogador->animacaoAndandoRapido.executarUmaVez = false;
    novoJogador->animacaoAndandoRapido.finalizada = false;
    criarQuadrosAnimacao(&novoJogador->animacaoAndandoRapido, novoJogador->animacaoAndandoRapido.quantidadeQuadros);
    inicializarQuadrosAnimacao(
        novoJogador->animacaoAndandoRapido.quadros,
        novoJogador->animacaoAndandoRapido.quantidadeQuadros,
        40,       // duração padrão para cada quadro
        664, 324, // início
        48, 48,   // dimensões
        4,        // separação
        false,    // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            32, 20, 42, 76
            // 18, 20, 54, 76
        });

    novoJogador->animacaoCorrendo.quantidadeQuadros = 4;
    novoJogador->animacaoCorrendo.quadroAtual = 0;
    novoJogador->animacaoCorrendo.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoCorrendo.pararNoUltimoQuadro = false;
    novoJogador->animacaoCorrendo.executarUmaVez = false;
    novoJogador->animacaoCorrendo.finalizada = false;
    criarQuadrosAnimacao(&novoJogador->animacaoCorrendo, novoJogador->animacaoCorrendo.quantidadeQuadros);
    inicializarQuadrosAnimacao(
        novoJogador->animacaoCorrendo.quadros,
        novoJogador->animacaoCorrendo.quantidadeQuadros,
        20,      // duração padrão para cada quadro
        24, 397, // início
        48, 48,  // dimensões
        4,       // separação
        false,   // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            32, 20, 42, 76
            // 18, 20, 54, 76
        });

    novoJogador->animacaoPulando.quantidadeQuadros = 4;
    novoJogador->animacaoPulando.quadroAtual = 0;
    novoJogador->animacaoPulando.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoPulando.pararNoUltimoQuadro = false;
    novoJogador->animacaoPulando.executarUmaVez = false;
    novoJogador->animacaoPulando.finalizada = false;
    criarQuadrosAnimacao(&novoJogador->animacaoPulando, novoJogador->animacaoPulando.quantidadeQuadros);
    inicializarQuadrosAnimacao(
        novoJogador->animacaoPulando.quadros,
        novoJogador->animacaoPulando.quantidadeQuadros,
        40,       // duração padrão para cada quadro
        248, 397, // início
        48, 48,   // dimensões
        4,        // separação
        false,    // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            32, 46, 42, 50
            // 18, 36, 60, 60
        });

    novoJogador->animacaoPulandoRapido.quantidadeQuadros = 4;
    novoJogador->animacaoPulandoRapido.quadroAtual = 0;
    novoJogador->animacaoPulandoRapido.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoPulandoRapido.pararNoUltimoQuadro = false;
    novoJogador->animacaoPulandoRapido.executarUmaVez = false;
    novoJogador->animacaoPulandoRapido.finalizada = false;
    criarQuadrosAnimacao(&novoJogador->animacaoPulandoRapido, novoJogador->animacaoPulandoRapido.quantidadeQuadros);
    inicializarQuadrosAnimacao(
        novoJogador->animacaoPulandoRapido.quadros,
        novoJogador->animacaoPulandoRapido.quantidadeQuadros,
        25,       // duração padrão para cada quadro
        248, 397, // início
        48, 48,   // dimensões
        4,        // separação
        false,    // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            32, 46, 42, 50
            // 18, 36, 60, 60
        });

    novoJogador->animacaoPulandoCorrendo.quantidadeQuadros = 4;
    novoJogador->animacaoPulandoCorrendo.quadroAtual = 0;
    novoJogador->animacaoPulandoCorrendo.contadorTempoQuadro = 0.0f;
    novoJogador->animacaoPulandoCorrendo.pararNoUltimoQuadro = false;
    novoJogador->animacaoPulandoCorrendo.executarUmaVez = false;
    novoJogador->animacaoPulandoCorrendo.finalizada = false;
    criarQuadrosAnimacao(&novoJogador->animacaoPulandoCorrendo, novoJogador->animacaoPulandoCorrendo.quantidadeQuadros);
    inicializarQuadrosAnimacao(
        novoJogador->animacaoPulandoCorrendo.quadros,
        novoJogador->animacaoPulandoCorrendo.quantidadeQuadros,
        15,       // duração padrão para cada quadro
        248, 397, // início
        48, 48,   // dimensões
        4,        // separação
        false,    // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            32, 46, 42, 50
            // 18, 36, 60, 60
        });

    novoJogador->animacoes[ESTADO_JOGADOR_PARADO] = &novoJogador->animacaoParado;
    quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_ANDANDO] = &novoJogador->animacaoAndando;
    quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_ANDANDO_RAPIDO] = &novoJogador->animacaoAndandoRapido;
    quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_CORRENDO] = &novoJogador->animacaoCorrendo;
    quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_PULANDO] = &novoJogador->animacaoPulando;
    quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_PULANDO_RAPIDO] = &novoJogador->animacaoPulandoRapido;
    quantidadeAnimacoes++;
    novoJogador->animacoes[ESTADO_JOGADOR_PULANDO_CORRENDO] = &novoJogador->animacaoPulandoCorrendo;
    quantidadeAnimacoes++;
    novoJogador->quantidadeAnimacoes = quantidadeAnimacoes;

    return novoJogador;
}

/**
 * @brief Destrói um objeto Jogador e libera seus recursos.
 */
void destruirJogador(Jogador *j)
{
    if (j != NULL)
    {
        for (int i = 0; i < j->quantidadeAnimacoes; i++)
        {
            destruirQuadrosAnimacao(j->animacoes[i]);
        }
        free(j);
    }
}

/**
 * @brief Lê a entrada do usuário e atualiza as velocidades do jogador.
 */
void entradaJogador(Jogador *j, Personagem *p, float delta)
{

    EstadoJogador estadoAnterior = j->estado;

    bool direitaDown = IsKeyDown(KEY_RIGHT) || (IsGamepadAvailable(0) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT));
    bool esquerdaDown = IsKeyDown(KEY_LEFT) || (IsGamepadAvailable(0) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT));
    bool puloPressed = IsKeyPressed(KEY_SPACE) || (IsGamepadAvailable(0) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN));

    if (direitaDown)
    {
        if (j->vel.x < 0)
        {
            j->vel.x += j->frenagem * delta;
            if (!j->freando && j->estado == ESTADO_JOGADOR_CORRENDO)
            {
                PlaySound(rm.somFrenagem);
                j->freando = true;
            }
            if (j->vel.x > 0)
            {
                j->vel.x = 0;
                j->freando = false;
            }
        }
        else
        {
            j->vel.x += j->aceleracao * delta;
            if (j->vel.x > j->velCorrendo)
            {
                j->vel.x = j->velCorrendo;
            }
        }
        j->olhandoParaDireita = true;
    }
    else if (esquerdaDown)
    {
        if (j->vel.x > 0)
        {
            j->vel.x -= j->frenagem * delta;
            if (!j->freando && j->estado == ESTADO_JOGADOR_CORRENDO)
            {
                PlaySound(rm.somFrenagem);
                j->freando = true;
            }
            if (j->vel.x < 0)
            {
                j->vel.x = 0;
                j->freando = false;
            }
        }
        else
        {
            j->vel.x -= j->aceleracao * delta;
            if (j->vel.x < -j->velCorrendo)
            {
                j->vel.x = -j->velCorrendo;
            }
        }
        j->olhandoParaDireita = false;
    }
    else
    {
        if (j->vel.x > 0)
        {
            j->vel.x -= j->desaceleracao * delta;
            if (j->vel.x < 0)
            {
                j->vel.x = 0;
            }
        }
        else if (j->vel.x < 0)
        {
            j->vel.x += j->desaceleracao * delta;
            if (j->vel.x > 0)
            {
                j->vel.x = 0;
            }
        }
    }

    float absVelX = fabsf(j->vel.x);
    if (j->quantidadePulos > 0)
    {
        if (absVelX <= j->velAndando)
        {
            j->estado = ESTADO_JOGADOR_PULANDO;
        }
        else if (absVelX <= j->velAndandoRapido)
        {
            j->estado = ESTADO_JOGADOR_PULANDO_RAPIDO;
        }
        else
        {
            j->estado = ESTADO_JOGADOR_PULANDO_CORRENDO;
        }
    }
    else if (absVelX < 1.0f)
    {
        j->estado = ESTADO_JOGADOR_PARADO;
    }
    else if (absVelX <= j->velAndando)
    {
        j->estado = ESTADO_JOGADOR_ANDANDO;
    }
    else if (absVelX <= j->velAndandoRapido)
    {
        j->estado = ESTADO_JOGADOR_ANDANDO_RAPIDO;
    }
    else
    {
        j->estado = ESTADO_JOGADOR_CORRENDO;
    }

    if (puloPressed && j->quantidadePulos < j->quantidadeMaxPulos)
    {
        j->vel.y = j->velPulo;
        j->quantidadePulos++;
        PlaySound(rm.somPulo);
    }

    // sincronização de animações andando e andando rápido
    if (estadoAnterior == ESTADO_JOGADOR_ANDANDO && j->estado == ESTADO_JOGADOR_ANDANDO_RAPIDO)
    {
        sincronizarAnimacao(&j->animacaoAndandoRapido, &j->animacaoAndando);
    }
    else if (estadoAnterior == ESTADO_JOGADOR_ANDANDO_RAPIDO && j->estado == ESTADO_JOGADOR_ANDANDO)
    {
        sincronizarAnimacao(&j->animacaoAndando, &j->animacaoAndandoRapido);
    }
}

/**
 * @brief Aplica física e resolve colisões do jogador com o mundo.
 */
void atualizarJogador(Jogador *j, Personagem *p, GameWorld *gw, float delta)
{

    if (j->estado < ESTADO_JOGADOR_PULANDO)
    {
        p->comboAereo = 0;
    }

    if (j->invulneravel)
    {

        j->contadorTempoPiscaPisca += delta;
        if (j->contadorTempoPiscaPisca >= j->tempoPiscaPisca)
        {
            j->contadorTempoPiscaPisca = 0.0f;
            j->piscaPisca = !j->piscaPisca;
        }

        j->contadorTempoInvulnerabilidade += delta;
        if (j->contadorTempoInvulnerabilidade >= j->tempoInvulnerabilidade)
        {
            j->contadorTempoInvulnerabilidade = 0.0f;
            j->invulneravel = false;
            j->contadorTempoPiscaPisca = 0.0f;
            j->piscaPisca = false;
        }
    }

    Animacao *animacaoAtual = getAnimacaoAtualJogador(j);
    atualizarAnimacao(animacaoAtual, delta);

    // fase X: move horizontalmente e resolve colisões laterais
    j->ret.x += j->vel.x * delta;
    resolverColisaoJogadorObstaculosMapaX(j, gw->mapa);

    // fase Y: aplica gravidade, move verticalmente e resolve colisões verticais
    j->vel.y += gw->gravidade * delta;
    if (j->vel.y > j->velMaxQueda)
    {
        j->vel.y = j->velMaxQueda;
    }
    j->ret.y += j->vel.y * delta;
    resolverColisaoJogadorObstaculosMapaY(j, gw->mapa);

    resolverColisaoJogadorItensMapa(j, p, gw->mapa);
    resolverColisaoJogadorInimigosMapa(j, p, gw->mapa);
}

/**
 * @brief Desenha o jogador.
 */
void desenharJogador(Jogador *j)
{

    if (!j->piscaPisca)
    {
        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador(j);
        desenharQuadroAnimacaoJogador(j, qa, WHITE);
    }

    if (MOSTRAR_RETANGULOS)
    {
        DrawRectangleRec(j->ret, Fade(j->cor, 0.5f));
        DrawRectangleLines(j->ret.x, j->ret.y, j->ret.width, j->ret.height, BLACK);
    }
}

static void desenharQuadroAnimacaoJogador(Jogador *j, QuadroAnimacao *qa, Color tonalidade)
{

    if (qa != NULL)
    {

        DrawTexturePro(
            rm.texturaJogador,
            (Rectangle){
                qa->fonte.x,
                qa->fonte.y,
                j->olhandoParaDireita ? qa->fonte.width : -qa->fonte.width,
                qa->fonte.height},
            j->ret,
            (Vector2){0},
            0.0f,
            tonalidade);

        if (MOSTRAR_RETANGULOS)
        {
            float xDesenho = j->olhandoParaDireita
                                 ? j->ret.x + qa->retColisao.x
                                 : j->ret.x + j->ret.width - qa->retColisao.x - qa->retColisao.width;
            float yDesenho = j->ret.y + qa->retColisao.y;
            DrawRectangle(xDesenho, yDesenho, qa->retColisao.width, qa->retColisao.height, Fade(GREEN, 0.5f));
        }
    }
}

static QuadroAnimacao *getQuadroAnimacaoAtualJogador(Jogador *j)
{
    return getQuadroAtualAnimacao(getAnimacaoAtualJogador(j));
}

static Animacao *getAnimacaoAtualJogador(Jogador *j)
{
    return j->animacoes[j->estado];
}

/**
 * @brief Resolve colisões do jogador com o mapa no eixo X.
 */
static void resolverColisaoJogadorObstaculosMapaX(Jogador *j, Mapa *mapa)
{

    ElementoMapa *el = mapa->obstaculos;

    while (el != NULL)
    {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador(j);

        float deslocamentoX = j->olhandoParaDireita
                                  ? qa->retColisao.x
                                  : j->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height};

        Obstaculo *o = (Obstaculo *)el->objeto;

        if (CheckCollisionRecs(retColCalculado, o->ret))
        {
            if (retColCalculado.x + retColCalculado.width / 2 < o->ret.x + o->ret.width / 2)
            {
                j->ret.x = o->ret.x - qa->retColisao.width - deslocamentoX;
            }
            else
            {
                j->ret.x = o->ret.x + o->ret.width - deslocamentoX;
            }
            j->vel.x = 0;
        }

        el = el->proximo;
    }
}

/**
 * @brief Resolve colisões do jogador com o mapa no eixo Y.
 */
static void resolverColisaoJogadorObstaculosMapaY(Jogador *j, Mapa *mapa)
{

    ElementoMapa *el = mapa->obstaculos;

    while (el != NULL)
    {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador(j);

        float deslocamentoX = j->olhandoParaDireita
                                  ? qa->retColisao.x
                                  : j->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height};

        Obstaculo *o = (Obstaculo *)el->objeto;

        if (CheckCollisionRecs(retColCalculado, o->ret))
        {
            if (retColCalculado.y + retColCalculado.height / 2 < o->ret.y + o->ret.height / 2)
            {
                j->ret.y = o->ret.y - qa->retColisao.height - deslocamentoY;
                j->quantidadePulos = 0;
            }
            else
            {
                j->ret.y = o->ret.y + o->ret.height - deslocamentoY;
            }
            j->vel.y = 0;
        }

        el = el->proximo;
    }
}

static void resolverColisaoJogadorItensMapa(Jogador *j, Personagem *p, Mapa *mapa)
{

    ElementoMapa *el = mapa->itens;

    while (el != NULL)
    {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador(j);

        float deslocamentoX = j->olhandoParaDireita
                                  ? qa->retColisao.x
                                  : j->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height};

        Item *item = (Item *)el->objeto;

        if (item->tipo == TIPO_ITEM_ANEL)
        {

            ItemAnel *itemAnel = (ItemAnel *)item->objeto;

            if (!itemAnel->ativo || itemAnel->estado == ESTADO_ITEM_ANEL_COLETADO)
            {
                el = el->proximo;
                continue;
            }

            QuadroAnimacao *qaItem = getQuadroAnimacaoAtualItemAnel(itemAnel);

            Rectangle retColItemCalculado = {
                itemAnel->ret.x + qaItem->retColisao.x,
                itemAnel->ret.y + qaItem->retColisao.y,
                qaItem->retColisao.width,
                qaItem->retColisao.height};

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

            if (!itemAnelAzul->ativo || itemAnelAzul->estado == ESTADO_ITEM_ANEL_AZUL_COLETADO)
            {
                el = el->proximo;
                continue;
            }

            QuadroAnimacao *qaItem = getQuadroAnimacaoAtualItemAnelAzul(itemAnelAzul);

            Rectangle retColItemCalculado = {
                itemAnelAzul->ret.x + qaItem->retColisao.x,
                itemAnelAzul->ret.y + qaItem->retColisao.y,
                qaItem->retColisao.width,
                qaItem->retColisao.height};

            if (CheckCollisionRecs(retColCalculado, retColItemCalculado))
            {
                itemAnelAzul->estado = ESTADO_ITEM_ANEL_AZUL_COLETADO;
                p->quantidadeAneis += 10;
                p->score += 100;
                PlaySound(rm.somAnel);
            }
        }

        el = el->proximo;
    }
}

static void resolverColisaoJogadorInimigosMapa(Jogador *j, Personagem *p, Mapa *mapa)
{

    ElementoMapa *el = mapa->inimigos;
    // Max = 7 inimigos mortos em sequencia
    static int tabelaComboAereo[] = {100, 200, 500, 1000, 2000, 5000, 10000};

    while (el != NULL)
    {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualJogador(j);

        float deslocamentoX = j->olhandoParaDireita
                                  ? qa->retColisao.x
                                  : j->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            j->ret.x + deslocamentoX,
            j->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height};

        Inimigo *inimigo = (Inimigo *)el->objeto;

        QuadroAnimacao *qaInimigo = NULL;
        bool *olhandoParaDireita = NULL;
        Rectangle *ret = NULL;

        if (inimigo->tipo == TIPO_INIMIGO_MOTOBUG)
        {

            InimigoMotobug *motobug = (InimigoMotobug *)inimigo->objeto;

            if (!motobug->ativo || motobug->estado == ESTADO_INIMIGO_MOTOBUG_MORRENDO)
            {
                el = el->proximo;
                continue;
            }

            qaInimigo = getQuadroAnimacaoAtualInimigoMotobug(motobug);
            olhandoParaDireita = &motobug->olhandoParaDireita;
            ret = &motobug->ret;

            float deslocamentoX = *olhandoParaDireita
                                      ? ret->width - qaInimigo->retColisao.x - qaInimigo->retColisao.width
                                      : qaInimigo->retColisao.x;
            float deslocamentoY = qaInimigo->retColisao.y;

            Rectangle retColInimigoCalculado = {
                ret->x + deslocamentoX,
                ret->y + deslocamentoY,
                qaInimigo->retColisao.width,
                qaInimigo->retColisao.height};

            if (CheckCollisionRecs(retColCalculado, retColInimigoCalculado))
            {

                if (j->estado >= ESTADO_JOGADOR_PULANDO && j->estado <= ESTADO_JOGADOR_PULANDO_CORRENDO)
                {
                    j->vel.y = j->velPulo;
                    motobug->estado = ESTADO_INIMIGO_MOTOBUG_MORRENDO;
                    PlaySound(rm.somHitInimigo);
                    int idx = p->comboAereo >= 6 ? 6 : p->comboAereo;
                    p->score += tabelaComboAereo[idx];
                    p->comboAereo++;
                }
                else if (!j->invulneravel)
                {
                    if (p->quantidadeAneis > 0)
                    {
                        p->quantidadeAneis = 0;
                        PlaySound(rm.somHitComAnel);
                    }
                    else
                    {
                        p->quantidadeVidas--;
                        PlaySound(rm.somMorte);
                    }
                    j->invulneravel = true;
                }

                return; // um inimigo de cada vez!
            }
        }
        else if (inimigo->tipo == TIPO_INIMIGO_SPIKES)
        {

            InimigoSpikes *spikes = (InimigoSpikes *)inimigo->objeto;

            if (!spikes->ativo || spikes->estado == ESTADO_INIMIGO_SPIKES_MORRENDO)
            {
                el = el->proximo;
                continue;
            }

            qaInimigo = getQuadroAnimacaoAtualInimigoSpikes(spikes);
            olhandoParaDireita = &spikes->olhandoParaDireita;
            ret = &spikes->ret;

            float deslocamentoX = *olhandoParaDireita
                                      ? ret->width - qaInimigo->retColisao.x - qaInimigo->retColisao.width
                                      : qaInimigo->retColisao.x;
            float deslocamentoY = qaInimigo->retColisao.y;

            Rectangle retColInimigoCalculado = {
                ret->x + deslocamentoX,
                ret->y + deslocamentoY,
                qaInimigo->retColisao.width,
                qaInimigo->retColisao.height};

            if (CheckCollisionRecs(retColCalculado, retColInimigoCalculado))
            {
                if (j->estado >= ESTADO_JOGADOR_PULANDO && j->estado <= ESTADO_JOGADOR_PULANDO_CORRENDO)
                {
                    j->vel.y = j->velPulo;
                    spikes->estado = ESTADO_INIMIGO_SPIKES_MORRENDO;
                    PlaySound(rm.somHitInimigo);
                    int idx = p->comboAereo >= 6 ? 6 : p->comboAereo;
                    p->score += tabelaComboAereo[idx];
                    p->comboAereo++;
                }
                else if (!j->invulneravel)
                {
                    if (p->quantidadeAneis > 0)
                    {
                        p->quantidadeAneis = 0;
                        PlaySound(rm.somHitComAnel);
                    }
                    else
                    {
                        p->quantidadeVidas--;
                        PlaySound(rm.somMorte);
                    }
                    j->invulneravel = true;
                }

                return; // um inimigo de cada vez!
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
                                      ? ret->width - qaInimigo->retColisao.x - qaInimigo->retColisao.width
                                      : qaInimigo->retColisao.x;
            float deslocamentoY = qaInimigo->retColisao.y;

            Rectangle retColInimigoCalculado = {
                ret->x + deslocamentoX,
                ret->y + deslocamentoY,
                qaInimigo->retColisao.width,
                qaInimigo->retColisao.height};

            if (CheckCollisionRecs(retColCalculado, retColInimigoCalculado))
            {

                // Verifica se o Sonic está atacando/pulando por cima (Pulo bem-sucedido)
                if (j->estado >= ESTADO_JOGADOR_PULANDO && j->estado <= ESTADO_JOGADOR_PULANDO_CORRENDO)
                {

                    if (koopared->estado == ESTADO_KOOPA_ANDANDO)
                    {
                        // Transforma o Koopa em casco parado
                        koopared->estado = ESTADO_KOOPA_CASCO_PARADO;

                        // Redimensiona o retângulo do Koopa para o formato quadrado do casco (16x16)
                        koopared->ret.width = 16;
                        koopared->ret.height = 16;
                        koopared->ret.y += 16; // Compensa a altura (de 32 para 16) para ele não flutuar

                        j->vel.y = j->velPulo; // Sonic quica para cima
                        PlaySound(rm.somHitInimigo);

                        int idx = p->comboAereo >= 6 ? 6 : p->comboAereo;
                        p->score += tabelaComboAereo[idx];
                        p->comboAereo++;
                    }
                    else if (koopared->estado == ESTADO_KOOPA_CASCO_PARADO)
                    {
                        // Se pular em cima do casco parado, ele começa a correr
                        koopared->estado = ESTADO_KOOPA_CASCO_CORRENDO;
                        koopared->velAndando = 300;
                        j->vel.y = j->velPulo; // Quica novamente
                        PlaySound(rm.somHitInimigo);
                    }
                }
                else if (!j->invulneravel)
                {

                    // Se o Sonic trombar com o casco PARADO pelas laterais, ele CHUTA o casco em vez de tomar dano
                    if (koopared->estado == ESTADO_KOOPA_CASCO_PARADO)
                    {
                        koopared->estado = ESTADO_KOOPA_CASCO_CORRENDO;
                        koopared->velAndando = 350;

                        // Direciona o casco dependendo do lado que o Sonic atingiu
                        if (retColCalculado.x < koopared->ret.x)
                        {
                            koopared->olhandoParaDireita = true;
                        }
                        else
                        {
                            koopared->olhandoParaDireita = false;
                        }
                        PlaySound(rm.somHitInimigo);
                    }
                    else
                    {
                        // Se o Koopa estiver andando normal ou se o casco estiver correndo rápido, o Sonic toma dano
                        if (p->quantidadeAneis > 0)
                        {
                            p->quantidadeAneis = 0;
                            PlaySound(rm.somHitComAnel);
                        }
                        else
                        {
                            p->quantidadeVidas--;
                            PlaySound(rm.somMorte);
                        }
                        j->invulneravel = true;
                    }
                }
            }
        }

        el = el->proximo;
    }
}