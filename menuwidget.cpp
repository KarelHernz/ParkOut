#include "menuwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>

MenuWidget::MenuWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    // 2. Título
    QLabel *titulo = new QLabel("PARK OUT!", this);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("font-size: 32px; font-weight: bold; color: #2C3E50;");
    layout->addWidget(titulo);

    // 3. Botões
    QPushButton *bttJogar = new QPushButton("JOGAR", this);
    bttJogar->setFixedSize(380, 50);
    layout->addWidget(bttJogar);

    QPushButton *bttSair = new QPushButton("SAIR", this);
    bttSair->setFixedSize(380, 50);
    layout->addWidget(bttSair);

    // 4. Conexões
    connect(bttJogar, &QPushButton::clicked, this, &MenuWidget::jogarClicado);
    connect(bttSair, &QPushButton::clicked, qApp, &QApplication::quit);
}