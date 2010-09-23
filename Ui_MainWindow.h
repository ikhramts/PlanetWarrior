/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Thu Sep 23 14:47:58 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGraphicsView>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLineEdit *firstBotFile;
    QLabel *label;
    QPushButton *browseFirstBotButton;
    QLabel *label_2;
    QLineEdit *secondBotFile;
    QPushButton *browseSecondBotButton;
    QGraphicsView *gameView;
    QPushButton *playButton;
    QPushButton *stepForwardButton;
    QLabel *label_3;
    QLineEdit *mapFile;
    QPushButton *browseMapFileButton;
    QPushButton *pauseButton;
    QPushButton *resetButton;
    QTextEdit *logOutput;
    QPushButton *clearLogButton;
    QCheckBox *ignoreTimer;
    QLabel *label_6;
    QSpinBox *turnLength;
    QLabel *label_7;
    QSpinBox *maxTurns;
    QCheckBox *showPlanetIds;
    QCheckBox *showGrowthRates;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_5;
    QLabel *label_11;
    QCheckBox *logFirstPlayerStdIn;
    QCheckBox *logFirstPlayerStdOut;
    QCheckBox *logSecondPlayerStdIn;
    QCheckBox *logFirstPlayerStdErr;
    QCheckBox *logSecondPlayerStdOut;
    QCheckBox *logSecondPlayerStdErr;
    QLabel *label_12;
    QLabel *label_13;
    QSpinBox *firstTurnLength;
    QLabel *label_4;
    QLabel *label_14;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1161, 793);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        firstBotFile = new QLineEdit(centralWidget);
        firstBotFile->setObjectName(QString::fromUtf8("firstBotFile"));
        firstBotFile->setGeometry(QRect(70, 10, 321, 20));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 10, 46, 20));
        browseFirstBotButton = new QPushButton(centralWidget);
        browseFirstBotButton->setObjectName(QString::fromUtf8("browseFirstBotButton"));
        browseFirstBotButton->setGeometry(QRect(410, 10, 75, 23));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 40, 46, 20));
        secondBotFile = new QLineEdit(centralWidget);
        secondBotFile->setObjectName(QString::fromUtf8("secondBotFile"));
        secondBotFile->setGeometry(QRect(70, 40, 321, 20));
        browseSecondBotButton = new QPushButton(centralWidget);
        browseSecondBotButton->setObjectName(QString::fromUtf8("browseSecondBotButton"));
        browseSecondBotButton->setGeometry(QRect(410, 40, 75, 23));
        gameView = new QGraphicsView(centralWidget);
        gameView->setObjectName(QString::fromUtf8("gameView"));
        gameView->setGeometry(QRect(20, 160, 600, 600));
        playButton = new QPushButton(centralWidget);
        playButton->setObjectName(QString::fromUtf8("playButton"));
        playButton->setGeometry(QRect(130, 100, 31, 23));
        stepForwardButton = new QPushButton(centralWidget);
        stepForwardButton->setObjectName(QString::fromUtf8("stepForwardButton"));
        stepForwardButton->setGeometry(QRect(210, 100, 31, 23));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(40, 70, 31, 20));
        mapFile = new QLineEdit(centralWidget);
        mapFile->setObjectName(QString::fromUtf8("mapFile"));
        mapFile->setGeometry(QRect(70, 70, 321, 20));
        browseMapFileButton = new QPushButton(centralWidget);
        browseMapFileButton->setObjectName(QString::fromUtf8("browseMapFileButton"));
        browseMapFileButton->setGeometry(QRect(410, 70, 75, 23));
        pauseButton = new QPushButton(centralWidget);
        pauseButton->setObjectName(QString::fromUtf8("pauseButton"));
        pauseButton->setGeometry(QRect(170, 100, 31, 23));
        resetButton = new QPushButton(centralWidget);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));
        resetButton->setGeometry(QRect(70, 100, 51, 23));
        logOutput = new QTextEdit(centralWidget);
        logOutput->setObjectName(QString::fromUtf8("logOutput"));
        logOutput->setGeometry(QRect(640, 160, 491, 601));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        logOutput->setFont(font);
        logOutput->setLineWrapColumnOrWidth(80);
        logOutput->setReadOnly(true);
        clearLogButton = new QPushButton(centralWidget);
        clearLogButton->setObjectName(QString::fromUtf8("clearLogButton"));
        clearLogButton->setGeometry(QRect(1080, 130, 51, 23));
        ignoreTimer = new QCheckBox(centralWidget);
        ignoreTimer->setObjectName(QString::fromUtf8("ignoreTimer"));
        ignoreTimer->setGeometry(QRect(810, 10, 111, 21));
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(620, 10, 91, 21));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        turnLength = new QSpinBox(centralWidget);
        turnLength->setObjectName(QString::fromUtf8("turnLength"));
        turnLength->setGeometry(QRect(720, 10, 71, 22));
        turnLength->setMaximum(10000000);
        turnLength->setValue(1000);
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(650, 70, 61, 21));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        maxTurns = new QSpinBox(centralWidget);
        maxTurns->setObjectName(QString::fromUtf8("maxTurns"));
        maxTurns->setGeometry(QRect(720, 70, 71, 22));
        maxTurns->setMaximum(10000000);
        maxTurns->setValue(200);
        showPlanetIds = new QCheckBox(centralWidget);
        showPlanetIds->setObjectName(QString::fromUtf8("showPlanetIds"));
        showPlanetIds->setGeometry(QRect(70, 130, 101, 17));
        showPlanetIds->setChecked(true);
        showGrowthRates = new QCheckBox(centralWidget);
        showGrowthRates->setObjectName(QString::fromUtf8("showGrowthRates"));
        showGrowthRates->setGeometry(QRect(180, 130, 111, 17));
        showGrowthRates->setChecked(true);
        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(890, 140, 71, 16));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(890, 100, 71, 16));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(890, 120, 71, 16));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(990, 80, 46, 13));
        label_11 = new QLabel(centralWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(1040, 80, 46, 13));
        logFirstPlayerStdIn = new QCheckBox(centralWidget);
        logFirstPlayerStdIn->setObjectName(QString::fromUtf8("logFirstPlayerStdIn"));
        logFirstPlayerStdIn->setGeometry(QRect(1000, 100, 16, 17));
        logFirstPlayerStdOut = new QCheckBox(centralWidget);
        logFirstPlayerStdOut->setObjectName(QString::fromUtf8("logFirstPlayerStdOut"));
        logFirstPlayerStdOut->setGeometry(QRect(1000, 120, 16, 17));
        logSecondPlayerStdIn = new QCheckBox(centralWidget);
        logSecondPlayerStdIn->setObjectName(QString::fromUtf8("logSecondPlayerStdIn"));
        logSecondPlayerStdIn->setGeometry(QRect(1050, 100, 16, 17));
        logFirstPlayerStdErr = new QCheckBox(centralWidget);
        logFirstPlayerStdErr->setObjectName(QString::fromUtf8("logFirstPlayerStdErr"));
        logFirstPlayerStdErr->setGeometry(QRect(1000, 140, 16, 17));
        logFirstPlayerStdErr->setChecked(true);
        logFirstPlayerStdErr->setTristate(false);
        logSecondPlayerStdOut = new QCheckBox(centralWidget);
        logSecondPlayerStdOut->setObjectName(QString::fromUtf8("logSecondPlayerStdOut"));
        logSecondPlayerStdOut->setGeometry(QRect(1050, 120, 16, 17));
        logSecondPlayerStdErr = new QCheckBox(centralWidget);
        logSecondPlayerStdErr->setObjectName(QString::fromUtf8("logSecondPlayerStdErr"));
        logSecondPlayerStdErr->setGeometry(QRect(1050, 140, 16, 17));
        logSecondPlayerStdErr->setChecked(true);
        label_12 = new QLabel(centralWidget);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(640, 140, 46, 13));
        label_13 = new QLabel(centralWidget);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(590, 40, 121, 21));
        label_13->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        firstTurnLength = new QSpinBox(centralWidget);
        firstTurnLength->setObjectName(QString::fromUtf8("firstTurnLength"));
        firstTurnLength->setGeometry(QRect(720, 40, 71, 22));
        firstTurnLength->setMaximum(10000000);
        firstTurnLength->setValue(3000);
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(520, 140, 46, 16));
        QPalette palette;
        QBrush brush(QColor(225, 0, 3, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        QBrush brush1(QColor(120, 120, 120, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        label_4->setPalette(palette);
        label_14 = new QLabel(centralWidget);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setGeometry(QRect(570, 140, 46, 16));
        QPalette palette1;
        QBrush brush2(QColor(26, 130, 186, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush2);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        label_14->setPalette(palette1);
        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "PlanetWarrior", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        firstBotFile->setToolTip(QApplication::translate("MainWindow", "Enter the shell commant that will launch the first bot.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("MainWindow", "Player 1:", 0, QApplication::UnicodeUTF8));
        browseFirstBotButton->setText(QApplication::translate("MainWindow", "Browse...", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MainWindow", "Player 2:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        secondBotFile->setToolTip(QApplication::translate("MainWindow", "Enter the shell command that will launch the second bot.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        browseSecondBotButton->setText(QApplication::translate("MainWindow", "Browse...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        playButton->setToolTip(QApplication::translate("MainWindow", "Play", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        playButton->setText(QApplication::translate("MainWindow", ">", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        stepForwardButton->setToolTip(QApplication::translate("MainWindow", "Step Forward", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        stepForwardButton->setText(QApplication::translate("MainWindow", ">|", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MainWindow", "Map:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        mapFile->setToolTip(QApplication::translate("MainWindow", "Enter the map file to load.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        mapFile->setText(QString());
        browseMapFileButton->setText(QApplication::translate("MainWindow", "Browse...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pauseButton->setToolTip(QApplication::translate("MainWindow", "Pause", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pauseButton->setText(QApplication::translate("MainWindow", "||", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        resetButton->setToolTip(QApplication::translate("MainWindow", "Stop the game.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        resetButton->setText(QApplication::translate("MainWindow", "Reload", 0, QApplication::UnicodeUTF8));
        clearLogButton->setText(QApplication::translate("MainWindow", "Clear", 0, QApplication::UnicodeUTF8));
        ignoreTimer->setText(QApplication::translate("MainWindow", "Do not time bots", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MainWindow", "Turn length (ms):", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MainWindow", "Max turns:", 0, QApplication::UnicodeUTF8));
        showPlanetIds->setText(QApplication::translate("MainWindow", "Show planet IDs", 0, QApplication::UnicodeUTF8));
        showGrowthRates->setText(QApplication::translate("MainWindow", "Show growth rates", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MainWindow", "Show StdErr:", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MainWindow", "Show StdIn:", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("MainWindow", "Show StdOut:", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MainWindow", "Player 1", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("MainWindow", "Player 2", 0, QApplication::UnicodeUTF8));
        logFirstPlayerStdIn->setText(QString());
        logFirstPlayerStdOut->setText(QString());
        logSecondPlayerStdIn->setText(QString());
        logFirstPlayerStdErr->setText(QString());
        logSecondPlayerStdOut->setText(QString());
        logSecondPlayerStdErr->setText(QString());
        label_12->setText(QApplication::translate("MainWindow", "Log:", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("MainWindow", "First turn length (ms):", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindow", "Player 1", 0, QApplication::UnicodeUTF8));
        label_14->setText(QApplication::translate("MainWindow", "Player 2", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
