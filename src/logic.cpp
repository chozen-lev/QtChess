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

    void initPieces();
    int findByPosition(int x, int y);
};

void Logic::Impl::initPieces()
{
    for (int team = 0; team < 2; team++)
    {
        for (int x = 0; x < 8; x++) {
            figures << Figure { Pawn, team, x, (1 - team) * 5 + 1, 0 };
        }
        figures << Figure { King, team, 4, (1 - team) * 7, 0 };
        figures << Figure { Queen, team, 3, (1 - team) * 7, 0 };
        figures << Figure { Bishop, team, 2, (1 - team) * 7, 0 };
        figures << Figure { Bishop, team, 5, (1 - team) * 7, 0 };
        figures << Figure { Knight, team, 1, (1 - team) * 7, 0 };
        figures << Figure { Knight, team, 6, (1 - team) * 7, 0 };
        figures << Figure { Rook, team, 0, (1 - team) * 7, 0 };
        figures << Figure { Rook, team, 7, (1 - team) * 7, 0 };
    }
}

int Logic::Impl::findByPosition(int x, int y) {
    for (int i(0); i < figures.size(); ++i) {
        if (figures[i].x != x || figures[i].y != y ) {
            continue;
        }
        return i;
    }
    return -1;
}

Logic::Logic(QObject *parent) : QAbstractListModel(parent), impl(new Impl())
{

}

Logic::~Logic() {

}

int Logic::boardSize() const {
    return BOARD_SIZE;
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
    case Roles::Team    : return figure.team;
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

    switch (role)
    {
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
    beginResetModel();
    impl->figures.clear();
    endResetModel();
}

bool Logic::move(int fromX, int fromY, int toX, int toY) {
    int index = impl->findByPosition(fromX, fromY);

    if (index < 0) return false;

    if (toX < 0 || toX >= BOARD_SIZE || toY < 0 || toY >= BOARD_SIZE) {
        return false;
    }

    int index2 = impl->findByPosition(toX, toY);

    if (index2 >= 0 && impl->figures[index].team == impl->figures[index2].team) {
        return false;
    }

    if (index2 >= 0)
    {
        beginRemoveRows(QModelIndex(), index2, index2);
        impl->figures.removeAt(index2);
        endRemoveRows();

        if (index2 < index) {
            index--;
        }
    }

    impl->figures[index].x = toX;
    impl->figures[index].y = toY;
    QModelIndex topLeft = createIndex(index, 0);
    QModelIndex bottomRight = createIndex(index, 0);
    emit dataChanged(topLeft, bottomRight, { PositionX, PositionY });

    return true;
}
