//Available under GPL v3 license.
//Author: Iouri Khramtsov.

//This file contains graphics elements to be displayed on the game viewer.

#include "graphics.h"
#include <sstream>
#include <QLineF>
#include <QPointF>
#include <QPolygonF>
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
    m_settings->firstPlayerColor.setRgb(219, 80, 80); //Red-ish.
    m_settings->secondPlayerColor.setRgb(97, 191, 242); //Darker dirty cyan.
//    m_settings->secondPlayerColor.setRgb(26, 130, 186); //Darker dirty cyan.
    m_settings->neutralColor.setRgb(80,80,80);   //Grey.
    m_settings->textColor.setRgb(255, 255, 255); //White.

    m_settings->planetPen.setWidth(0.03 * scalingFactor);

    m_settings->planetFleetFont.setPointSizeF(0.4 * scalingFactor);
    m_settings->planetFleetFont.setFamily("Arial");
    //m_settings->planetFleetFont.setLetterSpacing(QFont::AbsoluteSpacing,
    //                                             -0.5 * scalingFactor);

    //Planet id settings.
    m_settings->planetIdColor.setRgb(0, 180, 0); //Green
    m_settings->planetIdFont.setFamily("Arial");
    m_settings->planetIdFont.setPointSizeF(0.3 * scalingFactor);;

    //Fleet settings.
    m_settings->fleetFont.setPointSizeF(0.35 * scalingFactor);
    m_settings->fleetFont.setFamily("Arial");
    m_settings->firstPlayerFleetColor.setRgb(190, 0, 0);
    m_settings->secondPlayerFleetColor.setRgb(0, 95, 163);
    m_settings->firstPlayerFleetPen.setColor(m_settings->firstPlayerFleetColor);
    m_settings->firstPlayerFleetPen.setWidthF(0.5);
    m_settings->secondPlayerFleetPen.setColor(m_settings->secondPlayerFleetColor);
    m_settings->secondPlayerFleetPen.setWidthF(0.5);

    m_settings->scalingFactor = scalingFactor;

    //Set up the graphics scene.
    this->setBackgroundBrush(m_settings->backgroundColor);
    m_showGrowthRates = true;
    m_showPlanetIds = true;
    m_showPlanetProps = true;
}

void PlanetWarsView::setGame(PlanetWarsGame *game) {
    m_game = game;

    QObject::connect(m_game, SIGNAL(turnEnded()), this, SLOT(redraw()));
}

void PlanetWarsView::reset() {
    //Remove old planet and fleet views.
    const int numOldPlanets = static_cast<int>(m_planetViews.size());

    for (int i = 0; i < numOldPlanets; ++i) {
        PlanetView* planetView = m_planetViews[i];
        this->removeItem(planetView);
        delete planetView;
    }

    for (FleetViewList::iterator it = m_fleetViews.begin(); it != m_fleetViews.end(); ++it) {
        FleetView* fleetView = *it;
        this->removeItem(fleetView);
        delete fleetView;
    }

    m_planetViews.clear();
    m_fleetViews.clear();

    //Create the planet views.
    std::vector<Planet*> planets(m_game->getPlanets());
    const int numPlanets = static_cast<int>(planets.size());

    for (int i = 0; i < numPlanets; ++i) {
        PlanetView* planetView = new PlanetView();
        planetView->setSettings(m_settings);
        planetView->setPlanet(planets[i]);
        planetView->setPlanetWarsView(this);

        this->addItem(planetView);
        m_planetViews.push_back(planetView);
    }

    //Create fleet views.
    FleetList fleets = m_game->getFleets();

    for (FleetList::iterator it = fleets.begin(); it != fleets.end(); ++it) {
        Fleet* fleet = (*it);
        FleetView* fleetView = new FleetView();
        fleetView->setSettings(m_settings);
        fleetView->setPlanetWarsView(this);
        fleetView->setFleet(fleet);

        this->addItem(fleetView);
        m_fleetViews.push_back(fleetView);
    }

    this->update();
}

void PlanetWarsView::redraw() {
    //Add any new fleets and invalidate the whole thing.
    std::vector<Fleet*> newFleets = m_game->getNewFleets();
    const int numNewFleets = static_cast<int>(newFleets.size());

    for (int i = 0; i < numNewFleets; ++i) {
        Fleet* fleet = newFleets[i];

        FleetView* fleetView =  new FleetView();
        fleetView->setSettings(m_settings);
        fleetView->setPlanetWarsView(this);
        fleetView->setFleet(fleet);

        const qreal scalingFactor = m_settings->scalingFactor;
        fleetView->setPos(scalingFactor * fleet->getX(), scalingFactor * fleet->getY());

        this->addItem(fleetView);
        //this->addItem(fleetView->getArrow());
        m_fleetViews.push_back(fleetView);
    }

    this->update();
}

void PlanetWarsView::removeFleetView(FleetView *fleetView) {
    m_fleetViews.remove(fleetView);
    this->removeItem(fleetView);
    delete fleetView;
}

void PlanetWarsView::setShowGrowthRates(bool showGrowthRates) {
    m_showGrowthRates = showGrowthRates;
    this->update();
}

void PlanetWarsView::setShowPlanetIds(bool showPlanetIds) {
    m_showPlanetIds = showPlanetIds;
    m_showPlanetProps = showPlanetIds;
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

    std::string val;
    if ((val = m_planet->getProperty("color")).size()) {
        painter->setBrush(QBrush(QColor(val.c_str())));
    }

    QPointF center(0, 0);

    painter->drawEllipse(center, m_radius, m_radius);

    //Draw the number of ships on the planet.
    std::stringstream streamNumShips;
    streamNumShips << m_planet->getNumShips();

    if (m_planetWarsView->getShowGrowthRates()) {
        //If requested, draw the growth rate as well.
        const int growthRate = m_planet->getGrowthRate();

        if (0 == growthRate) {
            streamNumShips << "x";

        } else {
            streamNumShips << "+" << m_planet->getGrowthRate();
        }
    }

    QString numShipsText(streamNumShips.str().c_str());

    painter->setPen(m_settings->textColor);
    painter->setFont(m_settings->planetFleetFont);
    painter->drawText(this->boundingRect(), Qt::AlignHCenter|Qt::AlignVCenter, numShipsText);

    //If requested, draw the planet IDs.
    if(m_planetWarsView->getShowPlanetIds()) {
        std::stringstream streamPlanetId;
        streamPlanetId << m_planet->getId();

        QString qPlanetId(streamPlanetId.str().c_str());

        painter->setPen(m_settings->planetIdColor);
        painter->setFont(m_settings->planetIdFont);
        painter->drawText(this->boundingRect(), Qt::AlignLeft|Qt::AlignTop, qPlanetId);
    }

    //If requested, draw additional planet property data.
    if (m_planetWarsView->getShowPlanetProps()) {
        std::stringstream streamPlanetProps;
        std::vector<std::string> propNames = m_planet->getPropNames();
        if (propNames.size()) {
            for (uint i = 0; i < propNames.size(); ++i) {
                streamPlanetProps << propNames[i] << ": " 
                    << m_planet->getProperty(propNames[i]) << endl;
            }

            QString qPlanetProps(streamPlanetProps.str().c_str());

            painter->setPen(QColor("white"));
            painter->setFont(m_settings->fleetFont);
            painter->drawText(center + QPoint(8, -24), qPlanetProps);
        }
    }

}

/*===================================================
                Class FleetView.
====================================================*/
FleetView::FleetView()
    :m_arrow(NULL) {
    m_connector = new FleetConnector(this);

}

FleetView::~FleetView() {
    delete m_connector;
    delete m_arrow;
}

void FleetView::setFleet(Fleet* fleet) {
    m_fleet = fleet;
    m_connector->setFleet(fleet);

    //Set up the arrow.
    const qreal scalingFactor = m_settings->scalingFactor;
    m_arrow = new QGraphicsPolygonItem(this);

    QPolygonF arrowPolygon;
    arrowPolygon << QPointF(0.5 * scalingFactor, 0)
            << QPointF(0.4 * scalingFactor, 0.2 * scalingFactor)
            << QPointF(0.9 * scalingFactor, 0)
            << QPointF(0.4 * scalingFactor, -0.2 * scalingFactor);

    m_arrow->setPolygon(arrowPolygon);

    //Set the arrow pen and color.
    if (m_fleet->getOwner()->getId() == 1) {
        m_arrow->setBrush(m_settings->firstPlayerFleetColor);
        m_arrow->setPen(m_settings->firstPlayerFleetPen);

    } else {
        m_arrow->setBrush(m_settings->secondPlayerFleetColor);
        m_arrow->setPen(m_settings->secondPlayerFleetPen);
    }

    //Set the arrow angle.
    const qreal sourceX = fleet->getSource()->getX();
    const qreal sourceY = fleet->getSource()->getY();
    const qreal destinationX = fleet->getDestination()->getX();
    const qreal destinationY = fleet->getDestination()->getY();

    QLineF trip(sourceX, sourceY, destinationX, destinationY);
    m_arrow->setRotation(-trip.angle());

}

QRectF FleetView::boundingRect() const {
    //return bounding rect.
    const qreal baseRadius = 1 * m_settings->scalingFactor;
    QRectF rect(-baseRadius, -baseRadius, baseRadius*2, baseRadius*2);
    return rect;
}

void FleetView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    const qreal scalingFactor = m_settings->scalingFactor;

    //Set the fleet position.
    const qreal x = static_cast<qreal>(m_fleet->getX()) * m_settings->scalingFactor;
    const qreal y = static_cast<qreal>(m_fleet->getY()) * m_settings->scalingFactor;

    this->setPos(x, y);

    //Draw the number of ships
    std::stringstream sNumShips;
    sNumShips << m_fleet->getNumShips();
    QString qNumShips(sNumShips.str().c_str());

    QColor fleetColor = m_fleet->getOwner()->getId() == 1 ? m_settings->firstPlayerFleetColor : m_settings->secondPlayerFleetColor;
    painter->setPen(fleetColor);
    painter->setFont(m_settings->fleetFont);

    const qreal baseRadius = 1 * scalingFactor;
    QRectF rect(-baseRadius, -baseRadius, baseRadius*2, baseRadius*2);
    painter->drawText(rect, Qt::AlignHCenter|Qt::AlignVCenter, qNumShips);

    //The arrow is already set up.
    //Paint the arrow.
//    m_arrow->paint(painter, option, widget);
}

void FleetView::removeSelf() {
    m_planetWarsView->removeFleetView(this);
}

/*===================================================
              Class FleetConnector.
====================================================*/
FleetConnector::FleetConnector(FleetView *fleetView)
    :QObject(NULL), m_fleetView(fleetView) {
}

void FleetConnector::setFleet(Fleet *fleet) {
    m_fleet = fleet;
    QObject::connect(m_fleet, SIGNAL(destroyed()), this, SLOT(removeFleet()));
}

void FleetConnector::removeFleet() {
    m_fleetView->removeSelf();
}

/*===================================================
              Class GraphicsSettings.
====================================================*/
GraphicsSettings::GraphicsSettings(QObject* parent)
    :QObject(parent) {
}
