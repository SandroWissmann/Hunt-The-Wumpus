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

#include "roomutility.h"

#include "room.h"

#include <QRandomGenerator>

static constexpr auto countOfRooms = 20;

void connectRoomsAsDodekaeder(QVector<Room *> &rooms)
{
    Q_ASSERT(rooms.size() == countOfRooms);

    setNeigbours(rooms, 0, 1, 4, 19);
    setNeigbours(rooms, 1, 0 , 2, 17);
    setNeigbours(rooms, 2, 1 , 3, 15);
    setNeigbours(rooms, 3, 2 , 4, 13);
    setNeigbours(rooms, 4, 0 , 3, 5);
    setNeigbours(rooms, 5, 4 , 6, 12);
    setNeigbours(rooms, 6, 5 , 7, 19);
    setNeigbours(rooms, 7, 6 , 8, 11);
    setNeigbours(rooms, 8, 7 , 9, 18);
    setNeigbours(rooms, 9, 8 , 10, 16);
    setNeigbours(rooms, 10, 9 , 11, 14);
    setNeigbours(rooms, 11, 7 , 10, 12);
    setNeigbours(rooms, 12, 5 , 11, 13);
    setNeigbours(rooms, 13, 3 , 12, 14);
    setNeigbours(rooms, 14, 10 , 13, 15);
    setNeigbours(rooms, 15, 2 , 14, 16);
    setNeigbours(rooms, 16, 9 , 15, 17);
    setNeigbours(rooms, 17, 1 , 16, 18);
    setNeigbours(rooms, 18, 8 , 17, 19);
    setNeigbours(rooms, 19, 0 , 6, 18);
}

void setNeigbours(QVector<Room *> &rooms,
                  int roomNo,
                  int roomNoNeigbourOne,
                  int roomNoNeigbourTwo,
                  int roomNoNeigbourThree)
{
    Q_ASSERT(roomNo >= 0 && roomNo < rooms.size());
    Q_ASSERT(roomNoNeigbourOne >= 0 && roomNoNeigbourOne < rooms.size());
    Q_ASSERT(roomNoNeigbourTwo >= 0 && roomNoNeigbourTwo < rooms.size());
    Q_ASSERT(roomNoNeigbourThree >= 0 && roomNoNeigbourThree < rooms.size());
    Q_ASSERT(roomNo != roomNoNeigbourOne);
    Q_ASSERT(roomNoNeigbourOne != roomNoNeigbourTwo);
    Q_ASSERT(roomNoNeigbourTwo != roomNoNeigbourThree);
    Q_ASSERT(roomNoNeigbourThree != roomNo);

    rooms[roomNo]->addNeighbour(rooms[roomNoNeigbourOne]);
    rooms[roomNo]->addNeighbour(rooms[roomNoNeigbourTwo]);
    rooms[roomNo]->addNeighbour(rooms[roomNoNeigbourThree]);
}

void populateRoomsRandom(
        QVector<Room *> &rooms, int countOfPits, int countOfBats)
{
    constexpr auto countOfWumpus = 1;

    Q_ASSERT(countOfPits >= 0);
    Q_ASSERT(countOfBats >= 0);
    Q_ASSERT(countOfPits + countOfBats + countOfWumpus < rooms.size());

    auto room = randomRoom(rooms);
    room->setWumpus(true);

    for (auto i = 0; i < countOfPits; ++i) {
        for (;;) {
            auto room = randomRoom(rooms);
            if (room->isEmpty()) {
                room->setPit(true);
                break;
            }
        }
    }

    for (auto i = 0; i < countOfBats; ++i) {
        for (;;) {
            auto room = randomRoom(rooms);
            if (room->isEmpty()) {
                room->setBat(true);
                break;
            }
        }
    }
}

void setPositionOfRooms(QVector<Room *> &rooms)
{
    const auto roomWidth = static_cast<int>(rooms[0]->size().width());
    const auto roomHeight = static_cast<int>(rooms[0]->size().height());

    QVector<QPoint> points
    {
        QPoint{ 400 - roomWidth / 2, 450 - roomHeight / 2},
        QPoint{ 600 - roomWidth / 2, 450 - roomHeight / 2},
        QPoint{ 600 - roomWidth / 2, 550 - roomHeight / 2},
        QPoint{ 500 - roomWidth / 2, 600 - roomHeight / 2},
        QPoint{ 400 - roomWidth / 2, 550 - roomHeight / 2},
        QPoint{ 250 - roomWidth / 2, 600 - roomHeight / 2},
        QPoint{ 250 - roomWidth / 2, 450 - roomHeight / 2},
        QPoint{ 100 - roomWidth / 2, 300 - roomHeight / 2},
        QPoint{ 500 - roomWidth / 2, 75 - roomHeight / 2},
        QPoint{ 900 - roomWidth / 2, 300 - roomHeight / 2},
        QPoint{ 900 - roomWidth / 2, 900 - roomHeight / 2},
        QPoint{ 100 - roomWidth / 2, 900 - roomHeight / 2},
        QPoint{ 300 - roomWidth / 2, 750 - roomHeight / 2},
        QPoint{ 500 - roomWidth / 2, 750 - roomHeight / 2},
        QPoint{ 750 - roomWidth / 2, 750 - roomHeight / 2},
        QPoint{ 750 - roomWidth / 2, 600 - roomHeight / 2},
        QPoint{ 800 - roomWidth / 2, 450 - roomHeight / 2},
        QPoint{ 700 - roomWidth / 2, 300 - roomHeight / 2},
        QPoint{ 500 - roomWidth / 2, 200 - roomHeight / 2},
        QPoint{ 350 - roomWidth / 2, 300 - roomHeight / 2},
    };

    Q_ASSERT(points.size() == rooms.size());

    for(auto i = 0; i<rooms.size(); ++i) {
        rooms[i]->setPos(points[i]);
    }
}

Room *randomRoom(const QVector<Room *> &rooms)
{
    auto idx = QRandomGenerator::global()->bounded(0, rooms.size() - 1);
    return rooms[idx];
}
