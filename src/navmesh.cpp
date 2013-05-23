#include "navmesh.h"
#include "node.h"

#include <QPainter>
#include <QPoint>
#include <QStack>

NavMesh::NavMesh()
{
}

NavMesh::~NavMesh()
{
    for( Graph::Iterator v = _vertices.begin(); v != _vertices.end(); ++v )
    {
        AdjacencyList::Iterator arc;
        for( arc = (*v)->adjacents.begin(); arc != (*v)->adjacents.end(); ++arc )
            delete (*arc);
        delete (*v);
    }
}

void NavMesh::render(QPainter& painter)
{
    // Itera entre os vertices do grafo
    for( Graph::Iterator v = _vertices.begin(); v != _vertices.end(); ++v )
    {
        int m_pointSize = 10;
        const QPoint point = (*v)->getPos();
        painter.setPen(Qt::gray);
        
        painter.setBrush( (*v)->getColor() );

        painter.drawEllipse(QRectF(point.x() - m_pointSize,
                    point.y() - m_pointSize,
                    m_pointSize*2, m_pointSize*2));

        // Desenha os arcos partindo deste nó
        /// @todo Fazer com que um arco A<->B não seja desenhado 2x
        AdjacencyList::Iterator arc;
        for( arc = (*v)->adjacents.begin(); arc != (*v)->adjacents.end(); ++arc )
        {
            if( (*arc)->visited )
                painter.setPen(Qt::red);
            painter.drawLine( (*arc)->from->getPos(), (*arc)->to->getPos() );
        }
    }
}

Node* NavMesh::findNearest( QPoint position )
{
    qreal distance = -1;
    Node* node = 0;

    for( Graph::Iterator v = _vertices.begin(); v != _vertices.end(); ++v )
    {
        qreal d = QLineF(position, (*v)->getPos()).length();
        if ((distance < 0) || d < distance)
        {
            distance = d;
            node = (*v);
        }
    }
    return node;
}

Node* NavMesh::find( QPoint position )
{
    qreal distance = -1;
    for( Graph::Iterator v = _vertices.begin(); v != _vertices.end(); ++v )
    {
        qreal d = QLineF(position, (*v)->getPos()).length();
        if ((distance < 0 && d < 12) || d < distance)
            return (*v);
    }
    return 0;
}

void NavMesh::addNode(const QPoint& position)
{
    _vertices += new Node(position);
}

void NavMesh::removeArc(Node* from, Node* to)
{
    int found = 0;
    for( Graph::Iterator v = _vertices.begin(); v != _vertices.end(); ++v )
    {
        AdjacencyList::Iterator arc;
        if( (*v) == from )
        {
            for( arc = (*v)->adjacents.begin(); arc != (*v)->adjacents.end(); ++arc )
            {
                if( (*arc)->to == to ) 
                {
                    delete (*arc);
                    (*v)->adjacents.erase( arc );
                    found++;
                }
            }
        }
        else if( (*v) == to )
        {
            for( arc = (*v)->adjacents.begin(); arc != (*v)->adjacents.end(); ++arc )
            {
                if( (*arc)->to == from )
                {
                    delete (*arc);
                    (*v)->adjacents.erase( arc );
                    found++;
                }
            }
        }
        if( found == 2 ) break;
    }
}

void NavMesh::addArc(Node* from, Node* to)
{
    Graph::Iterator v;
    bool from_found = false, to_found = false;

    for( v = _vertices.begin(); v != _vertices.end(); ++v )
    {
        if ( ((*v)->getPos() == from->getPos()) && !from_found )
        {
            Arc * newarc = new Arc(from,to);
            // Adiciona "to" como adjacente à "from"
            (*v)->adjacents += newarc;
            from_found = true;
        }
        else if( ((*v)->getPos() == to->getPos()) && !to_found  )
        {
            Arc * newarc = new Arc(to,from);
            (*v)->adjacents += newarc;
            to_found = true;
        }
        // retorna se já achamos ambos
        if (from_found && to_found ) break;
    }
}

bool NavMesh::Conected()
{
    /// @todo implement me
}

bool NavMesh::joint(Arc* arc)
{
    if( searchDepthFirst( arc->from, arc->to ) )
        if ( searchDepthFirst( arc->to, arc->from ) )
            return false;
    return true;
}

Node* NavMesh::EulerianNext(Node *start)
{
    AdjacencyList::Iterator arc;
    for( arc = start->adjacents.begin(); arc != start->adjacents.end(); ++arc )
    {
        if( !joint(*arc) )
            return (*arc)->to;
    }
}

Node* NavMesh::searchDepthFirst(Node *start, Node* goal)
{
    QStack<Node*> stack;
    stack.push(start);
    while( !stack.empty() )
    {
        Node* node = stack.pop();
        if ( node->visited() )
            continue;
        if ( node == goal ) 
            return node;
        node->setVisited(true);
        AdjacencyList::Iterator arc;
        for( arc = node->adjacents.begin(); arc != node->adjacents.end(); ++arc )
        {
            if ( ! (*arc)->visited )
                stack.push( (*arc)->to );
        }
    }
    return 0;
}

QVector<QPoint> NavMesh::getNodes()
{
    QVector<QPoint> points;
    Graph::Iterator v;
    for( v = _vertices.begin(); v != _vertices.end(); ++v )
    {
        points += (*v)->getPos();
    }
    return points;
}

QVector<QLine> NavMesh::getArcs()
{
    QVector<QLine> arcs;
    Graph::Iterator v;
    for( v = _vertices.begin(); v != _vertices.end(); ++v )
    {
        AdjacencyList::Iterator arc;
        for( arc = (*v)->adjacents.begin(); arc != (*v)->adjacents.end(); ++arc )
        {
            arcs += QLine( (*arc)->from->getPos(), (*arc)->to->getPos() );
        }
    }
    return arcs;
}

void NavMesh::reset()
{
    for( Graph::Iterator v = _vertices.begin(); v != _vertices.end(); ++v ) 
    {
        (*v)->setVisited(false);
        AdjacencyList::Iterator arc;
        for( arc = (*v)->adjacents.begin(); arc != (*v)->adjacents.end(); ++arc )
            (*arc)->visited = false;
        
    }
}

Arc* NavMesh::reverse(Arc* arc )
{
    for( Graph::Iterator v = _vertices.begin(); v != _vertices.end(); ++v ) 
    {
        if( (*v) == arc->to )
            for( AdjacencyList::Iterator rev = (*v)->adjacents.begin(); rev != (*v)->adjacents.end(); ++rev )
                if( (*rev)->to == arc->from )
                    return (*rev);
    }
}
