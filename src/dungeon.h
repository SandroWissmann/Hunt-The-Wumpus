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

#ifndef DUNGEON_H
#define DUNGEON_H

#include <QWidget>

class Room;

class QGraphicsScene;
class QGraphicsLineItem;
class DungeonView;

class Dungeon : public QWidget
{
    Q_OBJECT
public:
    explicit Dungeon(QWidget *parent = nullptr);

    void showHazards(bool value);

    void enter();
    void reset();

    int remainingArrows() const;

signals:
    void wumpusNear();
    void batNear();
    void pitNear();

    void playerDraggedByBat();

    void playerDiedFromWumpus();
    void playerDiedFromPit();
    void playerIsOutOfArrows();

    void playerKilledWumpus();

    void enteredRoom();
    void arrowShot();

private slots:
    void moveWumpusIfWakeUp();
    void showLinesToNeigboursOfRoom();
    void decreaseArrows();

private:
    void scaleViewToSize();

    void emptyRooms();
    void hideDungeon();
    void createRooms();
    void connectToRooms();
    void addRoomsToScene();

    void addLinesToScene();


    void addLineToNeigbours(const Room *room);
    bool lineExistsInScene(const QLineF &line);
    QGraphicsLineItem *findLineInScene(const QLineF &line);

    static constexpr auto mCountOfRooms = 20;
    static constexpr auto mCountOfPits = 2;
    static constexpr auto mCountOfBats = 2;
    static constexpr auto mCountOfArrows = 5;

    int mRemainingArrows{ mCountOfArrows };

    QGraphicsScene *mGraphicsScene;
    DungeonView *mDungeonView;
    QVector<Room *> mRooms;
};


#endif // DUNGEON_H
