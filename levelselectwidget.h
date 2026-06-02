// levelselectwidget.h
#pragma once
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>

class LevelSelectWidget : public QWidget {
    Q_OBJECT
public:
    LevelSelectWidget(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        QLabel *titulo = new QLabel("SELECIONE O NÍVEL", this);
        titulo->setAlignment(Qt::AlignCenter);
        titulo->setStyleSheet("font-size: 22px; font-weight: bold;");
        mainLayout->addWidget(titulo);

        // Grelha para colocar os botões dos níveis
        QGridLayout *gridLayout = new QGridLayout();
        mainLayout->addLayout(gridLayout);

        // Criar 6 botões de nível (pode ser expandido conforme leias do teu JSON)
        for (int i = 1; i <= 6; ++i) {
            QPushButton *btnNivel = new QPushButton(QString("Nível %1").arg(i), this);
            btnNivel->setFixedSize(100, 100);

            // Quando clicado, emite o sinal com o número do nível correspondente
            connect(btnNivel, &QPushButton::clicked, this, [this, i]() {
                emit nivelSelecionado(i);
            });

            // Organiza em colunas (ex: 3 colunas)
            gridLayout->addWidget(btnNivel, (i-1)/3, (i-1)%3);
        }

        QPushButton *btnVoltar = new QPushButton("VOLTAR AO MENU", this);
        connect(btnVoltar, &QPushButton::clicked, this, &LevelSelectWidget::voltarClicado);
        mainLayout->addWidget(btnVoltar, 0, Qt::AlignCenter);
    }
signals:
    void voltarClicado();
    void nivelSelecionado(int nivelId);
};