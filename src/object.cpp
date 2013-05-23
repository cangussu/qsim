#include "object.h"

Object::Object(const QPoint& c, int r)
    :
    m_center( c ),
    m_radius( r )
{
}

Object::~Object()
{
}

QDataStream &operator<<( QDataStream & out, const Object* obj )
{
    out << * obj->Center();
    return out;
}

QDataStream &operator>>( QDataStream & in, Object* obj )
{
    QPoint pos;
    in >> pos;
    obj->setPos(pos);
    return in;
}