#include "logic.h"
#include <QList>
#include <QByteArray>
#include <QFile>
#include <QUrl>
#include <QTextStream>

#include <QDebug>

struct Figure {
    int type;
    int team;
    int x;
    int y;
    int movesNum;
};

struct Logic::Impl {
    const QList<QString> boardCoordX = { "a", "b", "c", "d", "e", "f", "g", "h" };
    const QList<QString> boardCoordY = { "8", "7", "6", "5", "4", "3", "2", "1" };

    QList<Figure> figures;
    QList<QString> movements;
    QHash<int, Figure> captured;

    int currMove = 0;

    void initPieces();
    int findByPosition(int x, int y) const;
    bool isMoveValid(int index, int toX, int toY) const;
    bool canAttack(int attacker, int victim) const;
    bool move(int index, int toX, int toY);
    bool capture(int &attacker, int victim);
    int checkStatus(int team) const;
};

void Logic::Impl::initPieces() {
    for (int team = 0; team < 2; team++) {
        for (int x = 0; x < 8; x++) {
            figures << Figure { Pawn, team, x, team * (BOARD_SIZE - 3) + 1, 0 };
        }
        figures << Figure { King, team, 4, team * (BOARD_SIZE - 1), 0 };
        figures << Figure { Queen, team, 3, team * (BOARD_SIZE - 1), 0 };
        figures << Figure { Bishop, team, 2, team * (BOARD_SIZE - 1), 0 };
        figures << Figure { Bishop, team, 5, team * (BOARD_SIZE - 1), 0 };
        figures << Figure { Knight, team, 1, team * (BOARD_SIZE - 1), 0 };
        figures << Figure { Knight, team, 6, team * (BOARD_SIZE - 1), 0 };
        figures << Figure { Rook, team, 0, team * (BOARD_SIZE - 1), 0 };
        figures << Figure { Rook, team, 7, team * (BOARD_SIZE - 1), 0 };
    }
}

int Logic::Impl::findByPosition(int x, int y) const {
    for (int i(0); i < figures.size(); ++i) {
        if (figures[i].x != x || figures[i].y != y) {
            continue;
        }
        return i;
    }
    return -1;
}

bool Logic::Impl::isMoveValid(int index, int toX, int toY) const {
    int victim = findByPosition(toX, toY);

    if (victim >= 0 && figures[index].team == figures[victim].team) {
        if (figures[index].type == King && figures[victim].type == Rook) {
            if (figures[index].movesNum == 0 && figures[victim].movesNum == 0) {
                for (int i = figures[index].x; i != toX; (toX - figures[index].x) > 0 ? ++i : --i) {
                    if (findByPosition(i, figures[index].y) >= 0 && i != figures[index].x) {
                        return false;
                    }
                }
                return true;
            }
        }
        return false;
    }

    switch (figures[index].type) {
    case Pawn: {
        if ((toY - figures[index].y > 0) == (figures[index].team == White) || toY - figures[index].y == 0) {
            return false;
        }
        if (abs(toY - figures[index].y) > 2 || abs(toX - figures[index].x) > 1) {
            return false;
        }
        if (abs(toY - figures[index].y) == 2) {
            if (figures[index].movesNum > 0 || abs(toX - figures[index].x) > 0) {
                return false;
            }
            if (victim != -1 || findByPosition((figures[index].x + toX) / 2, (figures[index].y + toY) / 2) != -1) {
                return false;
            }
        } else {
            if (abs(toX - figures[index].x) == 1 && victim == -1) {
                return false;
            }
            if (abs(toX - figures[index].x) == 0 && victim >= 0) {
                return false;
            }
        }
        return true;
    }
    case Knight: {
        if (abs(toY - figures[index].y) == 2 && abs(toX - figures[index].x) == 1) {
            return true;
        }
        if (abs(toX - figures[index].x) == 2 && abs(toY - figures[index].y) == 1) {
            return true;
        }
        return false;
    }
    case Bishop: {
        if(abs(toX - figures[index].x) != abs(toY - figures[index].y)) {
            return false;
        }
        for (int i = 1; i < abs(toX - figures[index].x); ++i) {
            if (findByPosition(figures[index].x + (figures[index].x < toX ? i : -i), figures[index].y + (figures[index].y < toY ? i : -i)) >= 0) {
                return false;
            }
        }
        return true;
    }
    case Rook: {
        if ((abs(toX - figures[index].x) > 0) == (abs(toY - figures[index].y) > 0)) {
            return false;
        }
        for (int i = figures[index].x; i != toX; (toX - figures[index].x) > 0 ? ++i : --i) {
            if (findByPosition(i, figures[index].y) >= 0 && i != figures[index].x) {
                return false;
            }
        }
        for (int i = figures[index].y; i != toY; (toY - figures[index].y) > 0 ? ++i : --i) {
            if (findByPosition(figures[index].x, i) >= 0 && i != figures[index].y) {
                return false;
            }
        }
        return true;
    }
    case Queen: {
        if(abs(toX - figures[index].x) == abs(toY - figures[index].y)) {
            for (int i = 1; i < abs(toX - figures[index].x); ++i) {
                if (findByPosition(figures[index].x + (figures[index].x < toX ? i : -i), figures[index].y + (figures[index].y < toY ? i : -i)) >= 0) {
                    return false;
                }
            }
        } else if ((abs(toX - figures[index].x) > 0) != (abs(toY - figures[index].y) > 0)) {
            for (int i = figures[index].x; i != toX; (toX - figures[index].x) > 0 ? ++i : --i) {
                if (findByPosition(i, figures[index].y) >= 0 && i != figures[index].x) {
                    return false;
                }
            }
            for (int i = figures[index].y; i != toY; (toY - figures[index].y) > 0 ? ++i : --i) {
                if (findByPosition(figures[index].x, i) >= 0 && i != figures[index].y) {
                    return false;
                }
            }
        } else return false;

        return true;
    }
    case King: {
        if (abs(toY - figures[index].y) > 1 || abs(toX - figures[index].x) > 1) {
            return false;
        }
        return true;
    }
    }
    return true;
}

bool Logic::Impl::canAttack(int attacker, int victim) const
{
    if (attacker < 0 || attacker >= figures.size()) {
        return false;
    }
    if (victim < 0 || attacker >= figures.size()) {
        return false;
    }
    if (victim == attacker) {
        return false;
    }
    return isMoveValid(attacker, figures[victim].x, figures[victim].y);
}

bool Logic::Impl::move(int index, int toX, int toY)
{
    if (toX < 0 || toX >= BOARD_SIZE || toY < 0 || toY >= BOARD_SIZE) {
        return false;
    }

    movements << boardCoordX[figures[index].x] + boardCoordY[figures[index].y] + boardCoordX[toX] + boardCoordY[toY];

    figures[index].x = toX;
    figures[index].y = toY;
    figures[index].movesNum++;

    return true;
}

bool Logic::Impl::capture(int &attacker, int victim)
{
    if (attacker == victim) {
        return false;
    }
    if (attacker < 0 || attacker >= figures.size()) {
        return false;
    }
    if (victim < 0 || victim >= figures.size()) {
        return false;
    }

    captured.insert(currMove, figures[victim]);
    figures.removeAt(victim);

    if (victim < attacker) {
        attacker--;
    }

    return true;
}

int Logic::Impl::checkStatus(int team) const
{
    int king = 0, result = None;
    for (king = 0; king < figures.size(); king++) {
        if (figures[king].type == King && figures[king].team == team) {
            break;
        }
    }

    // заплатка на время, пока не реализована проверка на "шах и мат"
    ///////////////////////////////////
    if (king == figures.size()) {
        return None;
    }
    ///////////////////////////////////

    for (int i = 0; i < figures.size(); ++i) {
        if (figures[i].team != team && canAttack(i, king)) {
            result = Check;
            break;
        }
    }

    // проверка на мат

    return result;
}

Logic::Logic(QObject *parent) : QAbstractListModel(parent), impl(new Impl())
{

}

Logic::~Logic() {

}

int Logic::boardSize() const {
    return BOARD_SIZE;
}

int Logic::getTeam() const {
    return impl->currMove % 2;
}

int Logic::getMovementsNum() const {
    return impl->movements.size();
}

int Logic::getCurrMove() const {
    return impl->currMove;
}

void Logic::start() {
    beginInsertRows(QModelIndex(), 0, FIGURES_NUM - 1);
    impl->initPieces();
    endInsertRows();
}

int Logic::rowCount(const QModelIndex &) const {
    return impl->figures.size();
}

QHash<int, QByteArray> Logic::roleNames() const {
    QHash<int, QByteArray> names;
    names.insert(Roles::Team      , "team");
    names.insert(Roles::Type      , "type");
    names.insert(Roles::PositionX , "positionX");
    names.insert(Roles::PositionY , "positionY");
    return names;
}

QVariant Logic::data(const QModelIndex &modelIndex, int role) const {
    if (!modelIndex.isValid()) {
        return QVariant();
    }

    int index = static_cast<int>(modelIndex.row());

    if (index >= impl->figures.size() || index < 0) {
        return QVariant();
    }

    Figure &figure = impl->figures[index];

    switch (role) {
    case Roles::Team     : return figure.team;
    case Roles::Type     : return figure.type;
    case Roles::PositionX: return figure.x;
    case Roles::PositionY: return figure.y;
    }
    return QVariant();
}

bool Logic::setData(const QModelIndex &modelIndex, const QVariant &value, int role) {
    if (!modelIndex.isValid()) {
        return false;
    }

    switch (role) {
    case Roles::Team: {
        impl->figures[modelIndex.row()].team = value.toInt();
        break;
    }
    case Roles::Type: {
        impl->figures[modelIndex.row()].type = value.toInt();
        break;
    }
    case Roles::PositionX: {
        impl->figures[modelIndex.row()].x = value.toInt();
        break;
    }
    case Roles::PositionY: {
        impl->figures[modelIndex.row()].y = value.toInt();
        break;
    }
    default: return false;
    }

    emit dataChanged(modelIndex, modelIndex, { role });

    return true;
}

Qt::ItemFlags Logic::flags(const QModelIndex &modelIndex) const {
    if (!modelIndex.isValid()) {
        return Qt::ItemIsEnabled;
    }

    return QAbstractListModel::flags(modelIndex) | Qt::ItemIsEditable;
}

void Logic::clear() {
    beginRemoveRows(QModelIndex(), 0, rowCount(QModelIndex()) - 1);
    impl->figures.clear();
    endRemoveRows();

    impl->currMove = 0;
    impl->movements.clear();
    impl->captured.clear();

    emit currMoveChanged();
    emit movementsNumChanged();
}

void Logic::undo()
{
    if (impl->movements.isEmpty()) {
        return;
    }

    int index;
    int fromX, fromY, toX, toY;

    QString movement = impl->movements.takeLast();
    impl->currMove--;

    toX = impl->boardCoordX.indexOf(QString(movement[0])),
    toY = impl->boardCoordY.indexOf(QString(movement[1])),
    fromX = impl->boardCoordX.indexOf(QString(movement[2])),
    fromY = impl->boardCoordY.indexOf(QString(movement[3]));
    index = impl->findByPosition(fromX, fromY);

    impl->figures[index].x = toX;
    impl->figures[index].y = toY;
    impl->figures[index].movesNum--;
    QModelIndex topLeft = createIndex(index, 0);
    QModelIndex bottomRight = createIndex(index, 0);
    emit dataChanged(topLeft, bottomRight, { PositionX, PositionY });

    if (impl->captured.contains(impl->currMove)) {
        beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex()));
        impl->figures << impl->captured.take(impl->currMove);
        endInsertRows();
    } else if (movement.length() > 4 && movement[4] == '&') {
        toX = impl->boardCoordX.indexOf(QString(movement[5])),
        toY = impl->boardCoordY.indexOf(QString(movement[6])),
        fromX = impl->boardCoordX.indexOf(QString(movement[7])),
        fromY = impl->boardCoordY.indexOf(QString(movement[8]));
        index = impl->findByPosition(fromX, fromY);

        impl->figures[index].x = toX;
        impl->figures[index].y = toY;
        impl->figures[index].movesNum--;
        QModelIndex topLeft = createIndex(index, 0);
        QModelIndex bottomRight = createIndex(index, 0);
        emit dataChanged(topLeft, bottomRight, { PositionX, PositionY });
    }

    emit teamChanged();
    emit movementsNumChanged();
}

bool Logic::move(int fromX, int fromY, int toX, int toY) {
    int index = impl->findByPosition(fromX, fromY);
    if (index < 0) return false;

    if (!impl->isMoveValid(index, toX, toY)) {
        return false;
    }

    int index2 = impl->findByPosition(toX, toY);
    bool castling = false;

    if (index2 >= 0) {
        if (impl->figures[index].team != impl->figures[index2].team) {
            beginRemoveRows(QModelIndex(), index2, index2);
            impl->capture(index, index2);
            endRemoveRows();
        } else if (impl->figures[index].type == King && impl->figures[index2].type == Rook) {
            castling = true;

            // ход на две клетки в сторону ладьи
            toX = impl->figures[index].x + (toX - fromX > 0 ? 2 : -2);

            // ладья переносится через короля и ставится на следующее за ним поле
            if (!impl->move(index2, toX + (toX - fromX > 0 ? -1 : 1), impl->figures[index2].y)) {
                return false;
            }
            QModelIndex topLeft = createIndex(index2, 0);
            QModelIndex bottomRight = createIndex(index2, 0);
            emit dataChanged(topLeft, bottomRight, { PositionX, PositionY });
        }
    }

    if (!impl->move(index, toX, toY)) {
        return false;
    }
    QModelIndex topLeft = createIndex(index, 0);
    QModelIndex bottomRight = createIndex(index, 0);
    emit dataChanged(topLeft, bottomRight, { PositionX, PositionY });

    if (castling) {
        QString movement = impl->movements.takeLast();
        impl->movements.back().insert(0, movement + "&");
    }

    impl->currMove++;
    emit teamChanged();
    emit movementsNumChanged();

    return true;
}

bool Logic::save(QString path)
{
    path = QUrl(path).toLocalFile();

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream fstream(&file);
    for (int i = 0; i < impl->movements.size(); ++i) {
        fstream << impl->movements.at(i) << "\n";
    }
    file.close();

    return true;
}

bool Logic::load(QString path)
{
    path = QUrl(path).toLocalFile();

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    clear();

    Impl impl2;
    int index, index2 = -1, fromX, fromY, toX, toY;

    impl2.initPieces();

    QTextStream stream(&file);
    while (!stream.atEnd())
    {
        QString movement = stream.readLine();
        impl->movements << movement;

        fromX = impl2.boardCoordX.indexOf(QString(movement[0])),
        fromY = impl2.boardCoordY.indexOf(QString(movement[1])),
        toX = impl2.boardCoordX.indexOf(QString(movement[2])),
        toY = impl2.boardCoordY.indexOf(QString(movement[3]));
        index = impl2.findByPosition(fromX, fromY);

        if (index < 0) {
            return false;
        }
        if (!impl2.isMoveValid(index, toX, toY) && movement.size() == 4) {
            return false;
        }

        index2 = impl2.findByPosition(toX, toY);

        if (index2 >= 0) {
            if (impl2.figures[index].team == impl2.figures[index2].team) {
                return false;
            }
            impl2.capture(index, index2);
        } else if (movement.size() > 4 && movement.size() == 9 && movement[4] == '&') {
            index2 = impl2.findByPosition(impl2.boardCoordX.indexOf(QString(movement[5])), impl2.boardCoordY.indexOf(QString(movement[6])));
            if (index2 < 0) {
                return false;
            }
            if (!impl2.move(index2, impl2.boardCoordX.indexOf(QString(movement[7])), impl2.boardCoordY.indexOf(QString(movement[8])))) {
                return false;
            }
        }

        if (!impl2.move(index, toX, toY)) {
            return false;
        }
    }
    file.close();

    start();
    emit teamChanged();
    emit movementsNumChanged();

    return true;
}

bool Logic::prev()
{
    if (impl->movements.isEmpty()) {
        return false;
    }

    impl->currMove--;
    emit currMoveChanged();

    int toX = impl->boardCoordX.indexOf(QString(impl->movements[impl->currMove][0])),
        toY = impl->boardCoordY.indexOf(QString(impl->movements[impl->currMove][1])),
        fromX = impl->boardCoordX.indexOf(QString(impl->movements[impl->currMove][2])),
        fromY = impl->boardCoordY.indexOf(QString(impl->movements[impl->currMove][3])),
        index = impl->findByPosition(fromX, fromY);

    impl->figures[index].x = toX;
    impl->figures[index].y = toY;
    impl->figures[index].movesNum--;
    QModelIndex topLeft = createIndex(index, 0);
    QModelIndex bottomRight = createIndex(index, 0);
    emit dataChanged(topLeft, bottomRight, { PositionX, PositionY });

    if (impl->captured.contains(impl->currMove)) {
        beginInsertRows(QModelIndex(), rowCount(QModelIndex()), rowCount(QModelIndex()));
        impl->figures << impl->captured[impl->currMove];
        endInsertRows();
        impl->captured.remove(impl->currMove);
    } else if (impl->movements.length() > 4 && impl->movements[impl->currMove][4] == '&') {
        toX = impl->boardCoordX.indexOf(QString(impl->movements[impl->currMove][5])),
        toY = impl->boardCoordY.indexOf(QString(impl->movements[impl->currMove][6])),
        fromX = impl->boardCoordX.indexOf(QString(impl->movements[impl->currMove][7])),
        fromY = impl->boardCoordY.indexOf(QString(impl->movements[impl->currMove][8]));
        index = impl->findByPosition(fromX, fromY);

        impl->figures[index].x = toX;
        impl->figures[index].y = toY;
        impl->figures[index].movesNum--;
        QModelIndex topLeft = createIndex(index, 0);
        QModelIndex bottomRight = createIndex(index, 0);
        emit dataChanged(topLeft, bottomRight, { PositionX, PositionY });
    }

    emit teamChanged();
    emit movementsNumChanged();

    return true;
}

bool Logic::next()
{
    if (impl->currMove >= impl->movements.size() + 1) {
        return false;
    }

    int fromX = impl->boardCoordX.indexOf(QString(impl->movements[impl->currMove][0])),
        fromY = impl->boardCoordY.indexOf(QString(impl->movements[impl->currMove][1])),
        toX = impl->boardCoordX.indexOf(QString(impl->movements[impl->currMove][2])),
        toY = impl->boardCoordY.indexOf(QString(impl->movements[impl->currMove][3])),
        index = impl->findByPosition(fromX, fromY),
        index2 = impl->findByPosition(toX, toY);

    impl->figures[index].x = toX;
    impl->figures[index].y = toY;
    impl->figures[index].movesNum++;
    QModelIndex topLeft = createIndex(index, 0);
    QModelIndex bottomRight = createIndex(index, 0);
    emit dataChanged(topLeft, bottomRight, { PositionX, PositionY });

    if (index2 >= 0) {
        beginRemoveRows(QModelIndex(), index2, index2);
        impl->capture(index, index2);
        endRemoveRows();
    } else if (impl->movements.size() > 4 && impl->movements[impl->currMove][4] == '&') {
        fromX = impl->boardCoordX.indexOf(QString(impl->movements[impl->currMove][5])),
        fromY = impl->boardCoordY.indexOf(QString(impl->movements[impl->currMove][6])),
        toX = impl->boardCoordX.indexOf(QString(impl->movements[impl->currMove][7])),
        toY = impl->boardCoordY.indexOf(QString(impl->movements[impl->currMove][8]));
        index2 = impl->findByPosition(fromX, fromY);

        impl->figures[index2].x = toX;
        impl->figures[index2].y = toY;
        impl->figures[index2].movesNum++;
        QModelIndex topLeft = createIndex(index2, 0);
        QModelIndex bottomRight = createIndex(index2, 0);
        emit dataChanged(topLeft, bottomRight, { PositionX, PositionY });
    }

    impl->currMove++;
    emit currMoveChanged();
    emit teamChanged();
    emit movementsNumChanged();

    return true;
}
