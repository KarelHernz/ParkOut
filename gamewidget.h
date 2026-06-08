#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include "busitem.h"
#include "parkingarea.h"
#include "passengerqueue.h"
#include "estadojogo.h"
#include "autosolveria.h"

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QShowEvent>
#include <QList>
#include <QString>
#include <QPointer>
#include <QFrame>
#include <QTimer>
#include <QElapsedTimer>
#include <QLabel>

class GameWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
    ~GameWidget();
    void carregarNivel(int nivelId);

public slots:
    void onBusExitedParking(BusItem *bus);

signals:
    void voltarAoMenu();
    void solicitarCalculoIA(EstadoJogo estado);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void reiniciarNivel();
    void mostrarHint();
    void iniciarAutoSolve();
    void passoAutoSolve();
    void onJogadaIACalculada(int busId);
    void atualizarTimerJogo();

private:
    //Painel e visualização
    QGraphicsView *m_view;
    QGraphicsScene *m_scene;
    QGridLayout *m_mainLayout;
    QWidget *m_panelControlos;
    QGridLayout *m_controlosLayout;

    //Botões
    QPushButton *btnVoltar;
    QPushButton *btnReset;
    QPushButton *btnUndo;
    QPushButton *btnHint;
    QPushButton *btnAutoSolve;

    //Lógica do undo
    int m_nivelAtual;
    QList<QPointer<BusItem>> m_historicoUndo;
    void realizarUndo();

    //AutoSolveIA
    QTimer *m_timerAutoSolve;
    bool m_usouAutoSolve;
    bool m_esperandoHint = false;
    QThread* m_workerThread;
    AutoSolverIA* m_autoSolverIA;

    //Elementos do jogo
    QList<BusItem*> m_veiculosAtivos;
    PassengerQueue *m_passengerQueue;
    ParkingArea *m_parkingArea;

    //Painéis de Fim de Jogo
    QFrame *m_panelVitoria;
    QFrame *m_panelGameOver;

    //Gestão do tempo
    QElapsedTimer m_timerNivel;
    int m_tempoLimiteSegundos;
    QTimer *m_timerVisual;
    QTimer *m_timerJogo;
    int m_tempoDecorrido;
    int m_melhorTempo;

    QLabel *m_lblCronometro;
    QLabel *m_lblRecorde;

    //Métodos auxiliares
    void gerarFila(const QStringList &cores);
    void adicionarVeiculo(int id, const QString &cor, int capacidade, int tamanho, Direction dir, int col, int linha);
    void construirNivel(int nivelId);
    void processarFila();
    void verificarTempoLimite();
    void mostrarPainelVitoria();
    void mostrarPainelGameOver();
    QString formatarTempo(int segundosTotal);
    void atualizarInterfaceTempo();
    void configurarBarraSuperior();
    void configurarCena();
    void configurarPaineisFinais();
    void mostrarPainelAviso(const QString &titulo, const QString &texto, bool voltarMenuAoFechar);
};

#endif