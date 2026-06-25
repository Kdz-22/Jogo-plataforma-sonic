/**
 * @file ResourceManager.h
 * @author Prof. Dr. David Buzatto
 * @brief ResourceManager struct and function declarations.
 * 
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "raylib/raylib.h"

typedef struct ResourceManager {

    Texture2D texturaJogador; //Sonic
    Texture2D texturaMario;
    Texture2D texturaBadniks;
    Texture2D texturaItens;

    Texture2D texturaTerreno;
    Texture2D texturaFundo;
    Texture2D texturaCanos;
    Texture2D texturaHUD;
    Texture2D texturaHUDmario;
    Texture2D texturaGameOver;
    Texture2D texturaItensAnimadosMario;
    Texture2D texturaTelaInicial;

    Sound somAnel;
    Sound somFrenagem;
    Sound somHitComAnel;
    Sound somHitInimigo;
    Sound somMorte;
    Sound somPulo;

    Sound somPowerUp;
    Sound somPuloMario;
    Sound somMorrendo;
    Sound som1Up;
    Sound somPuloGirando;

    Music musicaFaseMario;
    Music musicaFase01;
    float volumeMusicaFase01;
    float volumeMusicaFaseMario;

} ResourceManager;

/**
 * @brief Global ResourceManager instance.
 */
extern ResourceManager rm;

/**
 * @brief Load global game resources, linking them in the global instance of
 * ResourceManager called rm.
 */
void loadResourcesResourceManager( void );

/**
 * @brief Unload global game resources.
 */
void unloadResourcesResourceManager( void );