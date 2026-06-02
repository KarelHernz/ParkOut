#ifndef GAMECONFIG_H
#define GAMECONFIG_H

namespace GameConfig {
// Dimensões do Ecrã / Cena
constexpr int LARGURA_CENA = 640;
constexpr int ALTURA_CENA = 850;

// Configuração da Grelha de Jogo
constexpr int TAMANHO_CELULA = 80;
constexpr int NUM_COLUNAS = 7;
constexpr int NUM_LINHAS = 8;
constexpr int TOPO_GRELHA_Y = 210;
constexpr int LARGURA_GRELHA = NUM_COLUNAS * TAMANHO_CELULA; // 560px

// Fila de Passageiros
constexpr int PASSAGEIROS_Y = 20;
constexpr int FILA_START_X = 50;
constexpr int FILA_ESPACAMENTO = 35;
constexpr int LIMITE_PREVISAO_FILA = 3; // Quantos passageiros a IA analisa na fila

// Parque de Estacionamento (Slots)
constexpr int NUM_SLOTS = 3;
constexpr int LARGURA_SLOT = 160;
constexpr int ALTURA_SLOT = 80;
constexpr int SLOT_Y = 100;
constexpr int ESPACAMENTO_SLOTS = 40;
constexpr int SLOT_OFFSET_CENTRO = 3; // Os +3 pixeis para centrar

// Tempos e Timers (em milissegundos ou segundos)
constexpr int TEMPO_LIMITE_NIVEL_SEG = 300;
constexpr int FRAME_RATE_MS = 16;             // ~60 FPS para o motor de animação
constexpr int INTERVALO_AUTO_SOLVE_MS = 800;
constexpr int DURACAO_HINT_MS = 1500;
constexpr int DELAY_FILA_BUS_CHEIO_MS = 200;
constexpr int DELAY_FILA_PASSAGEIRO_MS = 800;
}

#endif