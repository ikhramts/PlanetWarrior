#include "mainwindow.h"
#include "ui_mainwindow.h"

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
