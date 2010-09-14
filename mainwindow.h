/*
 * Copyright Iouri Khramtsov 2010.
 *
 * This file is part of PlanetWarrior program.  It is available freely
 * under GNU General Public License v3 included in gpl.txt file together
 * with this source code (also available online at http://www.gnu.org/licenses/gpl.txt).
 * Commercial licensing is available separately.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void on_browseMapFileButton_clicked();
    void on_browseSecondBotButton_clicked();
    void on_browseFirstBotButton_clicked();

private:
    QFileDialog* m_browseFirstBotDialog;
    QFileDialog* m_browseSecondBotDialog;
    QFileDialog* m_browseMapBotDialog;
};

#endif // MAINWINDOW_H
