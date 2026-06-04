#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// Forward declarations: dizemos ao compilador que estas classes existem
class MenuWidget;
class SelecaoNivelWidget;
class GameWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void mostrarMenuInicial();      // Mostra o ecrã inicial ("JOGAR" e "SAIR")
    void mostrarSelecaoNivel();     // Mostra a grelha com os níveis (1 a 6)
    void iniciarNivel(int nivelId); // Abre o tabuleiro do jogo e carrega o nível escolhido
};

#endif