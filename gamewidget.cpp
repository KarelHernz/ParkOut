#include "gamewidget.h"
#include "busitem.h"
#include "passengeritem.h"
#include "gamepersistence.h"
#include "levelloader.h"
#include "gameconfig.h"

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QPen>
#include <QBrush>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QMessageBox>
#include <QTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>

GameWidget::GameWidget(QWidget *parent) : QWidget(parent), m_nivelAtual(1) {
    m_tempoDecorrido = 0;
    m_melhorTempo = 9999;
    m_usouAutoSolve = false;

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    configurarBarraSuperior(mainLayout);
    configurarCena(mainLayout);
    configurarPaineisFinais();
}

void GameWidget::configurarBarraSuperior(QVBoxLayout *mainLayout) {
    QHBoxLayout *topBarLayout = new QHBoxLayout();

    btnVoltar = new QPushButton("SAIR", this);
    btnVoltar->setFixedSize(100, 40);
    btnVoltar->setStyleSheet("background-color: #E74C3C; color: white; font-weight: bold; border-radius: 10px;");

    btnReset = new QPushButton("RESET", this);
    btnReset->setFixedSize(100, 40);
    btnReset->setStyleSheet("background-color: #34495E; color: white; font-weight: bold; border-radius: 10px;");

    btnUndo = new QPushButton("UNDO", this);
    btnUndo->setFixedSize(100, 40);
    btnUndo->setStyleSheet("background-color: #9B59B6; color: white; font-weight: bold; border-radius: 10px;");

    btnHint = new QPushButton("HINT", this);
    btnHint->setFixedSize(100, 40);
    btnHint->setStyleSheet("background-color: #F1C40F; color: #2C3E50; font-weight: bold; border-radius: 10px;");

    btnAutoSolve = new QPushButton("AUTO-SOLVE", this);
    btnAutoSolve->setFixedSize(120, 40);
    btnAutoSolve->setStyleSheet("background-color: #E67E22; color: white; font-weight: bold; border-radius: 10px;");

    // Adiciona os botões ao Layout de topo
    topBarLayout->addWidget(btnVoltar);
    topBarLayout->addWidget(btnUndo);
    topBarLayout->addWidget(btnReset);
    topBarLayout->addWidget(btnHint);
    topBarLayout->addWidget(btnAutoSolve);
    topBarLayout->addWidget(m_lblCronometro);
    topBarLayout->addStretch();

    // Adiciona o topo à janela principal
    mainLayout->addLayout(topBarLayout);

    // Liga os botões
    connect(btnVoltar, &QPushButton::clicked, this, [this]() { emit voltarAoMenu(); });
    connect(btnUndo, &QPushButton::clicked, this, &GameWidget::realizarUndo);
    connect(btnReset, &QPushButton::clicked, this, &GameWidget::reiniciarNivel);
    connect(btnHint, &QPushButton::clicked, this, &GameWidget::mostrarHint);
    connect(btnAutoSolve, &QPushButton::clicked, this, &GameWidget::iniciarAutoSolve);

    // Configura os Timers
    m_timerJogo = new QTimer(this);
    connect(m_timerJogo, &QTimer::timeout, this, &GameWidget::atualizarTimerJogo);

    m_timerAutoSolve = new QTimer(this);
    connect(m_timerAutoSolve, &QTimer::timeout, this, &GameWidget::passoAutoSolve);
}

void GameWidget::configurarCena(QVBoxLayout *mainLayout) {
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, GameConfig::LARGURA_CENA, GameConfig::ALTURA_CENA);

    m_view = new QGraphicsView(m_scene, this);

    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform);
    m_view->setStyleSheet("background-color: #ECF0F1; border: 3px solid #2C3E50; border-radius: 15px;");
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Adiciona a visualização à janela principal
    mainLayout->addWidget(m_view);
}

void GameWidget::configurarPaineisFinais() {
    //Painel vitoria
    m_panelVitoria = new QFrame(this);
    m_panelVitoria->setFixedSize(300, 250);
    m_panelVitoria->setStyleSheet("QFrame { background-color: #ECF0F1; border: 4px solid #27AE60; border-radius: 15px; }");

    QVBoxLayout *vitoriaLayout = new QVBoxLayout(m_panelVitoria);

    QLabel *lblVitoria = new QLabel("NÍVEL CONCLUÍDO!", m_panelVitoria);
    lblVitoria->setAlignment(Qt::AlignCenter);
    lblVitoria->setStyleSheet("font-size: 22px; font-weight: bold; color: #27AE60; border: none;");

    QPushButton *btnProximoNivelVitoria = new QPushButton("PRÓXIMO NÍVEL", m_panelVitoria);
    btnProximoNivelVitoria->setFixedHeight(40);
    btnProximoNivelVitoria->setStyleSheet("background-color: #2980B9; color: white; font-weight: bold; border-radius: 10px;");

    QPushButton *btnReiniciarVitoria = new QPushButton("REINICIAR", m_panelVitoria);
    btnReiniciarVitoria->setFixedHeight(40);
    btnReiniciarVitoria->setStyleSheet("background-color: #34495E; color: white; font-weight: bold; border-radius: 10px;");

    QPushButton *btnMenuVitoria = new QPushButton("VOLTAR AO MENU", m_panelVitoria);
    btnMenuVitoria->setFixedHeight(40);
    btnMenuVitoria->setStyleSheet("background-color: #E74C3C; color: white; font-weight: bold; border-radius: 10px;");

    vitoriaLayout->addWidget(lblVitoria);
    vitoriaLayout->addWidget(btnProximoNivelVitoria);
    vitoriaLayout->addWidget(btnReiniciarVitoria);
    vitoriaLayout->addWidget(btnMenuVitoria);

    m_panelVitoria->hide();

    connect(btnProximoNivelVitoria, &QPushButton::clicked, this, [this](){
        m_panelVitoria->hide();
        carregarNivel(m_nivelAtual + 1);
    });
    connect(btnReiniciarVitoria, &QPushButton::clicked, this, [this](){
        m_panelVitoria->hide();
        reiniciarNivel();
    });
    connect(btnMenuVitoria, &QPushButton::clicked, this, [this](){
        m_panelVitoria->hide();
        emit voltarAoMenu();
    });

    //Painel derrota
    m_panelGameOver = new QFrame(this);
    m_panelGameOver->setFixedSize(300, 200);
    m_panelGameOver->setStyleSheet("QFrame { background-color: #ECF0F1; border: 4px solid #C0392B; border-radius: 15px; }");

    QVBoxLayout *gameOverLayout = new QVBoxLayout(m_panelGameOver);

    QLabel *lblGameOver = new QLabel("GAME OVER!", m_panelGameOver);
    lblGameOver->setAlignment(Qt::AlignCenter);
    lblGameOver->setStyleSheet("font-size: 22px; font-weight: bold; color: #C0392B; border: none;");

    QPushButton *btnReiniciarDerrota = new QPushButton("REINICIAR NÍVEL", m_panelGameOver);
    btnReiniciarDerrota->setFixedHeight(40);
    btnReiniciarDerrota->setStyleSheet("background-color: #34495E; color: white; font-weight: bold; border-radius: 10px;");

    QPushButton *btnMenuDerrota = new QPushButton("VOLTAR AO MENU", m_panelGameOver);
    btnMenuDerrota->setFixedHeight(40);
    btnMenuDerrota->setStyleSheet("background-color: #E74C3C; color: white; font-weight: bold; border-radius: 10px;");

    gameOverLayout->addWidget(lblGameOver);
    gameOverLayout->addWidget(btnReiniciarDerrota);
    gameOverLayout->addWidget(btnMenuDerrota);

    m_panelGameOver->hide();

    connect(btnReiniciarDerrota, &QPushButton::clicked, this, [this](){
        m_panelGameOver->hide();
        reiniciarNivel();
    });
    connect(btnMenuDerrota, &QPushButton::clicked, this, [this](){
        m_panelGameOver->hide();
        emit voltarAoMenu();
    });
}

void GameWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    if (m_view && m_scene) {
        m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }

    if (m_panelVitoria) {
        m_panelVitoria->move((width() - m_panelVitoria->width()) / 2,
                             (height() - m_panelVitoria->height()) / 2);
    }

    if (m_panelGameOver) {
        m_panelGameOver->move((width() - m_panelGameOver->width()) / 2,
                              (height() - m_panelGameOver->height()) / 2);
    }
}

void GameWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    if (m_view && m_scene) {
        m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void GameWidget::carregarNivel(int nivelId) {

}

void GameWidget::construirNivel(int nivelId) {
    m_timerNivel.start();
    m_tempoLimiteSegundos = 300;

    // O GameWidget já não faz ideia do que é um JSON. Ele apenas pede os dados à fábrica!
    LevelData dadosDoNivel = LevelLoader::carregar(nivelId);

    // Se o nível for inválido (não foi encontrado no ficheiro JSON)
    if (!dadosDoNivel.valido) {
        QMessageBox::information(this, "Fim do Jogo!", "Parabéns, concluíste todos os níveis disponíveis!");
        emit voltarAoMenu();
        return;
    }

    // Gerar a fila a partir da lista limpa
    m_passengerQueue->gerar(dadosDoNivel.coresDaFila);

    // Adicionar os autocarros à cena
    for (const BusData &b : dadosDoNivel.autocarros) {
        adicionarVeiculo(b.id, b.cor, b.capacidade, b.tamanho, b.direcao, b.coluna, b.linha);
    }
}

void GameWidget::adicionarVeiculo(int id, const QString &cor, int capacidade, int tamanho, Direction dir, int col, int linha) {

}

void GameWidget::onBusExitedParking(BusItem *bus) {

}

void GameWidget::processarFila() {

}

void GameWidget::mostrarPainelVitoria() {

}

void GameWidget::mostrarPainelGameOver() {
}

void GameWidget::reiniciarNivel() {

}

void GameWidget::realizarUndo() {

}

void GameWidget::mostrarHint() {

}

void GameWidget::iniciarAutoSolve() {

}

void GameWidget::passoAutoSolve() {

}

BusItem* GameWidget::avaliarMelhorJogada() {

}

void GameWidget::verificarTempoLimite() {

}