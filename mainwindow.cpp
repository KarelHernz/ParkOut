#include "mainwindow.h"
#include "menuwidget.h"
#include "selectnivelwidget.h"
#include "gamewidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Configuração inicial da janela
    this->setWindowTitle("Park Out!");
    this->setMinimumSize(800, 600);

    // Inicia a aplicação mostrando o Menu Inicial
    mostrarMenuInicial();
}

MainWindow::~MainWindow() {
    // O Qt trata da limpeza de memória dos widgets automaticamente
}

void MainWindow::mostrarMenuInicial() {
    // Cria o Menu
    MenuWidget *menu = new MenuWidget(this);
    setCentralWidget(menu);

    // Se o jogador clicar em "JOGAR", muda para o ecrã de níveis
    connect(menu, &MenuWidget::jogarClicado, this, &MainWindow::mostrarSelecaoNivel);
}

void MainWindow::mostrarSelecaoNivel() {
    // Cria o ecrã de Seleção de Níveis
    SelecaoNivelWidget *selecao = new SelecaoNivelWidget(this);
    setCentralWidget(selecao);

    // Se o jogador escolher um nível, inicia esse nível no jogo
    connect(selecao, &SelecaoNivelWidget::nivelSelecionado, this, &MainWindow::iniciarNivel);

    // Se o jogador clicar em "VOLTAR", regressa ao Menu Inicial
    connect(selecao, &SelecaoNivelWidget::voltarClicado, this, &MainWindow::mostrarMenuInicial);
}

void MainWindow::iniciarNivel(int nivelId) {
    // Cria o ecrã do Jogo (o tabuleiro)
    GameWidget *jogo = new GameWidget(this);
    setCentralWidget(jogo);

    // Carrega o nível que o jogador selecionou
    jogo->carregarNivel(nivelId);

    // Conecta um possível botão de "Sair do Nível" para voltar à seleção de níveis
    // (Assume que o GameWidget tem o sinal voltarAoMenu())
    connect(jogo, &GameWidget::voltarAoMenu, this, &MainWindow::mostrarSelecaoNivel);
}