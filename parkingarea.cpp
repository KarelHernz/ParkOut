#include "parkingarea.h"
#include "gameconfig.h"

ParkingArea::ParkingArea(QGraphicsScene *scene, QObject *parent)
    : QObject(parent), m_scene(scene) {
    m_slots.resize(GameConfig::NUM_SLOTS, nullptr); // Usa a constante (3)
}

bool ParkingArea::tentarEstacionar(BusItem *bus) {
    for (int i = 0; i < m_slots.size(); ++i) {
        if (m_slots[i] == nullptr) {
            m_slots[i] = bus;

            // Substituído por constantes!
            int slotX = GameConfig::ESPACAMENTO_SLOTS + i * (GameConfig::LARGURA_SLOT + GameConfig::ESPACAMENTO_SLOTS);

            bus->setPos(slotX + GameConfig::SLOT_OFFSET_CENTRO, GameConfig::SLOT_Y + GameConfig::SLOT_OFFSET_CENTRO);
            bus->parkInSlot();
            return true;
        }
    }
    return false;
}

void ParkingArea::clear() {
    m_slots.fill(nullptr);
}

bool ParkingArea::isFull() const {
    return !m_slots.contains(nullptr);
}

int ParkingArea::vagasLivres() const {
    return m_slots.count(nullptr);
}

BusItem* ParkingArea::getBus(int index) const {
    if (index >= 0 && index < m_slots.size()) return m_slots[index];
    return nullptr;
}

void ParkingArea::removerBus(BusItem* bus) {
    int index = m_slots.indexOf(bus);
    if (index != -1) {
        m_slots[index] = nullptr;
    }
}