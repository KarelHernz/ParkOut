#include "autosolveria.h"

AutoSolverIA::AutoSolverIA(QObject *parent) : QObject(parent) {
}

//Implementação da Lógica da Inteligência Artificial
void AutoSolverIA::calcularMelhorJogada(EstadoJogo estado) {
    int idMelhorAutocarro = -1;

    // Lógica de IA simplificada
    for (const auto& bus : estado.autocarros) {
        if (!bus.noParque && bus.cor == estado.corDaFrenteFila) {
            idMelhorAutocarro = bus.id;
            break;
        }
    }

    emit jogadaCalculada(idMelhorAutocarro);
}