#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "object.h"

class QPainter;
class QPoint;

class Obstacle : public Object
{
public:
	Obstacle(const QPoint& point, int r);
	virtual ~Obstacle();
    virtual void render(QPainter&);
	bool m_catched;
};

#endif
