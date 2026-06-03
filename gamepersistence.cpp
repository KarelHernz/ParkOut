#include "gamepersistence.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>

int GamePersistence::lerMelhorTempo(int nivelId) {
    QString caminhoArquivo = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/recordes.json";
    QFile file(caminhoArquivo);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject json = doc.object();
        file.close();

        QString chave = QString("Nivel_%1").arg(nivelId);
        if (json.contains(chave)) {
            return json[chave].toInt();
        }
    }
    return 9999; // Sem recorde
}

void GamePersistence::guardarRecorde(int nivelId, int tempo) {
    QString caminhoPasta = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(caminhoPasta);
    QString caminhoArquivo = caminhoPasta + "/recordes.json";

    QJsonObject json;
    QFile file(caminhoArquivo);
    if (file.open(QIODevice::ReadOnly)) {
        json = QJsonDocument::fromJson(file.readAll()).object();
        file.close();
    }

    json[QString("Nivel_%1").arg(nivelId)] = tempo;

    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(json).toJson());
        file.close();
    }
}

int GamePersistence::getNivelDesbloqueado() {
    QString caminho = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/progress.json";
    QFile file(caminho);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return 1;
    }
    return QJsonDocument::fromJson(file.readAll()).object()["unlockedLevel"].toInt(1);
}

void GamePersistence::guardarProgresso(int nivelDesbloqueado) {
    int nivelAtualGuardado = getNivelDesbloqueado();
    if (nivelDesbloqueado <= nivelAtualGuardado) return;

    QString caminhoPasta = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(caminhoPasta);
    QFile file(caminhoPasta + "/progress.json");

    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject obj;
        obj["unlockedLevel"] = nivelDesbloqueado;
        file.write(QJsonDocument(obj).toJson());
        file.close();
    }
}
