#include "selectnivelwidget.h"
#include "gamepersistence.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

SelecaoNivelWidget::SelecaoNivelWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    // 2. Título
    QLabel *titulo = new QLabel("SELECIONAR NÍVEL", this);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-size: 32px; font-weight: bold; background: transparent; margin-bottom: 30px;");
    mainLayout->addWidget(titulo);

    // 3. Grelha de Níveis (QGridLayout permite organizar como uma tabela)
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(20);

    int maxNivel = GamePersistence::getNivelDesbloqueado();
    // Vamos criar 6 níveis de exemplo
    for (int i = 1; i <= 6; ++i) {
        QPushButton *btnNivel = new QPushButton(this);
        btnNivel->setFixedSize(80, 80); // Botões quadrados

        if (i <= maxNivel) {
            // NÍVEL DESBLOQUEADO
            btnNivel->setText(QString::number(i));
            btnNivel->setStyleSheet("background-color: white; color: #2C3E50; border-radius: 15px; font-size: 28px; font-weight: bold;");

            // Só ligamos a função de clique se o nível estiver desbloqueado
            connect(btnNivel, &QPushButton::clicked, this, [this, i]() {
                emit nivelSelecionado(i);
            });
        } else {
            // NÍVEL BLOQUEADO
            btnNivel->setText("🔒\n" + QString::number(i)); // Adiciona um cadeado visualmente
            btnNivel->setStyleSheet("background-color: #7F8C8D; color: #BDC3C7; border-radius: 15px; font-size: 16px; font-weight: bold;");
            btnNivel->setEnabled(false); // Impede que o jogador clique à força
        }

        // Organizar numa grelha de 3 colunas
        int linha = (i - 1) / 3;
        int coluna = (i - 1) % 3;
        gridLayout->addWidget(btnNivel, linha, coluna);
    }

    mainLayout->addLayout(gridLayout);

    // 4. Botão de Voltar ao Menu
    QPushButton *btnVoltar = new QPushButton("VOLTAR", this);
    btnVoltar->setFixedSize(280, 50);
    // Cor diferente para o botão "Voltar" (ex: um tom avermelhado ou cinza)
    btnVoltar->setStyleSheet("color: white; border-radius: 15px; font-size: 18px; font-weight: bold; margin-top: 30px;");
    mainLayout->addWidget(btnVoltar, 0, Qt::AlignCenter);

    // Liga o botão ao respetivo sinal
    connect(btnVoltar, &QPushButton::clicked, this, &SelecaoNivelWidget::voltarClicado);
}