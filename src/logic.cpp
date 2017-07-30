#include "logic.h"
#include <QList>
#include <QByteArray>
#include <QHash>
#include <iostream>

struct Figure
{
    int x;
    int y;
    int color;
    int type;
    int moves;
};


struct Logic::Impl
{
    QList<Figure> figures;

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

void Logic::btnStart_Handler()
{
    beginResetModel();
    impl->figures.clear();
    initPieces();
    endResetModel();
}

bool Logic::move(int fromX, int fromY, int toX, int toY)
{
    int index = impl->findByPosition(fromX, fromY);

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
            if (((toY - fromY < 0) == (impl->figures[index].color <= 0))) {
                return false;
            }

            if (abs(fromX - toX) > 1) {
                return false;
            }

            if (abs(fromY - toY) > 2 || impl->figures[index].moves > 0 && abs(fromY - toY) == 2) {
                return false;
            }

//            if (abs(fromY - toY) == 2) { // TODO: проверка на наличие фигуры на пути пешки

//            }

            if (index2 == -1 && abs(fromX - toX)) {
                return false;
            }

            if (abs(fromY - toY) == 2 && abs(fromX - toX)) {
                return false;
            }

            if (index2 >= 0 && fromX - toX == 0) {
                return false;
            }

            break;
        }
        case Knight:
        {
            if ((abs(fromY - toY) != 2 || abs(fromX - toX) != 1) && (abs(fromX - toX) != 2 || abs(fromY - toY) != 1)) {
                return false;
            }

            break;
        }
        case Bishop:
        {
            if (fromY - toY == 0 || fromX - toX == 0) {
                return 0;
            }

//            if (abs(fromY - toY) > 1) { // TODO: проверка на наличие фигуры на пути слона
//                return false;
//            }

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
            if (abs(fromY - toY) > 1 || abs(fromX - toX) > 1) {
                return false;
            }

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

    impl->figures[index].x = toX;
    impl->figures[index].y = toY;
    impl->figures[index].moves++;
    emit layoutChanged();

    return true;
}

void Logic::initPieces()
{
    for (int i = 0; i < 2; ++i)
    {
        // pawns
        for (int j = 0; j < 8; ++j) {
            impl->figures << Figure { j, i * (BOARD_SIZE - 3) + 1, i, Pawn };
        }

        // kings
        impl->figures << Figure { 4, i * (BOARD_SIZE - 1), i, King };

        // queens
        impl->figures << Figure { 3, i * (BOARD_SIZE - 1), i, Queen };

        // bishops
        impl->figures << Figure { 2, i * (BOARD_SIZE - 1), i, Bishop };
        impl->figures << Figure { BOARD_SIZE - 3, i * (BOARD_SIZE - 1), i, Bishop };

        // knigths
        impl->figures << Figure { 1, i * (BOARD_SIZE - 1), i, Knight };
        impl->figures << Figure { BOARD_SIZE - 2, i * (BOARD_SIZE - 1), i, Knight };

        // rooks
        impl->figures << Figure { 0, i * (BOARD_SIZE - 1), i, Rook };
        impl->figures << Figure { BOARD_SIZE - 1, i * (BOARD_SIZE - 1), i, Rook };
    }

    emit layoutChanged();
}

void Logic::btnStop_Handler()
{
    beginResetModel();
    impl->figures.clear();
    endResetModel();
}
