#ifndef PASSENGERQUEUE_H
#define PASSENGERQUEUE_H

#include <QObject>
#include <QGraphicsScene>
#include <QList>
#include <QStringList>
#include "passengeritem.h"

class PassengerQueue : public QObject {
    Q_OBJECT
public:
    explicit PassengerQueue(QGraphicsScene *scene, QObject *parent = nullptr);

    void gerar(const QStringList &cores);
    void limpar();

    // Métodos de consulta (Getters)
    bool isEmpty() const;
    int tamanho() const;
    PassengerItem* getPassenger(int index) const;
    QString getPrimeiraCor() const;

    // Métodos de modificação da fila
    void removerPrimeiroEAvancar();
    void recriarNoInicio(const QString &cor);
    void atualizarPosicoesVisuais();

private:
    QGraphicsScene *m_scene;
    QList<PassengerItem*> m_queue;
};

#endif