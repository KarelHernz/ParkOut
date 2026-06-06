#include <QtTest>
#include <QSignalSpy>
#include <QGraphicsScene>

#include "autosolveria.h"
#include "estadojogo.h"
#include "gamepersistence.h"
#include "busitem.h"
#include "parkingarea.h"
#include "passengerqueue.h"

class Testes : public QObject {
    Q_OBJECT

private slots:
    //Teste de AutoSolverIA
    void testAutocarroCorretoDisponivel() {
        AutoSolverIA solver;
        QSignalSpy spy(&solver, &AutoSolverIA::jogadaCalculada);

        EstadoJogo estado;
        estado.corDaFrenteFila = "Red";

        EstadoJogo::DadosBus bus1 = {10, "Blue", 0, 0, 0, false};
        EstadoJogo::DadosBus bus2 = {11, "Red", 0, 0, 0, true};
        EstadoJogo::DadosBus bus3 = {12, "Red", 0, 0, 0, false}; // O ALVO

        estado.autocarros.append(bus1);
        estado.autocarros.append(bus2);
        estado.autocarros.append(bus3);

        solver.calcularMelhorJogada(estado);

        QCOMPARE(spy.count(), 1);
        int idRecebido = spy.takeFirst().at(0).toInt();
        QCOMPARE(idRecebido, 12);
    }

    void testNenhumAutocarroDisponivel() {
        AutoSolverIA solver;
        QSignalSpy spy(&solver, &AutoSolverIA::jogadaCalculada);

        EstadoJogo estado;
        estado.corDaFrenteFila = "Green";

        EstadoJogo::DadosBus bus1 = {1, "Yellow", 0, 0, 0, false};
        estado.autocarros.append(bus1);

        solver.calcularMelhorJogada(estado);

        QCOMPARE(spy.count(), 1);
        int idRecebido = spy.takeFirst().at(0).toInt();
        QCOMPARE(idRecebido, -1);
    }

    //Teste de GamePersistence
    void testGuardarERecuperarRecorde() {
        int nivelTeste = 999;
        int tempoTeste = 42;

        GamePersistence::guardarRecorde(nivelTeste, tempoTeste);
        int tempoRecuperado = GamePersistence::lerMelhorTempo(nivelTeste);

        QCOMPARE(tempoRecuperado, tempoTeste);
    }

    void testNovoNivelRetornaTempoMaximo() {
        int nivelNuncaJogado = 888;
        int tempo = GamePersistence::lerMelhorTempo(nivelNuncaJogado);
        QCOMPARE(tempo, 9999);
    }

    void testEmbarqueDePassageiro() {
        // Criamos uma cena invisível na memória RAM
        QGraphicsScene sceneInvisivel;
        ParkingArea parking(&sceneInvisivel, nullptr);
        PassengerQueue queue(&sceneInvisivel, nullptr);

        // Adiciona um autocarro Vermelho ao parque
        BusItem* bus = new BusItem(1, "Red", 3, 80, 2, Direction::Right);
        parking.tentarEstacionar(bus);

        // Gera fila com Vermelho e Azul
        QStringList coresFila = {"Red", "Blue"};
        queue.gerar(coresFila);

        // Simula a mecânica do jogo (processar fila)
        QString corDaFrente = queue.getPrimeiraCor();
        BusItem* busNoParque = parking.getBus(0);

        bool embarcou = false;
        if (busNoParque && busNoParque->colorName() == corDaFrente && !busNoParque->isFull()) {
            queue.removerPrimeiroEAvancar();
            busNoParque->addPassenger();
            embarcou = true;
        }

        // Verifica se a física funcionou como esperado
        QVERIFY(embarcou == true);
        QCOMPARE(busNoParque->passengerCount(), 1);
        QCOMPARE(queue.tamanho(), 1);
        QCOMPARE(queue.getPrimeiraCor(), QString("Blue"));
    }
};

QTEST_MAIN(Testes)
#include "testes.moc"