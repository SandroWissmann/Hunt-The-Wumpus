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

#ifndef GAMEOVERDIALOG_H
#define GAMEOVERDIALOG_H

#include <QDialog>

namespace Ui {
class GameOverDialog;
}

class GameOverDialog : public QDialog
{
    Q_OBJECT

public:
    enum class Reason{
        playerDiedFromWumpus,
        playerDiedFromPit,
        playerIsOutOfArrows
    };

    explicit GameOverDialog(QWidget *parent = nullptr);
    ~GameOverDialog();

    void setReason(const Reason& reason);

private:
    QString playerKilledByWumpusText() const;
    QString playerKilledByPitText() const;
    QString playerOutOfArrowsText() const;

    Ui::GameOverDialog *ui;
};

#endif // GAMEOVERDIALOG_H
