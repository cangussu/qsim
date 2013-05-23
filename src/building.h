#ifndef BUILDING_H
#define BUILDING_H

#include <object.h>
#include <QRect>
//#include <QtGlobal>

class Building : public Object
{
    public:
		  bool isICB;
        Building( const QRect& rect);
        ~Building();
        virtual void render(QPainter& painter);
        
        virtual void setPos( const QPoint& newpos ) { _rect.moveCenter(newpos); }
        QRect _rect;
};

#endif
