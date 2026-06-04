#ifndef SELECTNIVELWIDGET_H
#define SELECTNIVELWIDGET_H

#include <QWidget>

class SelecaoNivelWidget : public QWidget {
    Q_OBJECT
public:
    explicit SelecaoNivelWidget(QWidget *parent = nullptr);

signals:
    // Sinal enviado quando escolhes um nível
    void nivelSelecionado(int nivelId); 

    // Sinal enviado quando clicas em "Voltar"
    void voltarClicado();               
};

#endif