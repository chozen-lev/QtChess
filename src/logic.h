#pragma once
#include <memory>
#include <QAbstractListModel>

class Logic: public QAbstractListModel
{
    Q_OBJECT
public:
    enum GlobalConstants {
        BOARD_SIZE = 8,
        FIGURES_NUM = 32
    };

    enum Roles {
        Team = Qt::UserRole,
        Type,
        PositionX,
        PositionY,
    };

    enum Types {
        Pawn,
        Knight,
        Bishop,
        Rook,
        Queen,
        King
    };

    enum Teams {
        Black,
        White
    };

    enum Statuses {
        None,
        Check,
        Checkmate
    };
    
public:
    explicit Logic(QObject *parent = 0);
    ~Logic();

    Q_PROPERTY(int boardSize READ boardSize CONSTANT)
    int boardSize() const;

    Q_PROPERTY(int getTeam READ getTeam NOTIFY teamChanged)
    int getTeam() const;

    Q_PROPERTY(int getMovementsNum READ getMovementsNum NOTIFY movementsNumChanged)
    int getMovementsNum() const;

    Q_PROPERTY(int getCurrMove READ getCurrMove NOTIFY currMoveChanged)
    int getCurrMove() const;

    Q_INVOKABLE void start();
    Q_INVOKABLE void clear();
    Q_INVOKABLE void undo();
    Q_INVOKABLE bool move(int fromX, int fromY, int toX, int toY);
    Q_INVOKABLE bool save(QString path);

protected:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void teamChanged();
    void movementsNumChanged();
    void currMoveChanged();

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
