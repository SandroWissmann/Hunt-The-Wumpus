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

#include "dungeon.h"

#include "dungeonview.h"

#include "room.h"

#include <QGuiApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QRandomGenerator>
#include <QScreen>
#include <QVBoxLayout>

#include <algorithm>

Dungeon::Dungeon(QWidget *parent)
    : QWidget(parent),
      mGraphicsScene{ new QGraphicsScene },
    mDungeonView{ new DungeonView }
{
    connectRoomsAsDodekaeder();
    setPositionOfRooms();
    addRoomsToScene();
    addLinesToScene();
    connectToRooms();
    populateRooms();

    mDungeonView->setScene(mGraphicsScene);
    mDungeonView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mDungeonView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(mDungeonView, &DungeonView::shootHitWumpus,
            this, &Dungeon::playerKilledWumpus);
    connect(mDungeonView, &DungeonView::shootMissedWumpus,
            this, &Dungeon::moveWumpusIfWakeUp);
    connect(mDungeonView, &DungeonView::shootHitWumpus,
            this, &Dungeon::decreaseArrows);
    connect(mDungeonView, &DungeonView::shootMissedWumpus,
            this, &Dungeon::decreaseArrows);
    connect(mDungeonView, &DungeonView::shootHitWumpus,
            this, &Dungeon::arrowShot);
    connect(mDungeonView, &DungeonView::shootMissedWumpus,
            this, &Dungeon::arrowShot);


    auto layout = new QVBoxLayout;
    layout->addWidget(mDungeonView);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);

    scaleViewToSize();
    setFixedSize(size());
}

void Dungeon::showHazards(bool value)
{
    for(auto &room : mRooms) {
        room.showContent(value);
    }
}

void Dungeon::enter()
{
    for (;;) {
        auto idx = static_cast<std::size_t>(QRandomGenerator::global()->bounded(
                    0, static_cast<int>(mRooms.size()) - 1));

        if(mRooms[idx].isEmpty()) {
            mRooms[idx].enter();
            break;
        }
    }
}

void Dungeon::reset()
{
    emptyRooms();
    hideDungeon();
    mRemainingArrows = mCountOfArrows;

    populateRooms();
}

int Dungeon::remainingArrows() const
{
    return mRemainingArrows;
}

void Dungeon::moveWumpusIfWakeUp()
{
    auto direction = QRandomGenerator::global()->bounded(0, 3);

    if (direction == 3) {   // 25% chance that wumpus won't move
        return;
    }

    auto wumpusRoomIt{ std::find_if(mRooms.begin(), mRooms.end(),
                        [](Room &room) { return room.hasWumpus(); }) };

    (*wumpusRoomIt).setWumpus(false);
    auto newRoom = (*wumpusRoomIt).neighbours()[direction];
    newRoom->setWumpus(true);

    auto playerRoomIt{ std::find_if(mRooms.begin(), mRooms.end(),
                        [](Room &room) { return room.hasPlayer(); }) };

    (*playerRoomIt).emitNeigbourHazards();
}

void Dungeon::showLinesToNeigboursOfRoom()
{
    auto room = qobject_cast<Room *>(sender());
    auto startPoint = room->mapToScene(room->rect().center());
    auto neighbours = room->neighbours();

    for (const auto &neighbour : neighbours) {
        auto endPoint = neighbour->mapToScene(neighbour->rect().center());
        QLineF line{ startPoint, endPoint };

        auto lineItem = findLineInScene(line);
        if (lineItem) {
            lineItem->show();
        }
    }
}

void Dungeon::decreaseArrows()
{
    --mRemainingArrows;
    if (mRemainingArrows == 0) {
        emit playerIsOutOfArrows();
    }
}

void Dungeon::connectRoomsAsDodekaeder()
{
    auto makeNeighbours = [this](std::size_t src, std::array<std::size_t, 3> n){
        for (const auto i: n) {
            this->mRooms[src].addNeighbour(&(this->mRooms[i]));
        }
    };

    makeNeighbours(0, {1, 4, 19});
    makeNeighbours(1, {0 , 2, 17});
    makeNeighbours(2, {1 , 3, 15});
    makeNeighbours(3, {2 , 4, 13});
    makeNeighbours(4, {0 , 3, 5});
    makeNeighbours(5, {4 , 6, 12});
    makeNeighbours(6, {5 , 7, 19});
    makeNeighbours(7, {6 , 8, 11});
    makeNeighbours(8, {7 , 9, 18});
    makeNeighbours(9, {8 , 10, 16});
    makeNeighbours(10, {9 , 11, 14});
    makeNeighbours(11, {7 , 10, 12});
    makeNeighbours(12, {5 , 11, 13});
    makeNeighbours(13, {3 , 12, 14});
    makeNeighbours(14, {10 , 13, 15});
    makeNeighbours(15, {2 , 14, 16});
    makeNeighbours(16, {9 , 15, 17});
    makeNeighbours(17, {1 , 16, 18});
    makeNeighbours(18, {8 , 17, 19});
    makeNeighbours(19, {0 , 6, 18});
}

void Dungeon::setPositionOfRooms()
{
    const auto roomWidth = static_cast<int>(mRooms[0].size().width());
    const auto roomHeight = static_cast<int>(mRooms[0].size().height());

    std::array<QPoint, mCountOfRooms> points
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

    Q_ASSERT(points.size() == mRooms.size());

    std::size_t i{0};
    for(auto &room : mRooms) {
        room.setPos(points[i++]);
    }
}

void Dungeon::addRoomsToScene()
{
    for(auto &room : mRooms) {
        mGraphicsScene->addItem(&room);
    }
}

void Dungeon::addLinesToScene()
{
    for (const auto& room : mRooms) {
        addLineToNeigbours(&room);
    }
}

void Dungeon::connectToRooms()
{
    for(const auto &room : mRooms) {
        connect(&room, &Room::entered,
                this, &Dungeon::showLinesToNeigboursOfRoom);
        connect(&room, &Room::entered,
                this, &Dungeon::enteredRoom);

        connect(&room, &Room::wumpusNear,
                this, &Dungeon::wumpusNear);
        connect(&room, &Room::batNear,
                this, &Dungeon::batNear);
        connect(&room, &Room::pitNear,
                this, &Dungeon::pitNear);

        connect(&room, &Room::playerDiedFromWumpus,
                this, &Dungeon::playerDiedFromWumpus);
        connect(&room, &Room::playerDiedFromPit,
                this, &Dungeon::playerDiedFromPit);
        connect(&room, &Room::playerDraggedByBat,
                this, &Dungeon::playerDraggedByBat);
        connect(&room, &Room::playerDraggedByBat,
                this, &Dungeon::enter);
    }
}

void Dungeon::populateRooms()
{
    // create a temporary array of room pointers. We cannot shuffle directly
    // the rooms because they are connected with the neighbours already
    std::array<Room *, mCountOfRooms> mixer;
    std::size_t i{ 0 };
    for (auto &r : mRooms) {
        mixer[i++] = &r;
    }

    std::random_shuffle(mixer.begin(), mixer.end());
    auto it{ mixer.begin() };

    (*it++)->setWumpus(true);

    for (int bats{ mCountOfBats }; bats; --bats) {
        (*it++)->setBat(true);
    }
    for (int pits{ mCountOfPits }; pits; --pits) {
        (*it++)->setPit(true);
    }
}

void Dungeon::scaleViewToSize()
{
    mDungeonView->fitInView(mDungeonView->scene()->sceneRect(),
                             Qt::KeepAspectRatio);
}

void Dungeon::emptyRooms()
{
    for(auto &room : mRooms) {
        room.clear();
    }
}

void Dungeon::hideDungeon()
{
    auto items = mGraphicsScene->items();

    for (auto &item : items) {
        item->hide();
    }
}



void Dungeon::addLineToNeigbours(const Room *room)
{
    auto startPoint = room->mapToScene(room->rect().center());
    constexpr auto lineWidth = 10;
    QPen pen;
    pen.setBrush(Qt::black);
    pen.setWidth(lineWidth);

    auto neighbours = room->neighbours();

    for (const auto &neighbour : neighbours) {

        auto endPoint = neighbour->mapToScene(neighbour->rect().center());
        QLineF line{ startPoint, endPoint };

        if (lineExistsInScene(line)) {
            continue;
        }

        auto lineItem = mGraphicsScene->addLine(line, pen);
        lineItem->setZValue(-1);
        lineItem->hide();
    }

    mGraphicsScene->update();
}

bool Dungeon::lineExistsInScene(const QLineF &line)
{
    return findLineInScene(line) != nullptr;
}

QGraphicsLineItem *Dungeon::findLineInScene(const QLineF &line)
{
    auto items = mGraphicsScene->items();

    for(const auto &item : items) {
        auto existingLineItem = qgraphicsitem_cast<QGraphicsLineItem *>(item);

        if(existingLineItem) {
            auto p1 = existingLineItem->line().p1();
            auto p2 = existingLineItem->line().p2();

            auto newP1 = line.p1();
            auto newP2 = line.p2();

            if((p1 == newP1 && p2 == newP2) || (p1 == newP2 && p2 == newP1)) {
                return existingLineItem;
            }
        }
    }
    return nullptr;
}

