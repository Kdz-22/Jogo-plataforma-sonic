/**
 * @file Mario.c
 * @author Kevin e Rebeca
 * @brief Mario implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Mario.h"
#include "Animacao.h"
#include "Inimigo.h"
#include "InimigoMotobug.h"
#include "InimigoSpikes.h"
#include "InimigoKoopaRed.h"
#include "InimigoRex.h"
#include "Item.h"
#include "ItemAnel.h"
#include "ItemAnelAzul.h"
#include "Macros.h"
#include "Jogador.h"
#include "ResourceManager.h"
#include "Tipos.h"

static void desenharQuadroAnimacaoMario(Mario *m, QuadroAnimacao *qa, Color tonalidade);
static QuadroAnimacao *getQuadroAnimacaoAtualMario(Mario *m);
static Animacao *getAnimacaoAtualMario(Mario *m);

static void resolverColisaoMarioObstaculosMapaX(Mario *m, Mapa *mapa);
static void resolverColisaoMarioObstaculosMapaY(Mario *m, Mapa *mapa);

static void resolverColisaoMarioItensMapa(Mario *m, Personagem *p, Mapa *mapa);
static void resolverColisaoMarioInimigosMapa(Mario *m, Personagem *p, Mapa *mapa);

static const bool MOSTRAR_RETANGULOS = false;
static PersonagemFuncoes marioFuncoes;

Personagem *criarMario(float x, float y, float w, float h)
{

    Personagem *p = (Personagem *)malloc(sizeof(Personagem));

    Mario *m = (Mario *)malloc(sizeof(Mario));

    m->ret.x = x;
    m->ret.y = y;
    m->ret.width = w;
    m->ret.height = h;
    m->vel = (Vector2){0};

    m->cor = BLUE;

    m->velAndando = 200;
    m->velCorrendo = 500;
    m->velPulo = -550;
    m->velMaxQueda = 600;

    m->frenagem = 1800;
    m->aceleracao = 500;
    m->desaceleracao = 500;

    m->quantidadePulos = 0;
    m->quantidadeMaxPulos = 1;

    m->invulneravel = false;
    m->tempoInvulnerabilidade = 3.0f;
    m->contadorTempoInvulnerabilidade = 0.0f;

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
    criarQuadrosAnimacao(&m->animacaoParado, m->animacaoParado.quantidadeQuadros); // OK
    inicializarQuadrosAnimacao(
        m->animacaoParado.quadros,
        m->animacaoParado.quantidadeQuadros,
        1000,   // duração padrão para todos os quadros
        24, 48, // início
        16, 24, // dimensões
        4,      // separação
        true,   // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            32, 20, 42, 76
            // 18, 20, 54, 76
        });

    m->animacaoAndando.quantidadeQuadros = 2;
    m->animacaoAndando.quadroAtual = 0;
    m->animacaoAndando.contadorTempoQuadro = 0.0f;
    m->animacaoAndando.pararNoUltimoQuadro = false;
    m->animacaoAndando.executarUmaVez = false;
    m->animacaoAndando.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoAndando, m->animacaoAndando.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoAndando.quadros,
        m->animacaoAndando.quantidadeQuadros,
        150,     // duração padrão para cada quadro
        232, 48, // início
        16, 24,  // dimensões
        36,      // separação
        false,   // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            32, 20, 42, 76
            // 18, 20, 54, 76
        });

    m->animacaoCorrendo.quantidadeQuadros = 2;
    m->animacaoCorrendo.quadroAtual = 0;
    m->animacaoCorrendo.contadorTempoQuadro = 0.0f;
    m->animacaoCorrendo.pararNoUltimoQuadro = false;
    m->animacaoCorrendo.executarUmaVez = false;
    m->animacaoCorrendo.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoCorrendo, m->animacaoCorrendo.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoCorrendo.quadros,
        m->animacaoCorrendo.quantidadeQuadros,
        50,      // duração padrão para cada quadro
        388, 48, // início
        16, 24,  // dimensões
        36,      // separação
        false,   // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            32, 20, 42, 76
            // 18, 20, 54, 76
        });

    m->animacaoPulando.quantidadeQuadros = 1;
    m->animacaoPulando.quadroAtual = 0;
    m->animacaoPulando.contadorTempoQuadro = 0.0f;
    m->animacaoPulando.pararNoUltimoQuadro = false;
    m->animacaoPulando.executarUmaVez = false;
    m->animacaoPulando.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoPulando, m->animacaoPulando.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoPulando.quadros,
        m->animacaoPulando.quantidadeQuadros,
        40,      // duração padrão para cada quadro
        76, 112, // início
        16, 24,  // dimensões
        4,       // separação
        false,   // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            32, 46, 42, 50
            // 18, 36, 60, 60
        });

    m->animacaoPulandoCorrendo.quantidadeQuadros = 1;
    m->animacaoPulandoCorrendo.quadroAtual = 0;
    m->animacaoPulandoCorrendo.contadorTempoQuadro = 0.0f;
    m->animacaoPulandoCorrendo.pararNoUltimoQuadro = false;
    m->animacaoPulandoCorrendo.executarUmaVez = false;
    m->animacaoPulandoCorrendo.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoPulandoCorrendo, m->animacaoPulandoCorrendo.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoPulandoCorrendo.quadros,
        m->animacaoPulandoCorrendo.quantidadeQuadros,
        15,       // duração padrão para cada quadro
        180, 112, // início
        16, 24,   // dimensões
        4,        // separação
        false,    // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            32, 46, 42, 50
            // 18, 36, 60, 60
        });

    m->animacaoCaindo.quantidadeQuadros = 1;
    m->animacaoCaindo.quadroAtual = 0;
    m->animacaoCaindo.contadorTempoQuadro = 0.0f;
    m->animacaoCaindo.pararNoUltimoQuadro = false;
    m->animacaoCaindo.executarUmaVez = false;
    m->animacaoCaindo.finalizada = false;
    criarQuadrosAnimacao(&m->animacaoCaindo, m->animacaoCaindo.quantidadeQuadros); // ok
    inicializarQuadrosAnimacao(
        m->animacaoCaindo.quadros,
        m->animacaoCaindo.quantidadeQuadros,
        15,       // duração padrão para cada quadro
        128, 112, // início
        16, 24,   // dimensões
        4,        // separação
        false,    // de trás para frente
        (Rectangle){
            // retângulo de colisão padrão para cada quadro
            32, 46, 42, 50
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

    bool direitaDown = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D) || (IsGamepadAvailable(0) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT));
    bool esquerdaDown = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) || (IsGamepadAvailable(0) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT));
    bool cimaDown = IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) || (IsGamepadAvailable(0) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP));
    bool baixoDown = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S) || (IsGamepadAvailable(0) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN));
    bool puloPressed = IsKeyPressed(KEY_SPACE) || (IsGamepadAvailable(0) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN));
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
                    PlaySound(rm.somFrenagem);
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
                    PlaySound(rm.somFrenagem);
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
                    PlaySound(rm.somFrenagem);
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
                    PlaySound(rm.somFrenagem);
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
        if (m->vel.y < 0)
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

    if (puloPressed && m->quantidadePulos < m->quantidadeMaxPulos)
    {
        m->vel.y = m->velPulo;
        m->quantidadePulos++;
        // PlaySound( rm.somPulo );
    }

    // sincronização de animações andando e andando rápido
    /*if ( estadoAnterior == ESTADO_MARIO_ANDANDO && m->estado == ESTADO_MARIO_ANDANDO_RAPIDO ) {
         sincronizarAnimacao( &m->animacaoAndandoRapido, &m->animacaoAndando );
    } else if ( estadoAnterior == ESTADO_MARIO_ANDANDO_RAPIDO && m->estado == ESTADO_MARIO_ANDANDO ) {
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

    if (m->invulneravel)
    {

        m->contadorTempoPiscaPisca += delta;
        if (m->contadorTempoPiscaPisca >= m->tempoPiscaPisca)
        {
            m->contadorTempoPiscaPisca = 0.0f;
            m->piscaPisca = !m->piscaPisca;
        }

        m->contadorTempoInvulnerabilidade += delta;
        if (m->contadorTempoInvulnerabilidade >= m->tempoInvulnerabilidade)
        {
            m->contadorTempoInvulnerabilidade = 0.0f;
            m->invulneravel = false;
            m->contadorTempoPiscaPisca = 0.0f;
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
        DrawRectangleLines(m->ret.x, m->ret.y, m->ret.width, m->ret.height, BLACK);
    }
}

void resetarMario(void *dados)
{
    Mario *m = (Mario *)dados;

    m->invulneravel = false;
    m->contadorTempoInvulnerabilidade = 0.0f;
    m->contadorTempoPiscaPisca = 0.0f;
    m->piscaPisca = false;
    m->estado = ESTADO_MARIO_PARADO;

    // animações
    for (int i = 0; i < m->quantidadeAnimacoes; i++)
    {
        m->animacoes[i]->quadroAtual = 0;
        m->animacoes[i]->contadorTempoQuadro = 0.0f;
        m->animacoes[i]->finalizada = false;
    }
}

static PersonagemFuncoes marioFuncoes = {
    entradaMario,
    atualizarMario,
    desenharMario,
    destruirMario,
    resetarMario};

static void desenharQuadroAnimacaoMario(Mario *m, QuadroAnimacao *qa, Color tonalidade)
{

    if (qa != NULL)
    {

        DrawTexturePro(
            rm.texturaMario,
            (Rectangle){
                qa->fonte.x,
                qa->fonte.y,
                m->olhandoParaDireita ? -qa->fonte.width : qa->fonte.width,
                qa->fonte.height},
            m->ret,
            (Vector2){0},
            0.0f,
            tonalidade);

        if (MOSTRAR_RETANGULOS)
        {
            float xDesenho = m->olhandoParaDireita
                                 ? m->ret.x + qa->retColisao.x
                                 : m->ret.x + m->ret.width - qa->retColisao.x - qa->retColisao.width;
            float yDesenho = m->ret.y + qa->retColisao.y;
            DrawRectangle(xDesenho, yDesenho, qa->retColisao.width, qa->retColisao.height, Fade(GREEN, 0.5f));
        }
    }
}

static QuadroAnimacao *getQuadroAnimacaoAtualMario(Mario *m)
{
    return getQuadroAtualAnimacao(getAnimacaoAtualMario(m));
}

static Animacao *getAnimacaoAtualMario(Mario *m)
{
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

        float deslocamentoX = m->olhandoParaDireita
                                  ? qa->retColisao.x
                                  : m->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            m->ret.x + deslocamentoX,
            m->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height};

        Obstaculo *o = (Obstaculo *)el->objeto;

        if (!o->solido)
        {
            el = el->proximo;
            continue;
        }
        if (CheckCollisionRecs(retColCalculado, o->ret))
        {
            if (retColCalculado.x + retColCalculado.width / 2 < o->ret.x + o->ret.width / 2)
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

        float deslocamentoX = m->olhandoParaDireita
                                  ? qa->retColisao.x
                                  : m->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            m->ret.x + deslocamentoX,
            m->ret.y + deslocamentoY,
            qa->retColisao.width,
            qa->retColisao.height};

        Obstaculo *o = (Obstaculo *)el->objeto;

        if (!o->solido)
        {
            el = el->proximo;
            continue;
        }
        if (CheckCollisionRecs(retColCalculado, o->ret))
        {
            if (retColCalculado.y + retColCalculado.height / 2 < o->ret.y + o->ret.height / 2)
            {
                m->ret.y = o->ret.y - qa->retColisao.height - deslocamentoY;
                m->quantidadePulos = 0;
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

        float deslocamentoX = m->olhandoParaDireita
                                  ? qa->retColisao.x
                                  : m->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            m->ret.x + deslocamentoX,
            m->ret.y + deslocamentoY,
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

static void resolverColisaoMarioInimigosMapa(Mario *m, Personagem *p, Mapa *mapa)
{

    ElementoMapa *el = mapa->inimigos;
    // Max = 7 inimigos mortos em sequencia
    static int tabelaComboAereo[] = {100, 200, 500, 1000, 2000, 5000, 10000};

    while (el != NULL)
    {

        QuadroAnimacao *qa = getQuadroAnimacaoAtualMario(m);

        float deslocamentoX = m->olhandoParaDireita
                                  ? qa->retColisao.x
                                  : m->ret.width - qa->retColisao.x - qa->retColisao.width;
        float deslocamentoY = qa->retColisao.y;

        Rectangle retColCalculado = {
            m->ret.x + deslocamentoX,
            m->ret.y + deslocamentoY,
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

                if (m->estado == ESTADO_MARIO_PULANDO || m->estado == ESTADO_MARIO_PULANDO_CORRENDO || m->estado == ESTADO_MARIO_CAINDO)
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
                    m->invulneravel = true;
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
                if (m->estado == ESTADO_MARIO_PULANDO || m->estado == ESTADO_MARIO_PULANDO_CORRENDO || m->estado == ESTADO_MARIO_CAINDO)
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
                    m->invulneravel = true;
                }
            }
            else if (inimigo->tipo == TIPO_INIMIGO_KOOPARED)
            {

                InimigoKoopaRed *koopared = (InimigoKoopaRed *)inimigo->objeto;

                // Se o Koopa não estiver ativo, pula para o próximo
                if (!koopared->ativo)
                {
                    el = el->proximo;
                    continue;
                }

                // CORREÇÃO: Usando as funções e variáveis corretas do KoopaRed
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

                    // Verifica se o Mario está caindo ou pulando em cima dele (Pulo bem-sucedido)
                    if (m->estado == ESTADO_MARIO_PULANDO || m->estado == ESTADO_MARIO_PULANDO_CORRENDO || m->estado == ESTADO_MARIO_CAINDO)
                    {

                        if (koopared->estado == ESTADO_KOOPA_ANDANDO)
                        {
                            // Transforma o Koopa em casco parado
                            koopared->estado = ESTADO_KOOPA_CASCO_PARADO;

                            // Redimensiona o retângulo físico para o tamanho do casco de SNES (16x16)
                            koopared->ret.width = 16;
                            koopared->ret.height = 16;
                            koopared->ret.y += 16; // Empurra 16 pixels para baixo para ele não ficar flutuando no ar

                            m->vel.y = m->velPulo; // Mario quica para cima
                            PlaySound(rm.somHitInimigo);

                            int idx = p->comboAereo >= 6 ? 6 : p->comboAereo;
                            p->score += tabelaComboAereo[idx];
                            p->comboAereo++;
                        }
                        else if (koopared->estado == ESTADO_KOOPA_CASCO_PARADO)
                        {
                            // Se o casco estava parado e o Mario pulou em cima ou chutou pelo lado
                            koopared->estado = ESTADO_KOOPA_CASCO_CORRENDO;
                            koopared->velAndando = 300; // Define a velocidade do chute
                            m->vel.y = m->velPulo;      // Dá mais um quique
                            PlaySound(rm.somHitInimigo);
                        }
                    }
                    else if (!m->invulneravel)
                    {

                        // Se o Mario trombar com o casco parado pelo lado, ele CHUTA o casco em vez de tomar dano
                        if (koopared->estado == ESTADO_KOOPA_CASCO_PARADO)
                        {
                            koopared->estado = ESTADO_KOOPA_CASCO_CORRENDO;
                            koopared->velAndando = 350;

                            // Define a direção do casco baseado na posição do Mario
                            if (retColCalculado.x < koopared->ret.x)
                            {
                                koopared->olhandoParaDireita = true;
                            }
                            else
                            {
                                koopared->olhandoParaDireita = false;
                            }
                            PlaySound(rm.somHitInimigo); // Ou som de chute, se tiver
                        }
                        else
                        {
                            // Se o Koopa estiver ANDANDO ou o casco estiver CORRENDO, o Mario toma dano
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
                            m->invulneravel = true;
                        }

                        return; // um inimigo de cada vez!
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

                        if (m->estado == ESTADO_MARIO_PULANDO || m->estado == ESTADO_MARIO_PULANDO_CORRENDO || m->estado == ESTADO_MARIO_CAINDO)
                        {
                            m->vel.y = m->velPulo;
                            rex->estado = ESTADO_INIMIGO_REX_MORRENDO;
                            PlaySound(rm.somHitInimigo);
                            int idx = p->comboAereo >= 6 ? 6 : p->comboAereo;
                            p->score += tabelaComboAereo[idx];
                            p->comboAereo++;
                        }
                        else if (!m->invulneravel)
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
                            m->invulneravel = true;
                        }

<<<<<<< Updated upstream
=======
                        return; // um inimigo de cada vez!
                    }
>>>>>>> Stashed changes
                }
                el = el->proximo;
            }
        }
    }
}

/**
 * @brief Cria uma instância alocada dinamicamente da struct Jogador.
 */
