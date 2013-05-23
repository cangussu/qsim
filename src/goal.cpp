#include "goal.h"

#include <QPainter>
#include <QPoint>

Goal::Goal( const QPoint& point, int r )
    :
    Object(point,r),
    m_pointSize(10)
{
}


Goal::~Goal()
{
}

void Goal::render( QPainter& painter )
{
    const QPoint* point = Center();
    painter.setPen(Qt::black);
    painter.drawEllipse(QRectF(point->x() - m_pointSize,
                        point->y() - m_pointSize,
                        m_pointSize*2, m_pointSize*2));
}
