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

#include <string>
#include <vector>
#include <QtCore>
#include <QObject>
#include <QProcess>
#include <QString>

//Predeclared classes.
class PlanetWarsGame;
class Planet;
class Fleet;
class Player;

//A class responsible for keeping track of the game state.
class PlanetWarsGame : public QObject {
    Q_OBJECT

public:
    static const int TURN_LENGTH = 1000; //milliseconds
    static const int MAX_TURMS = 200;

    PlanetWarsGame(QObject* parent);

    //Game information.
    std::vector<Planet*> getPlanets() const     {return m_planets;}
    std::vector<Fleet*> getFleets() const       {return m_fleets;}
    int getWinner() const                       {return m_winner;}
    bool hasStarted() const                     {return m_hasStarted;}
    bool hasEnded() const                       {return m_hasEnded;}

    //Get the fleets that have appeared on the most recent turn.
    std::vector<Fleet*> getNewFleets() const    {return m_newFleets;}

    //Message string.
    void setMessage(const std::string& message);
    std::string getMessage() const              {return m_message;}

    //Access to players.
    Player* getFirstPlayer() const      { return m_firstPlayer;}
    Player* getSecondPlayer() const     { return m_secondPlayer;}
    Player* getNeutralPlayer() const    { return m_neutralPlayer;}
    Player* getPlayer(int playerId) const;

    //Create a string representation of the game state given a player whose POV should be used.
    std::string toString(int pointOfView) const;

signals:
    //A signal that the message string was changed.
    void messageSet(QString);

    //A signal that the game has been reset.
    void wasReset();

    //A signal that the game state has changed.
    void wasChanged();

public slots:
    void setMapFileName(QString mapFileName);

    //Running the game.
    void reset();
    void step();
    void run();
    void pause();

private:
    //Send information a player.
    void sendDataToPlayer(Player* player);

    //Game objects.
    Player* m_firstPlayer;
    Player* m_secondPlayer;
    Player* m_neutralPlayer;
    std::vector<Planet*> m_planets;
    std::vector<Fleet*> m_fleets;
    std::vector<Fleet*> m_newFleets;    //Fleets that appeared at last turn.
    std::string m_message;

    int m_winner;   //-1 = game not over; 0 = draw; 1 = player 1; 2 = player 2.
    bool m_hasStarted;
    bool m_hasEnded;

    std::string m_mapFileName;
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

    int getId() const                   { return m_id;}
    double getX() const                 { return m_x;}
    double getY() const                 { return m_y;}
    int getGrowthRate() const           { return m_growthRate;}
    Player* getOwner() const            { return m_owner;}
    int getNumShips() const             { return m_numShips;}

    //Advance a turn by growing new fleets on the planet.
    void growFleets();

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
    double getX() const;
    double getY() const;

signals:
    void positionChanged(double x, double y);

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

    void setId(int id)      { m_id = id;}

    int getId()             { return m_id;}

    //Start the process.  Return true if successfull, false if not.
    bool start();

    //Stop the process and reset the player to original state.
    void reset();

    //Read commands from the player process.
    std::string readCommands();

    //Send updated map to the player process.
    void sendGameState(PlanetWarsGame* game);

public slots:
    //Set the shell command used to launch the AI bot.
    void setLaunchCommand(QString launchCommand);
    void setLaunchCommand(const std::string& launchCommand);

private:
    int m_id;
    bool m_is_started;
    bool m_is_alive;
    std::string m_launchCommand; //The shell command used to launch the AI bot.
    QProcess* m_botProcess;
};

#endif // GAME_H