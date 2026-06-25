/**
 * @file GameWorld.c
 * @author Prof. Dr. David Buzatto
 * @brief Implementação do GameWorld.
 *
 * @copyright Copyright (c) 2026
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "GameWorld.h"
#include "GameWindow.h"
#include "Macros.h"
#include "Mapa.h"
#include "Mario.h"
#include "Obstaculo.h"
#include "ResourceManager.h"
#include "Sonic.h"
#include "Tipos.h"

#include "raylib/raylib.h"
#include "raylib/raymath.h"

static void desenharFundo(GameWorld *gw);
static void atualizarCamera(GameWorld *gw);

static void inicializar(GameWorld *gw);
static void reiniciar(GameWorld *gw);

static void desenharPontuacao(Texture2D hud, int valor, Vector2 posicao);
static void desenharTempo(Texture2D hud, int tempo, Vector2 posicao);
static void desenharTexto(Texture2D hud, const char *texto, Vector2 posicao,
                          float escala);

static void atualizarTelaInicial(GameWorld *gw, float delta);
static void desenharTelaInicial(GameWorld *gw);

/**
 * @brief Cria uma instância alocada dinamicamente da struct GameWorld.
 */
GameWorld *createGameWorld(void)
{
    GameWorld *gw = (GameWorld *)malloc(sizeof(GameWorld));
    inicializar(gw);

    gw->personagemSelecionado = 0;

    gw->estado = ESTADO_JOGO_TELA_INICIAL;
    gw->tempoTelaInicial = 0.0f;
    gw->teclaPressionada = false;

    return gw;
}

/**
 * @brief Destrói um objeto GameWorld e suas dependências.
 */
void destroyGameWorld(GameWorld *gw)
{
    if (gw != NULL)
    {
        if (gw->mapa != NULL)
        {
            destruirMapa(gw->mapa);
        }
        if (gw->personagem != NULL)
        {
            gw->personagem->funcoes->destruir(gw->personagem->dados);
            free(gw->personagem);
        }
        free(gw);
    }
}

/**
 * @brief Lê a entrada do usuário e atualiza o estado do jogo.
 */
void updateGameWorld(GameWorld *gw, float delta)
{

    if (!IsMusicStreamPlaying(rm.musicaFase01))
    {
        PlayMusicStream(rm.musicaFase01);
    }
    else
    {
        UpdateMusicStream(rm.musicaFase01);
    }
    if (IsMusicStreamPlaying(rm.musicaFase01))
    {

        if (IsKeyDown(KEY_MINUS) || IsKeyDown(KEY_KP_SUBTRACT))
        {
            rm.volumeMusicaFase01 =
                Clamp(rm.volumeMusicaFase01 - 0.01f, 0.0f, 1.0f);
            SetMusicVolume(rm.musicaFase01, rm.volumeMusicaFase01);
        }
        else if (IsKeyDown(KEY_EQUAL) || IsKeyDown(KEY_KP_ADD))
        {
            rm.volumeMusicaFase01 =
                Clamp(rm.volumeMusicaFase01 + 0.01f, 0.0f, 1.0f);
            SetMusicVolume(rm.musicaFase01, rm.volumeMusicaFase01);
        }
    }

    if (gw->estado == ESTADO_JOGO_TELA_INICIAL)
    {
        atualizarTelaInicial(gw, delta);
        return;
    }

    if (gw->estado == ESTADO_JOGO_SELECAO_PERSONAGEM)
    {
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT))
        {
            // alterna entre 0 e 1
            gw->personagemSelecionado = !gw->personagemSelecionado;
        }
        if (IsKeyPressed(KEY_ENTER))
        {
            inicializar(gw);
            gw->estado = ESTADO_JOGO_JOGANDO;
        }
        return;
    }

    if (gw->estado == ESTADO_JOGO_JOGANDO)
    {
        if (IsKeyPressed(KEY_R))
        {
            reiniciar(gw);
            return;
        }

        if (gw->mapa != NULL && gw->personagem != NULL)
        {
            atualizarMapa(gw->mapa, gw, delta);
            gw->personagem->funcoes->entrada(gw->personagem->dados, gw->personagem,
                                             delta);
            gw->personagem->funcoes->atualizar(gw->personagem->dados,
                                               gw->personagem, gw, delta);

            gw->personagem->time += delta;

            if (gw->estado == ESTADO_JOGO_JOGANDO)
            {
                atualizarCamera(gw);
            }
            // GAMEOVER
            if (gw->personagem->quantidadeVidas < 0)
            {
                gw->estado = ESTADO_JOGO_GAMEOVER;
                gw->tempoTelaInicial = 0.0f;
            }
        }
    }
    else if (gw->estado == ESTADO_JOGO_GAMEOVER)
    {
        gw->tempoTelaInicial += delta;

        // Se GAMEOVER apertar R para reiniciar (versão simples - sem destruir nada)
        if (IsKeyPressed(KEY_R))
        {
            printf("=== REINICIANDO JOGO (SIMPLES) ===\n");

            // Reseta os dados do personagem
            if (gw->personagem != NULL)
            {
                gw->personagem->quantidadeVidas = 3;
                gw->personagem->quantidadeAneis = 0;
                gw->personagem->score = 0;
                gw->personagem->comboAereo = 0;
                gw->personagem->time = 0.0f;
                gw->personagem->funcoes->resetar(gw->personagem->dados);
            }

            // Reposiciona o personagem no início
            if (gw->personagem != NULL && gw->mapa != NULL)
            {
                int alturaMapa = calcularAlturaMapa(gw->mapa);
                int screenWidth = GetScreenWidth();

                if (gw->personagemSelecionado == 0)
                {
                    Mario *m = (Mario *)gw->personagem->dados;
                    m->ret.x = screenWidth / 2 + 144;
                    m->ret.y = alturaMapa - 400;
                    m->vel = (Vector2){0};
                    m->quantidadePulos = 0;
                    m->estado = ESTADO_MARIO_PARADO;
                    m->invulneravel = false;
                    m->grande = false;
                    m->pulandoGirando = false;
                    m->freando = false;
                    // Reseta o retOriginal
                    m->retOriginal = m->ret;
                }
                else
                {
                    Jogador *j = (Jogador *)gw->personagem->dados;
                    j->ret.x = screenWidth / 2 + 144;
                    j->ret.y = alturaMapa - 400;
                    j->vel = (Vector2){0};
                    j->quantidadePulos = 0;
                    j->estado = ESTADO_JOGADOR_PARADO;
                    j->invulneravel = false;
                    j->freando = false;
                }
            }

            // Volta para o jogo
            gw->estado = ESTADO_JOGO_JOGANDO;
            printf("=== REINICIO CONCLUÍDO ===\n");
            return;
        }
        // Se apertar M, volta para a tela inicial
        else if (IsKeyPressed(KEY_M))
        {
            printf("=== VOLTANDO AO MENU ===\n");
            gw->estado = ESTADO_JOGO_TELA_INICIAL;
            gw->tempoTelaInicial = 0.0f;
            gw->teclaPressionada = false;

            if (IsMusicStreamPlaying(rm.musicaFase01))
            {
                StopMusicStream(rm.musicaFase01);
            }
            return;
        }
    }
    else if (gw->estado == ESTADO_JOGO_PROXIMA_FASE)
    {
        printf("proximaFase = %s\n", gw->proximaFase);
        printf("destruindo mapa...\n");
        destruirMapa(gw->mapa);
        gw->mapa = NULL;
        printf("carregando novo mapa...\n");
        gw->mapa = carregarMapa(gw->proximaFase);
        printf("resetando personagem...\n");
        gw->personagem->funcoes->resetar(gw->personagem->dados);
        printf("reposicionando mario...\n");
        Mario *m = (Mario *)gw->personagem->dados;
        m->ret.x = GetScreenWidth() / 2 + 144;
        m->ret.y = calcularAlturaMapa(gw->mapa) - 400;
        printf("trocando estado...\n");
        gw->estado = ESTADO_JOGO_JOGANDO;
        printf("feito!\n");
    }
}

/**
 * @brief Desenha o estado do jogo.
 */
void drawGameWorld(GameWorld *gw)
{

    BeginDrawing();

    if (gw->estado == ESTADO_JOGO_TELA_INICIAL)
    {
        desenharTelaInicial(gw);
        EndDrawing();
        return;
    }

    if (gw->estado == ESTADO_JOGO_JOGANDO)
    {
        ClearBackground((Color){36, 0, 180, 255});

        BeginMode2D(gw->camera);
        desenharFundo(gw);
        desenharMapa(gw->mapa);
        gw->personagem->funcoes->desenhar(gw->personagem->dados);
        EndMode2D();

        DrawText("VOL", 640, 45, 25, WHITE);
        DrawText("AUMENTAR = +", 640, 75, 20, YELLOW);
        DrawText("DIMINUIR = -", 640, 105, 20, YELLOW);
        desenharPontuacao(rm.texturaHUD, (int)(rm.volumeMusicaFase01 * 100),
                          (Vector2){700, 45});

        // Score
        DrawTexturePro(rm.texturaHUD, (Rectangle){24, 432, 40, 16},
                       (Rectangle){20, 15, 80, 32}, (Vector2){0, 0}, 0.0f,
                       WHITE);
        // Time
        DrawTexturePro(rm.texturaHUD, (Rectangle){24, 456, 32, 16},
                       (Rectangle){20, 45, 64, 32}, (Vector2){0, 0}, 0.0f,
                       WHITE);
        // Rings
        DrawTexturePro(rm.texturaHUD, (Rectangle){24, 480, 40, 16},
                       (Rectangle){20, 75, 80, 32}, (Vector2){0, 0}, 0.0f,
                       WHITE);

        Vector2 posVidas = {20, 380};

        // Life (fotinha do Sonic)
        DrawTexturePro(rm.texturaHUD, (Rectangle){40, 400, 16, 16},
                       (Rectangle){posVidas.x, posVidas.y, 32, 32},
                       (Vector2){0, 0}, 0.0f, WHITE);
        // Texto "Sonic"
        DrawTexturePro(rm.texturaHUD, (Rectangle){57, 401, 31, 7},
                       (Rectangle){posVidas.x + 40, posVidas.y, 80, 16},
                       (Vector2){0, 0}, 0.0f, WHITE);
        // Multiplicador "X"
        DrawTexturePro(rm.texturaHUD, (Rectangle){62, 410, 8, 8},
                       (Rectangle){posVidas.x + 36, posVidas.y + 18, 16, 16},
                       (Vector2){0, 0}, 0.0f, WHITE);
        // Número de vidas
        desenharPontuacao(rm.texturaHUD, gw->personagem->quantidadeVidas,
                          (Vector2){posVidas.x + 50, posVidas.y + 16});

        desenharPontuacao(rm.texturaHUD, gw->personagem->score,
                          (Vector2){110, 15});
        desenharTempo(rm.texturaHUD, (int)gw->personagem->time,
                      (Vector2){94, 45});
        desenharPontuacao(rm.texturaHUD, gw->personagem->quantidadeAneis,
                          (Vector2){110, 75});
    }
    else if (gw->estado == ESTADO_JOGO_SELECAO_PERSONAGEM)
    {

        ClearBackground(BLACK);

        int larguraTela = GetScreenWidth();
        int alturaTela = GetScreenHeight();

        // título
        int tamTitulo = MeasureText("SELECIONE O PERSONAGEM", 30);
        DrawText("SELECIONE O PERSONAGEM", larguraTela / 2 - tamTitulo / 2, 50,
                 30, WHITE);

        // destaca o selecionado com um retângulo
        int retX = gw->personagemSelecionado == 0 ? larguraTela / 4 - 60
                                                  : larguraTela / 4 * 3 - 60;
        Color corMario =
            gw->personagemSelecionado == 0 ? WHITE : (Color){60, 60, 60, 200};
        Color corSonic =
            gw->personagemSelecionado == 1 ? WHITE : (Color){60, 60, 60, 200};

        // Mario sempre na esquerda
        DrawTexturePro(
            rm.texturaMario, (Rectangle){8, 3453, 32, 32},
            (Rectangle){larguraTela / 4 - 60, alturaTela / 2 - 60, 120, 120},
            (Vector2){0, 0}, 0.0f, corMario);

        // Sonic sempre na direita
        DrawTexturePro(rm.texturaJogador, (Rectangle){260, 2245, 80, 56},
                       (Rectangle){larguraTela / 4 * 3 - 60,
                                   alturaTela / 2 - 60, 120, 120},
                       (Vector2){0, 0}, 0.0f, corSonic);

        DrawRectangleLines(retX, alturaTela / 2 - 60, 120, 120, YELLOW);

        // nomes dos personagens
        DrawText("MARIO", larguraTela / 4 - MeasureText("MARIO", 20) / 2,
                 alturaTela / 2 + 80, 20,
                 gw->personagemSelecionado == 0 ? YELLOW : GRAY);

        DrawText("SONIC", larguraTela / 4 * 3 - MeasureText("SONIC", 20) / 2,
                 alturaTela / 2 + 80, 20,
                 gw->personagemSelecionado == 1 ? YELLOW : GRAY);

        // instrução
        int tamConfirmar = MeasureText("ENTER PARA CONFIRMAR", 20);
        DrawText("ENTER PARA CONFIRMAR", larguraTela / 2 - tamConfirmar / 2,
                 alturaTela - 60, 20, LIGHTGRAY);
    }
    else if (gw->estado == ESTADO_JOGO_GAMEOVER)
    {
        ClearBackground(BLACK);

        int larguraTela = GetScreenWidth();
        int alturaTela = GetScreenHeight();

        // Efeito de fade/pisca no título
        float alpha = 0.5f + 0.5f * sinf(gw->tempoTelaInicial * 3.0f);

        // Título GAME OVER
        int tamTexto1 = MeasureText("GAME OVER", 60);
        DrawText("GAME OVER", larguraTela / 2 - tamTexto1 / 2,
                 alturaTela / 2 - 80, 60, (Color){255, 0, 0, (unsigned char)(alpha * 255)});

        // Mensagem para reiniciar (piscando)
        if ((int)(gw->tempoTelaInicial * 1.5f) % 2 == 0)
        {
            int tamTexto2 = MeasureText("PRESSIONE 'R' PARA REINICIAR", 25);
            DrawText("PRESSIONE 'R' PARA REINICIAR",
                     larguraTela / 2 - tamTexto2 / 2, alturaTela / 2 + 20, 25,
                     YELLOW);
        }

        // Mensagem para voltar à tela inicial
        int tamTexto3 = MeasureText("PRESSIONE 'M' PARA VOLTAR AO MENU", 20);
        DrawText("PRESSIONE 'M' PARA VOLTAR AO MENU",
                 larguraTela / 2 - tamTexto3 / 2, alturaTela / 2 + 70, 20,
                 LIGHTGRAY);
    }
    EndDrawing();
}

static void atualizarTelaInicial(GameWorld *gw, float delta)
{
    gw->tempoTelaInicial += delta;

    // Verifica se alguma tecla foi pressionada ou mouse clicado
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
        IsKeyPressed(KEY_I) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        gw->estado = ESTADO_JOGO_SELECAO_PERSONAGEM;
        gw->teclaPressionada = true;
    }
}

static void desenharTelaInicial(GameWorld *gw)
{
    ClearBackground(BLACK);

    // Verifica se a textura foi carregada
    if (rm.texturaTelaInicial.id != 0)
    {
        Texture2D textura = rm.texturaTelaInicial;

        // Calcula a escala para caber na tela mantendo a proporção
        float scaleX = (float)GetScreenWidth() / textura.width;
        float scaleY = (float)GetScreenHeight() / textura.height;
        float scale = fmin(scaleX, scaleY);

        float destWidth = textura.width * scale;
        float destHeight = textura.height * scale;
        float destX = (GetScreenWidth() - destWidth) / 2;
        float destY = (GetScreenHeight() - destHeight) / 2;

        DrawTexturePro(
            textura,
            (Rectangle){0, 0, textura.width, textura.height},
            (Rectangle){destX, destY, destWidth, destHeight},
            (Vector2){0, 0},
            0.0f,
            WHITE);
    }

    // Texto "Pressione qualquer tecla" com efeito de pisca-pisca
    if ((int)(gw->tempoTelaInicial * 2) % 2 == 0)
    {
        const char *msg = "Pressione ENTER ou clique para continuar";
        int fontSize = 30;
        int textWidth = MeasureText(msg, fontSize);
        DrawText(msg, (GetScreenWidth() - textWidth) / 2,
                 GetScreenHeight() - 80, fontSize, WHITE);
    }
}

static void desenharFundo(GameWorld *gw)
{

    int larguraFundo = rm.texturaFundo.width;
    int larguraMapa = calcularLarguraMapa(gw->mapa);
    int alturaMapa = calcularAlturaMapa(gw->mapa);
    int repeticoes = larguraMapa / larguraFundo;

    int deslocamentoParallax =
        (int)((gw->camera.target.x / (float)larguraMapa) * 200);

    for (int i = 0; i <= repeticoes; i++)
    {
        DrawTexture(rm.texturaFundo, larguraFundo * i - deslocamentoParallax,
                    alturaMapa - rm.texturaFundo.height, WHITE);
    }
}

static void atualizarCamera(GameWorld *gw)
{

    Rectangle ret;

    if (gw->personagemSelecionado == 0)
    {
        Mario *m = (Mario *)gw->personagem->dados;
        ret = m->ret;
    }
    else
    {
        Jogador *j = (Jogador *)gw->personagem->dados;
        ret = j->ret;
    }

    Camera2D *c = &gw->camera;

    c->offset.x = GetScreenWidth() / 2;
    c->offset.y = GetScreenHeight() / 2;

    c->target.x = roundf(ret.x + ret.width / 2.0f);
    c->target.y = roundf(ret.y + ret.height / 2.0f);

    int minX = GetScreenWidth() / 2;
    int maxX = calcularLarguraMapa(gw->mapa) - GetScreenWidth() / 2;
    int maxY = calcularAlturaMapa(gw->mapa) - GetScreenHeight() / 2;

    if (c->target.x < minX)
    {
        c->target.x = minX;
    }
    else if (c->target.x > maxX)
    {
        c->target.x = maxX;
    }

    if (c->target.y > maxY)
    {
        c->target.y = maxY;
    }
}

static void inicializar(GameWorld *gw)
{
    // Carrega o mapa
    gw->mapa = carregarMapa("resources/mapas/yoshi-island-1.txt");

    if (gw->mapa == NULL)
    {
        printf("ERRO: Não foi possível carregar o mapa!\n");
        return;
    }

    int alturaMapa = calcularAlturaMapa(gw->mapa);
    int screenWidth = GetScreenWidth();

    if (gw->personagemSelecionado == 0)
    {
        gw->personagem = criarMario(screenWidth / 2 + 144,
                                    alturaMapa - 400, 96, 96);
    }
    else
    {
        gw->personagem = criarSonic(screenWidth / 2 + 144,
                                    alturaMapa - 400, 96, 96);
    }

    if (gw->personagem == NULL)
    {
        printf("ERRO: Não foi possível criar o personagem!\n");
        return;
    }

    gw->camera = (Camera2D){
        .offset = {0},
        .target = {0},
        .rotation = 0.0f,
        .zoom = 0.9f};

    gw->gravidade = 900;
}

static void reiniciar(GameWorld *gw)
{
    printf("=== REINICIANDO JOGO (DURANTE O JOGO) ===\n");

    // Para a música se estiver tocando
    if (IsMusicStreamPlaying(rm.musicaFase01))
    {
        StopMusicStream(rm.musicaFase01);
        printf("Música parada.\n");
    }

    // Salva a seleção de personagem antes de destruir
    int personagemSelecionado = gw->personagemSelecionado;

    // Destroi o mapa atual
    if (gw->mapa != NULL)
    {
        printf("Destruindo mapa...\n");
        destruirMapa(gw->mapa);
        gw->mapa = NULL;
        printf("Mapa destruído.\n");
    }

    // Destroi o personagem atual
    if (gw->personagem != NULL)
    {
        printf("Destruindo personagem...\n");
        gw->personagem->funcoes->destruir(gw->personagem->dados);
        free(gw->personagem);
        gw->personagem = NULL;
        printf("Personagem destruído.\n");
    }

    // Recria o mapa e o personagem
    printf("Recriando mapa e personagem...\n");
    gw->personagemSelecionado = personagemSelecionado;
    inicializar(gw);

    // Reseta os dados do personagem (vidas, anéis, score, etc)
    if (gw->personagem != NULL)
    {
        gw->personagem->quantidadeVidas = 3;
        gw->personagem->quantidadeAneis = 0;
        gw->personagem->score = 0;
        gw->personagem->comboAereo = 0;
        gw->personagem->time = 0.0f;
        gw->personagem->funcoes->resetar(gw->personagem->dados);
    }

    printf("=== REINICIO CONCLUÍDO ===\n");
}

static void desenharPontuacao(Texture2D hud, int valor, Vector2 posicao)
{

    char buffer[16];
    sprintf(buffer, "%d", valor);

    int digitoW = 8;
    int digitoH = 16;
    int inicioY = 432;
    int espacamento = 8;

    for (int i = 0; buffer[i] != '\0'; i++)
    {
        int digito = buffer[i] - '0';

        Rectangle origem = {(digito * (digitoW + espacamento)) + 72, inicioY,
                            digitoW, digitoH};

        Rectangle dest = {posicao.x + i * (digitoW * 2), posicao.y, digitoW * 2,
                          digitoH * 2};

        DrawTexturePro(hud, origem, dest, (Vector2){0, 0}, 0.0f, WHITE);
    }
}

static void desenharTempo(Texture2D hud, int tempo, Vector2 posicao)
{

    int minutos = tempo / 60;
    int segundos = tempo % 60;

    char buffer[16];
    sprintf(buffer, "%d:%02d", minutos, segundos);

    int digitoW = 8;
    int digitoH = 16;
    int inicioY = 432;
    int espacamento = 8;

    for (int i = 0; buffer[i] != '\0'; i++)
    {

        if (buffer[i] == ':')
        {
            DrawRectangle(posicao.x + i * (digitoW * 2) + 6, posicao.y + 8, 4,
                          4, WHITE);

            DrawRectangle(posicao.x + i * (digitoW * 2) + 6, posicao.y + 20, 4,
                          4, WHITE);

            continue;
        }

        int digito = buffer[i] - '0';

        Rectangle origem = {(digito * (digitoW + espacamento)) + 72, inicioY,
                            digitoW, digitoH};

        Rectangle dest = {posicao.x + i * (digitoW * 2), posicao.y, digitoW * 2,
                          digitoH * 2};

        DrawTexturePro(hud, origem, dest, (Vector2){0, 0}, 0.0f, WHITE);
    }
}

static void desenharTexto(Texture2D hud, const char *texto, Vector2 posicao,
                          float escala)
{

    int charW = 7;
    int charH = 11;
    int inicioX = 9;
    int inicioY = 82;
    int espacamento = 2;
    int cursor = 0;

    for (int i = 0; texto[i] != '\0'; i++)
    {

        char c = texto[i];
        int idx = -1;

        if (c >= 'A' && c <= 'Z')
        {
            idx = c - 'A';
        }
        else if (c == ' ')
        {
            cursor++;
            continue;
        }

        if (idx < 0)
            continue;

        Rectangle origem = {inicioX + (idx * (charW + espacamento)), inicioY,
                            charW, charH};

        Rectangle dest = {posicao.x + cursor * (charW * escala), posicao.y,
                          charW * escala, charH * escala};

        DrawTexturePro(hud, origem, dest, (Vector2){0, 0}, 0.0f, WHITE);
        cursor++;
    }
}