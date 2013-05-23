#include <QTimer>
#include <QFile>
#include <QDataStream>

#include "thief.h"
#include "simulator.h"
#include "agent.h"
#include "goal.h"
#include "obstacle.h"
#include "building.h"
#include "renderarea.h"

#include <math.h>

Simulator::Simulator( int x, int y )
    :
    m_sizeX(x),
    m_sizeY(y)
{
    m_timer    = new QTimer(this);
    m_interval = 100;

    // evento de timeout() dá um step no simulador
    connect(m_timer, SIGNAL(timeout()), this, SLOT(step()));
}

Simulator::~Simulator()
{
      delete m_timer;
}

void Simulator::setStepInterval( int msec )
{
    m_timer->setInterval( msec );
}


void Simulator::start()
{
    m_timer->start( m_interval );
}

void Simulator::stop()
{
    m_timer->stop();
}

void Simulator::pause()
{
    m_timer->stop();
}

void Simulator::reset()
{
    stop();
    m_agents.clear();
    m_obstacles.clear();
    m_goals.clear();
    m_buildings.clear();
    
}

void Simulator::inTorus( QPoint& pt )
{
    pt.setX(pt.rx() % m_sizeX);
    pt.setY(pt.ry() % m_sizeY);
}


float Simulator::gradx(float x,float y)
{
	float sum = 0;

    for( ObjectGroup<Goal*>::iterator it = m_goals.begin();
         it != m_goals.end();
         ++it
       )
    {
        const QPoint* point = (*it)->Center();
		//sum += (-10000 *(x-point->x())) /(sqrt( pow((x-point->x())*(x-point->x()) + (y-point->y())*(y-point->y()),3)) );
		  sum += -(2*(x-point->x()))/100;		
    }

    for( ObjectGroup<Obstacle*>::iterator it = m_obstacles.begin();
         it != m_obstacles.end();
         ++it
       )
    {
        const QPoint* point = (*it)->Center();
		sum += (10000 *(x-point->x())) /(sqrt( pow((x-point->x())*(x-point->x()) + (y-point->y())*(y-point->y()),3)) );
	}	
    
	return sum;
}

float Simulator::grady(float x, float y)
{
	float sum = 0;

    for( ObjectGroup<Goal*>::iterator it = m_goals.begin(); it != m_goals.end(); ++it )
    {
        const QPoint* point = (*it)->Center();
		//sum += (-10000 *(y-point->y())) /(sqrt( pow((y-point->y())*(y-point->y()) + (x-point->x())*(x-point->x()),3)) );
		sum += -(2*(y-point->y()))/100;		
    }
    
    for( ObjectGroup<Obstacle*>::iterator it = m_obstacles.begin(); it != m_obstacles.end(); ++it )
    {
        const QPoint* point = (*it)->Center();
		sum += (10000 *(y-point->y())) /(sqrt( pow((y-point->y())*(y-point->y()) + (x-point->x())*(x-point->x()),3)) );
    }
    return sum;
}

void Simulator::step()
{
    for( ObjectGroup<Agent*>::iterator a = m_agents.begin(); a != m_agents.end(); ++a )
    {
        (*a)->think();
    }
    
/*    for( ObjectGroup<Thief*>::iterator t = m_thiefs.begin(); t != m_thiefs.end(); ++t )
    {
        (*t)->think();
    }*/
}

bool Simulator::addAgent( const QPoint& position )
{
    m_agents += new Agent( this, position, 10 );
    return true;
}

bool Simulator::addThief( const QPoint& position )
{
    //m_thiefs += new Thief( this, position, 10 );
    return true;
}

bool Simulator::addGoal( const QPoint& position )
{
    //if ((m_navegationType == CAMPO_POTENCIAL) || (m_navegationType == PERSEGUICAO_FUGA && m_goals.size() == 0))
	    m_goals += new Goal( position, 10 );

	 return true;
}

bool Simulator::addObstacle( const QPoint& position )
{
    m_obstacles += new Obstacle( position, 50 );
    return true;
}

bool Simulator::addRectangle( const QRect& rect, bool isICB )
{
	 Building *build = new Building( rect );
	 build->isICB = isICB;
    m_buildings += build;
    return true;
}

void Simulator::addNode(const QPoint& position)
{
    m_navmesh.addNode(position);
}

void Simulator::addArc(const QPoint& from, const QPoint& to)
{
    Node* fromNode = m_navmesh.find(from);
    Node* toNode = m_navmesh.find(to);
    if(!toNode || !fromNode) return;
    m_navmesh.addArc(fromNode, toNode);
}

bool Simulator::removeAgent( const QPoint& position )
{
    return false;
}

bool Simulator::removeGoal( const QPoint& position )
{
    return false;
}

bool Simulator::removeObstacle( const QPoint& position  )
{
    return false;
}


QPoint Simulator::getGradient(const QPoint& pos)
{
    QPoint pot( gradx(pos.x(),pos.y()) ,grady(pos.x(),pos.y()) );
    return pot;
}

void Simulator::save( QString& filename )
{
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);   // we will serialize the data into the file
    
    // Salva os agentes
    QVector<QPoint> agents;
    for( ObjectGroup<Agent*>::iterator a = m_agents.begin(); a != m_agents.end(); ++a )
        agents += (*a)->getPos();
    out << m_agents;
    
    // Salva os predios
    QVector<QRect> buildings;
    for( ObjectGroup<Building*>::iterator b = m_buildings.begin(); b != m_buildings.end(); ++b )
        buildings += (*b)->_rect;
    
    out << buildings;
    
    // Salva os ICB
    
    // Salva os nodos
    QVector<QPoint> nodes = m_navmesh.getNodes();
    out << nodes;
    
    // Salva as arestas
    QVector<QLine> arcs = m_navmesh.getArcs();
    out << arcs;
}


bool Simulator::load( QString& filename )
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);    // read the data serialized from the file
    
    QVector<QPoint> agents;
    in >> agents;
    for( QVector<QPoint>::iterator a = agents.begin(); a != agents.end(); ++a )
        addAgent(*a);
    
    QVector<QRect> buildings;
    in >> buildings;
    for( QVector<QRect>::iterator b = buildings.begin(); b != buildings.end(); ++b )
    {
        addRectangle(*b, false);
    }
    
    QVector<QPoint> nodes;
    in >> nodes;
    for( QVector<QPoint>::iterator n = nodes.begin(); n != nodes.end(); ++n )
        m_navmesh.addNode(*n);
    
    QVector<QLine> arcs;
    in >> arcs;
    for( QVector<QLine>::iterator a = arcs.begin(); a != arcs.end(); ++a )
        m_navmesh.addArc( m_navmesh.find(a->p1()), m_navmesh.find(a->p2()) );
    
    
}
