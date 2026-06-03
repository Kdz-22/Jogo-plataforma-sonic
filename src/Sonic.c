// Sonic.c
#include <stdlib.h>

#include "Jogador.h"
#include "Sonic.h"
#include "Tipos.h"

static void _entrada( void *dados, Personagem *p, float delta ) {
    entradaJogador( (Jogador*) dados, p, delta );
}

static void _atualizar( void *dados, Personagem *p, struct GameWorld *gw, float delta ) {
    atualizarJogador( (Jogador*) dados, p, gw, delta );
}

static void _desenhar( void *dados ) {
    desenharJogador( (Jogador*) dados );
}

static void _destruir( void *dados ) {
    destruirJogador( (Jogador*) dados );
}

static void _resetar( void *dados ) {
    Jogador *j = (Jogador*) dados;
    j->invulneravel = false;
    j->contadorTempoInvulnerabilidade = 0.0f;
    j->contadorTempoPiscaPisca = 0.0f;
    j->piscaPisca = false;
    j->estado = ESTADO_JOGADOR_PARADO;
}

static PersonagemFuncoes sonicFuncoes = {
    _entrada,
    _atualizar,
    _desenhar,
    _destruir,
    _resetar
};

Personagem *criarSonic( float x, float y, float w, float h ) {
    Personagem *p = (Personagem*) malloc( sizeof( Personagem ) );
    p->dados = criarJogador( x, y, w, h );
    p->funcoes = &sonicFuncoes;

    p->quantidadeAneis = 0;
    p->quantidadeVidas = 3;
    p->score = 0;
    p->comboAereo = 0;
    p->time = 0.0f;

    return p;
}