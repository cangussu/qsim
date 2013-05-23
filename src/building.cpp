#include "building.h"
#include <QPainter>

Building::Building( const QRect& rect) 
    : Object( QPoint( 0,0), 1 ),
      _rect(rect)
{
}

Building::~Building()
{
}

void Building::render(QPainter& painter)
{
	if(isICB) 
		painter.setBrush(Qt::magenta);
	else painter.setBrush(Qt::blue);

 	painter.drawRect(_rect);
   /// @todo implement me
}
