#include "menuwidget.h"
#include "gamepersistence.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QPixmap>

MenuWidget::MenuWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    QLabel *lbl_titulo = new QLabel(this);
    QPixmap imagemLogo(":/src/img/ParkOut_Logo.png");
    lbl_titulo->setPixmap(imagemLogo.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    lbl_titulo->setAlignment(Qt::AlignCenter);
    lbl_titulo->setStyleSheet("background: transparent;");
    layout->addWidget(lbl_titulo);

    QPushButton *bttJogar = new QPushButton("JOGAR", this);
    makeStyle(bttJogar, "#D9D9D9");
    bttJogar->setFixedSize(380, 50);
    layout->addWidget(bttJogar);

    QPushButton *bttApagar = new QPushButton("APAGAR PROGRESSO", this);
    bttApagar->setFixedSize(380, 50);
    makeStyle(bttApagar, "#D9D9D9");
    layout->addWidget(bttApagar);

    QPushButton *bttSair = new QPushButton("SAIR", this);
    bttSair->setFixedSize(380, 50);
    makeStyle(bttSair, "#D9D9D9");
    layout->addWidget(bttSair);

    connect(bttJogar, &QPushButton::clicked, this, &MenuWidget::jogarClicado);
    connect(bttApagar, &QPushButton::clicked, this, &MenuWidget::painelApagarProgresso);
    connect(bttSair, &QPushButton::clicked, qApp, &QApplication::quit);
}

void MenuWidget::makeStyle(QPushButton *btt, QString backgroudColor){
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

void MenuWidget::painelApagarProgresso() {
    //Criar um painel personalizado
    QDialog *painelConfirmacao = new QDialog(this);

    //Remove as bordas padrão da janela
    painelConfirmacao->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    //Estilizar o fundo do painel
    painelConfirmacao->setStyleSheet("QDialog { "
                                     "background-color: #2C3E50;" // Fundo azul escuro
                                     "border: 3px solid #E74C3C;" // Borda vermelha de aviso
                                     "border-radius: 15px;}");
    painelConfirmacao->setFixedSize(450, 220);

    //Cria os elementos do painel
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(painelConfirmacao);
    layoutPrincipal->setAlignment(Qt::AlignCenter);
    layoutPrincipal->setSpacing(20);

    QLabel *lblTitulo = new QLabel("APAGAR PROGRESSO", painelConfirmacao);
    lblTitulo->setStyleSheet("color: #E74C3C; font-size: 22px; font-weight: bold; border: none; background: transparent");
    lblTitulo->setAlignment(Qt::AlignCenter);

    QLabel *lblTexto = new QLabel("Tens a certeza que queres apagar tudo?\nEsta ação não pode ser desfeita.", painelConfirmacao);
    lblTexto->setStyleSheet("color: #ECF0F1; font-size: 16px; border: none; background: transparent");
    lblTexto->setAlignment(Qt::AlignCenter);

    // Layout horizontal para os dois botões
    QHBoxLayout *layoutBotoes = new QHBoxLayout();
    layoutBotoes->setSpacing(20);

    //Botão de apagar
    QPushButton *btnSim = new QPushButton("APAGAR", painelConfirmacao);
    btnSim->setFixedSize(150, 45);
    btnSim->setStyleSheet(
        "QPushButton { background-color: #E74C3C; color: white; font-size: 14px; font-weight: bold; border-radius: 10px; }"
        "QPushButton:hover { background-color: #C0392B; }");

    //Butão de cancelar
    QPushButton *btnNao = new QPushButton("CANCELAR", painelConfirmacao);
    btnNao->setFixedSize(150, 45);
    btnNao->setStyleSheet(
        "QPushButton { background-color: #95A5A6; color: white; font-size: 14px; font-weight: bold; border-radius: 10px; }"
        "QPushButton:hover { background-color: #7F8C8D; }");

    layoutBotoes->addStretch();
    layoutBotoes->addWidget(btnNao);
    layoutBotoes->addWidget(btnSim);
    layoutBotoes->addStretch();

    layoutPrincipal->addWidget(lblTitulo);
    layoutPrincipal->addWidget(lblTexto);
    layoutPrincipal->addLayout(layoutBotoes);

    connect(btnSim, &QPushButton::clicked, painelConfirmacao, &QDialog::accept);
    connect(btnNao, &QPushButton::clicked, painelConfirmacao, &QDialog::reject);

    QPoint centroMenu = this->mapToGlobal(this->rect().center());

    //Move o painel para o centro
    painelConfirmacao->move(centroMenu.x() - (painelConfirmacao->width() / 2),
                            centroMenu.y() - (painelConfirmacao->height() / 2));

    //Mostra o painel e espera uma resposta
    if (painelConfirmacao->exec() != QDialog::Accepted) {
        painelConfirmacao->deleteLater();
        return;
    }
    painelConfirmacao->deleteLater();

    //Apaga os ficheiros
    bool apagouAlgo = GamePersistence::apagarProgresso();

    QDialog *painelResultado = new QDialog(this);
    painelResultado->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    // Verde se apagou, laranja se não havia nada
    QString corTema = apagouAlgo ? "#27AE60" : "#F39C12";
    // Cor mais clara para o Hover
    QString corHover = apagouAlgo ? "#2ECC71" : "#F1C40F";
    QString tituloResultado = apagouAlgo ? "SUCESSO" : "AVISO";
    QString textoResultado = apagouAlgo ? "O teu progresso foi reiniciado com sucesso!" : "Ainda não tinhas progresso guardado.";

    painelResultado->setStyleSheet(QString("QDialog { background-color: #2C3E50; border: 3px solid %1; border-radius: 15px; }").arg(corTema));
    painelResultado->setFixedSize(400, 200);

    QVBoxLayout *layoutRes = new QVBoxLayout(painelResultado);
    layoutRes->setAlignment(Qt::AlignCenter);
    layoutRes->setSpacing(20);

    QLabel *lblTituloRes = new QLabel(tituloResultado, painelResultado);
    lblTituloRes->setStyleSheet(QString("color: %1; font-size: 22px; font-weight: bold; border: none; background: transparent;").arg(corTema));
    lblTituloRes->setAlignment(Qt::AlignCenter);

    QLabel *lblTextoRes = new QLabel(textoResultado, painelResultado);
    lblTextoRes->setStyleSheet("color: #ECF0F1; font-size: 16px; border: none; background: transparent;");
    lblTextoRes->setAlignment(Qt::AlignCenter);

    QPushButton *btnOk = new QPushButton("OK", painelResultado);
    btnOk->setFixedSize(150, 45);
    btnOk->setStyleSheet(QString("QPushButton { background-color: %1; color: white; font-size: 14px; font-weight: bold; border-radius: 10px; }"
                             "QPushButton:hover { background-color: %2; }").arg(corTema, corHover));

    layoutRes->addWidget(lblTituloRes);
    layoutRes->addWidget(lblTextoRes);

    // Adicionar o botão centrado usando layout
    QHBoxLayout *linhaBtn = new QHBoxLayout();
    linhaBtn->addStretch();
    linhaBtn->addWidget(btnOk);
    linhaBtn->addStretch();
    layoutRes->addLayout(linhaBtn);

    connect(btnOk, &QPushButton::clicked, painelResultado, &QDialog::accept);

    QPoint centroMenuResultado = this->mapToGlobal(this->rect().center());
    painelResultado->move(centroMenuResultado.x() - (painelResultado->width() / 2),
                          centroMenuResultado.y() - (painelResultado->height() / 2));

    painelResultado->exec();
    painelResultado->deleteLater();
}