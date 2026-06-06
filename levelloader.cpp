#include "levelloader.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QMessageBox>

LevelData LevelLoader::carregar(int nivelId) {
    LevelData data;

    QString caminhoFile = ":/src/json/levels.json";
    QFile file(caminhoFile);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, "Erro Crítico", "Os ficheiros do jogo estão corrompidos ou em falta. Por favor, reinstala a aplicação.");
        return data;
    }

    QByteArray bytes = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    QJsonObject root = doc.object();
    QJsonArray levelsArray = root["levels"].toArray();

    for (int i = 0; i < levelsArray.size(); ++i) {
        QJsonObject nivelObj = levelsArray[i].toObject();

        if (nivelObj["id"].toInt() == nivelId) {
            data.valido = true;

            //Lê a fila
            QJsonArray filaArray = nivelObj["fila"].toArray();
            for (int j = 0; j < filaArray.size(); ++j) {
                data.coresDaFila.append(filaArray[j].toString());
            }

            //Lê os autocarros
            QJsonArray veiculosArray = nivelObj["veiculos"].toArray();
            for (int j = 0; j < veiculosArray.size(); ++j) {
                QJsonObject v = veiculosArray[j].toObject();
                BusData bus;

                bus.id = v["id"].toInt();
                bus.cor = v["cor"].toString();
                bus.capacidade = v["capacidade"].toInt();
                bus.tamanho = v["tamanho"].toInt();
                bus.coluna = v["coluna"].toInt();
                bus.linha = v["linha"].toInt();

                QString dirStr = v["direcao"].toString();
                bus.direcao = Direction::Up;
                if (dirStr == "Right")      bus.direcao = Direction::Right;
                else if (dirStr == "Down")  bus.direcao = Direction::Down;
                else if (dirStr == "Left")  bus.direcao = Direction::Left;

                data.autocarros.append(bus);
            }
            return data;
        }
    }
    return data;
}