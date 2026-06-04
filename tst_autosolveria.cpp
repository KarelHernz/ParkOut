#include <QtTest>
#include <QSignalSpy>
#include "autosolveria.h"
#include "estadojogo.h"

class TestAutoSolverIA : public QObject {
    Q_OBJECT

private slots:
    void testAutocarroCorretoDisponivel() {
        // 1. PREPARAÇÃO (Arrange)
        AutoSolverIA solver;
        QSignalSpy spy(&solver, &AutoSolverIA::jogadaCalculada); // Espião no sinal

        EstadoJogo estado;
        estado.corDaFrenteFila = "Red";

        // Autocarro 1: Cor errada
        EstadoJogo::DadosBus bus1;
        bus1.id = 10;
        bus1.cor = "Blue";
        bus1.noParque = false;

        // Autocarro 2: Cor certa, mas já está no parque
        EstadoJogo::DadosBus bus2;
        bus2.id = 11;
        bus2.cor = "Red";
        bus2.noParque = true;

        // Autocarro 3: O CANDIDATO PERFEITO
        EstadoJogo::DadosBus bus3;
        bus3.id = 12;
        bus3.cor = "Red";
        bus3.noParque = false;

        estado.autocarros.append(bus1);
        estado.autocarros.append(bus2);
        estado.autocarros.append(bus3);

        // 2. AÇÃO (Act)
        solver.calcularMelhorJogada(estado);

        // 3. VALIDAÇÃO (Assert)
        // Verifica se o sinal foi emitido exatamente 1 vez
        QCOMPARE(spy.count(), 1);

        // Recupera o valor que a IA enviou no sinal (o ID do autocarro)
        QList<QVariant> argumentosDoSinal = spy.takeFirst();
        int idRecebido = argumentosDoSinal.at(0).toInt();

        // O teste passa se a IA escolheu o Autocarro 3 (ID 12)
        QCOMPARE(idRecebido, 12);
    }

    void testNenhumAutocarroDisponivel() {
        AutoSolverIA solver;
        QSignalSpy spy(&solver, &AutoSolverIA::jogadaCalculada);

        EstadoJogo estado;
        estado.corDaFrenteFila = "Green";

        // Só há um autocarro e é de cor errada
        EstadoJogo::DadosBus bus1;
        bus1.id = 1;
        bus1.cor = "Yellow";
        bus1.noParque = false;
        estado.autocarros.append(bus1);

        solver.calcularMelhorJogada(estado);

        QCOMPARE(spy.count(), 1);
        int idRecebido = spy.takeFirst().at(0).toInt();

        // Como não há opções, a IA tem de devolver -1
        QCOMPARE(idRecebido, -1);
    }
};

QTEST_MAIN(TestAutoSolverIA)
#include "tst_autosolveria.moc"