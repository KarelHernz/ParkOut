#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QStandardPaths>
#include <QFile>
#include <QDir>

class MenuWidget : public QWidget {
    Q_OBJECT
public:
    explicit MenuWidget(QWidget *parent = nullptr);

private:
    void makeStyle(QPushButton *btt, QString backgroundColor);

private slots:
    void painelApagarProgresso();

signals:
    void jogarClicado();
};

#endif