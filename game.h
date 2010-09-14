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

class Player;
class Planet;
class Fleet;
class PlanetWarsException;

class PlanetWarsGame : public QObject {
    Q_OBJECT

public:
    static const int TURN_LENGTH = 1000; //milliseconds
    static const int MAX_TURMS = 200;

    PlanetWarsGame();

    //Initializing the game.
    void setMap(const std::string& map_string);
    void setFirstPlayer(const Player& player);
    void setSecondPlayer(const Player& player);

    //Clear all information and reset the game.
    void reset();

    //Advances a game by one turn.  Returns true if the game hasn't completed; false if the game ended.
    bool step();

    //Game information.
    std::vector<Planet*> getPlanets() const;
    std::vector<Fleet*> getFleets() const;
    std::string getMessage() const;

private:
    //Send information a player.
    void sendDataToPlayer(Player* player);

    //Game objects.
    Player* m_firstPlayer;
    Player* m_secondPlayer;
    std::vector<Planet*> m_planets;
    std::vector<Fleet*> m_fleets;
    std::string m_message;
};

#endif // GAME_H
