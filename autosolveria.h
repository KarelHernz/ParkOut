#ifndef AUTOSOLVERIA_H
#define AUTOSOLVERIA_H

#include <QObject>
#include "estadojogo.h"

class AutoSolverIA : public QObject {
    Q_OBJECT
public:
    explicit AutoSolverIA(QObject *parent = nullptr);

public slots:
    void calcularMelhorJogada(EstadoJogo estado);

signals:
    void jogadaCalculada(int busId);
};

#endif