#ifndef GOAL_H
#define GOAL_H

#include "object.h"

class QPoint;

class Goal : public Object
{
    public:
        Goal(const QPoint& point, int raio);
        ~Goal();
        virtual void render(QPainter&);
        int m_pointSize;
};

#endif
