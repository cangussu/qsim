#include "agent.h"
#include <QPainter>
#include <math.h>
#include "building.h"
#include <QMessageBox>

#include "simulator.h"

Agent::Agent( Simulator* sim, const QPoint& point, int r )
    :
    Object(point,r),
    m_simulator(sim),
    m_oldpos(point),
    m_storeTrajectory(false),
    m_pointSize(10),
	_state(0),
    m_agent_state(SEARCH_METEORITO),
	 m_catched_meteorito(false),
	 m_sensor_radio(120),
	 m_currentNode(NULL),
	 m_meteorito(NULL),
	 FACTOR_RANDOM(0.0),
	 m_times_drunk_in_icb(0),
	 goal_changed(true)
{
}

Agent::~Agent()
{
}

bool Agent::ICB_near()
{
   for( ObjectGroup<Building*>::iterator build = m_simulator->m_buildings.begin(); 
						 build != m_simulator->m_buildings.end(); ++build )
	{
		if ( (*build)->isICB )
		{
			int x, y, w, h;
	  		QPoint current = Object::getPos();
			(*build)->_rect.getRect( &x, &y, &w, &h );
			x += w/2;
			y += h/2;
 			if( sqrt(pow(current.x() -x ,2) + pow(current.y() - y,2)) < m_sensor_radio )
			{
				return true;
			}
		}
	}
	return false;
}

void Agent::setPos( const QPoint& newpos )
{

	 
    if ( ICB_near() && m_agent_state != DRINK_BEER )
	 {
		reset_graph();
		int probability = 0;
		srand ( time(NULL) );
		if ( m_agent_state == DRUNK )
		{
			FACTOR_RANDOM = 3.5f;
		}
		else FACTOR_RANDOM = 0.0f;

		if ( m_agent_state == GO_BACK_IGC )
			probability = (rand()%30);
		else if ( m_agent_state == SEARCH_METEORITO )
			probability = (rand()%10);
		else if ( m_agent_state == DRUNK )
			probability = (rand()%1);

		if ( probability > 0  && probability < 4 )
		{
			m_last_state = m_agent_state;
		 	m_agent_state = DRINK_BEER;
			m_started_drink_beer = QDateTime::currentDateTime();
		}
	 }
	 
    if( m_storeTrajectory ) {
        m_trajectory += newpos;
    }
    Object::setPos(newpos);
}

void Agent::go_back_igc()
{
    switch( _state )
    {
        case ENTER_NAVIGATION_MESH:
        {
            m_goal = m_simulator->getNavMesh().findNearest( getPos() );
            m_goal->setVisited(false);
            m_gradient = m_simulator->getGradient( *m_goal->Center() );
            _state = GOTO_NODE;
        }
        break;

        case STUCK:    
		  
					 if (m_meteorito != NULL) 
			  			delete m_meteorito;
					 m_meteorito = NULL;
					 m_catched_meteorito = false;
  			 		 reset_graph();
  		 			 m_last_state = m_agent_state;
					 m_agent_state = SEARCH_METEORITO;
					 _state = ENTER_NAVIGATION_MESH;
					 
					 break; 
					
		  case GOTO_NODE:
        {
            QPoint current = Object::getPos();
            QPoint pos_goal = m_goal->getPos();

            // Verifica se o objetivo já foi atingido
            if( (pow((current.x() - m_goal->getPos().x()),2) +
                 pow((current.y() - m_goal->getPos().y()),2) ) > m_pointSize*2 )
            {
                // Não foi, continua perseguindo
                setPos( m_simulator->chase( getPos(), m_goal->getPos() ) );
            }
            else {
                // Chegou no objetivo, 
                m_currentNode = m_goal;
                _state = FIND_NEXT_NODE;
            }
        }
        break;

        case FIND_NEXT_NODE:
        {
            // Pega a lista de nós adjacentes a posição atual.
            /// @todo Verificar se o ponteiro m_currentNode é válido (se é que é necessário)
            Node* old_goal = m_goal;
            AdjacencyList::Iterator arc;
            for( arc = m_currentNode->adjacents.begin(); arc != m_currentNode->adjacents.end(); ++arc )
            {
                (*arc)->to->setColor(Qt::red);
                QPoint ajacent_pot = m_simulator->getGradient( (*arc)->to->getPos() );
                QPoint current_pot = m_simulator->getGradient( m_center );
                double potencial_adjacente = (pow(ajacent_pot.x(),2) + pow(ajacent_pot.y(),2));
                double potencial_atual = (pow(current_pot.x(),2) + pow(current_pot.y(),2));
                if( potencial_adjacente < potencial_atual )
                {
                    m_goal  = (*arc)->to;
                }
                else
                    (*arc)->to->setColor(Qt::gray);
            }
            if( old_goal != m_goal )
                _state = GOTO_NODE;
            else
                _state = STUCK;
        }
        break;
    }
}

void Agent::drink_beer()
{
  ObjectGroup<Building*>::iterator build;
  QDateTime now;

	for( build = m_simulator->m_buildings.begin(); 
						 	build != m_simulator->m_buildings.end(); ++build )
	{
		if ( (*build)->isICB )
		{
	  		QPoint agentPos = Object::getPos();
			int x, y, w, h;
			(*build)->_rect.getRect( &x, &y, &w, &h );
			x += w/2;
			y += h/2;
			const QPoint point(x,y);
			
			if ((*build)->_rect.contains(agentPos))
			{
				if ( m_last_state == DRUNK )
					m_times_drunk_in_icb++;
				now = QDateTime::currentDateTime();
				break;
			}
			else setPos(m_simulator->chase(agentPos, point));
		}
	}
	if (build != m_simulator->m_buildings.end())
	{
		if ( m_started_drink_beer.secsTo(now) >  TIME_TO_DRINK_BEER+FACTOR_RANDOM )
		{
		  	m_started_drink_beer = QDateTime::currentDateTime();
			reset_graph();
   		m_last_state = m_agent_state;
			m_agent_state = DRUNK;
			_state = ENTER_NAVIGATION_MESH;	
			
		}
	}
}

void Agent::drunk()
{
  QDateTime now = QDateTime::currentDateTime();
	if ( m_started_drink_beer.secsTo(now) >  TIME_DRUNK )
	{
		reset_graph();
		if ( m_times_drunk_in_icb > 3 )
		{
			m_times_drunk_in_icb = 0;
			m_catched_meteorito = false;
			m_simulator->addObstacle( m_meteorito->getPos() );

			if (m_meteorito != NULL)
			{
				delete m_meteorito;
				m_meteorito = NULL;
			}
		}
		if ( m_catched_meteorito )
		{
  			m_last_state = m_agent_state;
		  m_agent_state = GO_BACK_IGC;
		_state = ENTER_NAVIGATION_MESH;	
		}
		else
		{
  			m_last_state = m_agent_state;
		  m_agent_state = SEARCH_METEORITO;
			_state = ENTER_NAVIGATION_MESH;	
		}
	}
	walk_in_graph();
	QPoint point(getPos().x()+1, getPos().y()+1);
	setPos( point );
}

Obstacle* Agent::there_is_meteorito_near()
{

	for( ObjectGroup<Obstacle*>::iterator meteorito = m_simulator->m_obstacles.begin(); 
						 	meteorito != m_simulator->m_obstacles.end();	++meteorito )
   {
	   const QPoint* point = (*meteorito)->Center();
  		QPoint current = Object::getPos();
		if( sqrt(pow((current.x() - point->x()),2) +
           pow((current.y() - point->y()),2) ) < m_sensor_radio &&
		 	!(*meteorito)->m_catched )
		{
			return (*meteorito);
		}
	}
	return NULL;
}

bool Agent::catched_meteorito( Obstacle* meteorito )
{
   QPoint current = Object::getPos();
	if( sqrt(pow((current.x() - meteorito->getPos().x()),2) +
   		pow((current.y() - meteorito->getPos().y()),2) ) < m_pointSize )
	{
		return true;
	}
	return false;
}

Node* Agent::reset_graph()
{
	AdjacencyList::Iterator arc;
	for( arc = m_currentNode->adjacents.begin(); arc != m_currentNode->adjacents.end(); ++arc )
	{
		if ( (*arc)->to != NULL)
			(*arc)->to->setVisited(false);
		
		if ( (*arc)->from != NULL)
			(*arc)->from->setVisited(false);
	}
   return m_simulator->getNavMesh().findNearest( getPos() );
}

void Agent::walk_in_graph()
{
	if (m_currentNode == NULL)
       m_currentNode = m_simulator->getNavMesh().findNearest( getPos() );
	else
	{			  
		setPos(m_simulator->chase( getPos(), m_currentNode->getPos() ));
  	 	QPoint current = Object::getPos();
		if ( sqrt(pow((current.x() - m_currentNode->getPos().x()),2) +
   		pow((current.y() - m_currentNode->getPos().y()),2) ) < m_pointSize )
		{
			Node* node = NULL;
	     	AdjacencyList::Iterator arc;
	      for( arc = m_currentNode->adjacents.begin(); arc != m_currentNode->adjacents.end(); ++arc )
			{
				if ( (*arc)->to != NULL )
				{
					node = (*arc)->to;
				}
				else if ( (*arc)->from != NULL )
				{
					node = (*arc)->from;
				}
				if ( node->visited() )
					node = NULL;
				else break;
				
			}
			if (node == NULL)
				node = reset_graph();

			m_currentNode = node;
			node->setVisited(true);
		}
	}
}

void Agent::catch_meteorito()
{
	if ( m_meteorito->m_catched )
	{
		m_last_state = m_agent_state;
 		m_agent_state = SEARCH_METEORITO;
		_state = ENTER_NAVIGATION_MESH;	
	}
			
	if ( catched_meteorito(m_meteorito) )
	{
		// remove o objeto meteorito da lista do simulador
		m_meteorito->m_catched = true;
		for( ObjectGroup<Obstacle*>::iterator it = m_simulator->m_obstacles.begin(); it != 
												  m_simulator->m_obstacles.end(); ++it )
    	{
      	if( *it == m_meteorito ) 
			{
      		m_simulator->m_obstacles.erase(it);
				break;
			}
       }

		m_catched_meteorito = true;
		reset_graph();
		m_last_state = m_agent_state;
		m_agent_state = GO_BACK_IGC;
	}
	//else	setPos(m_simulator->chase( getPos(), m_meteorito->getPos() ));
}

void Agent::search_meteorito()
{
	Obstacle* meteorito = there_is_meteorito_near();
	if ( meteorito != NULL )
	{
		reset_graph();
		m_last_state = m_agent_state;
		m_agent_state = CATCH_METEORITO;
		m_meteorito = meteorito;
	}
	else	walk_in_graph();
}

void Agent::think()
{
	switch( m_agent_state )
	{
		case DRINK_BEER:
				 drink_beer();
				 break;
		case DRUNK:
				 drunk();
				 break;
		case SEARCH_METEORITO:
				 search_meteorito();
				 break;
		case CATCH_METEORITO:
				 catch_meteorito();
				 break;
		case GO_BACK_IGC:
				 go_back_igc();
				 break;
	}
}

void Agent::render( QPainter& painter )
{
	const QPoint* point = Center();
	
	painter.setPen(Qt::black);
	if ( m_agent_state == DRINK_BEER )
		painter.setBrush(Qt::darkYellow);
	else if ( m_agent_state == DRUNK || m_last_state == DRUNK )
		painter.setBrush(Qt::darkMagenta);
	else if ( m_agent_state == CATCH_METEORITO )
		painter.setBrush(Qt::yellow);
	else if ( m_catched_meteorito )
		painter.setBrush(Qt::cyan);
	else painter.setBrush(Qt::white);
	
	painter.drawEllipse(QRectF(point->x() - m_sensor_radio,
                        point->y() - m_sensor_radio,
                        m_sensor_radio*2, m_sensor_radio*2));

  if( m_storeTrajectory )
  {
	 painter.setPen(Qt::green);
          
        for( QVector<QPoint>::Iterator i = trajectory().begin();
             i != trajectory().end();
             ++i )
        {
            painter.drawEllipse(QRectF(i->x() - m_pointSize/2,
                                i->y() - m_pointSize/2,
                                m_pointSize, m_pointSize));
        }
    }
	 painter.setBrush(Qt::green);
    painter.setPen(Qt::black);
    painter.drawEllipse(QRectF(point->x() - m_pointSize,
                        point->y() - m_pointSize,
                        m_pointSize*2, m_pointSize*2));
	 

}

/*
TIPO_NAVEGACAO Agent::getNavigationType()
{
    if( state == 0)
{
        state = 1;
        return ENTRY_SEARCH;
}
    else if(state == 1)
{   
        QPoint current = Object::getPos();
        QPoint pos_goal = m_goal->getPos();

        if( (pow((current.x() - m_goal->getPos().x()),2) + pow((current.y() - m_goal->getPos().y()),2) ) > 20)
            return PERSEGUICAO_FUGA;
        else state = 2;
}
    else if(state == 2)
{
        state = 3;
        return NEXT_NODES;
}
    else if(state == 3) 
{
        QPoint current = Object::getPos();
        QPoint pos_goal = m_goal->getPos();

        if( (pow((current.x() - m_goal->getPos().x()),2) + pow((current.y() - m_goal->getPos().y()),2) ) > 20)
            return PERSEGUICAO_FUGA;

        else state = 0;
}
}*/
