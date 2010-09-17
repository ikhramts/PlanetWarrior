// Copyright 2010 owners of the AI Challenge project
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at http://www.apache.org/licenses/LICENSE-2.0 . Unless
// required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
// Author:	Jeff Cameron (jeff@jpcameron.com)
// ported to C++ by Albert Zeyer
// integrated better to work with Qt by Iouri Khramtsov
//
// Stores the game state.

#include "game.h"
#include <cmath>
#include <fstream>
#include <sstream>

#include "utils.h"

/*===================================================
                Class PlanetWarsGame.
====================================================*/
PlanetWarsGame::PlanetWarsGame(QObject* parent)
    :QObject(parent) {

    //Set up the players
    m_neutralPlayer = new Player(this);
    m_firstPlayer = new Player(this);
    m_secondPlayer = new Player(this);

    m_neutralPlayer->setId(0);
    m_firstPlayer->setId(1);
    m_secondPlayer->setId(2);

    //Set object names.
    this->setObjectName("Game Engine");
    m_firstPlayer->setObjectName("Player 1");
    m_secondPlayer->setObjectName("Player 2");

    //Initialize game state.
    m_state = STOPPED;
    m_runningState = PAUSED;
    m_turn = 0;

    //Initialize the timer.
    m_timer = new QTimer(this);
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(completeStep()));
}

void PlanetWarsGame::reset() {
    this->logMessage("Reloading the game... ");

    //Stop any running processes.
    this->stop();

    //Attempt to read the map from file.
    std::ifstream mapFile(m_mapFileName.c_str());

    if (mapFile.fail()) {
        this->logError("Unable to open the map file.");
        return;
    }

    std::string mapString = std::string(std::istreambuf_iterator<char>(mapFile), std::istreambuf_iterator<char>());

    //Attempt to parse the map.
    bool failed = false;
    std::vector<Planet*> planets;
    std::vector<Fleet*> fleets;

    //Lines on which we find fleets.
    std::vector<size_t> fleetLines;

    //Process each line in the map file, reading the fleet and planet information from
    //respective lines.
    std::vector<std::string> lines = Tokenize(mapString, "\n");

    for (size_t i = 0; i < lines.size(); ++i) {
        std::string& line = lines[i];

        //Remove comments.
        size_t commentBegin = line.find('#');

        if (commentBegin != std::string::npos)
            line = line.substr(0, commentBegin);

        if (TrimSpaces(line).size() == 0) continue;

        //Split the line into individual elements.
        std::vector<std::string> tokens = Tokenize(line, " ");

        if (tokens.size() == 0) continue;

        if (tokens[0] == "P") {
            //Record a planet.
            if (tokens.size() != 6) {
                std::stringstream message;
                message <<"Map file error [line " << i
                        << "]: expected 6 tokens on a planet line, have " << tokens.size() <<".";
                this->logError(message.str());
                failed = true;
                break;
            }

            Planet* planet = new Planet(this);
            planet->setX(atof(tokens[1].c_str()));
            planet->setY(atof(tokens[2].c_str()));
            planet->setOwner(this->getPlayer(atoi(tokens[3].c_str())));
            planet->setNumShips(atoi(tokens[4].c_str()));
            planet->setGrowthRate(atoi(tokens[5].c_str()));
            planet->setId(planets.size());

            planets.push_back(planet);

        } else if (tokens[0] == "F") {
            //Record a fleet.
            if (tokens.size() != 7) {
                std::stringstream message;
                message <<"Map file error [line " << i
                        << "]: expected 7 tokens on a fleet line, have " << tokens.size() <<".";
                this->logError(message.str());
                failed = true;
                break;
            }

            Fleet* fleet = new Fleet(this);
            fleet->setOwner(this->getPlayer(atoi(tokens[1].c_str())));
            fleet->setNumShips(atoi(tokens[2].c_str()));
            fleet->setSourceId(atoi(tokens[3].c_str()));
            fleet->setDestinationId(atoi(tokens[4].c_str()));
            fleet->setTotalTripLength(atoi(tokens[5].c_str()));
            fleet->setTurnsRemaining(atoi(tokens[6].c_str()));

            fleets.push_back(fleet);
            fleetLines.push_back(i);

        } else {
            std::stringstream message;
            message << "Map file error [line " << i
                    << "]: a non-empty line that does not contain a planet or a fleet.";
            this->logError(message.str());
            failed = true;
            break;
        }
    }

    const int numFleets = static_cast<int>(fleets.size());
    const int numPlanets = static_cast<int>(planets.size());

    //Resolve planet references inside fleets.
    if (!failed) {
        for (int i = 0; i < numFleets; ++i) {
            Fleet* fleet = fleets[i];

            //Attempt to resolve the source planet.
            const int sourceId = fleet->getSourceId();

            if (sourceId < 0 || sourceId >= numPlanets) {
                std::stringstream message;
                message << "Map file error [line " << fleetLines[i]
                        << "]: fleet refers to an invalid planet with id=" << sourceId << ".";
                this->logError(message.str());
                failed = true;
                break;

            } else {
                fleet->setSource(planets[sourceId]);
            }

            //Attempt to resolve the destination planet.
            const int destinationId = fleet->getSourceId();

            if (destinationId < 0 || destinationId >= numPlanets) {
                std::stringstream message;
                message << "Map file error [line " << fleetLines[i]
                        << "]: fleet refers to an invalid planet with id=" << destinationId << ".";
                this->logError(message.str());
                failed = true;
                break;

            } else {
                fleet->setSource(planets[sourceId]);
            }
        }
    }

    if (failed) {
        //Clean up.
        for (int i = 0; i < numPlanets; ++i) delete planets[i];
        for (int i = 0; i < numFleets; ++i) delete fleets[i];

        return;
    }

    //If didn't fail, replace the old planets and fleets with the new.
    const int numOldPlanets = static_cast<int>(m_planets.size());
    const int numOldFleets = static_cast<int>(m_fleets.size());
    for (int i = 0; i < numOldPlanets; ++i) delete m_planets[i];
    for (int i = 0; i < numOldFleets; ++i) delete m_fleets[i];

    m_planets = planets;
    m_fleets = fleets;
    m_newFleets = fleets;

    //Reset all flags and counters.
    m_state = RESET;
    m_turn = 1;

    //Notify everyone of the resetn
    this->logMessage("Game reloaded.");
    emit wasReset();
}

Player* PlanetWarsGame::getPlayer(int playerId) const {
    switch (playerId) {
    case 0:
        return m_neutralPlayer;

    case 1:
        return m_firstPlayer;

    case 2:
        return m_secondPlayer;

    default:
        //The program has screwed up.
        return NULL;
    }
}

void PlanetWarsGame::step() {
    if (STOPPED == m_state || STEPPING == m_state) {
        return;
    }

    //On the first turn, launch the player bots.
    if (RESET == m_state) {
        m_firstPlayer->start();
        m_secondPlayer->start();

        m_state = READY;
    }

    //Check whether the players are still running.  If they aren't, stop the game.
    if (!m_firstPlayer->isRunning() || !m_secondPlayer->isRunning()) {
        this->stop();
        return;
    }

    //Send the current game state to the bots.
    std::string messageToFirstPlayer = this->toString(m_firstPlayer);
    std::string messageToSecondPlayer = this->toString(m_secondPlayer);

    m_firstPlayer->sendGameState(messageToFirstPlayer);
    m_secondPlayer->sendGameState(messageToSecondPlayer);

    m_state = STEPPING;

    if (m_isTimerIgnored) {
        //Complete the turn right away.
        this->completeStep();

    } else {
        //Set the timer; once the turn is over, completeStep() will be called.
        m_timer->start(m_turnLength);
    }
}

void PlanetWarsGame::completeStep() {
    //Proceed only if there's an unfinished step.
    if (STEPPING != m_state) {
        return;
    }

    m_state = READY;

    //Clear the old new fleets.
    m_newFleets.clear();

    //Read and process the the responses from each of the players.
    std::string firstPlayerOutput(m_firstPlayer->readCommands());
    std::string secondPlayerOutput(m_secondPlayer->readCommands());

    bool isFirstPlayerRunning = this->processOrders(firstPlayerOutput, m_firstPlayer);
    bool isSecondPlayerRunning = this->processOrders(secondPlayerOutput, m_secondPlayer);

    //Advance the planets and fleets.
    //TODO: implement.
}

bool PlanetWarsGame::processOrders(const std::string &allOrders, Player *player) {
    std::vector<std::string> lines = Tokenize(allOrders, "\r\n");
    const int numLines = static_cast<int>(lines.size());

    //Indicator whether "go" message was encountered.
    bool foundGo = false;


    for (int i = 0; i < numLines; ++i) {
        std::string& line = lines[i];

        if (line.size() == 0) {
            //Skip empty lines.
            continue;

        } else if (line.compare("go")) {
            foundGo = true;
            break;
        }

        std::vector<std::string> tokens = Tokenize(line, " ");

        //Check whether the player bot output a bad thing.
        if (3 != tokens.size()) {
            std::stringstream message;
            message << "Error on line " << i << " of stdout output; expected 3 tokens on a move order line, have "
                    << tokens.size() << ".";
            player->logError(message.str());
            return false;
        }

        const int sourcePlanetId = atoi(tokens[0].c_str());
        const int destinationPlanetId = atoi(tokens[1].c_str());
        const int numShips = atoi(tokens[2].c_str());

        //Check whether the player has made any illegal moves.
        const int numPlanets = static_cast<int>(m_planets.size());

        if (sourcePlanetId < 0 || sourcePlanetId >= numPlanets) {
            std::stringstream message;
            message << "Error on line " << i << " of stdout output.  Source planet "
                    << sourcePlanetId << " does not exist.";
            player->logError(message.str());
            return false;
        }

        if (destinationPlanetId < 0 || destinationPlanetId >= numPlanets) {
            std::stringstream message;
            message << "Error on line " << i << " of stdout output.  Destination planet "
                    << destinationPlanetId << " does not exist.";
            player->logError(message.str());
            return false;
        }

        if (sourcePlanetId == destinationPlanetId) {
            std::stringstream message;
            message << "Error on line " << i
                    << " of stdout output.  Source planet and destination planet are the same. ";
            player->logError(message.str());
            return false;
        }

        Planet* sourcePlanet = m_planets[sourcePlanetId];
        Planet* destinationPlanet = m_planets[destinationPlanetId];

        if (destinationPlanet->getOwner()->getId() != player->getId()) {
            std::stringstream message;
            message << "Error on line " << i << " of stdout output.  Source planet "
                    << destinationPlanetId << " does not belong to this player.";
            player->logError(message.str());
            return false;
        }

        if (numShips > sourcePlanet->getNumShips() || numShips < 0) {
            std::stringstream message;
            message << "Error on line " << i << " of stdout output.  Cannot send " << numShips
                    << " ships from planet " << sourcePlanetId
                    << ".  Planet has " << sourcePlanet->getNumShips() << " ships.";
            player->logError(message.str());
            return false;
        }

        //Create a new fleet.
        Fleet* fleet = new Fleet(this);
        fleet->setOwner(player);
        fleet->setSource(sourcePlanet);
        fleet->setDestination(destinationPlanet);
        const int distance = sourcePlanet->getDistanceTo(destinationPlanet);
        fleet->setTotalTripLength(distance);
        fleet->setTurnsRemaining(distance);

        m_newFleets.push_back(fleet);
        m_fleets.push_back(fleet);
    }

    if (!foundGo) {
        std::stringstream message;
        message << "Error: player did not send \"go\" within allotted time.";
        player->logError(message.str());
        return false;
    }

    return true;
}

void PlanetWarsGame::run() {
    //TODO: implement.
}

void PlanetWarsGame::pause() {
    //TODO: implement.
}

void PlanetWarsGame::stop() {
    if (STOPPED != m_state) {
        //Stop the players.
        m_firstPlayer->stop();
        m_secondPlayer->stop();
        this->logMessage("Game ended.");
        m_state = STOPPED;
        m_runningState = PAUSED;
    }
}

void PlanetWarsGame::setMapFileName(QString mapFileName) {
    m_mapFileName = mapFileName.toStdString();
}

std::string PlanetWarsGame::toString(Player* pov) const {
    std::stringstream gameState;
    const int numPlanets = static_cast<int>(m_planets.size());
    const int numFleets = static_cast<int>(m_fleets.size());

    //Write the planets.
    for (int i = 0; i < numPlanets; ++i) {
        Planet* planet = m_planets[i];
        gameState << "P " << planet->getX()
                << " " << planet->getY()
                << " " << pov->povId(planet->getOwner())
                << " " << planet->getNumShips()
                << " " << planet->getGrowthRate()
                << std::endl;
    }

    //Write the fleets.
    for (int i = 0; i < numFleets; ++i) {
        Fleet* fleet = m_fleets[i];
        gameState << "F " << pov->povId(fleet->getOwner())
                << " " << fleet->getNumShips()
                << " " << fleet->getSource()->getId()
                << " " << fleet->getDestination()->getId()
                << " " << fleet->getTotalTripLength()
                << " " << fleet->getTurnsRemaining()
                << std::endl;
    }

    gameState << "go" << std::endl;

    return gameState.str();

}

void PlanetWarsGame::logMessage(const std::string &message) {
    emit logMessage(message, this);
}

void PlanetWarsGame::logError(const std::string &message) {
    emit logError(message, this);
}

/*===================================================
                Class Planet.
====================================================*/
Planet::Planet(QObject *parent)
    :QObject(parent) {
}

void Planet::setOwner(Player *player) {
    m_owner = player;
    emit ownerSet(player);
}

void Planet::setNumShips(int numShips) {
    m_numShips = numShips;
    emit numShipsSet(m_numShips);
}

int Planet::getDistanceTo(Planet *planet) const {
    const double dx = planet->m_x - this->m_x;
    const double dy = planet->m_y - this->m_y;
    const int distance = static_cast<int>(ceil(sqrt(dx*dx + dy*dy)));
    return distance;
}

void Planet::growFleets() {
    m_numShips += m_growthRate;
    emit numShipsSet(m_numShips);
}

void Planet::subtractShips(int numShips) {
    m_numShips -= numShips;
    emit numShipsSet(m_numShips);
}

void Planet::landFleet(Fleet *fleet) {
    //TODO: implement.
}

/*===================================================
                Class Fleet.
====================================================*/
Fleet::Fleet(QObject *parent)
    :QObject(parent) {
}

void Fleet::setTurnsRemaining(int turnsRemaining) {
    //TODO: include updating the position.
    m_turnsRemaining = turnsRemaining;

    //emit positionChanged(...)
}

/*===================================================
                Class Player.
====================================================*/
Player::Player(QObject *parent)
    :QObject(parent), m_is_started(false), m_is_alive(false), m_process(NULL) {
}

void Player::start() {
    //Make sure we don't re-start a running process.
    if (this->isRunning()) {
        this->logError("Attempting to start a running process.");
        return;
    }

    //Launch a new bot process.
    m_process = new QProcess(this);
    QString launchCommand(m_launchCommand.c_str());
    m_process->start(launchCommand);

    if (m_process->waitForStarted(1000)) {
        this->logMessage("Bot process started.");

        //Make some signal/slot connections.
        QObject::connect(m_process, SIGNAL(readyReadStandardOutput()),
                         this, SLOT(readStdOut()));
        QObject::connect(m_process, SIGNAL(readyReadStandardError()),
                         this, SLOT(readStdErr()));
        QObject::connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
                         this, SLOT(onProcessFinished(int,QProcess::ExitStatus)));

    } else {
        this->logError("Couldn't start the bot.");
    }
}

void Player::stop() {
    if (this->isRunning()) {
        //Ask the process politely to exit.
        m_process->terminate();

        if (!m_process->waitForFinished(1000)) {
            //Didn't quit after 1 sec.  Kill it.
            m_process->kill();
            this->logError("Bot process killed.");

        } else {
            this->logMessage("Bot process terminated.");
        }

        delete m_process;
        m_process = NULL;
    }
}

std::string Player::readCommands() {
    //Return everything accumulated in the stdout buffer.
    std::string commands(m_stdoutBuffer);
    m_stdoutBuffer.clear();
    return commands;
}

bool Player::isRunning() const {
    if (m_process == NULL || m_process->state() == QProcess::NotRunning) {
        return false;
    }

    return true;
}

void Player::sendGameState(const std::string& gameState) {
    if (this->isRunning()) {
        m_process->write(gameState.c_str());
        this->logStdIn(gameState);
    }
}

int Player::povId(Player *player) const {
    const int playerId = player->getId();

    if (0 == playerId) {
        return 0;
    } else if (playerId != m_id) {
        return 2;
    } else {
        return 1;
    }
}

void Player::setLaunchCommand(QString launchCommand) {
    this->setLaunchCommand(launchCommand.toStdString());
}

void Player::setLaunchCommand(const std::string &launchCommand) {
    m_launchCommand = launchCommand;
}

void Player::readStdOut() {
    //Append the contents to the stdout buffer.
    QString qContents(m_process->readAllStandardOutput());
    std::string contents(qContents.toStdString());

    m_stdoutBuffer.append(contents);
    this->logStdOut(contents);
}

void Player::readStdErr() {
    if (NULL != m_process) {
        QString qOutput(m_process->readAllStandardError());
        this->logStdErr(qOutput.toStdString());
    }
}

void Player::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (QProcess::CrashExit == exitStatus) {
        this->logError("Bot process crashed.");
    } else {
        this->logMessage("Bot process exited.");
    }

}

void Player::logMessage(const std::string &message) {
    emit logMessage(message, this);
}

void Player::logError(const std::string &message) {
    emit logError(message, this);
}

void Player::logStdErr(const std::string &message) {
    emit logStdErr(message, this);
}

void Player::logStdIn(const std::string &message) {
    emit logStdIn(message, this);
}

void Player::logStdOut(const std::string &message) {
    emit logStdOut(message, this);
}

