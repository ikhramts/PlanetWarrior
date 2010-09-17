//Available under GPL v3 license.
//Author: Iouri Khramtsov.

//This file contains the logging display manager.

#include "logger.h"

/*===================================================
                   Class Logger.
====================================================*/
Logger::Logger(QObject *parent)
    : QObject(parent) {
    //Initialize styles.
    m_plainColor.setRgb(0, 0, 0);           //Black
    m_gameMessageColor.setRgb(34, 112, 36); //Dark green
    m_gameErrorColor.setRgb(255, 0, 0);     //Red
    m_playerMessageColor.setRgb(34, 112, 36); //Dark green
    m_playerErrorColor.setRgb(255, 0, 0);   //Red
    m_playerStdErrColor.setRgb(122, 20, 116);//Dark purple
    m_playerStdInColor.setRgb(115, 95, 15); //Dark dirty khaki
    m_playerStdOutColor.setRgb(49, 49, 140);//Dark blue

    //Initialize logging settings.
    m_logFirstPlayerStdIn = false;
    m_logFirstPlayerStdOut = false;
    m_logFirstPlayerStdErr = true;
    m_logSecondPlayerStdIn = false;
    m_logSecondPlayerStdOut = false;
    m_logSecondPlayerStdErr = true;

}

void Logger::recordMessage(const std::string &message, QObject *sender) {
    this->recordLog(message, sender, Logger::MESSAGE);
}

void Logger::recordError(const std::string &message, QObject *sender) {
    this->recordLog(message, sender, Logger::ERROR);
}

void Logger::recordStdErr(const std::string &message, QObject *sender) {
    this->recordLog(message, sender, Logger::STDERR);
}

void Logger::recordStdIn(const std::string &message, QObject *sender) {
    this->recordLog(message, sender, Logger::STDIN);
}

void Logger::recordStdOut(const std::string &message, QObject *sender) {
    this->recordLog(message, sender, Logger::STDOUT);
}

void Logger::recordLog(const std::string &message, QObject *sender, const int messageType) {
    QString senderName = sender->objectName();

    //Check whether we should be logging this.
    if (0 == senderName.compare("Player 1")) {
        if (!m_logFirstPlayerStdIn && (Logger::STDIN == messageType)) {
            return;
        } else if (!m_logFirstPlayerStdOut && (Logger::STDOUT == messageType)) {
            return;
        } else if (!m_logFirstPlayerStdErr && (Logger::STDERR == messageType)) {
            return;
        }
    } else if (0 == senderName.compare("Player 2")) {
        if (!m_logSecondPlayerStdIn && (Logger::STDIN == messageType)) {
            return;
        } else if (!m_logSecondPlayerStdOut && (Logger::STDOUT == messageType)) {
            return;
        } else if (!m_logSecondPlayerStdErr && (Logger::STDERR == messageType)) {
            return;
        }
    }

    //Compose the full messag string.
    QString qMessage(message.c_str());
    QString fullMessage;
    fullMessage.append('[').append(senderName);

    if (STDIN == messageType) {
        fullMessage.append(" stdin");

    } else if (STDOUT == messageType) {
        fullMessage.append(" stdout");

    } else if (STDERR == messageType) {
        fullMessage.append(" stderr");
    }

    fullMessage.append("]: ").append(qMessage);

    //Select the appropriate font style.
    QColor color;

    if (0 == senderName.size()) {
        color = m_plainColor;

    } else if ('G' == senderName.at(0)) {
        color = (messageType == Logger::MESSAGE ? m_gameMessageColor : m_gameErrorColor);

    } else if ('P' == senderName.at(0)) {
        switch(messageType) {
        case Logger::MESSAGE:
            color = m_playerMessageColor;
            break;

        case Logger::ERROR:
            color = m_playerErrorColor;
            break;

        case Logger::STDERR:
            color = m_playerStdErrColor;
            break;

        case Logger::STDIN:
            color = m_playerStdInColor;
            break;

        case Logger::STDOUT:
            color = m_playerStdOutColor;
            break;

        default:
            color = m_plainColor;
        }

    } else {
        color = m_plainColor;
    }

    //Write to the log.
    m_logOutput->setTextColor(color);
    m_logOutput->append(fullMessage);
}
