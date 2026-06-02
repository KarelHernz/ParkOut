#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QWidget>

class MenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit MenuWidget(QWidget *parent = nullptr);

signals:
    void jogarClicado(); // Sinal para avisar a MainWindow para mudar de ecrã
};

#endif // MENUWIDGET_H