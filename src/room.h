/* hunt the wumpus, gui implementation using C++ and QT
 * Copyright (C) 2019  Sandro Wißmann
 *
 * hunt the wumpus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * hunt the wumpus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with hunt the wumpus. If not, see <http://www.gnu.org/licenses/>.
 *
 * Web-Site: https://github.com/sandro4912/hunt-the-wumpus
 */

#ifndef ROOM_H
#define ROOM_H

#include <QGraphicsWidget>

#include <array>

class QAction;

class Room : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit Room(QGraphicsItem *parent = nullptr);

    void emitNeigbourHazards();

    bool isEmpty() const;

    void setWumpus(bool value);
    bool hasWumpus() const;

    void setPit(bool value);
    bool hasPit() const;

    void setBat(bool value);
    bool hasBat() const;

    void setPlayer(bool value);
    bool hasPlayer() const;

    void setPlayerWasHere(bool value);
    bool playerWasHere() const;

    void setTarget(bool value);
    bool isTarget() const;

    void addNeighbour(Room *neighbour);
    QVector<Room *> neighbours() const;

    void showContent(bool value);

    void clear();

    QRectF boundingRect() const override;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    enum { Type = UserType + 1 };

    int type() const override;
signals:
    void wumpusNear();
    void batNear();
    void pitNear();

    void playerDiedFromWumpus();
    void playerDiedFromPit();
    void playerDraggedByBat();

    void entered();

public slots:
    void enter();

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

private slots:
    void toggleGuessWumpus();
    void toggleGuessPit();
    void toggleGuessBat();

private:
    bool isNeighbour(Room *room) const;

    bool minDistanceReached(QGraphicsSceneMouseEvent *event,
                            Qt::MouseButton button);
    void executePlayerDrag(QGraphicsSceneMouseEvent *event);

    bool isPlayerDrag(QGraphicsSceneDragDropEvent *event);

    void drawRoom(QPainter *painter, const QImage &roomImage);
    void drawWumpus(QPainter *painter, const QImage &wumpusImage);
    void drawBat(QPainter *painter, const QImage &batImage);
    void drawPit(QPainter *painter, const QImage &pitImage);
    void drawPlayer(QPainter *painter);

    QImage roomImage() const;
    QImage roomVisitedImage() const;
    QImage roomTargetImage() const;
    QImage batImage() const;
    QImage batConfirmedImage() const;
    QImage pitImage() const;
    QImage pitConfirmedImage() const;
    QImage wumpusImage() const;
    QImage wumpusConfirmedImage() const;
    QImage playerImage() const;
    QImage playerDraggedImage() const;

    bool mBatConfirmed{ false };

    bool mHasWumpus{ false };
    bool mHasPit{ false };
    bool mHasBat{ false };
    bool mHasPlayer{ false };
    bool mPlayerWasHere{ false };

    bool mGuessWumpus{ false };
    bool mGuessBat{ false };
    bool mGuessPit{ false };

    bool mShowContent{ false };

    bool mIsTarget{ false };

    QAction *mGuessWumpusAction;
    QAction *mGuessBatAction;
    QAction *mGuessPitAction;

    QVector<Room *> mNeighbours;
};

#endif // ROOM_H
