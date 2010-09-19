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

#ifndef GAME_H
#define GAME_H

#include <list>
#include <string>
#include <vector>
#include <QtCore>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QTimer>

//Predeclared classes.
class PlanetWarsGame;
class Planet;
class Fleet;
class Player;

typedef std::list<Fleet*> FleetList;

//A class responsible for keeping track of the game state.
class PlanetWarsGame : public QObject {
    Q_OBJECT

public:
    //Possible states for the game engine.
    enum GameState {
        NOT_RUN,
        STOPPED,
        RESET,
        READY,
        STEPPING,
        PROCESSING,
    };

    enum RunningState {
        RUNNING,
        PAUSED,
    };

    PlanetWarsGame(QObject* parent);

    //Game information.
    std::vector<Planet*> getPlanets() const     {return m_planets;}
    FleetList getFleets() const                 {return m_fleets;}
    int getWinner() const                       {return m_winner;}
    std::string getMapFileName() const          {return m_mapFileName;}
    int getFirstTurnLength() const              {return m_firstTurnLength;}
    int getTurnLength() const                   {return m_turnLength;}
    int isTimerIgnored() const                  {return m_isTimerIgnored;}
    int getMaxTurns() const                     {return m_maxTurns;}

    //Get the fleets that have appeared on the most recent turn.
    std::vector<Fleet*> getNewFleets() const    {return m_newFleets;}

    //Access to players.
    Player* getFirstPlayer() const      { return m_firstPlayer;}
    Player* getSecondPlayer() const     { return m_secondPlayer;}
    Player* getNeutralPlayer() const    { return m_neutralPlayer;}
    Player* getPlayer(int playerId) const;

    //Create a string representation of the game state given a player whose
    //point of view should be used.
    std::string toString(Player* pov) const;

signals:
    //A signal that the game has been reset.
    void wasReset();

    //A signal that the game state has changed.
    void wasChanged();

    //Log signals.
    void logMessage(const std::string& message, QObject* sender);
    void logError(const std::string& message, QObject* sender);

    //A signal that the turn has ended.
    void turnEnded();

public slots:
    void setMapFileName(QString mapFileName);

    //Running the game.
    void reset();
    void step();
    void run();
    void pause();
    void stop();

    //Slots for accepting timer settings.
    void setTurnLength(int turnLength)          {m_turnLength = turnLength;}
    void setFirstTurnLength(int firstTurnLength){m_firstTurnLength = firstTurnLength;}
    void setTimerIgnored(bool isTimerIgnored)   {m_isTimerIgnored = isTimerIgnored;}
    void setMaxTurns(int maxTurns)              {m_maxTurns = maxTurns;}

    //Complete the step once the timer times out.
    void completeStep();

    //Check whether it's time to complete the turn.
    void checkPlayerResponses();

    //Callback function for running the game.
    void continueRunning();

private:
    //Signal wrappers
    void logMessage(const std::string& message);
    void logError(const std::string& message);

    //Process messages from a player.  Return false if player made illegal moves, true otherwise.
    bool processOrders(const std::string& allOrders, Player* player);

    //Advance the game, growing fleets and fighting battles.
    void advanceGame();

    //Increment the current turn and send a notification.
    void incrementTurn();

    //Game objects.
    Player* m_firstPlayer;
    Player* m_secondPlayer;
    Player* m_neutralPlayer;
    std::vector<Planet*> m_planets;
    FleetList m_fleets;
    std::vector<Fleet*> m_newFleets;    //Fleets that appeared at last turn.

    //General game state.
    GameState m_state;
    int m_turn;
    int m_winner;   //-1 = game not over; 0 = draw; 1 = player 1; 2 = player 2.

    std::string m_mapFileName;

    //Timer.
    QTimer* m_timer;
    int m_firstTurnLength;
    int m_turnLength;
    bool m_isTimerIgnored;
    int m_maxTurns;

    //Run state settings.
    RunningState m_runningState;
    QTimer* m_runTimer;
};

//A class representing a planet.
class Planet : public QObject {
    Q_OBJECT

public:
    Planet(QObject* parent);

    //Properties.
    void setId(int id)                  { m_id = id;}
    void setX(double x)                 { m_x = x;}
    void setY(double y)                 { m_y = y;}
    void setGrowthRate(int growthRate)  { m_growthRate = growthRate;}
    void setOwner(Player* player);
    void setNumShips(int numShips);
    void setGame(PlanetWarsGame* game)  {m_game = game;}

    int getId() const                   { return m_id;}
    double getX() const                 { return m_x;}
    double getY() const                 { return m_y;}
    int getGrowthRate() const           { return m_growthRate;}
    Player* getOwner() const            { return m_owner;}
    int getNumShips() const             { return m_numShips;}
    int getDistanceTo(Planet* planet) const;

    //Advance a turn by growing new fleets on the planet.
    void growFleets();

    //Welcome (or fight) all fleets that arrived during this turn.
    void welcomeArrivedFleets();

    //Handle arrival of a fleet.
    void landFleet(Fleet* fleet);

signals:
    void ownerSet(Player* owner);
    void numShipsSet(int numShips);

private:
    int m_id;
    Player* m_owner;
    double m_x;
    double m_y;
    int m_numShips;
    int m_growthRate;
    PlanetWarsGame* m_game;

    std::vector<Fleet*> m_landedFleets;

};

//A class representing a fleet.
class Fleet : public QObject {
    Q_OBJECT

public:
    Fleet(QObject* parent);

    void setOwner(Player* owner)                { m_owner = owner; }
    void setNumShips(int numShips)              { m_numShips = numShips;}
    void setSourceId(int sourceId)              { m_sourceId = sourceId;}
    void setDestinationId(int destinationId)    { m_destinationId = destinationId;}
    void setSource(Planet* source)              { m_source = source;}
    void setDestination(Planet* destination)    { m_destination = destination;}
    void setTotalTripLength(int tripLength)     { m_totalTripLength = tripLength;}
    void setTurnsRemaining(int turnsRemaining);

    Player* getOwner() const                    { return m_owner;}
    int getNumShips() const                     { return m_numShips;}
    Planet* getSource() const                   { return m_source;}
    Planet* getDestination() const              { return m_destination;}
    int getSourceId() const                     { return m_sourceId;}
    int getDestinationId() const                { return m_destinationId;}
    int getTotalTripLength() const              { return m_totalTripLength;}
    int getTurnsRemaining() const               { return m_turnsRemaining;}

    //Move towards a planet.
    void advance();

    //State of the fleet.
    bool hasArrived() const;
    double getX() const         {return m_x;}
    double getY() const         {return m_y;}

private:
    Player* m_owner;
    int m_numShips;
    Planet* m_source;
    Planet* m_destination;
    int m_totalTripLength;
    int m_turnsRemaining;

    //These are temporary, to be used when the planet object was not built yet.
    //m_source and m_destination should be set before actually using this object.
    int m_sourceId;
    int m_destinationId;

    double m_x;
    double m_y;

    double m_sourceX;
    double m_sourceY;
    double m_destinationX;
    double m_destinationY;
};

//A class representing a player.
class Player : public QObject {
    Q_OBJECT

public:
    Player(QObject* parent);
    ~Player();

    void setId(int id)                      { m_id = id;}

    int getId() const                       { return m_id;}
    std::string getLaunchCommand() const    { return m_launchCommand;}

    //Start the process.  Return true if successfull, false if not.
    void start();

    //Stop the game execution without a full reset.
    void stop();

    //Check whether the process is running.
    bool isRunning() const;

    //Read commands from the player process.
    std::string readCommands();

    //Send updated map to the player process.
    void sendGameState(const std::string& gameState);

    //Return player's POV from point of view of another player.
    int povId(Player* player) const;

    //Signal wrappers.
    void logMessage(const std::string& message);
    void logError(const std::string& message);
    void logStdErr(const std::string& message);
    void logStdIn(const std::string& message);
    void logStdOut(const std::string& message);

    //Check whether the player is done the turn.
    bool isDoneTurn();

public slots:
    //Set the shell command used to launch the AI bot.
    void setLaunchCommand(QString launchCommand);
    void setLaunchCommand(const std::string& launchCommand);

    //Slots to be used for receiving some data from the bot process.
    void readStdOut();
    void readStdErr();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

    //Perform scheduled deletion of the QProcess object.
    void deleteProcess();

signals:
    //Log signals.
    void logMessage(const std::string& message, QObject* sender);
    void logError(const std::string& message, QObject* sender);
    void logStdErr(const std::string& message, QObject* sender);
    void logStdIn(const std::string& message, QObject* sender);
    void logStdOut(const std::string& message, QObject* sender);

    void receivedStdOut();

private:
    int m_id;
    bool m_is_started;
    bool m_is_alive;
    std::string m_launchCommand; //The shell command used to launch the AI bot.
    QProcess* m_process;
    std::string m_stdoutBuffer;   //A place for temporary storage of stdout output.
    bool m_isDoneTurn;

    QTimer* m_processDeletionTimer; //A timer for scheduling QProcess object deletion.

};

#endif // GAME_H
