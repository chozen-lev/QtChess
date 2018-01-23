#pragma once
#include <memory>
#include <QAbstractListModel>

class Logic: public QAbstractListModel
{
    Q_OBJECT
public:

    enum GlobalConstants {
        BOARD_SIZE = 8
    };

    enum Roles {
        Team = Qt::UserRole,
        Type,
        PositionX,
        PositionY,
    };

    enum Types {
        Pawn,
        Bishop,
        Knight,
        Rook,
        Queen,
        King
    };

    enum Teams {
        White,
        Black
    };
    
public:
    explicit Logic(QObject *parent = 0);
    ~Logic();

    Q_PROPERTY(int boardSize READ boardSize CONSTANT)
    int boardSize() const;

    Q_INVOKABLE void start();
    Q_INVOKABLE void clear();
    Q_INVOKABLE bool move(int fromX, int fromY, int toX, int toY);

protected:
    int rowCount(const QModelIndex & parent) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
