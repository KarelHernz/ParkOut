#include "busitem.h"
#include <QDebug>
#include <QPainter>

BusItem::BusItem(int id, const QString &colorName, int capacity, int cellSize, int lengthCells, Direction dir, QGraphicsItem *parent)
    : QGraphicsObject(parent), m_id(id), m_colorName(colorName), m_capacity(capacity),
    m_passengerCount(0), m_direction(dir), m_isMoving(false)
{
    m_speed = 5.0; // Velocidade da animação

    // 1. CALCULAR O TAMANHO DO VEÍCULO CONSOANTE A SUA ORIENTAÇÃO
    // Margem de 6 píxeis para o autocarro não colar visualmente às células
    m_shortSide = cellSize - 6;
    m_longSide = (cellSize * lengthCells) - 6;

    if (m_direction == Direction::Left || m_direction == Direction::Right) {
        m_width = m_longSide;
        m_height = m_shortSide;
    } else {
        m_width = m_shortSide;
        m_height = m_longSide;
    }

    // Já não carrega texturas. Apenas diz ao Qt para pintar o item
    update();
}

void BusItem::addPassenger() {
    if (m_passengerCount < m_capacity) {
        m_passengerCount++;
        update(); // Força o redesenho automático para mostrar o novo número
    }
}

QRectF BusItem::boundingRect() const {
    return QRectF(0, 0, m_width, m_height);
}

void BusItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option); Q_UNUSED(widget);

    // Ativa a suavização de contornos (Anti-aliasing)
    painter->setRenderHint(QPainter::Antialiasing);

    // 1. DEFINIR A COR DO AUTOCARRO (Converte a tua string m_colorName diretamente para QColor)
    QColor busColor(m_colorName);
    if (!busColor.isValid()) {
        busColor = Qt::darkGray; // Cor de segurança caso o nome da cor falhe
    }

    // 2. DESENHAR O CORPO PRINCIPAL DO AUTOCARRO (Retângulo arredondado)
    painter->setBrush(busColor);
    painter->setPen(QPen(Qt::black, 2)); // Contorno preto com 2px de espessura
    painter->drawRoundedRect(0, 0, m_width, m_height, 8, 8);

    // 3. DESENHAR UMA JANELA REBAIXADA (Dá um visual muito mais profissional)
    painter->setBrush(QColor(255, 255, 255, 90)); // Branco semi-transparente
    painter->setPen(Qt::NoPen);
    if (m_direction == Direction::Left || m_direction == Direction::Right) {
        painter->drawRect(6, 4, m_width - 12, m_height / 4);
    } else {
        painter->drawRect(4, 6, m_width / 4, m_height - 12);
    }

    // 4. DEFINIR O TEXTO INTERNO (Seta se estiver parado, Passageiros/Capacidade se mover)
    QString textToShow = "";

    if (m_inSlot) {
        // Se estiver no slot, mostra SEMPRE os passageiros!
        textToShow = QString("%1/%2").arg(m_passengerCount).arg(m_capacity);
    }
    else{
        switch (m_direction) {
            case Direction::Right: textToShow = "→"; break;
            case Direction::Left:  textToShow = "←"; break;
            case Direction::Up:    textToShow = "↑"; break;
            case Direction::Down:  textToShow = "↓"; break;
        }
    }

    // 5. CONFIGURAR A FONTE E DESENHAR O TEXTO CENTRADO
    QFont font = painter->font();
    int menorDimensao = qMin(m_width, m_height);
    font.setPixelSize(menorDimensao * 0.55); // O tamanho do texto adapta-se ao tamanho do autocarro
    font.setBold(true);
    painter->setFont(font);

    // Efeito de sombra preta atrás do texto (melhora drasticamente a leitura)
    painter->setPen(QColor(0, 0, 0, 160));
    painter->drawText(boundingRect().translated(1, 1), Qt::AlignCenter, textToShow);

    // Texto principal em branco
    painter->setPen(Qt::white);
    painter->drawText(boundingRect(), Qt::AlignCenter, textToShow);

    if (m_isHighlighted) {
        painter->setBrush(Qt::NoBrush);
        // Desenha uma borda dourada/amarela com 4px de espessura e tracejada
        painter->setPen(QPen(QColor(241, 196, 15), 4, Qt::DashLine));
        painter->drawRoundedRect(2, 2, m_width - 4, m_height - 4, 8, 8);
    }
}

void BusItem::advanceStep() {
    if (m_inSlot || !m_isMoving) {
        return;
    }

    qreal dx = 0;
    qreal dy = 0;
    QRectF lookAheadRect; // O "para-choques" virtual

    // 1. DEFINIR O SENTIDO DO MOVIMENTO E PROJETAR A CAIXA DE COLISÃO
    switch (m_direction) {
    case Direction::Right:
        dx = m_speed;
        lookAheadRect = QRectF(m_width, 0, 2, m_height);
        break;
    case Direction::Left:
        dx = -m_speed;
        lookAheadRect = QRectF(-2, 0, 2, m_height);
        break;
    case Direction::Down:
        dy = m_speed;
        lookAheadRect = QRectF(0, m_height, m_width, 2);
        break;
    case Direction::Up:
        dy = -m_speed;
        lookAheadRect = QRectF(0, -2, m_width, 2);
        break;
    }

    // 2. VERIFICAR SE O PARA-CHOQUES TOCOU NOUTRO AUTOCARRO
    if (scene()) {
        QPolygonF scenePolygon = mapToScene(lookAheadRect);
        QList<QGraphicsItem*> collidingItems = scene()->items(scenePolygon);

        for (QGraphicsItem* item : collidingItems) {
            if (item == this) continue; // Ignora-se a si próprio

            BusItem* otherBus = dynamic_cast<BusItem*>(item);
            if (otherBus && otherBus != this) {

                // Se o outro carro já estiver estacionado no slot, também o podemos ignorar para conseguir entrar
                if (!otherBus->isInSlot()) {
                    m_isMoving = false; // Para o carro porque bateu num obstáculo válido
                    // Se necessário, recua 1 píxel para não colar: moveBy(0, 2);
                    return;
                }
            }
        }
    }


    // 3. CAMINHO LIVRE: Executa a deslocação
    moveBy(dx, dy);

    // 4. VERIFICAR SE ULTRAPASSOU OS LIMITES DO PARKING
    // O teu parking ocupa a zona X(0 a 640) e Y(210 a 850).
    QRectF parkingRect(0, 210, 640, 640);

    if (!parkingRect.intersects(sceneBoundingRect())) {
        m_isMoving = false;

        // EM VEZ DE APAGAR, AVISA O JOGO!
        emit exitedParking(this);
        return;
    }
}

void BusItem::setMoving(bool moving) {
    m_isMoving = moving;
    update(); // Força a repintura para atualizar o desenho imediatamente (troca a seta pelo número)
}

bool BusItem::isMoving() const { return m_isMoving; }
int BusItem::id() const { return m_id; }
QString BusItem::colorName() const { return m_colorName; }
int BusItem::capacity() const { return m_capacity; }
int BusItem::passengerCount() const { return m_passengerCount; }
bool BusItem::isFull() const { return m_passengerCount >= m_capacity; }

void BusItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (m_inSlot) {
        return;
    }

    emit clicked(this);
    QGraphicsObject::mousePressEvent(event); // <--- Chama a lógica base por segurança
}

void BusItem::parkInSlot() {
    m_inSlot = true;
    m_isMoving = false;

    // AVISO OBRIGATÓRIO NO QT: Diz à placa gráfica que o tamanho deste item vai mudar!
    prepareGeometryChange();

    // Força o autocarro a ficar horizontal para o texto ficar direito
    m_direction = Direction::Right;

    // Altera o tamanho para encaixar perfeitamente no slot de 160x80 (com 3px de margem)
    m_width = 154;
    m_height = 74;

    update(); // Redesenha o autocarro com o novo tamanho e com o texto "0/X"
}

void BusItem::removePassenger() {
    if (m_passengerCount > 0) {
        m_passengerCount--;
        update(); // Força o Qt a redesenhar o número atualizado no ecrã
    }
}

void BusItem::resetarEstadoGrelha() {
    m_inSlot = false;
    m_isMoving = false;

    // 1. Repõe a direção original
    m_direction = direcaoOriginal;

    // 2. Repõe as dimensões corretas (Vertical vs Horizontal)
    // NOTA: Ajusta "50" para o tamanho exato da tua célula da grelha (ex: 40, 50, 60...)
    // Ajusta "m_tamanho", "m_width" e "m_height" para os nomes exatos das tuas variáveis
    int tamanhoCelula = 50;

    if (m_direction == Direction::Left || m_direction == Direction::Right) {
        m_width = m_longSide;
        m_height = m_shortSide;
    } else {
        m_width = m_shortSide;
        m_height = m_longSide;
    }

    // Se por acaso usaste rotação para o estacionar (ex: setRotation), deves repor a zeros:
    // setRotation(0);

    // 3. Força o ecrã a redesenhar a forma corrigida
    update();
}

void BusItem::setHighlighted(bool h) {
    m_isHighlighted = h;
    update(); // Força o Qt a redesenhar o autocarro com/sem a borda
}

gamewidget.cpp
Dentro de "void GameWidget::adicionarVeiculo(int id, const QString &cor, int capacidade, int tamanho, Direction dir, int col, int linha)"
    int topoGrelhaY = 210; // A altura onde a grelha de jogo começa
    int tamanhoCelula = 80;

    //Cria o autocarro
    BusItem *bus = new BusItem(id, cor, capacidade, tamanhoCelula, tamanho, dir);
    m_veiculosAtivos.append(bus);

    //Coloca-o na coluna e linha indicadas
    bus->setPos(col * tamanhoCelula, topoGrelhaY + (linha * tamanhoCelula));
    bus->posicaoOriginal = bus->pos();
    bus->direcaoOriginal = dir;
    bus->passageirosApanhadosNoSlot = 0;

    //Adiciona à cena
    m_scene->addItem(bus);

    //Liga todos os sinais automáticos
    connect(bus, &BusItem::clicked, this, [this](BusItem *b) {
        if (!m_parkingArea->isFull()) {
            b->setMoving(true);
        }
    });
    connect(bus, &BusItem::exitedParking, this, &GameWidget::onBusExitedParking);