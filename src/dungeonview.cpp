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

#include "dungeonview.h"

#include "room.h"

#include <QMouseEvent>

#include <algorithm>

DungeonView::DungeonView(QWidget *parent)
    :QGraphicsView{ parent }
{
    setMouseTracking(true);
}

void DungeonView::mousePressEvent(QMouseEvent *event)
{
    if (auto room = qgraphicsitem_cast<Room *>(itemAt(event->pos()));
            room && room->hasPlayer()) {

        if (event->button() == Qt::RightButton) {           
            startShootArrowMode(room);
        }
        else if (event->button() == Qt::LeftButton) {
            startDragPlayerMode(event);
        }
    }
}

void DungeonView::mouseReleaseEvent(QMouseEvent *event)
{
    if (mShootArrowSelectOn && event->button() == Qt::RightButton) {
        stopShootArrowMode(event);
    }
    else if (auto room = qgraphicsitem_cast<Room *>(itemAt(event->pos())) ;
             room && room->hasPlayer() &&
             event->button() == Qt::LeftButton) {       
        stopDragPlayerMode(event);
    }
}

void DungeonView::mouseMoveEvent(QMouseEvent *event)
{
    auto room = qgraphicsitem_cast<Room *>(itemAt(event->pos()));
    if(room && !mShootArrowSelectOn) {
        QGraphicsView::mouseMoveEvent(event);
    }

    if(room == mlastRoom) {
        return;
    }
    if(!mShootArrowSelectOn && leftRoom(room, mlastRoom)) {
        setCursor(Qt::ArrowCursor);
    }
    else if (mShootArrowSelectOn && !maxArrowRangeReached() &&
             !isMarked(room) && isNeigbourOfLastMarkedRoom(room)) {        
        room->setTarget(true);
        mMarkedRooms.push_back(room);
    }
    else if (enteredRoom(room, mlastRoom) && room->hasPlayer()) {
        setCursor(Qt::OpenHandCursor);
    }
    mlastRoom = room;
}

void DungeonView::startShootArrowMode(Room *room)
{
    mShootArrowSelectOn = true;

    room->setTarget(true);
    mlastRoom = room;
    mMarkedRooms.push_back(room);

    setCursor(Qt::CrossCursor);
}

void DungeonView::stopShootArrowMode(QMouseEvent *event)
{
    for (auto &room : mMarkedRooms) {
        room->setTarget(false);
    }

    if (mMarkedRooms.size() > 1) {
        shootArrow(event);
    }

    mMarkedRooms.clear();
    mlastRoom = nullptr;
    mShootArrowSelectOn = false;

    setCursor(Qt::ArrowCursor);
}

void DungeonView::shootArrow(QMouseEvent *event)
{
    auto room = qgraphicsitem_cast<Room *>(itemAt(event->pos()));

    if(room && room == mMarkedRooms.back()) {
        auto hitWumpus = false;

        for (auto i = 1; i < mMarkedRooms.size(); ++i) {
            if(room->hasWumpus()) {
                emit shootHitWumpus();
                hitWumpus = true;
                break;
            }
        }
        if (!hitWumpus) {
            emit shootMissedWumpus();
        }
    }
}

void DungeonView::startDragPlayerMode(QMouseEvent *event)
{
    setCursor(Qt::ClosedHandCursor);
    QGraphicsView::mousePressEvent(event);
}

void DungeonView::stopDragPlayerMode(QMouseEvent *event)
{
    setCursor(Qt::OpenHandCursor);
    QGraphicsView::mousePressEvent(event);
}

bool DungeonView::maxArrowRangeReached() const
{
    return mMarkedRooms.size() > mArrowRoomRange;
}

bool DungeonView::isMarked(Room *room) const
{
    if(mMarkedRooms.empty()) {
        return false;
    }

    return std::find(mMarkedRooms.begin(),
                     mMarkedRooms.end(), room) != mMarkedRooms.end();
}

bool DungeonView::isNeigbourOfLastMarkedRoom(Room *room) const
{
    if(mMarkedRooms.empty()) {
        return false;
    }

    auto lastRoomNeigbours = mMarkedRooms.back()->neighbours();

    return std::find(lastRoomNeigbours.begin(),
                     lastRoomNeigbours.end(), room) != lastRoomNeigbours.end();
}

bool DungeonView::leftRoom(Room *current, Room *last)
{
    return (current == nullptr && last != nullptr);
}

bool DungeonView::enteredRoom(Room *current, Room *last)
{
    return (current != nullptr && last == nullptr);
}
