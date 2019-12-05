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

#include "room.h"

#include "dragplayermimedata.h"

#include <QApplication>
#include <QAction>
#include <QCursor>
#include <QDrag>
#include <QMimeData>
#include <QGraphicsWidget>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QMenuBar>
#include <QMouseEvent>
#include <QPainter>

Room::Room(QGraphicsItem *parent)
    : QGraphicsWidget(parent)
{     
    Q_ASSERT(roomImage().size() == roomVisitedImage().size());
    Q_ASSERT(roomVisitedImage().size() == roomTargetImage().size());
    Q_ASSERT(roomImage().height() ==
             pitImage().height() + batImage().height());
    Q_ASSERT(roomImage().height() ==
             pitImage().height() + playerImage().height());   
    Q_ASSERT(roomImage().height() ==
             pitImage().height() + wumpusImage().height());
    Q_ASSERT(roomImage().width() ==
             batImage().width() + playerImage().width() +
             wumpusImage().width());
    Q_ASSERT(batImage().size() == batConfirmedImage().size());
    Q_ASSERT(pitImage().size() == pitConfirmedImage().size());
    Q_ASSERT(wumpusImage().size() == wumpusConfirmedImage().size());

    setAcceptDrops(true);

    mGuessWumpusAction = new QAction{tr("Has &Wumpus")};
    mGuessWumpusAction->setCheckable(true);
    connect(mGuessWumpusAction, &QAction::triggered,
            this, &Room::toggleGuessWumpus);

    mGuessBatAction = new QAction{tr("Has &Bat")};
    mGuessBatAction->setCheckable(true);
    connect(mGuessBatAction, &QAction::triggered, this, &Room::toggleGuessBat);

    mGuessPitAction = new QAction{tr("Has &Pit")};
    mGuessPitAction->setCheckable(true);
    connect(mGuessPitAction, &QAction::triggered, this, &Room::toggleGuessPit);

    addAction(mGuessWumpusAction);
    addAction(mGuessBatAction);
    addAction(mGuessPitAction);

    resize(boundingRect().size());

    setVisible(false);
}

void Room::emitNeigbourHazards()
{
    auto batEmitted = false;
    auto wumpusEmitted = false;
    auto pitEmitted = false;
    for(const auto& neigbour : mNeighbours) {
        if(neigbour == nullptr) {
            continue;
        }

        if (!batEmitted && neigbour->hasBat()) {
            emit batNear();
            batEmitted = true;
        }
        if (!wumpusEmitted && neigbour->hasWumpus()) {
            emit wumpusNear();
            wumpusEmitted = true;
        }
        if (!pitEmitted && neigbour->hasPit()) {
            emit pitNear();
            pitEmitted = true;
        }
    }
}

bool Room::isEmpty() const
{
    return !hasPlayer() && !hasBat()&& !hasPit() && !hasWumpus();
}

void Room::setWumpus(bool value)
{
    if(value && hasPlayer()) {
        emit playerDiedFromWumpus();
    }

    mHasWumpus = value;
    update();
}

bool Room::hasWumpus() const
{
    return mHasWumpus;
}

void Room::setPit(bool value)
{
    mHasPit = value;
    update();
}

bool Room::hasPit() const
{
    return mHasPit;
}

void Room::setBat(bool value)
{
    mHasBat = value;
    update();
}

bool Room::hasBat() const
{
    return mHasBat;
}

void Room::setPlayer(bool value)
{
    mHasPlayer = value;

    if (value) {
        mPlayerWasHere = true;
    }
    update();
}

bool Room::hasPlayer() const
{
    return mHasPlayer;
}

void Room::setPlayerWasHere(bool value)
{
    mPlayerWasHere = value;
    update();
}

bool Room::playerWasHere() const
{
    return mPlayerWasHere;
}

void Room::setTarget(bool value)
{
    mIsTarget = value;
    update();
}

bool Room::isTarget() const
{
    return mIsTarget;
}

void Room::addNeighbour(Room *neighbour)
{  
    mNeighbours.push_back(neighbour);
}

QVector<Room *> Room::neighbours() const
{
    return mNeighbours;
}

void Room::showContent(bool value)
{
    mShowContent = value;
    update();
}

void Room::clear()
{
    mBatConfirmed = false;

    mHasWumpus = false;
    mHasPit = false;
    mHasBat = false;
    mHasPlayer = false;
    mPlayerWasHere = false;

    mGuessWumpus = false;
    mGuessBat = false;
    mGuessPit = false;

    mShowContent = false;

    mIsTarget = false;
}

QRectF Room::boundingRect() const
{
    return QRectF{ 0, 0,
        static_cast<qreal>(roomImage().width()),
        static_cast<qreal>(roomImage().height())};
}

void Room::paint(
        QPainter *painter,
        const QStyleOptionGraphicsItem *option,
        QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)


    if (isTarget()) {
        drawRoom(painter, roomTargetImage());
    }
    else if (playerWasHere()) {
        drawRoom(painter, roomVisitedImage());
    }
    else {
        drawRoom(painter, roomImage());
    }

    if (hasPlayer()) {
        drawPlayer(painter);
    }

    if (mShowContent) {
        if (hasWumpus()) {
            drawWumpus(painter, wumpusConfirmedImage());
        }
        if (hasBat()) {
            drawBat(painter, batConfirmedImage());
        }
        if (hasPit()) {
            drawPit(painter, pitConfirmedImage());
        }
    }
    else {
        if (mGuessWumpus) {
            drawWumpus(painter, wumpusImage());
        }
        if (mBatConfirmed) {
            drawBat(painter, batConfirmedImage());
        }
        else if (mGuessBat) {
            drawBat(painter, batImage());
        }
        if (mGuessPit) {
            drawPit(painter, pitImage());
        }
    }
}

int Room::type() const
{
    return Type;
}

void Room::enter()
{   
    if (mHasWumpus) {
        emit playerDiedFromWumpus();
        return;
    }
    if (mHasPit) {
        emit playerDiedFromPit();
        return;
    }
    if (mHasBat) {
        emit playerDraggedByBat();
        mBatConfirmed = true;
        mGuessBatAction->setChecked(true);
        mGuessBatAction->setEnabled(false);
        return;
    }

    setVisible(true);
    for(auto &neighbour : mNeighbours) {
        neighbour->setVisible(true);
        neighbour->update();
    }

    emitNeigbourHazards();
    emit entered();
    setPlayer(true);
}

void Room::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (hasPlayer()) {
        return;
    }

    QMenu menu;
    menu.addAction(mGuessWumpusAction);
    menu.addAction(mGuessBatAction);
    menu.addAction(mGuessPitAction);
    menu.exec(event->screenPos());
}

void Room::mousePressEvent(QGraphicsSceneMouseEvent *event)
// needs to be overriden to toggle mouseMoveEvent
{
    Q_UNUSED(event)
}

void Room::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!hasPlayer()) {
        return;
    }

    if (minDistanceReached(event, Qt::LeftButton)) {
        executePlayerDrag(event);
    }
}

void Room::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
// needs to be overriden to toggle mouseMoveEvent
{
    Q_UNUSED(event)
}

void Room::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    const auto dragPlayerData =
            qobject_cast<const DragPlayerMimeData *>(event->mimeData());

    if(dragPlayerData && isNeighbour(dragPlayerData->room()) &&
            dragPlayerData->imageData() == playerDraggedImage()) {
        event->setAccepted(true);
    }
    else {
        event->setAccepted(false);
    }
}

void Room::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (isPlayerDrag(event)) {
        setPlayer(false);
        update();
    }
}

void Room::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    const auto roomData =
            qobject_cast<const DragPlayerMimeData *>(event->mimeData());

    if(roomData && isNeighbour(roomData->room()) &&
            roomData->imageData() == playerDraggedImage()) {
        enter();
        update();
        event->setAccepted(true);
    }
    else {
        event->setAccepted(false);
    }
}

void Room::toggleGuessWumpus()
{
    if(mGuessWumpus) {
        mGuessWumpus = false;
        mGuessWumpusAction->setChecked(false);
    }
    else {
        mGuessWumpus = true;
        mGuessWumpusAction->setChecked(true);
    }
    update();
}

void Room::toggleGuessPit()
{
    if(mGuessPit) {
        mGuessPit = false;
        mGuessPitAction->setChecked(false);
    }
    else {
        mGuessPit = true;
        mGuessPitAction->setChecked(true);
    }
    update();
}

void Room::toggleGuessBat()
{
    if(mGuessBat) {
        mGuessBat = false;
        mGuessBatAction->setChecked(false);
    }
    else {
        mGuessBat = true;
        mGuessBatAction->setChecked(true);
    }
    update();
}

bool Room::isNeighbour(Room *room) const
{
    if (room == nullptr) {
        return false;
    }

    for(const auto &neighbour : mNeighbours) {
        if (room == neighbour) {
            return true;
        }
    }
    return false;
}

bool Room::minDistanceReached(QGraphicsSceneMouseEvent *event,
                              Qt::MouseButton button)
{
    return QLineF(event->screenPos(),
                  event->buttonDownScreenPos(button))
           .length() >= QApplication::startDragDistance();
}

void Room::executePlayerDrag(QGraphicsSceneMouseEvent *event)
{
    setPlayer(false);

    auto drag = new QDrag(event->widget());
    auto mime = new DragPlayerMimeData;
    mime->setRoom(this);
    drag->setMimeData(mime);

    mime->setImageData(playerDraggedImage());

    drag->setPixmap(QPixmap::fromImage(playerDraggedImage()));
    drag->setHotSpot(QPoint(0, 0));

    auto result = drag->exec();

    if (result == Qt::IgnoreAction) {
        setPlayer(true);
    }
}

bool Room::isPlayerDrag(QGraphicsSceneDragDropEvent *event)
{
    return qobject_cast<const DragPlayerMimeData *>(event->mimeData()) &&
            event->mimeData()->imageData() == playerDraggedImage();
}

void Room::drawRoom(QPainter *painter, const QImage &roomImage)
{
    painter->drawImage(boundingRect(), roomImage);
}

void Room::drawWumpus(QPainter *painter, const QImage &wumpusImage)
{
    auto xOffset = roomImage().width() - wumpusImage.width();
    painter->drawImage(
                QPointF{boundingRect().x() + xOffset,  boundingRect().y()},
                wumpusImage);
}

void Room::drawBat(QPainter *painter, const QImage &batImage)
{
    painter->drawImage(
                QPointF{boundingRect().x(),  boundingRect().y()}, batImage);
}


void Room::drawPit(QPainter *painter, const QImage &pitImage)
{
    auto yOffset = roomImage().height() - pitImage.height();
    painter->drawImage(
                QPointF{boundingRect().x(),  boundingRect().y() + yOffset},
                pitImage);
}

void Room::drawPlayer(QPainter *painter)
{
    auto xOffset = roomImage().width() - wumpusImage().width() -
            playerImage().width();
    painter->drawImage(
                QPointF{boundingRect().x() + xOffset,  boundingRect().y()},
                playerImage());
}

QImage Room::roomImage() const
{
    return QImage{":/ressources/room.png"};
}

QImage Room::roomVisitedImage() const
{
    return QImage{":/ressources/room_visited.png"};
}

QImage Room::roomTargetImage() const
{
    return QImage{":/ressources/room_target.png"};
}

QImage Room::batImage() const
{
    return QImage{":/ressources/bat.png"};
}

QImage Room::batConfirmedImage() const
{
    return QImage{":/ressources/bat_confirmed.png"};
}

QImage Room::pitImage() const
{
    return QImage{":/ressources/pit.png"};
}

QImage Room::pitConfirmedImage() const
{
    return QImage{":/ressources/pit_confirmed.png"};
}

QImage Room::wumpusImage() const
{
    return QImage{":/ressources/wumpus.png"};
}

QImage Room::wumpusConfirmedImage() const
{
    return QImage{":/ressources/wumpus_confirmed.png"};
}

QImage Room::playerImage() const
{
    return QImage{":/ressources/player.png"};
}

QImage Room::playerDraggedImage() const
{
    return QImage{":/ressources/player_dragged.png"};
}

