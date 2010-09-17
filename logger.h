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
    void recordStdIn(const std::string& message, QObject* sender);
    void recordStdOut(const std::string& message, QObject* sender);

    void setLogFirstPlayerStdIn(bool doLog)     { m_logFirstPlayerStdIn = doLog;}
    void setLogFirstPlayerStdOut(bool doLog)    { m_logFirstPlayerStdOut = doLog;}
    void setLogFirstPlayerStdErr(bool doLog)    { m_logFirstPlayerStdErr = doLog;}
    void setLogSecondPlayerStdIn(bool doLog)    { m_logSecondPlayerStdIn = doLog;}
    void setLogSecondPlayerStdOut(bool doLog)   { m_logSecondPlayerStdOut = doLog;}
    void setLogSecondPlayerStdErr(bool doLog)   { m_logSecondPlayerStdErr = doLog;}

    bool isLoggingFirstPlayerStdIn() const      { return m_logFirstPlayerStdIn;}
    bool isLoggingFirstPlayerStdOut() const     { return m_logFirstPlayerStdOut;}
    bool isLoggingFirstPlayerStdErr() const     { return m_logFirstPlayerStdErr;}
    bool isLoggingSecondPlayerStdIn() const     { return m_logSecondPlayerStdIn;}
    bool isLoggingSecondPlayerStdOut() const    { return m_logSecondPlayerStdOut;}
    bool isLoggingSecondPlayerStdErr() const    { return m_logSecondPlayerStdErr;}

private:
    //Message types:
    static const int MESSAGE = 0;
    static const int ERROR = 1;
    static const int STDERR = 2;
    static const int STDIN = 3;
    static const int STDOUT = 4;

    //Record the log entry.
    void recordLog(const std::string& message, QObject* sender, int messageType);

    QTextEdit* m_logOutput;

    QColor m_plainColor;
    QColor m_gameMessageColor;
    QColor m_gameErrorColor;
    QColor m_playerMessageColor;
    QColor m_playerErrorColor;
    QColor m_playerStdErrColor;
    QColor m_playerStdInColor;
    QColor m_playerStdOutColor;

    bool m_logFirstPlayerStdIn;
    bool m_logFirstPlayerStdOut;
    bool m_logFirstPlayerStdErr;
    bool m_logSecondPlayerStdIn;
    bool m_logSecondPlayerStdOut;
    bool m_logSecondPlayerStdErr;
};


#endif // LOGGER_H
