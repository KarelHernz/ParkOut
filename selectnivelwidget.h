#ifndef SELECTNIVELWIDGET_H
#define SELECTNIVELWIDGET_H

#include <QWidget>

class SelecaoNivelWidget : public QWidget {
    Q_OBJECT
public:
    explicit SelecaoNivelWidget(QWidget *parent = nullptr);

signals:
    void nivelSelecionado(int nivelId); // Sinal enviado quando escolhes um nível
    void voltarClicado();               // Sinal enviado quando clicas em "Voltar"
};

#endif // SELECAONIVELWIDGET_H