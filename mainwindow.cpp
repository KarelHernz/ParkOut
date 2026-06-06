#include "mainwindow.h"
#include "menuwidget.h"
#include "selectnivelwidget.h"
#include "gamewidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    this->setWindowTitle("Park Out!");
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet("background-color: #88F1E7;");
    this->setMinimumSize(650, 720);
    mostrarMenuInicial();
}

MainWindow::~MainWindow() {}

void MainWindow::mostrarMenuInicial() {
    MenuWidget *menu = new MenuWidget(this);
    setCentralWidget(menu);
    connect(menu, &MenuWidget::jogarClicado, this, &MainWindow::mostrarSelecaoNivel);
}

//Função que muda para a seleção de niveis
void MainWindow::mostrarSelecaoNivel() {
    SelecaoNivelWidget *selecao = new SelecaoNivelWidget(this);
    setCentralWidget(selecao);

    connect(selecao, &SelecaoNivelWidget::nivelSelecionado, this, &MainWindow::iniciarNivel);
    connect(selecao, &SelecaoNivelWidget::voltarClicado, this, &MainWindow::mostrarMenuInicial);
}

void MainWindow::iniciarNivel(int nivelId) {
    GameWidget *jogo = new GameWidget(this);
    setCentralWidget(jogo);

    jogo->carregarNivel(nivelId);
    connect(jogo, &GameWidget::voltarAoMenu, this, &MainWindow::mostrarSelecaoNivel);
}