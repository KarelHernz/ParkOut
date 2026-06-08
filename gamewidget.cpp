#include "gamewidget.h"
#include "busitem.h"
#include "passengeritem.h"
#include "gamepersistence.h"
#include "levelloader.h"
#include "autosolveria.h"
#include "gameconfig.h"

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QPen>
#include <QBrush>
#include <QGridLayout>
#include <QWidget>
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
#include <QThread>

//Construtor
GameWidget::GameWidget(QWidget *parent) : QWidget(parent), m_nivelAtual(1) {
    m_tempoDecorrido = 0;
    m_melhorTempo = 9999;
    m_usouAutoSolve = false;
    qRegisterMetaType<EstadoJogo>("EstadoJogo");

    m_mainLayout = new QGridLayout(this);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

    configurarBarraSuperior();
    configurarCena();
    configurarPaineisFinais();

    m_passengerQueue = new PassengerQueue(m_scene, this);
    m_parkingArea = new ParkingArea(m_scene, this);

    m_workerThread = new QThread(this);
    m_autoSolverIA = new AutoSolverIA();

    m_autoSolverIA->moveToThread(m_workerThread);
    connect(this, &GameWidget::solicitarCalculoIA, m_autoSolverIA, &AutoSolverIA::calcularMelhorJogada);
    connect(m_autoSolverIA, &AutoSolverIA::jogadaCalculada, this, &GameWidget::onJogadaIACalculada);

    connect(m_workerThread, &QThread::finished, m_autoSolverIA, &QObject::deleteLater);
    m_workerThread->start();

    //Timer para fazer que os veiculos avancem
    QTimer *gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, [this]() {
        for (BusItem *bus : m_veiculosAtivos) {
            if (bus) {
                bus->advanceStep();
            }
        }
    });
    gameTimer->start(GameConfig::FRAME_RATE_MS);
}

//Destrutor
GameWidget::~GameWidget() {
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
    }
}

void GameWidget::configurarBarraSuperior() {
    //Wwidget para agrupar os controlos e um layout em grelha interna
    m_panelControlos = new QWidget(this);
    m_controlosLayout = new QGridLayout(m_panelControlos);
    m_controlosLayout->setContentsMargins(0, 0, 0, 0);

    //Criação dos botões
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
    btnAutoSolve->setFixedSize(100, 40);
    btnAutoSolve->setStyleSheet("background-color: #E67E22; color: white; font-weight: bold; border-radius: 10px;");

    m_lblCronometro = new QLabel("⏱00:00", this);
    m_lblCronometro->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF; padding-bottom: 2px;");

    m_lblRecorde = new QLabel("🏆--:--", this);
    m_lblRecorde->setStyleSheet("font-size: 18px; font-weight: bold; color: #DC9B00; padding-top: 2px;");

    m_mainLayout->addWidget(m_panelControlos, 0, 0);

    // Ligações dos botões
    connect(btnVoltar, &QPushButton::clicked, this, [this]() { emit voltarAoMenu(); });
    connect(btnUndo, &QPushButton::clicked, this, &GameWidget::realizarUndo);
    connect(btnReset, &QPushButton::clicked, this, &GameWidget::reiniciarNivel);
    connect(btnHint, &QPushButton::clicked, this, &GameWidget::mostrarHint);
    connect(btnAutoSolve, &QPushButton::clicked, this, &GameWidget::iniciarAutoSolve);

    m_timerJogo = new QTimer(this);
    connect(m_timerJogo, &QTimer::timeout, this, &GameWidget::atualizarTimerJogo);
    m_timerAutoSolve = new QTimer(this);
    connect(m_timerAutoSolve, &QTimer::timeout, this, &GameWidget::passoAutoSolve);
}

void GameWidget::configurarCena() {
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, GameConfig::LARGURA_GRELHA, GameConfig::ALTURA_CENA);

    m_view = new QGraphicsView(m_scene, this);
    m_view->setAlignment(Qt::AlignCenter);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform);
    m_view->setStyleSheet("background-color: #ECF0F1; border: 3px solid #2C3E50; border-radius: 15px;");
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_mainLayout->addWidget(m_view, 1, 0);
}

void GameWidget::configurarPaineisFinais() {
    //Painel vitoria
    m_panelVitoria = new QFrame(this);
    m_panelVitoria->setFixedSize(300, 250);
    m_panelVitoria->setStyleSheet("QFrame { background-color: rgba(30, 34, 40, 240); border-radius: 15px; border: 3px solid #27AE60; }"
                                  "QLabel { color: white; font-size: 16px; border: none; background: transparent; }");

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
    m_panelGameOver->setStyleSheet("QFrame { background-color: rgba(30, 34, 40, 240); border-radius: 15px; border: 3px solid #C0392B; }"
                                   "QLabel { color: white; font-size: 16px; border: none; background: transparent; }");

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

    //Mantém os slots visíveis
    if (m_view && m_scene) {
        int larguraTotalSlots = (GameConfig::NUM_SLOTS * GameConfig::LARGURA_SLOT) +
                                ((GameConfig::NUM_SLOTS - 1) * GameConfig::ESPACAMENTO_SLOTS);
        double startX_Slots = (GameConfig::LARGURA_GRELHA - larguraTotalSlots) / 2.0;
        double minX = qMin(0.0, startX_Slots);
        double maxX = qMax((double)GameConfig::LARGURA_GRELHA, startX_Slots + larguraTotalSlots);
        QRectF areaIdeal(minX, m_scene->sceneRect().top(), maxX - minX, m_scene->sceneRect().height());
        areaIdeal.adjust(-20, -20, 20, 20);
        m_view->fitInView(areaIdeal, Qt::KeepAspectRatio);
    }

    //Reorganização dinâmica do layout (muda de acordo com a rotação)
    bool isLandscape = (width() > height());

    if (m_mainLayout && m_panelControlos && m_controlosLayout) {
        // Remove tudo temporariamente para evitar conflito de posições
        m_mainLayout->removeWidget(m_panelControlos);
        m_mainLayout->removeWidget(m_view);

        m_controlosLayout->removeWidget(btnVoltar);
        m_controlosLayout->removeWidget(btnReset);
        m_controlosLayout->removeWidget(btnUndo);
        m_controlosLayout->removeWidget(btnHint);
        m_controlosLayout->removeWidget(btnAutoSolve);
        m_controlosLayout->removeWidget(m_lblCronometro);
        m_controlosLayout->removeWidget(m_lblRecorde);

        // Limpa as "molas" (stretches) antigas para garantir uma transição limpa
        for (int i = 0; i <= 7; ++i) m_controlosLayout->setRowStretch(i, 0);
        for (int i = 0; i <= 4; ++i) m_controlosLayout->setColumnStretch(i, 0);

        if (isLandscape) {
            //Modo horizontal
            m_mainLayout->addWidget(m_panelControlos, 0, 0);
            m_mainLayout->addWidget(m_view, 0, 1);

            m_mainLayout->setColumnStretch(0, 0);
            m_mainLayout->setColumnStretch(1, 1);
            m_mainLayout->setRowStretch(0, 1);
            m_mainLayout->setRowStretch(1, 0);

            m_lblCronometro->setAlignment(Qt::AlignCenter);
            m_lblRecorde->setAlignment(Qt::AlignCenter);

            //Contadores no topo
            m_controlosLayout->addWidget(m_lblCronometro, 0, 0);
            m_controlosLayout->addWidget(m_lblRecorde, 1, 0);

            //Mola invisível no meio para empurrar os botões para o fundo
            m_controlosLayout->setRowStretch(2, 1);

            //Botões ordenados de baixo para cima
            m_controlosLayout->addWidget(btnAutoSolve, 3, 0);
            m_controlosLayout->addWidget(btnHint, 4, 0);
            m_controlosLayout->addWidget(btnUndo, 5, 0);
            m_controlosLayout->addWidget(btnReset, 6, 0);
            m_controlosLayout->addWidget(btnVoltar, 7, 0);

        } else {
            //Modo vertical
            m_mainLayout->addWidget(m_panelControlos, 0, 0);
            m_mainLayout->addWidget(m_view, 1, 0);

            m_mainLayout->setRowStretch(0, 0);
            m_mainLayout->setRowStretch(1, 1);
            m_mainLayout->setColumnStretch(0, 1);
            m_mainLayout->setColumnStretch(1, 0);

            m_lblCronometro->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            m_lblRecorde->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

            // Linha 1 do Topo
            m_controlosLayout->addWidget(btnVoltar, 0, 0);
            m_controlosLayout->addWidget(btnReset, 0, 1);
            m_controlosLayout->addWidget(btnUndo, 0, 2);
            m_controlosLayout->addWidget(m_lblCronometro, 0, 4);

            // Linha 2 do Topo
            m_controlosLayout->addWidget(btnHint, 1, 0);
            m_controlosLayout->addWidget(btnAutoSolve, 1, 1);
            m_controlosLayout->addWidget(m_lblRecorde, 1, 4);

            m_controlosLayout->setColumnStretch(3, 1);
        }
    }

    //Paines de vitoria e game over
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
        int larguraTotalSlots = (GameConfig::NUM_SLOTS * GameConfig::LARGURA_SLOT) + ((GameConfig::NUM_SLOTS - 1) * GameConfig::ESPACAMENTO_SLOTS);
        double startX_Slots = (GameConfig::LARGURA_GRELHA - larguraTotalSlots) / 2.0;
        double minX = qMin(0.0, startX_Slots);
        double maxX = qMax((double)GameConfig::LARGURA_GRELHA, startX_Slots + larguraTotalSlots);
        QRectF areaIdeal(minX, m_scene->sceneRect().top(), maxX - minX, m_scene->sceneRect().height());
        areaIdeal.adjust(-20, -20, 20, 20);
        m_view->fitInView(areaIdeal, Qt::KeepAspectRatio);
    }
}

void GameWidget::carregarNivel(int nivelId) {
    m_usouAutoSolve = false;
    if (m_timerAutoSolve->isActive()) {
        m_timerAutoSolve->stop();
    }

    m_historicoUndo.clear();
    m_passengerQueue->clear();
    m_veiculosAtivos.clear();
    m_parkingArea->clear();
    m_scene->clear();

    if (m_panelVitoria) {
        m_panelVitoria->hide();
    }
    if (m_panelGameOver) {
        m_panelGameOver->hide();
    }

    // Atualiza a variável de controlo do nível atual
    m_nivelAtual = nivelId;

    m_tempoDecorrido = 0;
    m_melhorTempo = GamePersistence::lerMelhorTempo(m_nivelAtual);
    m_timerJogo->start(1000);

    int larguraTotalSlots = (GameConfig::NUM_SLOTS * GameConfig::LARGURA_SLOT) + ((GameConfig::NUM_SLOTS - 1) * GameConfig::ESPACAMENTO_SLOTS);
    int startX_Slots = (GameConfig::LARGURA_GRELHA - larguraTotalSlots) / 2;

    //Zona de espera
    QGraphicsTextItem *txtFila = m_scene->addText(QString("FILA DE ESPERA - NÍVEL %1").arg(nivelId));
    txtFila->setFont(QFont("Arial", 18, QFont::Bold));
    int textX = (GameConfig::LARGURA_GRELHA - txtFila->boundingRect().width()) / 2;
    txtFila->setPos(textX, GameConfig::PASSAGEIROS_Y - 20);
    txtFila->setDefaultTextColor(QColor("#D35400"));

    //Os slots de estacionamento
    QPen penSlot(QColor("#7F8C8D"), 2, Qt::DashLine);
    for (int i = 0; i < GameConfig::NUM_SLOTS; ++i) {
        int slotX = startX_Slots + i * (GameConfig::LARGURA_SLOT + GameConfig::ESPACAMENTO_SLOTS);
        m_scene->addRect(slotX, GameConfig::SLOT_Y, GameConfig::LARGURA_SLOT, GameConfig::ALTURA_SLOT, penSlot, QBrush(QColor("#DFE4EA")));
    }

    //Desenha as linhas da grelha
    QPen penGrelha(Qt::lightGray, 1, Qt::DashLine);
    for (int y = GameConfig::TOPO_GRELHA_Y; y <= GameConfig::TOPO_GRELHA_Y + (GameConfig::NUM_LINHAS * GameConfig::TAMANHO_CELULA); y += GameConfig::TAMANHO_CELULA) {
        m_scene->addLine(0, y, GameConfig::LARGURA_GRELHA, y, penGrelha);
    }
    for (int x = 0; x <= GameConfig::LARGURA_GRELHA; x += GameConfig::TAMANHO_CELULA) {
        m_scene->addLine(x, GameConfig::TOPO_GRELHA_Y, x, GameConfig::TOPO_GRELHA_Y + (GameConfig::NUM_LINHAS * GameConfig::TAMANHO_CELULA), penGrelha);
    }

    construirNivel(nivelId);
}

void GameWidget::construirNivel(int nivelId) {
    m_timerNivel.start();
    m_tempoLimiteSegundos = 300;

    // O GameWidget já não faz ideia do que é um JSON. Ele apenas pede os dados à fábrica!
    LevelData dadosDoNivel = LevelLoader::carregar(nivelId);

    if (!dadosDoNivel.valido) {
        mostrarPainelAviso("Fim do Jogo!",
                           "Parabéns, concluíste todos os níveis disponíveis!",
                           true);
    }

    //Gera a fila a partir da lista limpa
    m_passengerQueue->gerar(dadosDoNivel.coresDaFila);

    //Adiciona os autocarros à cena
    for (const BusData &b : dadosDoNivel.autocarros) {
        adicionarVeiculo(b.id, b.cor, b.capacidade, b.tamanho, b.direcao, b.coluna, b.linha);
    }

    //Ativa o resizeEvent uma única vez para que o layout se adapte
    QTimer::singleShot(10, this, [this]() {
        QResizeEvent eventoFalso(this->size(), this->size());
        this->resizeEvent(&eventoFalso);
    });
}

void GameWidget::adicionarVeiculo(int id, const QString &cor, int capacidade, int tamanho, Direction dir, int col, int linha) {
    int topoGrelhaY = 210; // A altura onde a grelha de jogo começa
    int tamanhoCelula = 80;

    //Cria o autocarro
    BusItem *bus = new BusItem(id, cor, capacidade, tamanhoCelula, tamanho, dir);
    m_veiculosAtivos.append(bus);

    //Coloca-o na coluna e linha indicadas
    bus->setPos(col * tamanhoCelula, topoGrelhaY + (linha * tamanhoCelula));
    bus->posicaoOriginal = bus->pos();
    bus->direcaoOriginal = dir;
    bus->passageirosApanhadosNoSlot = 0;

    //Adiciona à cena
    m_scene->addItem(bus);

    //Liga todos os sinais automáticos
    connect(bus, &BusItem::clicked, this, [this](BusItem *b) {
        if (!m_parkingArea->isFull()) {
            b->setMoving(true);
        }
    });
    connect(bus, &BusItem::exitedParking, this, &GameWidget::onBusExitedParking);
}

void GameWidget::onBusExitedParking(BusItem *bus) {
    if (m_parkingArea->tentarEstacionar(bus)) {
        m_historicoUndo.append(bus);
        processarFila();
    } else {
        m_panelGameOver->show();
    }
}

void GameWidget::processarFila() {
    if (m_passengerQueue->isEmpty()) {
        mostrarPainelVitoria();
        return;
    }

    QString corDaFrente = m_passengerQueue->getPrimeiraCor();

    for (int i = 0; i < GameConfig::NUM_SLOTS; ++i) {
        BusItem* bus = m_parkingArea->getBus(i);

        if (bus != nullptr && bus->colorName() == corDaFrente && !bus->isFull()) {

            //O gestor remove o passageiro e avança a fila automaticamente
            m_passengerQueue->removerPrimeiroEAvancar();

            bus->addPassenger();
            bus->passageirosApanhadosNoSlot++;

            if (bus->isFull()) {
                m_veiculosAtivos.removeOne(bus);
                m_scene->removeItem(bus);
                bus->deleteLater();
                m_parkingArea->removerBus(bus);
                QTimer::singleShot(GameConfig::DELAY_FILA_BUS_CHEIO_MS, this, &GameWidget::processarFila);
            } else {
                QTimer::singleShot(GameConfig::DELAY_FILA_PASSAGEIRO_MS, this, &GameWidget::processarFila);
            }
            return;
        }
    }

    if (m_parkingArea->isFull()) {
        mostrarPainelGameOver();
    }
}

void GameWidget::mostrarPainelVitoria() {
    m_timerJogo->stop();

    if (!m_usouAutoSolve) {
        if (m_tempoDecorrido < m_melhorTempo || m_melhorTempo == 9999) {
            //Salvaguarda o recorde usando o gestor estático
            GamePersistence::guardarRecorde(m_nivelAtual, m_tempoDecorrido);
        }

        m_panelVitoria->move((width() - m_panelVitoria->width()) / 2,
                             (height() - m_panelVitoria->height()) / 2);
        m_panelVitoria->show();
        m_panelVitoria->raise();

        //Guarda o progresso usando o gestor estático
        GamePersistence::guardarProgresso(m_nivelAtual + 1);
    } else {
        mostrarPainelAviso("Piloto Automático",
                           "O nível foi resolvido automaticamente!\n"
                           "Atenção: Como utilizou a ajuda do Auto-Solve, o progresso não vai ficar guardado e o próximo nível continua bloqueado.\n"
                           "Tente resolver o nível sem ajuda para o desbloquear!",
                           false);
    }
}

void GameWidget::mostrarPainelGameOver() {
    m_panelGameOver->move((width() - m_panelGameOver->width()) / 2,
                          (height() - m_panelGameOver->height()) / 2);
    m_panelGameOver->show();
    m_panelGameOver->raise();
}

void GameWidget::reiniciarNivel() {
    carregarNivel(m_nivelAtual);
}

void GameWidget::realizarUndo() {
    //Se não há movimentos para desfazer, ignorar
    if (m_historicoUndo.isEmpty()) return;

    //Recupera o último veículo de forma segura
    QPointer<BusItem> bus = m_historicoUndo.takeLast();

    //Se o veículo for nulo é cancelado o Undo
    if (!bus) {
        return;
    }

    //Remove o autocarro do slot de estacionamento superior
    m_parkingArea->removerBus(bus.data());

    //Cuspir os passageiros de volta para a fila
    for (int i = 0; i < bus->passageirosApanhadosNoSlot; ++i) {
        m_passengerQueue->recriarNoInicio(bus->colorName()); // <-- Delega ao gestor
        bus->removePassenger();
    }
    bus->passageirosApanhadosNoSlot = 0;

    //Reorganiza visualmente toda a fila de passageiros para trás
    m_passengerQueue->atualizarPosicoesVisuais();

    //Teletransporta o autocarro de volta à sua posição original
    bus->setPos(bus->posicaoOriginal);
    bus->resetarEstadoGrelha();
}

void GameWidget::mostrarHint() {
    if (m_passengerQueue->isEmpty()) return;

    // Avisamos o sistema que o próximo resultado da IA é apenas para dar uma dica
    m_esperandoHint = true;

    // Criar os dados para a Thread como no Auto-Solve
    EstadoJogo estado;
    estado.corDaFrenteFila = m_passengerQueue->getPrimeiraCor();

    for (BusItem *bus : m_veiculosAtivos) {
        if (!bus) continue;
        EstadoJogo::DadosBus d;
        d.id = bus->id();
        d.cor = bus->colorName();
        d.noParque = bus->isInSlot();
        estado.autocarros.append(d);
    }

    // Envia para o cérebro secundário!
    emit solicitarCalculoIA(estado);
}

void GameWidget::iniciarAutoSolve() {
    m_usouAutoSolve = true;
    m_timerAutoSolve->start(GameConfig::INTERVALO_AUTO_SOLVE_MS);
}

void GameWidget::onJogadaIACalculada(int busId) {
    if (busId == -1) {
        m_esperandoHint = false;
        return;
    }

    //Procura o autocarro correspondente ao ID
    BusItem *busAlvo = nullptr;
    for (BusItem *bus : m_veiculosAtivos) {
        if (bus && bus->id() == busId) {
            busAlvo = bus;
            break;
        }
    }

    if (!busAlvo) return;

    //Se pedimos um Hint, apenas acendemos a luz
    if (m_esperandoHint) {
        busAlvo->setHighlighted(true);
        QTimer::singleShot(GameConfig::DURACAO_HINT_MS, this, [busAlvo]() {
            if (busAlvo) {
                busAlvo->setHighlighted(false);
            }
        });
        m_esperandoHint = false; // Tarefa concluída, desligamos o modo hint
    }
    //Se não, é o Auto-Solve em ação
    else {
        if (m_parkingArea->vagasLivres() > 0 && !busAlvo->isMoving()) {
            busAlvo->setMoving(true);
        }
    }
}

void GameWidget::passoAutoSolve() {
    if (m_passengerQueue->isEmpty()) {
        m_timerAutoSolve->stop();
        return;
    }

    //Criar os dados para à Thread
    EstadoJogo estado;
    estado.corDaFrenteFila = m_passengerQueue->getPrimeiraCor();

    for (BusItem *bus : m_veiculosAtivos) {
        if (!bus) continue;
        EstadoJogo::DadosBus d;
        d.id = bus->id();
        d.cor = bus->colorName();
        d.noParque = bus->isInSlot();
        estado.autocarros.append(d);
    }

    emit solicitarCalculoIA(estado);
}

void GameWidget::verificarTempoLimite() {
    if (m_timerNivel.elapsed() / 1000 >= m_tempoLimiteSegundos) {

        m_timerVisual->stop();
        mostrarPainelGameOver();
    }
}

void GameWidget::atualizarTimerJogo() {
    m_tempoDecorrido++;

    int minAtual = m_tempoDecorrido / 60;
    int segAtual = m_tempoDecorrido % 60;
    QString tempoAtual = QString("%1:%2")
                                .arg(minAtual, 2, 10, QChar('0'))
                                .arg(segAtual, 2, 10, QChar('0'));

    QString melhorTempo;
    if (m_melhorTempo == 9999 || m_melhorTempo <= 0) {
        melhorTempo = "--:--";
    } else {
        int minMelhor = m_melhorTempo / 60;
        int segMelhor = m_melhorTempo % 60;
        melhorTempo = QString("%1:%2")
                             .arg(minMelhor, 2, 10, QChar('0'))
                             .arg(segMelhor, 2, 10, QChar('0'));
    }

    m_lblCronometro->setText(QString("⏱%1").arg(tempoAtual));
    m_lblRecorde->setText(QString("🏆%1").arg(melhorTempo));
}

void GameWidget::mostrarPainelAviso(const QString &titulo, const QString &texto, bool voltarMenuAoFechar){
    QFrame *panelAviso = new QFrame(this);
    panelAviso->setFixedSize(480, 260);

    // Estilo do painel (fundo escuro, texto branco, bordas amarelo/laranja)
    panelAviso->setStyleSheet(
        "QFrame { background-color: rgba(30, 34, 40, 240); border-radius: 15px; border: 3px solid #e5c07b; }"
        "QLabel { color: white; font-size: 16px; border: none; background: transparent; }"
        );

    QVBoxLayout *layoutAviso = new QVBoxLayout(panelAviso);
    layoutAviso->setContentsMargins(20, 20, 20, 20);
    layoutAviso->setSpacing(15);

    //Título
    QLabel *lblTitulo = new QLabel(titulo, panelAviso);
    lblTitulo->setStyleSheet("color: #e5c07b; font-size: 20px; font-weight: bold;");
    lblTitulo->setAlignment(Qt::AlignCenter);

    //Texto
    QLabel *lblTexto = new QLabel(texto, panelAviso);
    lblTexto->setAlignment(Qt::AlignCenter);
    lblTexto->setWordWrap(true);

    //Botão
    QPushButton *btnOk = new QPushButton("OK", panelAviso);
    btnOk->setCursor(Qt::PointingHandCursor);
    btnOk->setStyleSheet("QPushButton { ""background-color: #e5c07b; "
                                         "color: #1e2228; "
                                         "border-radius: 8px; "
                                         "padding: 10px 40px; "
                                         "font-size: 16px; font-weight: "
                                         "bold; "
                                         "border: none; }"
                         "QPushButton:hover { background-color: #d19a66; }");

    connect(btnOk, &QPushButton::clicked, this, [this, panelAviso, voltarMenuAoFechar]() {
        panelAviso->deleteLater();
        if (voltarMenuAoFechar) {
            emit voltarAoMenu();
        } else {
            reiniciarNivel();
        }
    });

    layoutAviso->addWidget(lblTitulo);
    layoutAviso->addWidget(lblTexto);
    layoutAviso->addWidget(btnOk, 0, Qt::AlignCenter);

    // Centrar e mostrar
    panelAviso->move((width() - panelAviso->width()) / 2, (height() - panelAviso->height()) / 2);
    panelAviso->show();
    panelAviso->raise();
}