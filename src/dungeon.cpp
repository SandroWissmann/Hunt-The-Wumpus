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
#include "roomutility.h"

#include <QGuiApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QRandomGenerator>
#include <QScreen>
#include <QVBoxLayout>

#include <QDebug>

#include <algorithm>

Dungeon::Dungeon(QWidget *parent)
    : QWidget(parent),
      mGraphicsScene{ new QGraphicsScene },
    mDungeonView{ new DungeonView }
{
    createRooms();
    connectRoomsAsDodekaeder(mRooms);
    setPositionOfRooms(mRooms);
    addRoomsToScene();
    addLinesToScene();
    connectToRooms();

    populateRoomsRandom(mRooms, mCountOfPits, mCountOfBats);

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
        room->showContent(value);
    }
}

void Dungeon::enter()
{
    for (;;) {
        auto idx = QRandomGenerator::global()->bounded(0, mRooms.size() - 1);

        if(mRooms[idx]->isEmpty()) {
            mRooms[idx]->enter();
            break;
        }
    }
}

void Dungeon::reset()
{
    emptyRooms();
    hideDungeon();
    mRemainingArrows = mCountOfArrows;

    populateRoomsRandom(mRooms, mCountOfPits, mCountOfBats);
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
                        [](Room *room) { return room->hasWumpus(); }) };

    (*wumpusRoomIt)->setWumpus(false);
    auto newRoom = (*wumpusRoomIt)->neighbours()[direction];
    newRoom->setWumpus(true);

    auto playerRoomIt{ std::find_if(mRooms.begin(), mRooms.end(),
                        [](Room *room) { return room->hasPlayer(); }) };

    (*playerRoomIt)->emitNeigbourHazards();
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

void Dungeon::scaleViewToSize()
{
    mDungeonView->fitInView(mDungeonView->scene()->sceneRect(),
                             Qt::KeepAspectRatio);
}

void Dungeon::emptyRooms()
{
    for(auto room : mRooms) {
        room->clear();
    }
}

void Dungeon::hideDungeon()
{
    auto items = mGraphicsScene->items();

    for (auto &item : items) {
        item->hide();
    }
}

void Dungeon::createRooms()
{
    mRooms.reserve(mCountOfRooms);

    for(int i = 0; i< mCountOfRooms; ++i) {
        mRooms.push_back(new Room);
    }
}

void Dungeon::connectToRooms()
{
    for(const auto &room : mRooms) {
        connect(room, &Room::entered,
                this, &Dungeon::showLinesToNeigboursOfRoom);
        connect(room, &Room::entered,
                this, &Dungeon::enteredRoom);

        connect(room, &Room::wumpusNear,
                this, &Dungeon::wumpusNear);
        connect(room, &Room::batNear,
                this, &Dungeon::batNear);
        connect(room, &Room::pitNear,
                this, &Dungeon::pitNear);

        connect(room, &Room::playerDiedFromWumpus,
                this, &Dungeon::playerDiedFromWumpus);
        connect(room, &Room::playerDiedFromPit,
                this, &Dungeon::playerDiedFromPit);
        connect(room, &Room::playerDraggedByBat,
                this, &Dungeon::playerDraggedByBat);
        connect(room, &Room::playerDraggedByBat,
                this, &Dungeon::enter);
    }
}

void Dungeon::addRoomsToScene()
{
    for(const auto &room : mRooms) {
        mGraphicsScene->addItem(room);
    }
}

void Dungeon::addLinesToScene()
{
    for (const auto& room : mRooms) {
        addLineToNeigbours(room);
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

