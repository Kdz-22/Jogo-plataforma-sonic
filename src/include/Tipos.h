/**
 * @file Tipos.h
 * @author Prof. Dr. David Buzatto
 * @brief Definição dos tipos utilizados no jogo.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include <stdbool.h>

#include "raylib/raylib.h"

typedef struct GameWorld GameWorld;
typedef struct Personagem Personagem;

/**
 * @brief Representa o estado do jogador.
 */
typedef enum EstadoJogador {
    ESTADO_JOGADOR_PARADO,
    ESTADO_JOGADOR_ANDANDO,
    ESTADO_JOGADOR_ANDANDO_RAPIDO,
    ESTADO_JOGADOR_CORRENDO,
    ESTADO_JOGADOR_PULANDO,
    ESTADO_JOGADOR_PULANDO_RAPIDO,
    ESTADO_JOGADOR_PULANDO_CORRENDO,
} EstadoJogador;

/**
 * @brief Representa o estado do Mario.
 */
typedef enum EstadoMario {
    ESTADO_MARIO_PARADO,
    ESTADO_MARIO_ANDANDO,
    ESTADO_MARIO_CORRENDO,
    ESTADO_MARIO_PULANDO,
    ESTADO_MARIO_PULANDO_CORRENDO,
    ESTADO_MARIO_PULANDO_GIRANDO,
    ESTADO_MARIO_CAINDO,
    ESTADO_MARIO_ABAIXADO,
    ESTADO_MARIO_CRESCENDO,
} EstadoMario;

/**
 * @brief Representa o estado do jogado.
 */
typedef enum EstadoJogo {
    ESTADO_JOGO_SELECAO_PERSONAGEM,
    ESTADO_JOGO_JOGANDO,
    ESTADO_JOGO_GAMEOVER,
} EstadoJogo;

/**
 * @brief Representa o estado do inimigo do tipo Motobug.
 */
typedef enum EstadoInimigoMotobug {
    ESTADO_INIMIGO_MOTOBUG_ANDANDO,
    ESTADO_INIMIGO_MOTOBUG_MORRENDO,
} EstadoInimigoMotobug;

typedef enum EstadoInimigoSpikes {
    ESTADO_INIMIGO_SPIKES_ANDANDO,
    ESTADO_INIMIGO_SPIKES_MORRENDO,
} EstadoInimigoSpikes;

typedef enum EstadoInimigoTonTon {
    ESTADO_INIMIGO_TONTON_ANDANDO,
    ESTADO_INIMIGO_TONTON_MORRENDO,
} EstadoInimigoTonTon;

/* INIMIGOS MARIO*/
typedef enum EstadoInimigoKoopaRed {
    ESTADO_KOOPA_ANDANDO,
    ESTADO_KOOPA_MORRENDO,
    ESTADO_KOOPA_CASCO_PARADO,
    ESTADO_KOOPA_CASCO_CORRENDO,
} EstadoInimigoKoopaRed;

typedef enum EstadoInimigoRex {
    ESTADO_INIMIGO_REX_ANDANDO,
    ESTADO_INIMIGO_REX_MORRENDO,
} EstadoInimigoRex;

typedef enum EstadoInimigoNegoPreto
{
    ESTADO_INIMIGO_NEGOPRETO_VOANDO,
} EstadoInimigoNegoPreto;

/**
 * @brief Representa o tipo de um inimigo.
 */
typedef enum TipoInimigo {
    TIPO_INIMIGO_MOTOBUG,
    TIPO_INIMIGO_SPIKES,
    TIPO_INIMIGO_TONTON,
    TIPO_INIMIGO_KOOPARED,
    TIPO_INIMIGO_REX,
    TIPO_INIMIGO_NEGOPRETO,
} TipoInimigo;

/**
 * @brief Representa o estado do item do tipo anel.
 */
typedef enum EstadoItemAnel {
    ESTADO_ITEM_ANEL_PARADO,
    ESTADO_ITEM_ANEL_COLETADO,
} EstadoItemAnel;

typedef enum EstadoItemAnelAzul {
    ESTADO_ITEM_ANEL_AZUL_PARADO,
    ESTADO_ITEM_ANEL_AZUL_COLETADO,
} EstadoItemAnelAzul;

typedef enum EstadoItemCogumeloVermelho {
    ESTADO_ITEM_COGUMELO_VERMELHO_PARADO,
    ESTADO_ITEM_COGUMELO_VERMELHO_COLETADO,
} EstadoItemCogumeloVermelho;

typedef enum EstadoItemFlorPreta {
    ESTADO_ITEM_FLOR_PRETA_PARADA,
    ESTADO_ITEM_FLOR_PRETA_SAINDO_DO_BLOCO,
    ESTADO_ITEM_FLOR_PRETA_COLETADA
} EstadoItemFlorPreta;

/**
 * @brief Representa o tipo de um item.
 */
typedef enum TipoItem {
    TIPO_ITEM_ANEL,
    TIPO_ITEM_ANEL_AZUL,
    TIPO_ITEM_COGUMELO_VERMELHO,
    TIPO_ITEM_FLOR_PRETA,
} TipoItem;

/**
 * @brief Representa o tipo de um elemento do mapa
 */
typedef enum TipoElementoMapa {
    TIPO_ELEMENTO_MAPA_OBSTACULO,
    TIPO_ELEMENTO_MAPA_ITEM,
    TIPO_ELEMENTO_MAPA_INIMIGO,
} TipoElementoMapa;

/**
 * @brief Representa um quadro de animação com imagem.
 */
typedef struct QuadroAnimacao {
    Rectangle fonte;
    int duracao;          // milisegundos
    Rectangle retColisao; // deve ser interpretado como posicionamento relativo
} QuadroAnimacao;

/**
 * @brief Representa uma animação.
 */
typedef struct Animacao {
    QuadroAnimacao *quadros;
    int quantidadeQuadros;
    int quadroAtual;
    int contadorTempoQuadro;
    bool pararNoUltimoQuadro;
    bool executarUmaVez;
    bool finalizada;
} Animacao;

/**
 * @brief Representa o jogador controlado pelo usuário.
 */
typedef struct Jogador {

    Rectangle ret;
    Vector2 vel;
    Color cor;

    float velAndando;
    float velAndandoRapido;
    float velCorrendo;
    float velPulo;
    float velMaxQueda;

    float aceleracao;
    float desaceleracao;
    float frenagem;

    int quantidadePulos;
    int quantidadeMaxPulos;

    bool invulneravel;
    float tempoInvulnerabilidade;
    float contadorTempoInvulnerabilidade;
    int vidas;

    bool piscaPisca;
    float tempoPiscaPisca;
    float contadorTempoPiscaPisca;

    bool freando;

    EstadoJogador estado;
    bool olhandoParaDireita;

    Animacao *animacoes[20];
    int quantidadeAnimacoes;

    Animacao animacaoParado;
    Animacao animacaoAndando;
    Animacao animacaoAndandoRapido;
    Animacao animacaoCorrendo;
    Animacao animacaoPulando;
    Animacao animacaoPulandoRapido;
    Animacao animacaoPulandoCorrendo;

} Jogador;

typedef struct Mario {

    Rectangle ret;
    Rectangle retOriginal;
    Vector2 vel;
    Color cor;

    float velAndando;
    float velCorrendo;
    float velPulo;
    float velMaxQueda;

    float frenagem;
    float aceleracao;
    float desaceleracao;

    int quantidadePulos;
    int quantidadeMaxPulos;

    bool invulneravel;
    float tempoInvulnerabilidade;
    float contadorTempoInvulnerabilidade;
    int vidas;

    bool piscaPisca;
    float tempoPiscaPisca;
    float contadorTempoPiscaPisca;

    bool freando;
    bool grande;
    bool pulandoGirando;

    EstadoMario estado;
    bool olhandoParaDireita;

    Animacao *animacoes[20];
    Animacao *animacoesGrande[20];
    int quantidadeAnimacoes;

    Animacao animacaoParado;
    Animacao animacaoAndando;
    Animacao animacaoCorrendo;
    Animacao animacaoPulando;
    Animacao animacaoPulandoCorrendo;
    Animacao animacaoPulandoGirando;
    Animacao animacaoCaindo;
    Animacao animacaoAbaixado;

    Animacao animacaoCrescendo;

    Animacao animacaoGrandeParado;
    Animacao animacaoGrandeAndando;
    Animacao animacaoGrandeCorrendo;
    Animacao animacaoGrandePulando;
    Animacao animacaoGrandePulandoCorrendo;
    Animacao animacaoGrandePulandoGirando;
    Animacao animacaoGrandeCaindo;
    Animacao animacaoGrandeAbaixado;

} Mario;

/* INIMIGOS SONIC */
/**
 * @brief Representa um inimigo do tipo Motobug.
 */
typedef struct InimigoMotobug {

    Rectangle ret;
    Vector2 vel;
    Color cor;

    float velAndando;
    float velMaxQueda;

    EstadoInimigoMotobug estado;
    bool ativo;
    bool olhandoParaDireita; // *cuidado! a reflexão dos inimigos é ao contrário
                             // do jogador! eles começam olhando para a esquerda
                             // e as sprites são orientadas para a esquerda
                             // inicialmente
    Animacao *animacoes[2];
    int quantidadeAnimacoes;

    Animacao animacaoAndando;
    Animacao animacaoMorrendo;

} InimigoMotobug;

typedef struct InimigoSpikes {

    Rectangle ret;
    Vector2 vel;
    Color cor;

    float velAndando;
    float velMaxQueda;

    EstadoInimigoSpikes estado;
    bool ativo;
    bool olhandoParaDireita;

    Animacao *animacoes[2];
    int quantidadeAnimacoes;

    Animacao animacaoAndando;
    Animacao animacaoMorrendo;

} InimigoSpikes;

typedef struct BolaTonTon {

    Rectangle ret;
    Vector2 vel;
    bool ativo;

    Animacao animacao;

} BolaTonTon;

typedef struct InimigoTonTon {

    Rectangle ret;
    Vector2 vel;
    Color cor;

    float velAndando;
    float velMaxQueda;

    BolaTonTon bola[5];
    float contadorTiroBola;

    EstadoInimigoTonTon estado;
    bool ativo;
    bool olhandoParaDireita;

    Animacao *animacoes[2];
    int quantidadeAnimacoes;

    Animacao animacaoAndando;
    Animacao animacaoMorrendo;

} InimigoTonTon;

/* INIMIGOS MARIO */
typedef struct InimigoKoopaRed {

    Rectangle ret;
    Vector2 vel;
    Color cor;

    float velAndando;
    float velMaxQueda;

    EstadoInimigoKoopaRed estado;
    bool ativo;
    bool olhandoParaDireita;

    Animacao *animacoes[2];
    int quantidadeAnimacoes;
    Animacao animacaoAndando;
    Animacao animacaoCasco;

} InimigoKoopaRed;

typedef struct InimigoRex {

    Rectangle ret;
    Vector2 vel;
    Color cor;

    float velAndando;
    float velMaxQueda;

    EstadoInimigoRex estado;
    bool ativo;
    bool olhandoParaDireita;

    Animacao *animacoes[2];
    int quantidadeAnimacoes;

    Animacao animacaoAndando;
    Animacao animacaoMorrendo;

} InimigoRex;

typedef struct InimigoNegoPreto
{

    Rectangle ret;
    Vector2 vel;
    Color cor;

    float velVoo;

    EstadoInimigoNegoPreto estado;
    bool ativo;
    bool olhandoParaDireita;

    Animacao *animacoes[1];
    int quantidadeAnimacoes;

    Animacao animacaoVoando;

} InimigoNegoPreto;

/**
 * @brief Representa um inimigo.
 * O inimigo de fato é endereçado via membro "objeto".
 */
typedef struct Inimigo {
    void *objeto;
    TipoInimigo tipo;
} Inimigo;

/**
 * @brief Representa um item do tipo anel.
 */
typedef struct ItemAnel {

    Rectangle ret;
    Color cor;

    EstadoItemAnel estado;
    bool ativo;

    Animacao *animacoes[2];
    int quantidadeAnimacoes;

    Animacao animacaoParado;
    Animacao animacaoColetando;

} ItemAnel;

/**
 * @brief Representa um item do tipo anel.
 */
typedef struct ItemAnelAzul {

    Rectangle ret;
    Color cor;

    EstadoItemAnelAzul estado;
    bool ativo;

    Animacao *animacoes[2];
    int quantidadeAnimacoes;

    Animacao animacaoParado;
    Animacao animacaoColetando;

} ItemAnelAzul;

typedef struct ItemCogumeloVermelho {

    Rectangle ret;
    Color cor;

    EstadoItemCogumeloVermelho estado;
    bool ativo;

    Animacao *animacoes[2];
    int quantidadeAnimacoes;

    Animacao animacaoParado;
    Animacao animacaoColetando;

} ItemCogumeloVermelho;

typedef struct ItemFlorPreta {

    Rectangle ret;
    Color cor;

    EstadoItemFlorPreta estado;
    bool ativo;

    Animacao *animacoes[3];
    int quantidadeAnimacoes;

    Animacao animacaoParado;
    Animacao animacaoSaindoDoBloco;
    Animacao animacaoColetando;


} ItemCogumeloVeItemFlorPretarmelho;

/**
 * @brief Representa um item estático do mapa.
 * O item de fato é endereçado via membro "objeto".
 */
typedef struct Item {
    void *objeto;
    TipoItem tipo;
} Item;

/**
 * @brief Representa um obstáculo estático do mapa.
 */
typedef struct Obstaculo {
    Rectangle ret;
    Color cor;
    Rectangle fonte;
    Rectangle fonteUsado;
    Texture2D *textura;
    

    bool solido;
    bool eBlocoInterrogacao;

    bool eBlocoGiratorio;
    bool quebrando;
    bool quebrado;
    int quadroQuebra;
    float tempoQuadroQuebra;
} Obstaculo;

/**
 * @brief Representa um elemento do mapa.
 * O elemento de fato é endereçado via membro "objeto".
 */
typedef struct ElementoMapa ElementoMapa;
struct ElementoMapa {
    void *objeto;
    TipoElementoMapa tipo;
    ElementoMapa *proximo;
};

/**
 * @brief Representa um mapa de fase do jogo.
 */
typedef struct Mapa {

    // listas ligadas de elementos do mapa
    ElementoMapa *obstaculos; // marca o fim da lista
    int quantidadeObstaculos;

    ElementoMapa *itens; // marca o fim da lista
    int quantidadeItens;

    ElementoMapa *inimigos; // marca o fim da lista
    int quantidadeInimigos;

    float dimensaoPadraoElementos;
    int linhas;
    int colunas;

} Mapa;

typedef struct PersonagemFuncoes {
    void (*entrada)(void *dados, Personagem *p, float delta);
    void (*atualizar)(void *dados, Personagem *p, struct GameWorld *gw,
                      float delta);
    void (*desenhar)(void *dados);
    void (*destruir)(void *dados);
    void (*resetar)(void *dados);
} PersonagemFuncoes;

struct Personagem {
    void *dados;
    PersonagemFuncoes *funcoes;

    int quantidadeAneis;
    int quantidadeVidas;
    int score;
    float time;
    int comboAereo;
};
/**
 * @brief Representa o mundo do jogo e seus elementos.
 */
struct GameWorld {

    Mapa *mapa;

    Personagem *personagem;

    Camera2D camera;

    float gravidade;

    EstadoJogo estado;

    int personagemSelecionado;
};
