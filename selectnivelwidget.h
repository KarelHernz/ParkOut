#ifndef SELECTNIVELWIDGET_H
#define SELECTNIVELWIDGET_H

#include <QWidget>
#include <QPushButton>

class SelecaoNivelWidget : public QWidget {
    Q_OBJECT
public:
    explicit SelecaoNivelWidget(QWidget *parent = nullptr);

private:
    void makeStyle(QPushButton *btt);

signals:
    void nivelSelecionado(int nivelId); 
    void voltarClicado();               
};

#endif