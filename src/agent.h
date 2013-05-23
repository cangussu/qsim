#ifndef AGENT_H
#define AGENT_H

#include "object.h"
#include "node.h"
#include "obstacle.h"
#include <QVector>
#include <QPoint>
#include <QDateTime>

class Simulator;

enum TIPO_NAVEGACAO {CAMPO_POTENCIAL, PERSEGUICAO_FUGA, ENTRY_SEARCH,NEXT_NODES, NONE };
enum STATE {DRINK_BEER, DRUNK, SEARCH_METEORITO, CATCH_METEORITO, GO_BACK_IGC};

const float TIME_TO_DRINK_BEER = 3.0f; // 3 segundos pra encher a cara! 
const float TIME_DRUNK = 7.0f; // 5 segundos pra voltar ficar sobrio! 

class Agent : public Object
{
    private:
        Simulator* m_simulator;
        Node* m_goal;
        Node* m_currentNode;
        STATE m_agent_state;
        QPoint m_gradient;
        bool goal_changed;
        bool m_storeTrajectory;
        int m_pointSize;
        int _state;
		  bool m_catched_meteorito;
		  int m_sensor_radio;
	     Obstacle* m_meteorito;
   	  QPoint m_pos_goal;
		  float FACTOR_RANDOM;
		  int m_times_drunk_in_icb;
		  STATE m_last_state;

		  QDateTime m_started_drink_beer;

        QVector<QPoint> m_trajectory;

        enum State {
            ENTER_NAVIGATION_MESH = 0,
            GOTO_NODE,
            FIND_NEXT_NODE,
            STUCK
        };

		  void walk_in_graph();
   	  Node* reset_graph();
		  bool catched_meteorito( Obstacle* meteorito );
		  Obstacle* there_is_meteorito_near();




    protected:
        void drink_beer();
        void drunk();
        void search_meteorito();
        void catch_meteorito();
        void go_back_igc();
	     bool ICB_near();
 
    public:
        Agent( Simulator* sim, const QPoint& point, int r );
        ~Agent();
        void storeOldPos() { m_oldpos = m_center; }
        QPoint m_oldpos;
        virtual void setPos( const QPoint& newpos );

		QPoint getGoalPos(){ return m_goal->getPos();}
		
		//navigation
		Node* getNode(){return m_goal;}


        QVector<QPoint>& trajectory() { return m_trajectory; }
        
        virtual void render(QPainter&);
        
        void think();
};

#endif
