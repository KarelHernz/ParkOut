#ifndef GAMEPERSISTENCE_H
#define GAMEPERSISTENCE_H

#include <QString>

class GamePersistence {
public:
    static int lerMelhorTempo(int nivelId);
    static void guardarRecorde(int nivelId, int tempoGasto);
    static int getNivelDesbloqueado();
    static void guardarProgresso(int nivelDesbloqueado);
    static bool apagarProgresso();
};

#endif