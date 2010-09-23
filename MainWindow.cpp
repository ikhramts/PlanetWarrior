/*
 * Copyright Iouri Khramtsov 2010.
 *
 * This file is part of PlanetWarrior program.  It is available freely
 * under GNU General Public License v3 included in gpl.txt file together
 * with this source code (also available online at http://www.gnu.org/licenses/gpl.txt).
 * Commercial licensing is available separately.
 */

#include "MainWindow.h"

#include <QCheckBox>
#include <QGraphicsView>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSpinBox>
#include <QTextEdit>
#include "Ui_MainWindow.h"
#include "game.h"
#include "graphics.h"
#include "logger.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Initialize general application settings.
    QString applicationName("PlanetWarrior");
    QString organizationName("ikhramts");
    QCoreApplication::setApplicationName(applicationName);
    QCoreApplication::setOrganizationName(organizationName);

    //Initialize the browse file dialogs, connect them to appropriate text fields.
    m_browseFirstBotDialog = new QFileDialog(this);
    m_browseSecondBotDialog = new QFileDialog(this);
    m_browseMapBotDialog = new QFileDialog(this);

    QLineEdit* firstBotFile = this->findChild<QLineEdit*>("firstBotFile");
    QLineEdit* secondBotFile = this->findChild<QLineEdit*>("secondBotFile");
    QLineEdit* mapFile = this->findChild<QLineEdit*>("mapFile");

    QObject::connect(m_browseFirstBotDialog, SIGNAL(fileSelected(QString)), firstBotFile, SLOT(setText(QString)));
    QObject::connect(m_browseSecondBotDialog, SIGNAL(fileSelected(QString)), secondBotFile, SLOT(setText(QString)));
    QObject::connect(m_browseMapBotDialog, SIGNAL(fileSelected(QString)), mapFile, SLOT(setText(QString)));

    //Set up the game.
    m_game = new PlanetWarsGame(this);

    Player* firstPlayer = m_game->getFirstPlayer();
    Player* secondPlayer = m_game->getSecondPlayer();

    //Connect the text fields to appropriate game variables.
    QObject::connect(firstBotFile, SIGNAL(textChanged(QString)), firstPlayer, SLOT(setLaunchCommand(QString)));
    QObject::connect(secondBotFile, SIGNAL(textChanged(QString)), secondPlayer, SLOT(setLaunchCommand(QString)));
    QObject::connect(mapFile, SIGNAL(textChanged(QString)), m_game, SLOT(setMapFileName(QString)));

    //Connect the buttons to the game object controls.
    QPushButton* resetButton = this->findChild<QPushButton*>("resetButton");
    QPushButton* playButton = this->findChild<QPushButton*>("playButton");
    QPushButton* pauseButton = this->findChild<QPushButton*>("pauseButton");
    QPushButton* stepButton = this->findChild<QPushButton*>("stepForwardButton");

    QObject::connect(resetButton, SIGNAL(clicked()), m_game, SLOT(reset()));
    QObject::connect(playButton, SIGNAL(clicked()), m_game, SLOT(run()));
    QObject::connect(pauseButton, SIGNAL(clicked()), m_game, SLOT(pause()));
    QObject::connect(stepButton, SIGNAL(clicked()), m_game, SLOT(step()));

    //Set up the graphics view.
    PlanetWarsView* planetWarsView = new PlanetWarsView(this);
    m_gameView = planetWarsView;
    planetWarsView->setGame(m_game);

    QGraphicsView* gameView = this->findChild<QGraphicsView*>("gameView");
    gameView->setScene(planetWarsView);
    QObject::connect(m_game, SIGNAL(wasReset()), planetWarsView, SLOT(reset()));

    //Set up the logger
    QTextEdit* logOutput = this->findChild<QTextEdit*>("logOutput");
    Logger* logger = new Logger(logOutput);
    m_logger = logger;
    logger->setLogOutput(logOutput);

    QPushButton* clearLogButton = this->findChild<QPushButton*>("clearLogButton");
    QObject::connect(clearLogButton, SIGNAL(clicked()), logOutput, SLOT(clear()));

    //Connect various parts of the game engine to the logger.
    QObject::connect(m_game, SIGNAL(logMessage(std::string,QObject*)),
                     logger, SLOT(recordMessage(std::string,QObject*)));
    QObject::connect(m_game, SIGNAL(logError(std::string,QObject*)),
                     logger, SLOT(recordError(std::string,QObject*)));

    //Connect the first player to the logger.
    QObject::connect(firstPlayer, SIGNAL(logMessage(std::string,QObject*)),
                     logger, SLOT(recordMessage(std::string,QObject*)));
    QObject::connect(firstPlayer, SIGNAL(logError(std::string,QObject*)),
                     logger, SLOT(recordError(std::string,QObject*)));
    QObject::connect(firstPlayer, SIGNAL(logStdErr(std::string,QObject*)),
                     logger, SLOT(recordStdErr(std::string,QObject*)));
    QObject::connect(firstPlayer, SIGNAL(logStdIn(std::string,QObject*)),
                     logger, SLOT(recordStdIn(std::string,QObject*)));
    QObject::connect(firstPlayer, SIGNAL(logStdOut(std::string,QObject*)),
                     logger, SLOT(recordStdOut(std::string,QObject*)));

    //Connect the second player to the logger.
    QObject::connect(secondPlayer, SIGNAL(logMessage(std::string,QObject*)),
                     logger, SLOT(recordMessage(std::string,QObject*)));
    QObject::connect(secondPlayer, SIGNAL(logError(std::string,QObject*)),
                     logger, SLOT(recordError(std::string,QObject*)));
    QObject::connect(secondPlayer, SIGNAL(logStdErr(std::string,QObject*)),
                     logger, SLOT(recordStdErr(std::string,QObject*)));
    QObject::connect(secondPlayer, SIGNAL(logStdIn(std::string,QObject*)),
                     logger, SLOT(recordStdIn(std::string,QObject*)));
    QObject::connect(secondPlayer, SIGNAL(logStdOut(std::string,QObject*)),
                     logger, SLOT(recordStdOut(std::string,QObject*)));

    //Connect logging settings in the GUI to the logger settings.
    QCheckBox* logFirstPlayerStdIn = this->findChild<QCheckBox*>("logFirstPlayerStdIn");
    QCheckBox* logFirstPlayerStdOut = this->findChild<QCheckBox*>("logFirstPlayerStdOut");
    QCheckBox* logFirstPlayerStdErr = this->findChild<QCheckBox*>("logFirstPlayerStdErr");
    QCheckBox* logSecondPlayerStdIn = this->findChild<QCheckBox*>("logSecondPlayerStdIn");
    QCheckBox* logSecondPlayerStdOut = this->findChild<QCheckBox*>("logSecondPlayerStdOut");
    QCheckBox* logSecondPlayerStdErr = this->findChild<QCheckBox*>("logSecondPlayerStdErr");

    QObject::connect(logFirstPlayerStdIn, SIGNAL(toggled(bool)),
                     logger, SLOT(setLogFirstPlayerStdIn(bool)));
    QObject::connect(logFirstPlayerStdOut, SIGNAL(toggled(bool)),
                     logger, SLOT(setLogFirstPlayerStdOut(bool)));
    QObject::connect(logFirstPlayerStdErr, SIGNAL(toggled(bool)),
                     logger, SLOT(setLogFirstPlayerStdErr(bool)));
    QObject::connect(logSecondPlayerStdIn, SIGNAL(toggled(bool)),
                     logger, SLOT(setLogSecondPlayerStdIn(bool)));
    QObject::connect(logSecondPlayerStdOut, SIGNAL(toggled(bool)),
                     logger, SLOT(setLogSecondPlayerStdOut(bool)));
    QObject::connect(logSecondPlayerStdErr, SIGNAL(toggled(bool)),
                     logger, SLOT(setLogSecondPlayerStdErr(bool)));

    //Connect the settings inputs to the game and graphics engines.
    QSpinBox* turnLength = this->findChild<QSpinBox*>("turnLength");
    QSpinBox* firstTurnLength = this->findChild<QSpinBox*>("firstTurnLength");
    QCheckBox* ignoreTimer = this->findChild<QCheckBox*>("ignoreTimer");
    QSpinBox* maxTurns = this->findChild<QSpinBox*>("maxTurns");
    QCheckBox* showGrowthRates = this->findChild<QCheckBox*>("showGrowthRates");
    QCheckBox* showPlanetIds = this->findChild<QCheckBox*>("showPlanetIds");

    m_game->setTurnLength(turnLength->value());
    m_game->setFirstTurnLength(firstTurnLength->value());
    m_game->setTimerIgnored(ignoreTimer->isChecked());
    m_game->setMaxTurns(maxTurns->value());

    planetWarsView->setShowGrowthRates(showGrowthRates->isChecked());
    planetWarsView->setShowPlanetIds(showPlanetIds->isChecked());

    QObject::connect(turnLength, SIGNAL(valueChanged(int)), m_game, SLOT(setTurnLength(int)));
    QObject::connect(firstTurnLength, SIGNAL(valueChanged(int)), m_game, SLOT(setFirstTurnLength(int)));
    QObject::connect(ignoreTimer, SIGNAL(toggled(bool)), m_game, SLOT(setTimerIgnored(bool)));
    QObject::connect(maxTurns, SIGNAL(valueChanged(int)), m_game, SLOT(setMaxTurns(int)));

    QObject::connect(ignoreTimer, SIGNAL(clicked(bool)), turnLength, SLOT(setDisabled(bool)));
    QObject::connect(ignoreTimer, SIGNAL(clicked(bool)), firstTurnLength, SLOT(setDisabled(bool)));

    QObject::connect(showGrowthRates, SIGNAL(toggled(bool)), planetWarsView, SLOT(setShowGrowthRates(bool)));
    QObject::connect(showPlanetIds, SIGNAL(toggled(bool)), planetWarsView, SLOT(setShowPlanetIds(bool)));

    //Load the settings.
    QSettings settings("PlanetWarrior.ini", QSettings::IniFormat);

    firstBotFile->setText(settings.value("firstPlayer", "").toString());
    secondBotFile->setText(settings.value("secondPlayer", "").toString());
    mapFile->setText(settings.value("mapFileName", "").toString());

    turnLength->setValue(settings.value("turnLength", 1000).toInt());
    firstTurnLength->setValue(settings.value("firstTurnLength", 3000).toInt());
    ignoreTimer->setChecked(settings.value("isTimerIgnored", false).toBool());
    maxTurns->setValue(settings.value("maxTurns", 200).toInt());
    showGrowthRates->setChecked(settings.value("showGrowthRates", false).toBool());
    showPlanetIds->setChecked(settings.value("showPlanetIds", false).toBool());

    logFirstPlayerStdIn->setChecked(settings.value("logFirstPlayerStdIn", false).toBool());
    logFirstPlayerStdOut->setChecked(settings.value("logFirstPlayerStdOut", false).toBool());
    logFirstPlayerStdErr->setChecked(settings.value("logFirstPlayerStdErr", false).toBool());
    logSecondPlayerStdIn->setChecked(settings.value("logSecondPlayerStdIn", false).toBool());
    logSecondPlayerStdOut->setChecked(settings.value("logSecondPlayerStdOut", false).toBool());
    logSecondPlayerStdErr->setChecked(settings.value("logSecondPlayerStdErr", false).toBool());
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

void MainWindow::closeEvent(QCloseEvent* event) {
    //Store the settings.
    QSettings settings("PlanetWarrior.ini", QSettings::IniFormat);

    settings.setValue("firstPlayer", m_game->getFirstPlayer()->getLaunchCommand().c_str());
    settings.setValue("secondPlayer", m_game->getSecondPlayer()->getLaunchCommand().c_str());
    settings.setValue("mapFileName", m_game->getMapFileName().c_str());

    settings.setValue("turnLength", m_game->getTurnLength());
    settings.setValue("firstTurnLength", m_game->getFirstTurnLength());
    settings.setValue("isTimerIgnored", m_game->isTimerIgnored());
    settings.setValue("maxTurns", m_game->getMaxTurns());
    settings.setValue("showGrowthRates", m_gameView->getShowGrowthRates());
    settings.setValue("showPlanetIds", m_gameView->getShowPlanetIds());

    settings.setValue("logFirstPlayerStdIn", m_logger->isLoggingFirstPlayerStdIn());
    settings.setValue("logFirstPlayerStdOut", m_logger->isLoggingFirstPlayerStdOut());
    settings.setValue("logFirstPlayerStdErr", m_logger->isLoggingFirstPlayerStdErr());
    settings.setValue("logSecondPlayerStdIn", m_logger->isLoggingSecondPlayerStdIn());
    settings.setValue("logSecondPlayerStdOut", m_logger->isLoggingSecondPlayerStdOut());
    settings.setValue("logSecondPlayerStdErr", m_logger->isLoggingSecondPlayerStdErr());

    event->accept();
}

void MainWindow::on_playButton_clicked()
{

}
