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
    m_playerStdErrColor.setRgb(49, 49, 140);//Dark blue
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

void Logger::recordLog(const std::string &message, QObject *sender, int messageType) {
    QString senderName = sender->objectName();

    //Compose the full messag string.
    QString qMessage(message.c_str());
    QString fullMessage;
    fullMessage.append('[').append(senderName).append("]: ").append(qMessage);

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
