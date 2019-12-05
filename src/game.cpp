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

#include "game.h"

#include "dungeon.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontMetrics>

#include <QDebug>

Game::Game(QWidget *parent)
    : QWidget(parent),
      mAnnouncement{},
      mAnnouncementLabel{ new QLabel },
      mArrowsLabel{ new QLabel },
      mDungeon{ new Dungeon }
{
    mAnnouncementLabel->setAlignment(Qt::AlignLeft);
    mAnnouncementLabel->setStyleSheet(
                "color: red; Font : 20pt; border: 1px solid black");

    mArrowsLabel->setAlignment(Qt::AlignRight);
    mArrowsLabel->setStyleSheet("Font : 20pt; border: 1px solid black");

    connect(mDungeon, &Dungeon::enteredRoom,
            this, &Game::showAnnouncement);

    connect(mDungeon, &Dungeon::wumpusNear,
            this, &Game::addWumpusNearText);
    connect(mDungeon, &Dungeon::batNear,
            this, &Game::addBatNearText);
    connect(mDungeon, &Dungeon::pitNear,
            this, &Game::addPitNearText);

    connect(mDungeon, &Dungeon::playerDraggedByBat,
            this, &Game::addDraggedByBatText);

    connect(mDungeon, &Dungeon::arrowShot,
            this, &Game::updateArrowsLabel);

    connect(mDungeon, &Dungeon::playerDiedFromWumpus,
            this, &Game::playerDiedFromWumpus);
    connect(mDungeon, &Dungeon::playerDiedFromPit,
            this, &Game::playerDiedFromPit);
    connect(mDungeon, &Dungeon::playerIsOutOfArrows,
            this, &Game::playerIsOutOfArrows);

    connect(mDungeon, &Dungeon::playerKilledWumpus,
            this, &Game::playerKilledWumpus);

    auto vLayout = new QVBoxLayout;
    vLayout->addWidget(mArrowsLabel);
    vLayout->addWidget(mAnnouncementLabel);
    vLayout->addWidget(mDungeon);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0,0,0,0);
    setLayout(vLayout);

    QFontMetrics fm{ mAnnouncementLabel->font() };
    mAnnouncementLabel->setFixedHeight(fm.lineSpacing() * 4);

    updateArrowsLabel();
}

void Game::start()
{
    mDungeon->enter();
}

void Game::showDungeonHazards(bool value)
{
    mDungeon->showHazards(value);
}

void Game::reset()
{
    mAnnouncement.clear();
    mDungeon->reset();
    updateArrowsLabel();
}

void Game::showAnnouncement()
{
    mAnnouncementLabel->setText(mAnnouncement);
    mAnnouncement.clear();
}

void Game::addWumpusNearText()
{
    addTextToAnnouncement(tr("I smell the wumpus"));
}

void Game::addPitNearText()
{
    addTextToAnnouncement(tr("I feel a breeze"));
}

void Game::addBatNearText()
{
    addTextToAnnouncement(tr("I hear a bat"));
}

void Game::addDraggedByBatText()
{
    addTextToAnnouncement(tr(
        "Gigantic bat appeared and dragged you to a new room "));
}

void Game::updateArrowsLabel()
{
    mArrowsLabel->setText(tr("Arrows: %1").
                          arg(mDungeon->remainingArrows()));
}

void Game::addTextToAnnouncement(const QString &text)
{
    if (!mAnnouncement.isEmpty()) {
        mAnnouncement.append("\n");
    }
    mAnnouncement.append(text);
}

