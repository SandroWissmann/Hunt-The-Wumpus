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

#ifndef ROOMUTILITY_H
#define ROOMUTILITY_H

#include <QVector>

#include <array>

class Room;

void connectRoomsAsDodekaeder(QVector<Room *> &rooms);

void setNeigbours(QVector<Room *> &rooms,
                  int roomNo,
                  int roomNoNeigbourOne,
                  int roomNoNeigbourTwo,
                  int roomNoNeigbourThree);

void populateRoomsRandom(
        QVector<Room *> &rooms, int countOfPits, int countOfBats);

void setPositionOfRooms(QVector<Room *> &rooms);

Room *randomRoom(const QVector<Room *> &rooms);

#endif // ROOMUTILITY_H
