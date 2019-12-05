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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class QStackedWidget;
class Game;
class GameOverDialog;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:

public slots:

void keyPressEvent(QKeyEvent *event) override;

private slots:
    void startNewGame();

    void showWinDialog();

    void showPlayerDiedFromWumpus();
    void showPlayerDiedFromPit();
    void showPlayerIsOutOfArrows();

    void quitProgramm();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void showDungeonHazards(bool value);

    Game *game();
    GameOverDialog *gameOverDialog();

    QStackedWidget *mStackedWidget;
    QString mKeyBuffer;

    static constexpr auto startDialogIndex = 0;
    static constexpr auto gameIndex = 1;
    static constexpr auto gameOverDialogIndex = 2;
    static constexpr auto winDialogIndex = 3;
};

#endif // MAINWINDOW_H
