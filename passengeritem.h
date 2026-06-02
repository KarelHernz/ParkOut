#ifndef PASSENGERITEM_H
#define PASSENGERITEM_H

#include <QGraphicsObject>
#include <QString>
#include <QColor>
#include <QPainter>

class PassengerItem : public QGraphicsObject {
    Q_OBJECT
public:
    explicit PassengerItem(const QString &colorName, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QString colorName() const { return m_colorName; }

private:
    QString m_colorName;
    QColor m_color;
    int m_radius;
};

#endif