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

    enum Roles
    {
        Type = Qt::UserRole,
        PositionX,
        PositionY,
        Color
    };

    enum Types
    {
        Pawn,
        Knight,
        Bishop,
        Rook,
        Queen,
        King
    };

    enum Colors
    {
        Black,
        White
    };

public:
    explicit Logic(QObject *parent = 0);
    ~Logic();

    Q_PROPERTY(int boardSize READ boardSize CONSTANT)
    int boardSize() const;

    Q_PROPERTY(int getColor READ getColor NOTIFY colorChanged)
    int getColor() const;

    Q_PROPERTY(int getLastMove READ getLastMove NOTIFY moveChanged)
    int getLastMove() const;

    Q_PROPERTY(int getMovesNum READ getMovesNum NOTIFY moveNumChanged)
    int getMovesNum() const;

    Q_INVOKABLE void clear();
    Q_INVOKABLE void init();
    Q_INVOKABLE bool move(int fromX, int fromY, int toX, int toY);
    Q_INVOKABLE bool load(QString);
    Q_INVOKABLE bool save(QString);
    Q_INVOKABLE bool move(bool);

protected:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    int lastMove;

signals :
    void colorChanged();
    void moveChanged();
    void moveNumChanged();
};
