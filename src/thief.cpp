#include "thief.h"
#include "navmesh.h"
#include "simulator.h"

#include <QLineF>
#include <QPainter>

Thief::Thief(Simulator* sim, const QPoint& point, int r)
    : Agent(sim, point, r),
      m_pointSize(10),
      _state(START),
      _sensorRadius(20),
      _map(m_simulator->getNavMesh())
{
    // Copia a navmesh, assim podemos fazer de tudo com ela
    
}

Thief::~Thief()
{
}

void Thief::think()
{
    switch( _state )
    {
        case START:
            start();
            break;
        
        case GOTO:
            gotoGoal();
            break;

        case SEEK:
            seek(); // Procura por uma vítima
            break;
        
        case CHASE:
            chase(); // Persegue uma vítima
            break;
            
        case STEAL:
            steal(); // Rouba o meteorito
            break;
    }
}

// Facilita o acesso à malha de navegação
//#define m_simulator->getNavMesh() MESH

void Thief::steal()
{
}

void Thief::chase()
{
    //setPos( Agent::chase( getPos(), _victim->getPos() ) );
    if ( catched_meteorito( _victim ) )
    {
        _victim->steal();
        _state = SEEK;
    }
}

void Thief::seek()
{
    navigate();
}

void Thief::start()
{
    _map.reset();
    m_goal = m_simulator->getNavMesh().findNearest( getPos() );
    _stack.push(m_goal);
    _state = GOTO;
}

void Thief::gotoGoal()
{    
    if( !reached( *m_goal->Center(), getPos(), 10) )
    {
        // não chegou no nó, continua
        setPos( Agent::chase( getPos(), m_goal->getPos() ) );
    }
    else
    {
        // chegou
        m_currentNode = m_goal;
        _state = SEEK;
    }
}


Agent* Thief::senseAgent()
{
    qreal best_distance = _sensorRadius;
    Agent* selected_agent = 0;

    // para todos os agentes...
    for( ObjectGroup<Agent*>::iterator a = m_simulator->getAgents().begin();
         a != m_simulator->getAgents().end();
         ++a )
    {
        // Calcula a distância entre o o ladrão e o agente
        qreal current_agent_distance = QLineF( m_center, *(*a)->Center()).length();

        if ( current_agent_distance < best_distance )
        {
            selected_agent = (*a);
            best_distance = current_agent_distance;
        }
    }
    return selected_agent;
}

void Thief::render( QPainter& painter )
{
     const QPoint* point = Center();
     painter.setPen(Qt::red);
     painter.setBrush(Qt::red);
     painter.drawEllipse(QRectF(point->x() - m_pointSize,
                         point->y() - m_pointSize,
                         m_pointSize*2, m_pointSize*2));
}

void Thief::navigate()
{
    AdjacencyList::Iterator arc;
    for( arc = m_currentNode->adjacents.begin(); arc != m_currentNode->adjacents.end(); ++arc )
    {
        Arc* ab = *arc;
        Arc* ba = _map.reverse(ab);
        
        ab->visited = ba->visited = true;
        if( ! _map.joint(ab) )
        {                
            m_goal = (*arc)->to;
            _state = GOTO;
            return;
        }
        ab->visited = ba->visited = false;
    }
    _map.reset();
}

// void Thief::navigate()
// {
//     // Procura um nó não visitado:
//     while( !_stack.empty() )
//     {
//         Node* node = _stack.pop();
//         if ( node->visited() ) 
//             continue; // já foi visitado, pega o próximo
// 
//         // visita
//         //node->setVisited(true);
//         m_goal = node;
// 
//         // expande
//         AdjacencyList::Iterator arc;
//         for( arc = node->adjacents.begin(); arc != node->adjacents.end(); ++arc )
//         {
//             if ( ! (*arc)->to->visited() )
//                 if( (*arc)->to != node )
//                     _stack.push( (*arc)->to );
//         }
//         _state = GOTO;
//         return;
//     }
//     // não restam mais nós, começa denovo
//     _map.reset();
//     _stack.push(m_currentNode);
// }


// void Thief::seek()
// {
//     Agent* agent = senseAgent();
//     if ( agent != NULL )
//     {
//         if ( agent->m_catched_meteorito )
//         {
//             reset_graph();
//             _state = CHASE;
//             _victim = agent;
//         }
//     }
//     else
//         walk_in_graph();
// }
