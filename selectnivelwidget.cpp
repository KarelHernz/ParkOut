#include "selectnivelwidget.h"
#include "gamepersistence.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

SelecaoNivelWidget::SelecaoNivelWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    QLabel *titulo = new QLabel("SELECIONAR NÍVEL", this);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-size: 32px; font-weight: bold; background: transparent; margin-bottom: 30px; color: #FFFFFF;");
    mainLayout->addWidget(titulo);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(20);

    int maxNivel = GamePersistence::getNivelDesbloqueado();
    for (int i = 1; i <= 6; ++i) {
        QPushButton *bttNivel = new QPushButton(this);
        bttNivel->setFixedSize(70, 70);

        //O nível está desbloqueado
        if (i <= maxNivel) {
            bttNivel->setText(QString::number(i));
            makeStyle(bttNivel);

            // Só ligamos a função de clique se o nível estiver desbloqueado
            connect(bttNivel, &QPushButton::clicked, this, [this, i]() {
                emit nivelSelecionado(i);
            });
        } else {
            // O nível está bloqueado
            bttNivel->setText("🔒\n" + QString::number(i)); // Adiciona um cadeado visualmente
            bttNivel->setStyleSheet("background-color: #7F8C8D; color: #BDC3C7; border-radius: 15px; font-size: 16px; font-weight: bold;");
            bttNivel->setEnabled(false); // Impede que o jogador clique à força
        }

        //Organiza numa grelha de 3 colunas
        int linha = (i - 1) / 3;
        int coluna = (i - 1) % 3;
        gridLayout->addWidget(bttNivel, linha, coluna);
    }

    mainLayout->addLayout(gridLayout);

    QPushButton *bttVoltar = new QPushButton("VOLTAR", this);
    bttVoltar->setFixedSize(380, 50);
    makeStyle(bttVoltar);
    mainLayout->addWidget(bttVoltar, 0, Qt::AlignCenter);

    connect(bttVoltar, &QPushButton::clicked, this, &SelecaoNivelWidget::voltarClicado);
}

void SelecaoNivelWidget::makeStyle(QPushButton *btt){
    btt->setStyleSheet("QPushButton {"
                       "background-color: #F0F0F0;"
                       "color: #2C3E50;"
                       "border: 1px solid #CCCCCC;"
                       "border-radius: 10px;"
                       "font-weight: bold;"
                       "font-size: 18px;"
                       "padding: 10px 20px;""}"
                       "QPushButton:hover {"
                       "background-color: #D9D9D9;"
                       "border: 1px solid #B3B3B3;""}"
                       "QPushButton:pressed {""background-color: #BFBFBF;");
}