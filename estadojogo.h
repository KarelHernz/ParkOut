#ifndef ESTADOJOGO_H
#define ESTADOJOGO_H

#include <QString>
#include <QList>
#include <QMetaType>

struct EstadoJogo {
    QString corDaFrenteFila;

    struct DadosBus {
        int id;
        QString cor;
        int linha;
        int coluna;
        int direcao;
        bool noParque;
    };

    QList<DadosBus> autocarros;
};

//Avisa o motor do Qt que esta estrutura pode viajar através de Sinais/Slots em Threads
Q_DECLARE_METATYPE(EstadoJogo)

#endif