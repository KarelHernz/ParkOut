#ifndef PARKINGAREA_H
#define PARKINGAREA_H

#include <QGraphicsScene>
#include <QVector>
#include "busitem.h"

class ParkingArea : public QObject {
    Q_OBJECT
public:
    explicit ParkingArea(QGraphicsScene *scene, QObject *parent = nullptr);

    bool tentarEstacionar(BusItem *bus);
    void clear();

    // Novos superpoderes para o GameWidget comunicar!
    bool isFull() const;
    int vagasLivres() const;
    BusItem* getBus(int index) const;
    void removerBus(BusItem* bus);

private:
    QGraphicsScene *m_scene;
    QVector<BusItem*> m_slots;
};

#endif