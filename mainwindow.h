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
