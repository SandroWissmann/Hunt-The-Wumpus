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

#include "mainwindow.h"

#include "startdialog.h"
#include "game.h"
#include "gameoverdialog.h"
#include "windialog.h"

#include <QApplication>
#include <QEvent>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QWidget{ parent },
      mStackedWidget{ new QStackedWidget }
{
    auto startDialog = new StartDialog;
    auto game = new Game;
    auto gameOverDialog = new GameOverDialog;
    auto winDialog = new WinDialog;

    connect(startDialog, &StartDialog::finished,
            this, &MainWindow::startNewGame);

    connect(game, &Game::playerDiedFromWumpus,
            this, &MainWindow::showPlayerDiedFromWumpus);
    connect(game, &Game::playerDiedFromPit,
            this, &MainWindow::showPlayerDiedFromPit);
    connect(game, &Game::playerIsOutOfArrows,
            this, &MainWindow::showPlayerIsOutOfArrows);

    connect(game, &Game::playerKilledWumpus,
            this, &MainWindow::showWinDialog);

    connect(gameOverDialog, &GameOverDialog::accepted,
            this, &MainWindow::startNewGame);
    connect(gameOverDialog, &GameOverDialog::rejected,
            this, &MainWindow::quitProgramm);

    connect(winDialog, &GameOverDialog::accepted,
            this, &MainWindow::startNewGame);
    connect(winDialog, &GameOverDialog::rejected,
            this, &MainWindow::quitProgramm);

    mStackedWidget->addWidget(startDialog);
    mStackedWidget->addWidget(game);
    mStackedWidget->addWidget(gameOverDialog);
    mStackedWidget->addWidget(winDialog);

    auto layout = new QHBoxLayout;
    layout->addWidget(mStackedWidget);
    setLayout(layout);

    setWindowTitle("Hunt the Wumpus");
}

static QString showDungeonHazardsKeyword = "show";
static QString hideDungeonHazardsKeyword = "hide";

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (mStackedWidget->currentIndex() != gameIndex) {
        return;
    }

    auto key = event->text();

    mKeyBuffer.append(key);

    if(mKeyBuffer == showDungeonHazardsKeyword) {      
        showDungeonHazards(true);
        mKeyBuffer.clear();
    }
    else if(mKeyBuffer == hideDungeonHazardsKeyword) {
        showDungeonHazards(false);
        mKeyBuffer.clear();
    }
    else if (!showDungeonHazardsKeyword.startsWith(mKeyBuffer)  &&
            !hideDungeonHazardsKeyword.startsWith(mKeyBuffer)) {
           mKeyBuffer.clear();
    }
}

void MainWindow::startNewGame()
{
    mStackedWidget->setCurrentIndex(gameIndex);
    mKeyBuffer.clear();

    auto gameWidget = game();
    Q_ASSERT(gameWidget != nullptr);

    gameWidget->reset();
    gameWidget->start();
}

void MainWindow::showWinDialog()
{
    mStackedWidget->setCurrentIndex(winDialogIndex);
}

void MainWindow::showPlayerDiedFromWumpus()
{
    mStackedWidget->setCurrentIndex(gameOverDialogIndex);
    auto gameOverDialogWidget = gameOverDialog();
    Q_ASSERT(gameOverDialogWidget != nullptr);

    gameOverDialogWidget->setReason(
                GameOverDialog::Reason::playerDiedFromWumpus);
}

void MainWindow::showPlayerDiedFromPit()
{
    mStackedWidget->setCurrentIndex(gameOverDialogIndex);
    auto gameOverDialogWidget = gameOverDialog();
    Q_ASSERT(gameOverDialogWidget != nullptr);

    gameOverDialogWidget->setReason(
                GameOverDialog::Reason::playerDiedFromPit);
}

void MainWindow::showPlayerIsOutOfArrows()
{
    mStackedWidget->setCurrentIndex(gameOverDialogIndex);
    auto gameOverDialogWidget = gameOverDialog();
    Q_ASSERT(gameOverDialogWidget != nullptr);

    gameOverDialogWidget->setReason(
                GameOverDialog::Reason::playerIsOutOfArrows);
}

void MainWindow::quitProgramm()
{
    QApplication::quit();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    // Hack because resizing in the constructor does not work
    // (then the view is to small)
    setFixedSize(size());
}

void MainWindow::showDungeonHazards(bool value)
{
    auto gameWidget = game();
    Q_ASSERT(gameWidget != nullptr);
    gameWidget->showDungeonHazards(value);
}

Game *MainWindow::game()
{
    return qobject_cast<Game *>(mStackedWidget->widget(gameIndex));
}

GameOverDialog *MainWindow::gameOverDialog()
{
    return qobject_cast<GameOverDialog *>(
                mStackedWidget->widget(gameOverDialogIndex));
}
