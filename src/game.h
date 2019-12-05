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

#ifndef GAME_H
#define GAME_H

#include <QWidget>

class QLabel;
class Dungeon;

class Game : public QWidget
{
    Q_OBJECT
public:
    explicit Game(QWidget *parent = nullptr);

    void start();

    void showDungeonHazards(bool value);

signals:
    void playerDiedFromPit();
    void playerDiedFromWumpus();
    void playerIsOutOfArrows();

    void playerKilledWumpus();

protected:
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void reset();

private slots:
    void showAnnouncement();
    void addWumpusNearText();
    void addPitNearText();
    void addBatNearText();

    void addDraggedByBatText();

    void updateArrowsLabel();

private:
    void addTextToAnnouncement(const QString &text);

    QString mAnnouncement;

    QLabel *mAnnouncementLabel;
    QLabel *mArrowsLabel;
    Dungeon *mDungeon;

    static constexpr auto mCountOfArrows = 3;
    int remainingArrows{};
};

#endif // GAME_H
