#include "logic.h"
#include <QList>
#include <QByteArray>
#include <QHash>
#include <iostream>
#include <QFile>
#include <QUrl>
#include <QTextStream>

#include <QDebug>

struct Figure
{
    int x;
    int y;
    int color;
    int type;
    int movesNum;
};


struct Logic::Impl
{
    QList<QString> const boardCoordX = { "a", "b", "c", "d", "e", "f", "g", "h" };
    QList<QString> const boardCoordY = { "8", "7", "6", "5", "4", "3", "2", "1" };

    QList<Figure> figures;
    QList<QString> movements;
    QHash<int, int> removes;

    int lastMove = 0;

    int findByPosition(int x, int y) const;
    void initPieces();
};

int Logic::Impl::findByPosition(int x, int y) const
{
    for (int i(0); i < figures.size(); ++i)
    {
        if (figures[i].x != x || figures[i].y != y) {
            continue;
        }
        return i;
    }
    return -1;
}

void Logic::Impl::initPieces()
{
    for (int i = 0; i < 2; ++i)
    {
        // pawns
        for (int j = 0; j < 8; ++j) {
            figures << Figure { j, i * (BOARD_SIZE - 3) + 1, i, Pawn, 0 };
        }

        // kings
        figures << Figure { 4, i * (BOARD_SIZE - 1), i, King, 0 };

        // queens
        figures << Figure { 3, i * (BOARD_SIZE - 1), i, Queen, 0 };

        // bishops
        figures << Figure { 2, i * (BOARD_SIZE - 1), i, Bishop, 0 };
        figures << Figure { BOARD_SIZE - 3, i * (BOARD_SIZE - 1), i, Bishop, 0 };

        // knigths
        figures << Figure { 1, i * (BOARD_SIZE - 1), i, Knight, 0 };
        figures << Figure { BOARD_SIZE - 2, i * (BOARD_SIZE - 1), i, Knight, 0 };

        // rooks
        figures << Figure { 0, i * (BOARD_SIZE - 1), i, Rook, 0 };
        figures << Figure { BOARD_SIZE - 1, i * (BOARD_SIZE - 1), i, Rook, 0 };
    }
}


Logic::Logic(QObject *parent) : QAbstractListModel(parent), impl(new Impl())
{

}

Logic::~Logic()
{

}

int Logic::boardSize() const
{
    return BOARD_SIZE;
}

int Logic::rowCount(const QModelIndex &) const
{
    return impl->figures.size();
}

QHash<int, QByteArray> Logic::roleNames() const
{
    QHash<int, QByteArray> names;
    names.insert(Roles::Type      , "type");
    names.insert(Roles::PositionX , "positionX");
    names.insert(Roles::PositionY , "positionY");
    names.insert(Roles::Color     , "color");
    return names;
}

bool Logic::setData(const QModelIndex &modelIndex, const QVariant &value, int role)
{
    if (!modelIndex.isValid()) {
        return false;
    }

    switch (role)
    {
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
        case Roles::Color: {
            impl->figures[modelIndex.row()].color = value.toInt();
            break;
        }
        default: return false;
    }

    emit dataChanged(modelIndex, modelIndex, { role });

    return true;
}

Qt::ItemFlags Logic::flags(const QModelIndex &modelIndex) const
{
    if (!modelIndex.isValid()) {
        return Qt::ItemIsEnabled;
    }

    return QAbstractListModel::flags(modelIndex) | Qt::ItemIsEditable;
}

QVariant Logic::data(const QModelIndex &modelIndex, int role) const
{
    if (!modelIndex.isValid()) {
        return QVariant();
    }

    int index = static_cast<int>(modelIndex.row());

    if (index >= impl->figures.size() || index < 0) {
        return QVariant();
    }

    qDebug() << QString("Get Data - Row: %1, Col: %2, Role: %3").arg(modelIndex.row()).arg(modelIndex.column()).arg(role);

    Figure &figure = impl->figures[index];

    switch (role)
    {
        case Roles::Type     : return figure.type;
        case Roles::PositionX: return figure.x;
        case Roles::PositionY: return figure.y;
        case Roles::Color    : return figure.color;
    }
    return QVariant();
}

void Logic::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount(QModelIndex()) - 1);
    impl->figures.clear();
    endRemoveRows();

    impl->movements.clear();
    impl->removes.clear();

    lastMove = 0;
    emit moveChanged();
}

void Logic::init()
{
    beginInsertRows(QModelIndex(), 0, 31);
    impl->initPieces();
    endInsertRows();
}

bool Logic::move(int fromX, int fromY, int toX, int toY)
{
    int index = impl->findByPosition(fromX, fromY);

    if (index < 0) {
        return false;
    }

    qDebug() << getColor();

    if (toX < 0 || toX >= BOARD_SIZE || toY < 0 || toY >= BOARD_SIZE) {
        return false;
    }

    int index2 = impl->findByPosition(toX, toY);

    switch (impl->figures[index].type)
    {
        case Pawn:
        {
            if (index2 >= 0 && impl->figures[index].color == impl->figures[index2].color) {
                return false;
            }
            break;
        }
        case Knight:
        {
            if (index2 >= 0 && impl->figures[index].color == impl->figures[index2].color) {
                return false;
            }
            break;
        }
        case Bishop:
        {
            if (index2 >= 0 && impl->figures[index].color == impl->figures[index2].color) {
                return false;
            }
            break;
        }
        case Rook:
        {
            if (index2 >= 0 && impl->figures[index].color == impl->figures[index2].color) {
                return false;
            }
            break;
        }
        case Queen:
        {
            if (index2 >= 0 && impl->figures[index].color == impl->figures[index2].color) {
                return false;
            }
            break;
        }
        case King:
        {
            if (index2 >= 0 && impl->figures[index].color == impl->figures[index2].color) {
                return false;
            }
            break;
        }
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

    impl->figures[index].movesNum++;
    impl->movements << impl->boardCoordX[fromX] + impl->boardCoordY[fromY] + " " + impl->boardCoordX[toX] + impl->boardCoordY[toY];

    lastMove++;
    emit moveChanged();
    emit colorChanged();

    return true;
}

bool Logic::load(QString path)
{
    path = QUrl(path).toLocalFile();

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);

    clear();
    impl->initPieces();

    while (!stream.atEnd())
    {
        QString movement = stream.readLine();

        if (movement.length() < 5)
        {
            file.close();
            return false;
        }

        int fromX = impl->boardCoordX.indexOf(QString(movement[0])),
            fromY = impl->boardCoordY.indexOf(QString(movement[1])),
            toX = impl->boardCoordX.indexOf(QString(movement[3])),
            toY = impl->boardCoordY.indexOf(QString(movement[4]));

        if (fromX < 0 || fromY < 0 || movement[2] != ' ' || toX < 0 || toY < 0)
        {
            file.close();
            return false;
        }

        int index = impl->findByPosition(fromX, fromY);

        if (index < 0)
        {
            file.close();
            return false;
        }

        int index2 = impl->findByPosition(toX, toY);

        if (index2 >= 0)
        {
            if (index == index2)
            {
                file.close();
                return false;
            }

            impl->removes.insert(impl->movements.size(), impl->figures[index2].type);
            impl->figures.removeAt(index2);

            if (index2 < index) {
                index--;
            }
        }

        impl->figures[index].x = toX;
        impl->figures[index].y = toY;

        impl->movements << movement;
    }

    emit moveNumChanged();

    impl->figures.clear();
    init();

    file.close();

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

int Logic::getColor() const
{
    return lastMove % 2;
}

int Logic::getLastMove() const
{
    return lastMove;
}

int Logic::getMovesNum() const
{
    return impl->movements.size();
}

bool Logic::move(bool backward)
{
    lastMove += backward ? -1 : 1;
    emit moveChanged();

    return true;
}
