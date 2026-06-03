#include "passengerqueue.h"
#include "gameconfig.h"

PassengerQueue::PassengerQueue(QGraphicsScene *scene, QObject *parent)
    : QObject(parent), m_scene(scene) {}

void PassengerQueue::gerar(const QStringList &cores) {
    limpar();

    for (int i = 0; i < cores.size(); ++i) {
        PassengerItem *passageiro = new PassengerItem(cores[i]);
        m_scene->addItem(passageiro);
        m_queue.append(passageiro);
    }
    atualizarPosicoesVisuais();
}

void PassengerQueue::limpar() {
    // Remove os itens da cena e apaga-os da memória de forma segura
    for (PassengerItem *p : m_queue) {
        if (p) {
            m_scene->removeItem(p);
            delete p;
        }
    }
    m_queue.clear();
}

bool PassengerQueue::isEmpty() const {
    return m_queue.isEmpty();
}

int PassengerQueue::tamanho() const {
    return m_queue.size();
}

PassengerItem* PassengerQueue::getPassenger(int index) const {
    if (index >= 0 && index < m_queue.size()) {
        return m_queue[index];
    }
    return nullptr;
}

QString PassengerQueue::getPrimeiraCor() const {
    if (!m_queue.isEmpty() && m_queue.first()) {
        return m_queue.first()->colorName();
    }
    return QString();
}

void PassengerQueue::removerPrimeiroEAvancar() {
    if (m_queue.isEmpty()) return;

    PassengerItem *primeiro = m_queue.takeFirst();
    m_scene->removeItem(primeiro);
    delete primeiro;

    // Faz os restantes passageiros andarem suavemente para a frente
    for (PassengerItem *p : m_queue) {
        p->moveBy(-GameConfig::FILA_ESPACAMENTO, 0);
    }
}

void PassengerQueue::recriarNoInicio(const QString &cor) {
    PassengerItem *p = new PassengerItem(cor);
    m_scene->addItem(p);

    // O prepend empurra para o início exato da lista
    m_queue.prepend(p);
}

void PassengerQueue::atualizarPosicoesVisuais() {
    for (int i = 0; i < m_queue.size(); ++i) {
        m_queue[i]->setPos(GameConfig::FILA_START_X + (i * GameConfig::FILA_ESPACAMENTO),
                           GameConfig::PASSAGEIROS_Y + 10);
    }
}

gamewidget.cpp
Dentro de "void GameWidget::processarFila()"
    if (m_passengerQueue->isEmpty()) {
        mostrarPainelVitoria();
        return;
    }

    QString corDaFrente = m_passengerQueue->getPrimeiraCor();

    for (int i = 0; i < GameConfig::NUM_SLOTS; ++i) {
        BusItem* bus = m_parkingArea->getBus(i);

        if (bus != nullptr && bus->colorName() == corDaFrente && !bus->isFull()) {

            // O gestor remove o passageiro e avança a fila automaticamente!
            m_passengerQueue->removerPrimeiroEAvancar();

            bus->addPassenger();
            bus->passageirosApanhadosNoSlot++;

            if (bus->isFull()) {
                m_veiculosAtivos.removeOne(bus);
                m_scene->removeItem(bus);
                bus->deleteLater();
                m_parkingArea->removerBus(bus);
                QTimer::singleShot(GameConfig::DELAY_FILA_BUS_CHEIO_MS, this, &GameWidget::processarFila);
            } else {
                QTimer::singleShot(GameConfig::DELAY_FILA_PASSAGEIRO_MS, this, &GameWidget::processarFila);
            }
            return;
        }
    }

    if (m_parkingArea->isFull()) {
        mostrarPainelGameOver();
    }