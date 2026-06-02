#include "passengeritem.h"

PassengerItem::PassengerItem(const QString &colorName, QGraphicsItem *parent)
    : QGraphicsObject(parent), m_colorName(colorName), m_radius(30)
{
    m_color = QColor(colorName);
}

QRectF PassengerItem::boundingRect() const {
    return QRectF(0, 0, m_radius, m_radius);
}

void PassengerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Desenha o círculo do passageiro com antialiasing para ficar suave
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_color);

    painter->drawEllipse(0, 0, m_radius, m_radius);
}