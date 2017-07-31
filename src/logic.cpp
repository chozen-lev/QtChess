#include "logic.h"
#include <QList>
#include <QByteArray>
#include <QHash>
#include <iostream>
#include <QtDebug>

struct Figure
{
    int x;
    int y;
    int color;
    int type;
    int moves;

    void initPieces();
};

struct Logic::Impl
{
    QString const boardCoordX[BOARD_SIZE] = { "a", "b", "c", "d", "e", "f", "g", "h" };
    QString const boardCoordY[BOARD_SIZE] = { "8", "7", "6", "5", "4", "3", "2", "1" };

    QList<Figure> figures;
    QList<QString> movements;

    int findByPosition(int x, int y);
};

int Logic::Impl::findByPosition(int x, int y)
{
    for (int i = 0; i < figures.size(); ++i)
    {
        if (figures[i].x != x || figures[i].y != y ) {
            continue;
        }
        return i;
    }
    return -1;
}

Logic::Logic(QObject *parent): QAbstractListModel(parent), impl(new Impl())
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
    names.insert(Roles::PositionX , "positionX");
    names.insert(Roles::PositionY , "positionY");
    names.insert(Roles::Color     , "color");
    names.insert(Roles::Type      , "type");
    names.insert(Roles::Moves     , "moves");
    return names;
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

    Figure &figure = impl->figures[index];

    switch (role)
    {
        case Roles::PositionX: return figure.x;
        case Roles::PositionY: return figure.y;
        case Roles::Color    : return figure.color;
        case Roles::Type     : return figure.type;
        case Roles::Moves    : return figure.moves;
    }
    return QVariant();
}

void Logic::clear()
{
    beginResetModel();
    impl->figures.clear();
    impl->movements.clear();
    endResetModel();
}

bool Logic::move(int fromX, int fromY, int toX, int toY)
{
    int index = impl->findByPosition(fromX, fromY);

    if (index < 0) return false;

    if (index < 0) {
        return false;
    }

    if (toX < 0 || toX >= BOARD_SIZE || toY < 0 || toY >= BOARD_SIZE) {
        return false;
    }

    int index2 = impl->findByPosition(toX, toY);

    switch (impl->figures[index].type)
    {
        case Pawn:
        {

            break;
        }
        case Knight:
        {

            break;
        }
        case Bishop:
        {

            break;
        }
        case Rook:
        {

            break;
        }
        case Queen:
        {

            break;
        }
        case King:
        {

            break;
        }
    }

    if (index2 >= 0)
    {
        if (impl->figures[index].color != impl->figures[index2].color)
        {
            beginResetModel();
            impl->figures.removeAt(index2);
            endResetModel();

            if (index2 < index) {
                index--;
            }
        }
        else return false;
    }

    QString movement = impl->boardCoordX[fromX] + impl->boardCoordY[fromY] + " " + impl->boardCoordX[toX] + impl->boardCoordY[toY];
    impl->movements << movement;
    qDebug().noquote() << movement;

    impl->figures[index].x = toX;
    impl->figures[index].y = toY;
    impl->figures[index].moves++;    
    emit layoutChanged();

    return true;
}

void Logic::initPieces()
{
    beginResetModel();
    for (int i = 0; i < 2; ++i)
    {
        // pawns
        for (int j = 0; j < 8; ++j) {
            impl->figures << Figure { j, i * (BOARD_SIZE - 3) + 1, i, Pawn, 0 };
        }

        // kings
        impl->figures << Figure { 4, i * (BOARD_SIZE - 1), i, King, 0 };

        // queens
        impl->figures << Figure { 3, i * (BOARD_SIZE - 1), i, Queen, 0 };

        // bishops
        impl->figures << Figure { 2, i * (BOARD_SIZE - 1), i, Bishop, 0 };
        impl->figures << Figure { BOARD_SIZE - 3, i * (BOARD_SIZE - 1), i, Bishop, 0 };

        // knigths
        impl->figures << Figure { 1, i * (BOARD_SIZE - 1), i, Knight, 0 };
        impl->figures << Figure { BOARD_SIZE - 2, i * (BOARD_SIZE - 1), i, Knight, 0 };

        // rooks
        impl->figures << Figure { 0, i * (BOARD_SIZE - 1), i, Rook, 0 };
        impl->figures << Figure { BOARD_SIZE - 1, i * (BOARD_SIZE - 1), i, Rook, 0 };
    }
    endResetModel();
}
