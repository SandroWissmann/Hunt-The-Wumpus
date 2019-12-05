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

#ifndef DUNGEONVIEW_H
#define DUNGEONVIEW_H

#include <QGraphicsView>

class Room;

class DungeonView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit DungeonView(QWidget *parent = nullptr);

signals:
    void shootHitWumpus();
    void shootMissedWumpus();

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void startShootArrowMode(Room *room);
    void stopShootArrowMode(QMouseEvent *event);

    void shootArrow(QMouseEvent *event);

    void startDragPlayerMode(QMouseEvent *event);
    void stopDragPlayerMode(QMouseEvent *event);

    bool maxArrowRangeReached() const;
    bool isMarked(Room *room) const;
    bool isNeigbourOfLastMarkedRoom(Room *room) const;

    bool leftRoom(Room *current, Room* last);
    bool enteredRoom(Room *current, Room* last);

    static constexpr auto mArrowRoomRange{ 3 };


    bool mShootArrowSelectOn{ false };

    Room *mlastRoom{ nullptr };

    QVector<Room *> mMarkedRooms;
};

#endif // DUNGEONVIEW_H
