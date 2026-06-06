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
#include <QThread>

//Construtor
GameWidget::GameWidget(QWidget *parent) : QWidget(parent), m_nivelAtual(1) {
    m_tempoDecorrido = 0;
    m_melhorTempo = 9999;
    m_usouAutoSolve = false;
    qRegisterMetaType<EstadoJogo>("EstadoJogo");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    configurarBarraSuperior(mainLayout);
    configurarCena(mainLayout);
    configurarPaineisFinais();

    m_passengerQueue = new PassengerQueue(m_scene, this);
    m_parkingArea = new ParkingArea(m_scene, this);

    m_workerThread = new QThread(this);
    m_autoSolverIA = new AutoSolverIA();

    //Movemos o trabalhador para dentro da nova Thread
    m_autoSolverIA->moveToThread(m_workerThread);
    connect(this, &GameWidget::solicitarCalculoIA, m_autoSolverIA, & AutoSolverIA::calcularMelhorJogada);
    connect(m_autoSolverIA, & AutoSolverIA::jogadaCalculada, this, &GameWidget::onJogadaIACalculada);

    //Limpeza de memória quando a thread terminar
    connect(m_workerThread, &QThread::finished, m_autoSolverIA, &QObject::deleteLater);
    m_workerThread->start();

    //Motor de animação
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

void GameWidget::configurarBarraSuperior(QVBoxLayout *mainLayout) {
    //Criar um container vertical que vai segurar os dois layouts
    QVBoxLayout *topBarContainer = new QVBoxLayout();

    //Cria dois layouts horizontais
    QHBoxLayout *linha1 = new QHBoxLayout();
    QHBoxLayout *linha2 = new QHBoxLayout();

    //Criação dos botões
    btnVoltar = new QPushButton("SAIR", this);
    btnVoltar->setFixedSize(90, 40);
    btnVoltar->setStyleSheet("background-color: #E74C3C; color: white; font-weight: bold; border-radius: 10px;");

    btnReset = new QPushButton("RESET", this);
    btnReset->setFixedSize(90, 40);
    btnReset->setStyleSheet("background-color: #34495E; color: white; font-weight: bold; border-radius: 10px;");

    btnUndo = new QPushButton("UNDO", this);
    btnUndo->setFixedSize(90, 40);
    btnUndo->setStyleSheet("background-color: #9B59B6; color: white; font-weight: bold; border-radius: 10px;");

    btnHint = new QPushButton("HINT", this);
    btnHint->setFixedSize(90, 40);
    btnHint->setStyleSheet("background-color: #F1C40F; color: #2C3E50; font-weight: bold; border-radius: 10px;");

    btnAutoSolve = new QPushButton("AUTO-SOLVE", this);
    btnAutoSolve->setFixedSize(110, 40);
    btnAutoSolve->setStyleSheet("background-color: #E67E22; color: white; font-weight: bold; border-radius: 10px;");

    m_lblCronometro = new QLabel("⏱00:00", this);
    m_lblCronometro->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF; padding-bottom: 2px;");
    m_lblCronometro->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_lblRecorde = new QLabel("🏆--:--", this);
    m_lblRecorde->setStyleSheet("font-size: 18px; font-weight: bold; color: #F1C40F; padding-top: 2px;");
    m_lblRecorde->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    //Preenche a linha 1
    linha1->addWidget(btnVoltar);
    linha1->addWidget(btnReset);
    linha1->addWidget(btnUndo);
    linha1->addStretch();
    linha1->addWidget(m_lblCronometro);

    //Preenche a linha 2
    linha2->addWidget(btnHint);
    linha2->addWidget(btnAutoSolve);
    linha2->addStretch();
    linha2->addWidget(m_lblRecorde);

    //Adiciona as duas linhas ao contentor do topo
    topBarContainer->addLayout(linha1);
    topBarContainer->addLayout(linha2);

    //Adiciona o topo à janela principal
    mainLayout->addLayout(topBarContainer);

    //Liga os botões
    connect(btnVoltar, &QPushButton::clicked, this, [this]() { emit voltarAoMenu(); });
    connect(btnUndo, &QPushButton::clicked, this, &GameWidget::realizarUndo);
    connect(btnReset, &QPushButton::clicked, this, &GameWidget::reiniciarNivel);
    connect(btnHint, &QPushButton::clicked, this, &GameWidget::mostrarHint);
    connect(btnAutoSolve, &QPushButton::clicked, this, &GameWidget::iniciarAutoSolve);

    //Configura os Timers
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

    //Zona de espera
    QGraphicsTextItem *txtFila = m_scene->addText(QString("FILA DE ESPERA - NÍVEL %1").arg(nivelId));
    txtFila->setDefaultTextColor(QColor("#D35400"));
    txtFila->setFont(QFont("Arial", 10, QFont::Bold));
    txtFila->setPos(40, GameConfig::PASSAGEIROS_Y - 20);

    //Os slots de estacionamento
    QPen penSlot(QColor("#7F8C8D"), 2, Qt::DashLine);
    for (int i = 0; i < GameConfig::NUM_SLOTS; ++i) {
        int slotX = GameConfig::ESPACAMENTO_SLOTS + i * (GameConfig::LARGURA_SLOT + GameConfig::ESPACAMENTO_SLOTS);
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
        QMessageBox::warning(this, "Piloto Automático",
                             "O nível foi resolvido automaticamente!\n\n"
                             "Atenção: Como usou a ajuda do Auto-Solve, o progresso NÃO foi guardado "
                             "e o próximo nível continua trancado.\n\n"
                             "Tente resolver o nível sem ajuda para o desbloquear!");
        emit reiniciarNivel();
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

    //Se o veículo for nulo cancelamos o Undo
    if (!bus) {
        return;
    }

    //Remover o autocarro do slot de estacionamento superior
    m_parkingArea->removerBus(bus.data());

    //Cuspir os passageiros de volta para a fila
    for (int i = 0; i < bus->passageirosApanhadosNoSlot; ++i) {
        m_passengerQueue->recriarNoInicio(bus->colorName()); // <-- Delega ao gestor
        bus->removePassenger();
    }
    bus->passageirosApanhadosNoSlot = 0;

    //Reorganizar visualmente toda a fila de passageiros para trás
    m_passengerQueue->atualizarPosicoesVisuais();

    //Teletransportar o autocarro de volta à sua garagem original
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
        m_esperandoHint = false; // Reset da variável de segurança
        return;
    }

    // Procura o autocarro correspondente ao ID
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
