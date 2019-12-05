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

#ifndef TESTROOM_H
#define TESTROOM_H

#include <QObject>

class TestRoom : public QObject
{
    Q_OBJECT
private slots:
    void room();

    void setAndHasPit_data();
    void setAndHasPit();

    void setAndHasBat_data();
    void setAndHasBat();

    void setAndHasPlayer_data();
    void setAndHasPlayer();

    void setAndPlayerWasHere_data();
    void setAndPlayerWasHere();

    void addAndNeighbours();

    void signal_enterRoom_data();
    void signal_enterRoom();

    void signal_enterRoom2_data();
    void signal_enterRoom2();
};

#endif // TESTROOM_H
