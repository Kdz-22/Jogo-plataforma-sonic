/**
 * @file ResourceManager.c
 * @author Prof. Dr. David Buzatto
 * @brief ResourceManager implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"

#include "ResourceManager.h"
#include "Utils.h"

ResourceManager rm = {0};

void loadResourcesResourceManager(void)
{

    rm.texturaJogador = carregarTexturaAlterandoCores(
        "resources/imagens/sprites/sonic.png",
        (Color[]){
            {37, 102, 26, 255},
            {13, 72, 7, 255},
        },
        (Color[]){
            BLANK,
            BLANK,
        },
        2);

    rm.texturaMario = carregarTexturaAlterandoCores(
        "resources/imagens/sprites/SNES-SuperMarioWorld-PlayableCharacters-Mario.png",
        (Color[]){
            {0, 116, 116, 255},
            {0, 84, 84, 255},
            {0, 52, 52, 255},
        },
        (Color[]){
            BLANK,
            BLANK,
            BLANK,
        },
        3);

    rm.texturaBadniks = carregarTexturaAlterandoCores(
        "resources/imagens/sprites/badniks.png",
        (Color[]){
            {13, 72, 7, 255},
        },
        (Color[]){
            BLANK,
        },
        1);

    rm.texturaItens = carregarTexturaAlterandoCores(
        "resources/imagens/itens/itens.png",
        (Color[]){
            {16, 112, 132, 255},
        },
        (Color[]){
            BLANK,
        },
        1);

    rm.texturaHUD = carregarTexturaAlterandoCores(
        "resources/imagens/hud/hud.png",
        (Color[]){
            {16, 112, 132, 255},
        },
        (Color[]){
            BLANK,
        },
        1);

    rm.texturaHUD2 = carregarTexturaAlterandoCores(
        "resources/imagens/hud/hud.png",
        (Color[]){
            {25, 214, 247, 255},
        },
        (Color[]){
            BLANK,
        },
        1);

    rm.texturaItensAnimadosMario = carregarTexturaAlterandoCores(
        "resources/imagens/itens/itens-animados-mario.png",
        (Color[]){
            {248, 0, 248, 255},
        },
        (Color[]){
            BLANK,
        },
        1);

    rm.texturaTerreno = LoadTexture("resources/imagens/tiles/terreno.png");
    rm.texturaFundo = LoadTexture("resources/imagens/fundo/fundo.png");
    rm.texturaCanos = LoadTexture("resources/imagens/tiles/canos.png");
    rm.texturaTelaInicial = LoadTexture("resources/telainicialMario.jpeg");

    // Filtro de textura nearest-neighbor (ponto) para todas as texturas do jogo.
    // Evita interpolação bilinear nas bordas dos tiles e sprites, que causaria
    // bleeding de cor entre pixels adjacentes — efeito indesejável em pixel art.
    SetTextureFilter(rm.texturaJogador, TEXTURE_FILTER_POINT);
    SetTextureFilter(rm.texturaMario, TEXTURE_FILTER_POINT);
    SetTextureFilter(rm.texturaBadniks, TEXTURE_FILTER_POINT);
    SetTextureFilter(rm.texturaItens, TEXTURE_FILTER_POINT);
    SetTextureFilter(rm.texturaTerreno, TEXTURE_FILTER_POINT);
    SetTextureFilter(rm.texturaFundo, TEXTURE_FILTER_POINT);
    SetTextureFilter(rm.texturaHUD, TEXTURE_FILTER_POINT);
    SetTextureFilter(rm.texturaHUD2, TEXTURE_FILTER_POINT);
    SetTextureFilter(rm.texturaCanos, TEXTURE_FILTER_POINT);
    SetTextureFilter(rm.texturaItensAnimadosMario, TEXTURE_FILTER_POINT);
    SetTextureFilter(rm.texturaTelaInicial, TEXTURE_FILTER_POINT);

    rm.somAnel = LoadSound("resources/sons/efeitos/anel.wav");
    rm.somFrenagem = LoadSound("resources/sons/efeitos/frenagem.wav");
    rm.somHitComAnel = LoadSound("resources/sons/efeitos/hit-com-anel.wav");
    rm.somHitInimigo = LoadSound("resources/sons/efeitos/hit-inimigo.wav");
    rm.somMorte = LoadSound("resources/sons/efeitos/morte.wav");
    rm.somPulo = LoadSound("resources/sons/efeitos/pulo.wav");

    rm.musicaFase01 = LoadMusicStream("resources/sons/musicas/green-hill-zone.mp3");
    rm.volumeMusicaFase01 = 0.0f;
    SetMusicVolume(rm.musicaFase01, rm.volumeMusicaFase01);
}

void unloadResourcesResourceManager(void)
{

    UnloadTexture(rm.texturaJogador);
    UnloadTexture(rm.texturaMario);
    UnloadTexture(rm.texturaBadniks);
    UnloadTexture(rm.texturaItens);
    UnloadTexture(rm.texturaTerreno);
    UnloadTexture(rm.texturaFundo);
    UnloadTexture(rm.texturaHUD);
    UnloadTexture(rm.texturaHUD2);
    UnloadTexture(rm.texturaCanos);
    UnloadTexture(rm.texturaItensAnimadosMario);
    UnloadTexture(rm.texturaTelaInicial);

    UnloadSound(rm.somAnel);
    UnloadSound(rm.somFrenagem);
    UnloadSound(rm.somHitComAnel);
    UnloadSound(rm.somHitInimigo);
    UnloadSound(rm.somMorte);
    UnloadSound(rm.somPulo);

    UnloadMusicStream(rm.musicaFase01);
}