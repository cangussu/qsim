#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QVector>
#include <QPoint>

#include "navmesh.h"

#include "thief.h"
#include "building.h"

class Goal;
class Agent;
class Obstacle;
class RenderArea;

class QTimer;
class QRect;
class QPainter;

template <typename ObjectType>
class ObjectGroup : public QVector<ObjectType>
{
    typedef QVector<ObjectType> BaseVector;
    typedef typename BaseVector::Iterator BaseIterator;

    public:
        void render(QPainter& painter) {
            for( BaseIterator it = this->begin(); it != this->end(); ++it )
                (*it)->render( painter );
        }
        
        inline void remove(ObjectType obj)
        {
            for( BaseIterator it = this->begin(); it != this->end(); ++it )
            {
                if( *it == obj ) {
                    delete (*it);
                    this->erase(it);
                    return;
                }
            }
        }
};

class Simulator : public QObject
{
    Q_OBJECT
public:
    QPoint chase(const QPoint&, const QPoint&) { return QPoint(0,0); }
    ObjectGroup<Obstacle*> m_obstacles;
    ObjectGroup<Building*> m_buildings;

	
    Simulator(int x, int y);
    virtual ~Simulator();
    
    inline void setView( RenderArea* view ) { m_view = view; };
    
    bool addAgent( const QPoint& position );
    bool addThief( const QPoint& position );
    bool addGoal( const QPoint& position );
    bool addObstacle( const QPoint& position );
    bool addRectangle( const QRect& rect, bool isICB );
    void addNode(const QPoint& position);
    void addArc(const QPoint& from, const QPoint& to);

    bool removeAgent( const QPoint& position );
    bool removeGoal( const QPoint& position );
	 bool removeObstacle( const QPoint& position  );

    inline ObjectGroup<Agent*>& getAgents() { return m_agents; }
    inline ObjectGroup<Thief*>& getThiefs() { return m_thiefs; }
    inline ObjectGroup<Goal*>& getGoals() { return m_goals; }
    inline ObjectGroup<Obstacle*>& getObstacles() { return m_obstacles; }
    inline ObjectGroup<Building*>& getBuildings() { return m_buildings; }

    inline NavMesh& getNavMesh(){ return m_navmesh; }

    void setStepInterval( int msec );

    float gradx(float x, float y);
	float grady(float  x,float y);
    
    QPoint getGradient(const QPoint& position);
    void save( QString& filename );
    bool load( QString& filename );
  
public slots:
    
    void start();
    void stop();
    void pause();
    void reset();
				
private slots:
 
     void step();

private:
    
    void inTorus( QPoint& pt );
    
    RenderArea* m_view;
    
    ObjectGroup<Agent*> m_agents;
    ObjectGroup<Thief*> m_thiefs;
    ObjectGroup<Goal*>  m_goals;
    NavMesh m_navmesh;
    
	    
    QTimer* m_timer;
    int m_interval;
    
    int m_sizeX;
    int m_sizeY;
};

#endif
