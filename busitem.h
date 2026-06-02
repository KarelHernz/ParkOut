#pragma once

#include "busitem.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsObject>
#include <QPixmap>
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>

// Enumeração para as direções possíveis de movimento
enum class Direction { Up, Down, Left, Right };

class BusItem : public QGraphicsObject {
    Q_OBJECT // Macro obrigatória para permitir que o autocarro emita Signals

public:
    // Construtor configurado para receber o nome da cor (ex: "red", "blue") e a capacidade (4, 6, 8, 12)
    explicit BusItem(int id, const QString &colorName, int capacity, int cellSize, int lengthCells, Direction dir, QGraphicsItem *parent = nullptr);

    // Métodos obrigatórios do Qt Graphics View Framework
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    // Métodos de controlo de movimento e animação
    void advanceStep();
    void setMoving(bool moving);
    bool isMoving() const;

    // Métodos de gestão do estado do autocarro e dos teus assets
    void addPassenger();

    QPointF posicaoOriginal;
    Direction direcaoOriginal;
    int passageirosApanhadosNoSlot = 0;

    void removePassenger();
    void resetarEstadoGrelha();

    int id() const;
    QString colorName() const;
    int capacity() const;
    int passengerCount() const;
    bool isFull() const;                 // Retorna true se atingiu a capacidade máxima
    void parkInSlot();
    bool isInSlot() const { return m_inSlot; }
    Direction direction() const { return m_direction; }
    void setHighlighted(bool h);

signals:
    // Sinal emitido quando o jogador clica ou toca no autocarro
    void clicked(BusItem *item);
    void exitedParking(BusItem *bus);

protected:
    // Captura o clique do rato (Desktop) ou toque (Mobile)
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    int m_id;
    QString m_colorName;    // Ex: "red", "blue", "green", "orange", "purple"
    int m_capacity;         // 4, 6, 8, ou 12
    int m_passengerCount;   // Começa em 0 até atingir m_capacity
    Direction m_direction;  // Direção de movimento do autocarro
    bool m_isMoving;        // Estado de movimento atual
    int m_shortSide;
    int m_longSide;
    qreal m_speed;          // Velocidade em píxeis por frame
    qreal m_width;          // Largura física em píxeis na cena
    qreal m_height;         // Altura física em píxeis na cena
    bool m_inSlot = false;
    bool m_isHighlighted = false;
};