#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include "busitem.h"
#include "parkingarea.h"
#include "passengerqueue.h"
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QShowEvent>
#include <QList>
#include <QString>
#include <QPointer>
#include <QFrame>
#include <QTimer>
#include <QElapsedTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QLabel>

class BusItem;
class PassengerItem;

class GameWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
    void carregarNivel(int nivelId);

public slots:
    void onBusExitedParking(BusItem *bus);

signals:
    void voltarAoMenu();

protected:
    // Eventos do sistema para tratar da responsividade
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void reiniciarNivel();
    void mostrarDica();
    void iniciarAutoSolve();
    void passoAutoSolve();
    void atualizarTimerJogo();

private:
    QGraphicsView *m_view;
    QGraphicsScene *m_scene;

    QPushButton *btnVoltar;
    QPushButton *btnReset;
    QPushButton *btnHint;
    QPushButton *btnAutoSolve;

    QTimer *m_timerAutoSolve;
    bool m_usouAutoSolve;

    QPushButton *btnUndo;
    QList<QPointer<BusItem>> m_historicoUndo; // O nosso histórico seguro

    void realizarUndo();
    void mostrarHint();
    int m_nivelAtual;

    void gerarFila(const QStringList &cores);
    void adicionarVeiculo(int id, const QString &cor, int capacidade, int tamanho, Direction dir, int col, int linha);
    void construirNivel(int nivelId);

    QList<BusItem*> m_veiculosAtivos;

    PassengerQueue *m_passengerQueue;
    void processarFila();

    ParkingArea *m_parkingArea;

    QFrame *m_panelVitoria;
    void mostrarPainelVitoria();
    QFrame *m_panelGameOver;
    void mostrarPainelGameOver();

    QElapsedTimer m_timerNivel; // Mede quanto tempo passou
    int m_tempoLimiteSegundos;  // O limite, ex: 300s (5 min)
    QTimer *m_timerVisual;      // Para atualizar o relógio no ecrã (se tiveres uma Label)
    void verificarTempoLimite();

    QLabel* m_lblCronometro;   // A label que vai mostrar os tempos
    QTimer* m_timerJogo;       // O timer que vai contar o tempo
    int m_tempoDecorrido;      // Segundos desde o início do nível
    int m_melhorTempo;         // Melhor tempo do nível atual (carregado do JSON)

    QString formatarTempo(int segundosTotal);
    void atualizarInterfaceTempo();

    BusItem* avaliarMelhorJogada();

    void configurarBarraSuperior(QVBoxLayout *mainLayout);
    void configurarCena(QVBoxLayout *mainLayout);
    void configurarPaineisFinais();
};

#endif