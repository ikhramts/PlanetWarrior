//Available under GPL v3 license.
//Author: Iouri Khramtsov.

//This file contains graphics elements to be displayed on the game viewer.

#include "graphics.h"
#include <sstream>
#include <QPointF>
#include "game.h"

/*===================================================
                Class PlanetWarsView.
====================================================*/
PlanetWarsView::PlanetWarsView(QObject *parent)
    :QGraphicsScene(parent) {

    const qreal scalingFactor = 22;

    //Set up the palette.
    m_settings = new GraphicsSettings(this);
    m_settings->backgroundColor.setRgb(0, 0, 0); //Black.
    m_settings->firstPlayerColor.setRgb(220, 0, 0); //Red-ish.
    m_settings->secondPlayerColor.setRgb(26, 130, 186); //Darker dirty cyan.
    m_settings->neutralColor.setRgb(80,80,80);   //Grey.
    m_settings->textColor.setRgb(255, 255, 255); //White.

    m_settings->planetPen.setWidth(0.03 * scalingFactor);

    m_settings->planetFleetFont.setPointSizeF(0.4 * scalingFactor);
    m_settings->planetFleetFont.setFamily("Arial");
    //m_settings->planetFleetFont.setLetterSpacing(QFont::AbsoluteSpacing,
    //                                             -0.5 * scalingFactor);

    m_settings->scalingFactor = scalingFactor;

    //Set up the graphics scene.
    this->setBackgroundBrush(m_settings->backgroundColor);

}

void PlanetWarsView::reset() {
    //Remove old planet views.
    const int numOldPlanets = static_cast<int>(m_planetViews.size());

    for (int i = 0; i < numOldPlanets; ++i) {
        PlanetView* planetView = m_planetViews[i];
        this->removeItem(planetView);
        delete planetView;
    }

    m_planetViews.clear();

    //Create the planet views.
    std::vector<Planet*> planets(m_game->getPlanets());
    const int numPlanets = static_cast<int>(planets.size());

    for (int i = 0; i < numPlanets; ++i) {
        PlanetView* planetView = new PlanetView();
        planetView->setSettings(m_settings);
        planetView->setPlanet(planets[i]);

        this->addItem(planetView);
        m_planetViews.push_back(planetView);
    }

    this->update();
}

void PlanetWarsView::redraw() {
    //Add any new fleets and invalidate the whole thing.
    this->update();
}

/*===================================================
                Class PlanetView.
====================================================*/
PlanetView::PlanetView() {
}

void PlanetView::setPlanet(Planet *planet) {
    m_planet = planet;

    //Set the radius.
    const qreal growthRate = static_cast<qreal>(planet->getGrowthRate());
    m_radius = 0.6 + growthRate / 8;
    m_radius *= m_settings->scalingFactor;

    //Set the position.
    const qreal x = m_planet->getX() * m_settings->scalingFactor;
    const qreal y = m_planet->getY() * m_settings->scalingFactor;
    this->setPos(x, y);
}

QRectF PlanetView::boundingRect() const {
    const qreal outerPlanetRadius = m_radius + m_settings->planetPen.widthF();
    QRectF rect(-outerPlanetRadius, -outerPlanetRadius, outerPlanetRadius*2, outerPlanetRadius*2);
    return rect;
}

void PlanetView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    //Turn on anti-aliasing.
    painter->setRenderHint(QPainter::Antialiasing, true);

    //Draw the planet.
    painter->setPen(m_settings->planetPen);
    const int ownerId = m_planet->getOwner()->getId();

    if (1 ==  ownerId) {
        painter->setBrush(m_settings->firstPlayerColor);

    } else if (2 ==  ownerId) {
        painter->setBrush(m_settings->secondPlayerColor);

    } else {
        painter->setBrush(m_settings->neutralColor);
    }

    QPointF center(0, 0);

    painter->drawEllipse(center, m_radius, m_radius);

    //Draw the number of ships on the planet.
    std::stringstream streamNumShips;
    streamNumShips << m_planet->getNumShips();
    QString numShipsText(streamNumShips.str().c_str());

    painter->setPen(m_settings->textColor);
    painter->setFont(m_settings->planetFleetFont);
    painter->drawText(this->boundingRect(), Qt::AlignHCenter|Qt::AlignVCenter, numShipsText);
}


/*===================================================
              Class GraphicsSettings.
====================================================*/
GraphicsSettings::GraphicsSettings(QObject* parent)
    :QObject(parent) {
}
