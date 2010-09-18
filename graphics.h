//Available under GPL v3 license.
//Author: Iouri Khramtsov.

//This file contains graphics elements to be displayed on the game viewer.

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <list>
#include <vector>
#include <QColor>
#include <QFont>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QPen>

//Forward-declared classes.
class PlanetWarsGame;
class Planet;
class Fleet;
class Player;
class PlanetView;
class FleetView;
class FleetConnector;
class GraphicsSettings;

typedef std::list<FleetView*> FleetViewList;

//The main class containing the graphics.
class PlanetWarsView : public QGraphicsScene {
    Q_OBJECT

public:
    PlanetWarsView(QObject* parent);

    void setGame(PlanetWarsGame* game);

public slots:
    //Redraw the game after a reset.
    void reset();

    //Update the game after its state changed.
    void redraw();

    //Remove a fleet.
    void removeFleetView(FleetView *fleetView);

private:
    PlanetWarsGame* m_game;
    GraphicsSettings* m_settings;
    std::vector<PlanetView*> m_planetViews;
    FleetViewList m_fleetViews;
};

//A class representing a planet.
class PlanetView : public QGraphicsItem {
public:
    PlanetView();

    void setPlanet(Planet* planet);
    void setSettings(GraphicsSettings* settings) { m_settings = settings;}

    //The main painting logic.
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //Handle changes in the planet's status.
    //void updateNumShips(int numShips);
    //void updateOwner(Player* owner);

private:
    Planet* m_planet;
    GraphicsSettings* m_settings;

    qreal m_radius;
};

//A class representing a fleet view.
class FleetView : public QGraphicsItem {
public:
    FleetView();
    ~FleetView();

    void setFleet(Fleet* fleet);
    void setSettings(GraphicsSettings* settings) { m_settings = settings;}
    void setPlanetWarsView(PlanetWarsView* view) { m_planetWarsView = view;}

    //The main painting logic.
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void removeSelf();

private:
    Fleet* m_fleet;
    FleetConnector* m_connector;
    GraphicsSettings* m_settings;
    PlanetWarsView* m_planetWarsView;

    qreal m_angle;
};

//This class is here because I can't seem to connect the fleet directly to the FleetView.
//Its job is to forward Fleet's signals to FleetView object.
class FleetConnector : public QObject {
    Q_OBJECT

public:
    FleetConnector(FleetView* fleetView);

    void setFleet(Fleet *fleet);

public slots:
    void removeFleet();

private:
    FleetView* m_fleetView;
    Fleet* m_fleet;
};

class GraphicsSettings : public QObject {
    Q_OBJECT

public:
    GraphicsSettings(QObject* parent);

    QColor firstPlayerColor;
    QColor secondPlayerColor;
    QColor neutralColor;
    QColor backgroundColor;
    QColor textColor;
    QFont planetFleetFont;
    QFont fleetFont;

    QPen planetPen;

    qreal scalingFactor;
};

#endif // GRAPHICS_H
