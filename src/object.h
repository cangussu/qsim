#ifndef OBJECT_H
#define OBJECT_H

#include <QPoint>
class QPainter;

class Object
{
public:

    Object(const QPoint& center, int radius);
    virtual ~Object();

    const QPoint* Center() const { return &m_center; }
    virtual void setPos( const QPoint& newpos ) { m_center = newpos; }
    inline const QPoint& getPos() { return m_center; }

    inline void setRadius( int r ) { m_radius = r; }
    
    virtual void render(QPainter&)=0;
    
//     QDataStream& operator<<(QDataStream& out) { 
//         //out << m_center;
//         return out;
//     }
//     
//     QDataStream& operator>>(QDataStream& in) { 
//         in >> m_center;
//         return in;
//     }
    
protected:
        QPoint m_center;
        int    m_radius;
};

QDataStream &operator<<( QDataStream & out, const Object* obj );
QDataStream &operator>>( QDataStream & in, Object* obj );

#endif
