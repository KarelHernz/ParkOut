#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <QString>
#include <QStringList>
#include <QList>
#include "busitem.h"

// Caixa de transporte para os dados de UM autocarro
struct BusData {
    int id;
    QString cor;
    int capacidade;
    int tamanho;
    Direction direcao;
    int coluna;
    int linha;
};

struct LevelData {
    bool valido = false; // Vai ser 'true' se o nível existir, 'false' se já não houver mais níveis
    QStringList coresDaFila;
    QList<BusData> autocarros;
};

class LevelLoader {
public:
    // A função estática que o GameWidget vai chamar
    static LevelData carregar(int nivelId);
};

#endif