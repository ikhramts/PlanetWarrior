//Available under GPL v3 license.
//Author: Iouri Khramtsov.

//This file contains the logging display manager.

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <QColor>
#include <QTextEdit>


//A class responsible for taking care of logging.
class Logger : public QObject {
    Q_OBJECT

public:
    Logger(QObject* parent);

    void setLogOutput(QTextEdit* output)    {m_logOutput = output;}

public slots:
    void recordMessage(const std::string& message, QObject* sender);
    void recordError(const std::string& message, QObject* sender);
    void recordStdErr(const std::string& message, QObject* sender);

private:
    //Message types:
    static const int MESSAGE = 0;
    static const int ERROR = 1;
    static const int STDERR = 2;

    //Record the log entry.
    void recordLog(const std::string& message, QObject* sender, int messageType);

    QTextEdit* m_logOutput;

    QColor m_plainColor;
    QColor m_gameMessageColor;
    QColor m_gameErrorColor;
    QColor m_playerMessageColor;
    QColor m_playerErrorColor;
    QColor m_playerStdErrColor;
};


#endif // LOGGER_H
