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
    Texture2D texturaHUD2;
    Texture2D texturaGameOver;

    Sound somAnel;
    Sound somFrenagem;
    Sound somHitComAnel;
    Sound somHitInimigo;
    Sound somMorte;
    Sound somPulo;

    Music musicaFase01;
    float volumeMusicaFase01;

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