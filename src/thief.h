#ifndef THIEF_H
#define THIEF_H

#include <QtGlobal>
#include <QStack>
#include "navmesh.h"
#include "agent.h"

/** 
    @class Thief
    
    Special Agent Marcos Valério, o ladrão de meteoritos!
*/
class Thief : public Agent
{
    public:
        Thief(Simulator* sim, const QPoint& point, int r);
        virtual ~Thief();

        virtual void render(QPainter&);
        virtual void think();

    protected:

        void start();
        void seek();
        void chase();
        void steal();
        void navigate();
        void gotoGoal();
        
        /** Sensor de agentes
        
            @return Retorna o agente mais próximo no raio do sensor ou NULL caso
                    não haja nenhum.
         */
        Agent* senseAgent();

        enum ThiefState
        {
            START,
            GOTO,
            SEEK,
            CHASE,
            STEAL
        };


        qreal _sensorRadius;
        Agent* _victim;
        int m_pointSize;
        int _state;
        NavMesh& _map;
        QStack<Node*> _stack;
};

#endif
