//Available under GPL v3 license.
//Author: Iouri Khramtsov.

//This file contains graphics elements to be displayed on the game viewer.

#ifndef GRAPHICS_H
#define GRAPHICS_H

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
class PlanetView;
class FleetView;
class GraphicsSettings;

//The main class containing the graphics.
class PlanetWarsView : public QGraphicsScene {
    Q_OBJECT

public:
    PlanetWarsView(QObject* parent);

    void setGame(PlanetWarsGame* game)      { m_game = game;}

public slots:
    //Redraw the game after a reset.
    void reset();

    //Update the game after its state changed.
    void redraw();

private:
    PlanetWarsGame* m_game;
    GraphicsSettings* m_settings;
    std::vector<PlanetView*> m_planetViews;

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

private:
    Planet* m_planet;
    GraphicsSettings* m_settings;

    qreal m_radius;
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

    QPen planetPen;
};

#endif // GRAPHICS_H
