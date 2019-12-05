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

#include "testroom.h"
#include "room.h"

#include <QTest>
#include <QSignalSpy>

void TestRoom::room()
{
    Room room;
    QCOMPARE(room.hasWumpus(), false);
    QCOMPARE(room.hasPit(), false);
    QCOMPARE(room.hasBat(), false);
    QCOMPARE(room.hasPlayer(), false);
    QCOMPARE(room.playerWasHere(), false);
    QCOMPARE(room.neighbours().empty(), true);
}

void TestRoom::setAndHasPit_data()
{
    QTest::addColumn<bool>("value");
    QTest::newRow("has pit") << true;
    QTest::newRow("no pit") << false;
}

void TestRoom::setAndHasPit()
{
    QFETCH(bool, value);
    Room room;
    QCOMPARE(room.hasPit(), false);
    room.setPit(value);
    QCOMPARE(room.hasPit(), value);
}

void TestRoom::setAndHasBat_data()
{
    QTest::addColumn<bool>("value");
    QTest::newRow("has bat") << true;
    QTest::newRow("no bat") << false;
}

void TestRoom::setAndHasBat()
{
    QFETCH(bool, value);
    Room room;
    QCOMPARE(room.hasBat(), false);
    room.setBat(value);
    QCOMPARE(room.hasBat(), value);
}

void TestRoom::setAndHasPlayer_data()
{
    QTest::addColumn<bool>("value");
    QTest::newRow("has player") << true;
    QTest::newRow("no player") << false;
}

void TestRoom::setAndHasPlayer()
{
    QFETCH(bool, value);
    Room room;
    QCOMPARE(room.hasPlayer(), false);
    room.setPlayer(value);
    QCOMPARE(room.hasPlayer(), value);
}

void TestRoom::setAndPlayerWasHere_data()
{
    QTest::addColumn<bool>("value");
    QTest::newRow("player was here") << true;
    QTest::newRow("no player was here") << false;
}

void TestRoom::setAndPlayerWasHere()
{
    QFETCH(bool, value);
    Room room;
    QCOMPARE(room.playerWasHere(), false);
    room.setPlayerWasHere(value);
    QCOMPARE(room.playerWasHere(), value);
}

void TestRoom::addAndNeighbours()
{
    Room room;
    Room neighbour1;
    Room neighbour2;
    Room neighbour3;
    QCOMPARE(room.neighbours().size(), 0);
    room.addNeighbour(&neighbour1);
    QCOMPARE(room.neighbours().size(), 1);
    QCOMPARE(room.neighbours()[0], &neighbour1);
    room.addNeighbour(&neighbour2);
    QCOMPARE(room.neighbours().size(), 2);
    QCOMPARE(room.neighbours()[0], &neighbour1);
    QCOMPARE(room.neighbours()[1], &neighbour2);
    room.addNeighbour(&neighbour3);
    QCOMPARE(room.neighbours().size(), 3);
    QCOMPARE(room.neighbours()[0], &neighbour1);
    QCOMPARE(room.neighbours()[1], &neighbour2);
    QCOMPARE(room.neighbours()[2], &neighbour3);
}


void TestRoom::signal_enterRoom_data()
{
    QTest::addColumn<bool>("hasWumpus");
    QTest::addColumn<bool>("hasPit");
    QTest::addColumn<bool>("hasBat");
    QTest::addColumn<int>("sigWumpus");
    QTest::addColumn<int>("sigPit");
    QTest::addColumn<int>("sigBat");
    QTest::newRow("wumpus") << true << false << false << 1 << 0 << 0;
    QTest::newRow("pit") << false << true << false << 0 << 1 << 0;
    QTest::newRow("bat") << false << false << true << 0 << 0 << 1;
    QTest::newRow("wumpus and pit") << true << true << false << 1 << 0 << 0;
    QTest::newRow("wumpus and bat") << true << false << true << 1 << 0 << 0;
    QTest::newRow("pit and bat") << false << true << true << 0 << 1 << 0;
    QTest::newRow("wumpus bat pit") << true << true << true << 1 << 0 << 0;

}

void TestRoom::signal_enterRoom()
{
    QFETCH(bool, hasWumpus);
    QFETCH(bool, hasPit);
    QFETCH(bool, hasBat);
    QFETCH(int, sigWumpus);
    QFETCH(int, sigPit);
    QFETCH(int, sigBat);

    Room room;

    room.setWumpus(hasWumpus);
    room.setPit(hasPit);
    room.setBat(hasBat);

    QSignalSpy spyPlayerDiedFromWumpus(&room, &Room::playerDiedFromWumpus);
    QSignalSpy spyPlayerDiedFromPit(&room, &Room::playerDiedFromPit);
    QSignalSpy spyPlayerDraggedByBat(&room, &Room::playerDraggedByBat);

    room.enter();

    QCOMPARE(spyPlayerDiedFromWumpus.count(), sigWumpus);
    QCOMPARE(spyPlayerDiedFromPit.count(), sigPit);
    QCOMPARE(spyPlayerDraggedByBat.count(), sigBat);
}

void TestRoom::signal_enterRoom2_data()
{
    QTest::addColumn<bool>("neighbour1hasWumpus");
    QTest::addColumn<bool>("neighbour1hasPit");
    QTest::addColumn<bool>("neighbour1hasBat");
    QTest::addColumn<bool>("neighbour2hasWumpus");
    QTest::addColumn<bool>("neighbour2hasPit");
    QTest::addColumn<bool>("neighbour2hasBat");
    QTest::addColumn<bool>("neighbour3hasWumpus");
    QTest::addColumn<bool>("neighbour3hasPit");
    QTest::addColumn<bool>("neighbour3hasBat");
    QTest::addColumn<int>("sigWumpusNear");
    QTest::addColumn<int>("sigPitNear");
    QTest::addColumn<int>("sigBatNear");
    QTest::newRow("1 wumpus ")
            << true << false << false
            << false << false << false
            << false << false << false
            << 1 << 0 << 0;
    QTest::newRow("1 pit ")
            << false << true << false
            << false << false << false
            << false << false << false
            << 0 << 1 << 0;
    QTest::newRow("1 bat ")
            << false << false << true
            << false << false << false
            << false << false << false
            << 0 << 0 << 1;
    QTest::newRow("2 wumpus ")
            << false << false << false
            << true << false << false
            << false << false << false
            << 1 << 0 << 0;
    QTest::newRow("2 pit ")
            << false << false << false
            << false << true << false
            << false << false << false
            << 0 << 1 << 0;
    QTest::newRow("2 bat ")
            << false << false << false
            << false << false << true
            << false << false << false
            << 0 << 0 << 1;
    QTest::newRow("3 wumpus ")
            << false << false << false
            << false << false << false
            << true << false << false
            << 1 << 0 << 0;
    QTest::newRow("3 pit ")
            << false << false << false
            << false << false << false
            << false << true << false
            << 0 << 1 << 0;
    QTest::newRow("3 bat ")
            << false << false << false
            << false << false << false
            << false << false << true
            << 0 << 0 << 1;
    QTest::newRow("1 wumpus 2 wumpus")
            << true << false << false
            << true << false << false
            << false << false << false
            << 1 << 0 << 0;
    QTest::newRow("1 pit 2 pit")
            << false << true << false
            << false << true << false
            << false << false << false
            << 0 << 1 << 0;
    QTest::newRow("1 bat 2 bat")
            << false << false << true
            << false << false << true
            << false << false << false
            << 0 << 0 << 1;
}

void TestRoom::signal_enterRoom2()
{
    QFETCH(bool, neighbour1hasWumpus);
    QFETCH(bool, neighbour1hasPit);
    QFETCH(bool, neighbour1hasBat);
    QFETCH(bool, neighbour2hasWumpus);
    QFETCH(bool, neighbour2hasPit);
    QFETCH(bool, neighbour2hasBat);
    QFETCH(bool, neighbour3hasWumpus);
    QFETCH(bool, neighbour3hasPit);
    QFETCH(bool, neighbour3hasBat);
    QFETCH(int, sigWumpusNear);
    QFETCH(int, sigPitNear);
    QFETCH(int, sigBatNear);

    Room room;
    Room neighbour1;
    Room neighbour2;
    Room neighbour3;

    neighbour1.setWumpus(neighbour1hasWumpus);
    neighbour1.setPit(neighbour1hasPit);
    neighbour1.setBat(neighbour1hasBat);

    neighbour2.setWumpus(neighbour2hasWumpus);
    neighbour2.setPit(neighbour2hasPit);
    neighbour2.setBat(neighbour2hasBat);

    neighbour3.setWumpus(neighbour3hasWumpus);
    neighbour3.setPit(neighbour3hasPit);
    neighbour3.setBat(neighbour3hasBat);

    room.addNeighbour(&neighbour1);
    room.addNeighbour(&neighbour2);
    room.addNeighbour(&neighbour3);

    QSignalSpy spyWumpusNear(&room, &Room::wumpusNear);
    QSignalSpy spyPitNear(&room, &Room::pitNear);
    QSignalSpy spyBatNear(&room, &Room::batNear);

    room.enter();

    QCOMPARE(spyWumpusNear.count(), sigWumpusNear);
    QCOMPARE(spyPitNear.count(), sigPitNear);
    QCOMPARE(spyBatNear.count(), sigBatNear);
}

QTEST_MAIN(TestRoom)
