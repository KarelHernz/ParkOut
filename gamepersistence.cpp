#include "gamepersistence.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

//Função auxiliar interna para descobrir onde é seguro gravar no dispositivo
QString obterCaminhoSeguro(const QString& nomeFicheiro) {
    // Pede ao sistema operativo a pasta de dados oficial da aplicação
    QString diretorio = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    //Garante que a pasta existe
    QDir dir;
    if (!dir.exists(diretorio)) {
        dir.mkpath(diretorio);
    }

    return diretorio + "/" + nomeFicheiro;
}

int GamePersistence::lerMelhorTempo(int nivelId) {
    QString caminhoArquivo = obterCaminhoSeguro("recordes.json");
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

//Guarda o recorde do jogador
void GamePersistence::guardarRecorde(int nivelId, int tempo) {
    QString caminhoArquivo = obterCaminhoSeguro("recordes.json");
    QJsonObject json;
    QFile file(caminhoArquivo);

    //Lê o que já lá estava para não apagar os recordes de outros níveis
    if (file.open(QIODevice::ReadOnly)) {
        json = QJsonDocument::fromJson(file.readAll()).object();
        file.close();
    }

    //Atualiza ou insere o novo recorde
    json[QString("Nivel_%1").arg(nivelId)] = tempo;

    //Grava tudo de volta no disco
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(json).toJson());
        file.close();
    } else {
        qWarning() << "Erro: Não foi possível gravar o recorde em" << caminhoArquivo;
    }
}

int GamePersistence::getNivelDesbloqueado() {
    QString caminhoArquivo = obterCaminhoSeguro("progress.json");
    QFile file(caminhoArquivo);

    //Se for a primeira vez a jogar, começa no nível 1
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return 1;
    }
    return QJsonDocument::fromJson(file.readAll()).object()["unlockedLevel"].toInt(1);
}

//Guarda o progresso do jogador
void GamePersistence::guardarProgresso(int nivelDesbloqueado) {
    int nivelAtualGuardado = getNivelDesbloqueado();

    if (nivelDesbloqueado <= nivelAtualGuardado) return;

    QString caminhoArquivo = obterCaminhoSeguro("progress.json");
    QFile file(caminhoArquivo);

    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject obj;
        obj["unlockedLevel"] = nivelDesbloqueado;
        file.write(QJsonDocument(obj).toJson());
        file.close();
    } else {
        qWarning() << "Erro: Não foi possível gravar o progresso em" << caminhoArquivo;
    }
}

//Apaga o progresso do jogador
bool GamePersistence::apagarProgresso() {
    QString pastaGuardar = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QFile ficheiroProgresso(pastaGuardar + "/progress.json");
    QFile ficheiroRecordes(pastaGuardar + "/recordes.json");

    bool apagouAlgo = false;

    if (ficheiroProgresso.exists()) {
        ficheiroProgresso.remove();
        apagouAlgo = true;
    }

    if (ficheiroRecordes.exists()) {
        ficheiroRecordes.remove();
        apagouAlgo = true;
    }

    return apagouAlgo;
}