#include "obstacle.h"

#include <QPoint>
#include <QPainter>

Obstacle::Obstacle(const QPoint& point, int r)
    :
	 m_catched(false),
    Object(point,r)
	 
{
}

Obstacle::~Obstacle(void)
{
}


void Obstacle::render( QPainter& painter )
{
    int m_pointSize = 10;
    const QPoint* point = Center();
    painter.drawEllipse(QRectF(point->x() - m_pointSize,
                                point->y() - 2*m_pointSize,
                                2*m_pointSize,
                                4*m_pointSize));
}
