#ifndef NAVMESH_H
#define NAVMESH_H

#include <QVector>
#include <QLine>

#include "node.h"

class QPoint;
class QPainter;

class NavMesh
{
    public:

        NavMesh();
        ~NavMesh();

        void addNode(const QPoint& position);
        void addArc( Node* from, Node* to);
        void removeArc(Node* from, Node* to);
        Node* find( QPoint position );
		Node* findNearest( QPoint position );
        Node* searchDepthFirst(Node *start, Node* goal);
        Node* EulerianNext(Node *start);
        bool joint(Arc*);	
        void render(QPainter&);
	    bool Conected();
        QVector<QPoint> getNodes();
        QVector<QLine> getArcs();
        void reset();
        Arc* reverse(Arc* arc );
    
    private:
        friend class Simulator;        
        // Grafo
        typedef QVector<Node*> Graph;
        Graph _vertices;

};

#endif
