/*
 * Copyright Iouri Khramtsov 2010.
 *
 * This file is part of PlanetWarrior program.  It is available freely
 * under GNU General Public License v3 included in gpl.txt file together
 * with this source code (also available online at http://www.gnu.org/licenses/gpl.txt).
 * Commercial licensing is available separately.
 */

#include "mainwindow.h"

#include <QPushButton>

#include "ui_mainwindow.h"
#include "game.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Initialize the browse file dialogs, connect them to appropriate labels.
    m_browseFirstBotDialog = new QFileDialog(this);
    m_browseSecondBotDialog = new QFileDialog(this);
    m_browseMapBotDialog = new QFileDialog(this);

    QLineEdit* firstBotFile = this->findChild<QLineEdit*>("firstBotFile");
    QLineEdit* secondBotFile = this->findChild<QLineEdit*>("secondBotFile");
    QLineEdit* mapFile = this->findChild<QLineEdit*>("mapFile");

    QObject::connect(m_browseFirstBotDialog, SIGNAL(fileSelected(QString)), firstBotFile, SLOT(setText(QString)));
    QObject::connect(m_browseSecondBotDialog, SIGNAL(fileSelected(QString)), secondBotFile, SLOT(setText(QString)));
    QObject::connect(m_browseMapBotDialog, SIGNAL(fileSelected(QString)), mapFile, SLOT(setText(QString)));

    //Pre-load some UI elements.
    m_statusLabel = this->findChild<QLabel*>("statusLabel");

    //Set up the game.
    m_game = new PlanetWarsGame(this);

    Player* firstPlayer = m_game->getFirstPlayer();
    Player* secondPlayer = m_game->getSecondPlayer();

    //Connect the text fields to appropriate game variables.
    QObject::connect(firstBotFile, SIGNAL(textChanged(QString)), firstPlayer, SLOT(setLaunchCommand(QString)));
    QObject::connect(secondBotFile, SIGNAL(textChanged(QString)), secondPlayer, SLOT(setLaunchCommand(QString)));
    QObject::connect(mapFile, SIGNAL(textChanged(QString)), m_game, SLOT(setMapFileName(QString)));

    //Connect the buttons to the game.
    QPushButton* resetButton = this->findChild<QPushButton*>("resetButton");
    QPushButton* playButton = this->findChild<QPushButton*>("playButton");
    QPushButton* pauseButton = this->findChild<QPushButton*>("pauseButton");
    QPushButton* stepButton = this->findChild<QPushButton*>("stepForwardButton");

    QObject::connect(resetButton, SIGNAL(clicked()), m_game, SLOT(reset()));
    QObject::connect(playButton, SIGNAL(clicked()), m_game, SLOT(run()));
    QObject::connect(pauseButton, SIGNAL(clicked()), m_game, SLOT(pause()));
    QObject::connect(stepButton, SIGNAL(clicked()), m_game, SLOT(step()));

    //Connect the game's message property to the message output.
    QLabel* statusLabel = this->findChild<QLabel*>("statusLabel");
    QObject::connect(m_game, SIGNAL(messageSet(QString)), statusLabel, SLOT(setText(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_browseFirstBotButton_clicked()
{
    m_browseFirstBotDialog->open();
}

void MainWindow::on_browseSecondBotButton_clicked()
{
    m_browseSecondBotDialog->open();
}

void MainWindow::on_browseMapFileButton_clicked()
{
    m_browseMapBotDialog->open();
}



void MainWindow::on_playButton_clicked()
{

}
