#include "logic.h"
#include <QList>
#include <QByteArray>
#include <QHash>

struct Figure
{
    int type;
    int team;
    int x;
    int y;
    bool firstmove;
};

struct Logic::Impl
{
    const QList<QString> boardCoordX = { "a", "b", "c", "d", "e", "f", "g", "h" };
    const QList<QString> boardCoordY = { "8", "7", "6", "5", "4", "3", "2", "1" };

    QList<Figure> figures;
    QList<QString> movements;

    int currMove = 0;

    void initPieces();
    int findByPosition(int x, int y) const;
    bool isMoveValid(int index, int toX, int toY) const;
};

void Logic::Impl::initPieces()
{
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
        if (figures[i].x != x || figures[i].y != y ) {
            continue;
        }
        return i;
    }
    return -1;
}

bool Logic::Impl::isMoveValid(int index, int toX, int toY) const {
    int victim = findByPosition(toX, toY);

    if (victim >= 0 && figures[index].team == figures[victim].team) {
        return false;
    }

    switch (figures[index].type) {
    case Pawn: {
        if ((toY - figures[index].y < 0) == (figures[index].team == White) || toY - figures[index].y == 0) {
            return false;
        }
        if (abs(toY - figures[index].y) > 2) {
            return false;
        }
        if (abs(toY - figures[index].y) == 2) {
            if (figures[index].firstmove || abs(toX - figures[index].x) > 0) {
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
        for (int i = 1; i < abs(toX - figures[index].x); i++) {
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
        for (int i = figures[index].x; i != toX; (toX - figures[index].x) > 0 ? i++ : i--) {
            if (findByPosition(i, figures[index].y) >= 0 && i != figures[index].x) {
                return false;
            }
        }
        for (int i = figures[index].y; i != toY; (toY - figures[index].y) > 0 ? i++ : i--) {
            if (findByPosition(figures[index].x, i) >= 0 && i != figures[index].y) {
                return false;
            }
        }
        return true;
    }
    case Queen: {
        if(abs(toX - figures[index].x) == abs(toY - figures[index].y)) {
            for (int i = 1; i < abs(toX - figures[index].x); i++) {
                if (findByPosition(figures[index].x + (figures[index].x < toX ? i : -i), figures[index].y + (figures[index].y < toY ? i : -i)) >= 0) {
                    return false;
                }
            }
        } else if ((abs(toX - figures[index].x) > 0) != (abs(toY - figures[index].y) > 0)) {
            for (int i = figures[index].x; i != toX; (toX - figures[index].x) > 0 ? i++ : i--) {
                if (findByPosition(i, figures[index].y) >= 0 && i != figures[index].x) {
                    return false;
                }
            }
            for (int i = figures[index].y; i != toY; (toY - figures[index].y) > 0 ? i++ : i--) {
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
        break;
    }
    }
    return true;
}

Logic::Logic(QObject *parent) : QAbstractListModel(parent), impl(new Impl())
{

}

Logic::~Logic() {

}

int Logic::boardSize() const {
    return BOARD_SIZE;
}

int Logic::getTeam() const
{
    return impl->currMove % 2;
}

int Logic::getMovementsNum() const
{
    return impl->movements.size();
}

int Logic::getCurrMove() const
{
    return impl->currMove;
}

void Logic::start() {
    beginInsertRows(QModelIndex(), 0, 31);
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

    emit currMoveChanged();
    emit movementsNumChanged();
}

bool Logic::move(int fromX, int fromY, int toX, int toY) {
    int index = impl->findByPosition(fromX, fromY);

    if (index < 0) return false;

    if (toX < 0 || toX >= BOARD_SIZE || toY < 0 || toY >= BOARD_SIZE) {
        return false;
    }

    if (!impl->isMoveValid(index, toX, toY)) {
        return false;
    }

    if (!impl->figures[index].firstmove) {
        switch (impl->figures[index].type) {
        case Pawn: {
            impl->figures[index].firstmove = true;
            break;
        }
        case Rook: {
            impl->figures[index].firstmove = true;
            break;
        }
        case King: {
            impl->figures[index].firstmove = true;
            break;
        }
        }
    }

    int victim = impl->findByPosition(toX, toY);

    if (victim >= 0) {
        beginRemoveRows(QModelIndex(), victim, victim);
        impl->figures.removeAt(victim);
        endRemoveRows();

        if (victim < index) {
            index--;
        }
    }

    impl->currMove++;

    impl->figures[index].x = toX;
    impl->figures[index].y = toY;
    impl->figures[index].firstmove = true;
    QModelIndex topLeft = createIndex(index, 0);
    QModelIndex bottomRight = createIndex(index, 0);
    emit dataChanged(topLeft, bottomRight, { PositionX, PositionY });
    emit teamChanged();
    emit movementsNumChanged();

    return true;
}
