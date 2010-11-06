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

    QObject::connect(m_firstPlayer, SIGNAL(receivedStdOut()), this, SLOT(checkPlayerResponses()));
    QObject::connect(m_secondPlayer, SIGNAL(receivedStdOut()), this, SLOT(checkPlayerResponses()));

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
    m_timer->setSingleShot(true);
    QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(checkPlayerResponses()));

    //Initialize the run mode timer.
    m_runTimer = new QTimer(this);
    m_runTimer->setSingleShot(true);
    QObject::connect(m_runTimer, SIGNAL(timeout()), this, SLOT(continueRunning()));
}

void PlanetWarsGame::reset() {
    this->logMessage("Reloading the game... ");

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
    std::list<Fleet*> fleets;

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
            planet->setGame(this);

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

    const int numPlanets = static_cast<int>(planets.size());

    //Resolve planet references inside fleets.
    int fleetIndex = 0;

    if (!failed) {
        for (FleetList::iterator it = fleets.begin(); it != fleets.end(); ++it) {
            Fleet* fleet = *it;

            //Attempt to resolve the source planet.
            const int sourceId = fleet->getSourceId();

            if (sourceId < 0 || sourceId >= numPlanets) {
                std::stringstream message;
                message << "Map file error [line " << fleetLines[fleetIndex]
                        << "]: fleet refers to an invalid planet with id=" << sourceId << ".";
                this->logError(message.str());
                failed = true;
                break;

            } else {
                fleet->setSource(planets[sourceId]);
            }

            //Attempt to resolve the destination planet.
            const int destinationId = fleet->getDestinationId();

            if (destinationId < 0 || destinationId >= numPlanets) {
                std::stringstream message;
                message << "Map file error [line " << fleetLines[fleetIndex]
                        << "]: fleet refers to an invalid planet with id=" << destinationId << ".";
                this->logError(message.str());
                failed = true;
                break;

            } else {
                fleet->setDestination(planets[destinationId]);
            }

            //This will force recalculation of the current position.
            fleet->setTurnsRemaining(fleet->getTurnsRemaining());

            ++fleetIndex;
        }
    }

    if (failed) {
        //Clean up.
        for (int i = 0; i < numPlanets; ++i) delete planets[i];
        for (FleetList::iterator it = fleets.begin(); it != fleets.end(); ++it) delete *it;

        return;
    }

    //Parsed the map successfully. Reset the game.
    //Stop any running processes.
    this->stop();
    this->stopPlayers();

    //If didn't fail, replace the old planets and fleets with the new.
    const int numOldPlanets = static_cast<int>(m_planets.size());
    for (int i = 0; i < numOldPlanets; ++i) delete m_planets[i];
    for (FleetList::iterator it = m_fleets.begin(); it != m_fleets.end(); ++it) delete (*it);

    m_planets = planets;
    m_fleets = fleets;
    m_newFleets.insert(m_newFleets.end(), fleets.begin(), fleets.end());

    //Reset all flags and counters.
    m_state = RESET;
    m_turn = 0;

    //Notify everyone of the resetn
    this->logMessage("================= Game reset. ==================");
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

    this->incrementTurn();

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
        //Check every once in a while whether the players are done.
        m_timer->start(100);

    } else {
        //Set the timer; once the turn is over, checkPlayerResponses() will be called.
        if (1 == m_turn) {
            m_timer->start(m_firstTurnLength);

        } else {
            m_timer->start(m_turnLength);
        }
    }
}

void PlanetWarsGame::checkPlayerResponses() {
    if (STEPPING != m_state) {
        return;
    }

    //Check whether the players are ready to have their orders processed.
    const bool arePlayersDone = (m_firstPlayer->isDoneTurn() && m_secondPlayer->isDoneTurn());

    if (arePlayersDone) {
        m_timer->stop();
        this->completeStep();

    } else {
        if (m_isTimerIgnored) {
            //Keep waiting until the players are done.
            m_timer->stop();
            m_timer->start(100);

        } else {
            if (m_timer->isActive()) {
                //Wait for the turn to complete.
                return;

            } else {
                //Complete the step whether the players are ready or not.
                this->completeStep();
            }
        }
    }
}

void PlanetWarsGame::completeStep() {
    //Proceed only if there's an unfinished step.
    if (STEPPING != m_state) {
        return;
    }

    m_state = PROCESSING;

    //Clear the old new fleets.
    m_newFleets.clear();

    //Read and process the the responses from each of the players.
    std::string firstPlayerOutput(m_firstPlayer->readCommands());
    std::string secondPlayerOutput(m_secondPlayer->readCommands());

    bool isFirstPlayerRunning = this->processOrders(firstPlayerOutput, m_firstPlayer);
    bool isSecondPlayerRunning = this->processOrders(secondPlayerOutput, m_secondPlayer);

    //Check whether the players are still alive.
    if (!isFirstPlayerRunning || !isSecondPlayerRunning) {
        this->stop();
        return;
    }

    this->advanceGame();

    //Check each player's position.
    int firstPlayerShips = 0;
    int secondPlayerShips = 0;

    const int numPlanets = static_cast<int>(m_planets.size());

    for (int i = 0; i < numPlanets; ++i) {
        Planet* planet = m_planets[i];
        const int ownerId = planet->getOwner()->getId();

        if (1 == ownerId) {
            firstPlayerShips += planet->getNumShips();

        } else if (2 == ownerId) {
            secondPlayerShips += planet->getNumShips();
        }
    }

    for (FleetList::iterator it = m_fleets.begin(); it != m_fleets.end(); ++it) {
        Fleet* fleet = *it;

        if (1 == fleet->getOwner()->getId()) {
            firstPlayerShips += fleet->getNumShips();

        } else {
            secondPlayerShips += fleet->getNumShips();
        }
    }

    emit turnEnded();

    //Check for game end conditions.
    bool isGameOver = false;

    if (firstPlayerShips == 0 && secondPlayerShips == 0) {
        this->logMessage("Draw.");
        isGameOver = true;

    } else if (firstPlayerShips == 0) {
        this->logMessage("Player 2 wins.");
        isGameOver = true;

    } else if (secondPlayerShips == 0) {
        this->logMessage("Player 1 wins.");
        isGameOver = true;

    } else if (m_turn >= m_maxTurns) {
        if (firstPlayerShips > secondPlayerShips) {
            this->logMessage("Player 1 wins.");
            isGameOver = true;

        } else if (firstPlayerShips < secondPlayerShips) {
            this->logMessage("Player 2 wins.");
            isGameOver = true;

        } else {
            this->logMessage("Draw.");
            isGameOver = true;
        }
    }

    if (isGameOver) {
        this->stop();
        return;
    }

    //Game is not over.
    m_state = READY;
    this->continueRunning();
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

        } else if (line.compare("go") == 0) {
            foundGo = true;
            break;

        } else if (line[0] == '#') {
            //Check whether this is a special directive to the visualizer to display
            //a planet property.  If so, save that property with the planet object.
            if (line.size() < 2 || line[1] != '-')
                continue;

            std::vector<std::string> tokens = Tokenize(line, " ");
            tokens.erase(tokens.begin());
            if (tokens.size() == 4 && tokens[0] == "planet") {
                int planetId = atoi(tokens[1].c_str());
                const std::string& name = tokens[2];
                const std::string& value = tokens[3];

                Planet* planet = m_planets[planetId];
                planet->setProperty(name, value);
            }

            continue;
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

        if (sourcePlanet->getOwner()->getId() != player->getId()) {
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

        sourcePlanet->setNumShips(sourcePlanet->getNumShips() - numShips);

        //Create a new fleet.
        Fleet* fleet = new Fleet(this);
        fleet->setOwner(player);
        fleet->setSource(sourcePlanet);
        fleet->setDestination(destinationPlanet);
        fleet->setNumShips(numShips);
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

void PlanetWarsGame::advanceGame() {
    //Make planets grow ships.
    const int numPlanets = static_cast<int>(m_planets.size());

    for (int i = 0; i < numPlanets; ++i) {
        m_planets[i]->growFleets();
    }

    //Advance fleets.
    for (FleetList::iterator it = m_fleets.begin(); it != m_fleets.end(); ++it) {
        (*it)->advance();
    }

    //Welcome (or fight) the arrived fleets.
    for (int i = 0; i < numPlanets; ++i) {
        m_planets[i]->welcomeArrivedFleets();
    }

    //Clean up arrived fleets.
    FleetList::iterator itFleet = m_fleets.begin();

    while (itFleet != m_fleets.end()) {
        FleetList::iterator itCurrent = itFleet;
        ++itFleet;

        Fleet* fleet = (*itCurrent);

        if (fleet->getTurnsRemaining() <= 0) {
            delete fleet;
            m_fleets.erase(itCurrent);

        }
    }
}

void PlanetWarsGame::run() {
    m_runningState = RUNNING;
    this->continueRunning();
}

void PlanetWarsGame::pause() {
    m_runningState = PAUSED;
}

void PlanetWarsGame::stop() {
    m_state = STOPPED;
    m_runningState = PAUSED;
    this->logMessage("Game ended");
}

void PlanetWarsGame::setMapFileName(QString mapFileName) {
    m_mapFileName = mapFileName.toStdString();
}

void PlanetWarsGame::continueRunning() {
    //Proceed only if the game is in "running" state.
    if (PAUSED == m_runningState) {
        return;
    }

    //Check whether the step has completed, or the pause has not passed.  If not, come back later.
    if (READY != m_state && RESET != m_state) {
        return;

    } else if (m_runTimer->isActive()) {
        //return;
        //Don't wait for 1 sec, go on to the next step.
        m_runTimer->stop();
    }

    //Pause for nice rendering
    class Sleeper: public QThread {
    public:
        Sleeper(int delay) {
            QThread::currentThread()->msleep(delay);
        }
    };
    Sleeper s(m_renderDelay);

    //Go on to the next step.
    this->step();
    m_runTimer->start(m_turnLength);
}

std::string PlanetWarsGame::toString(Player* pov) const {
    std::stringstream gameState;
    const int numPlanets = static_cast<int>(m_planets.size());

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
    for (FleetList::const_iterator it = m_fleets.begin(); it != m_fleets.end(); ++it) {
        Fleet* fleet = (*it);
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

void PlanetWarsGame::stopPlayers() {
    m_firstPlayer->stop();
    m_secondPlayer->stop();
}

void PlanetWarsGame::logMessage(const std::string &message) {
    emit logMessage(message, this);
}

void PlanetWarsGame::logError(const std::string &message) {
    emit logError(message, this);
}

void PlanetWarsGame::incrementTurn() {
    ++m_turn;
    std::stringstream message;
    message << "Turn " << m_turn;
    this->logMessage(message.str());
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
    //Grow fleets only on non-neutral planets.
    if (m_owner->getId() != 0) {
        m_numShips += m_growthRate;
        emit numShipsSet(m_numShips);
    }
}

void Planet::welcomeArrivedFleets() {
    const int ownerId = m_owner->getId();

    //Tally up the ships for each force.
    const int numArrivedFleets = static_cast<int>(m_landedFleets.size());

    if (numArrivedFleets == 0) {
        return;
    }

    int playerShips[3];
    playerShips[0] = (ownerId == 0) ? m_numShips : 0;
    playerShips[1] = (ownerId == 1) ? m_numShips : 0;
    playerShips[2] = (ownerId == 2) ? m_numShips : 0;

    for (int i = 0; i < numArrivedFleets; ++i) {
        Fleet* fleet = m_landedFleets[i];
        playerShips[fleet->getOwner()->getId()] += fleet->getNumShips();
    }

    m_landedFleets.clear();

    //Check who won.
    //Check whether the owner stays the same.
    if (playerShips[ownerId] >= std::max(playerShips[(ownerId+1)%3], playerShips[(ownerId+2)%3])) {
        this->setNumShips(playerShips[ownerId]
                          - std::max(playerShips[(ownerId+1)%3], playerShips[(ownerId+2)%3]));
        return;
    }

    //Otherwise, find the new owner.
    if (playerShips[1] > playerShips[2]) {
        this->setOwner(m_game->getFirstPlayer());
        const int remainingShips = playerShips[1] - std::max(playerShips[2], playerShips[0]);
        this->setNumShips(remainingShips);

    } else if (playerShips[2] > playerShips[1]) {
        this->setOwner(m_game->getSecondPlayer());
        const int remainingShips = playerShips[2] - std::max(playerShips[1], playerShips[0]);
        this->setNumShips(remainingShips);

    } else if (ownerId == 0 && playerShips[2] == playerShips[1]) {
        //The invading fleets are larger than the neutral planet, but equal in size.
        //Planet stays neutral.
        this->setNumShips(0);
    }

    //There should be no other cases.
}

void Planet::landFleet(Fleet *fleet) {
    m_landedFleets.push_back(fleet);
}

/*===================================================
                Class Fleet.
====================================================*/
Fleet::Fleet(QObject *parent)
    :QObject(parent), m_source(NULL), m_destination(NULL) {
}

void Fleet::setTurnsRemaining(int turnsRemaining) {
    m_turnsRemaining = turnsRemaining;

    //Update the position change.
    if (NULL != m_source && NULL != m_destination) {
        const double sourceX = m_source->getX();
        const double sourceY = m_source->getY();
        const double destinationX = m_destination->getX();
        const double destinationY = m_destination->getY();
        const double tripLength = static_cast<double>(m_totalTripLength);
        const double travelled = static_cast<double>(m_totalTripLength - turnsRemaining);

        const double tripDx = destinationX - sourceX;
        const double tripDy = destinationY - sourceY;

        const double x = sourceX + tripDx * travelled / tripLength;
        const double y = sourceY + tripDy * travelled / tripLength;

        m_x = x;
        m_y = y;
    }
}

void Fleet::advance() {
    this->setTurnsRemaining(--m_turnsRemaining);

    if (m_turnsRemaining <= 0) {
        //Arrived.
        m_destination->landFleet(this);
    }
}

/*===================================================
                Class Player.
====================================================*/
Player::Player(QObject *parent)
    :QObject(parent), m_is_started(false), m_is_alive(false), m_process(NULL) {
    //Set up the QProcess deletion timer.
    m_processDeletionTimer = new QTimer(this);
    m_processDeletionTimer->setSingleShot(true);
    QObject::connect(m_processDeletionTimer, SIGNAL(timeout()), this, SLOT(deleteProcess()));
}

Player::~Player() {
    this->stop();
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
    //Terminate the process.
    if (this->isRunning()) {
        //Ask the process politely to exit.
//        m_process->terminate();

//        if (!m_process->waitForFinished(1000)) {
//            //Didn't quit after 1 sec.  Kill it.
//            m_process->kill();
//            this->logError("Bot process killed.");

//        } else {
//            this->logMessage("Bot process terminated.");
//        }
        m_process->kill();
        this->logMessage("Bot process terminated.");
        m_process->waitForFinished(10);

        //Schedule deletion of the QProcess object in the immediate future.
        //QProcess object cannot be deleted here directly because somewhere
        //down the call stack there may be a QProcess function that still
        //wants to talk to this QProcess object.
        m_processDeletionTimer->start(100);
    }
}

void Player::deleteProcess() {
    m_process->deleteLater();
    m_process = NULL;
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

bool Player::isDoneTurn() {
    bool isDone = (m_stdoutBuffer.find("go") != std::string::npos);
    return isDone;
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

    emit receivedStdOut();
}

void Player::readStdErr() {
    if (NULL != m_process) {
        QString qOutput(m_process->readAllStandardError());
        this->logStdErr(qOutput.toStdString());
    }
}

void Player::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    this->logMessage("Bot process exited.");
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

