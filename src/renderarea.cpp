#include <QtGui>
#include <QTimer>
#include <QLineF>
#include <QColorDialog>

#include <math.h>
#include "mainwindow.h"
#include "renderarea.h"
#include "simulator.h"
#include "agent.h"
#include "goal.h"
#include "obstacle.h"
#include "building.h"


RenderArea::RenderArea(Simulator* sim, QWidget *parent)
    :
        QWidget(parent),
        m_modified(false),
        scribbling(false),
        m_drawpot(true),
        m_agentColor(Qt::green),
        m_goalColor(Qt::red),
        m_obstacleColor(Qt::blue),
        m_simulation(sim)
{
    shape = Polygon;
    antialiased = true;

    myPenWidth = 1;
    m_pointSize = 10;
    myPenColor = Qt::blue;
    setBackgroundRole(QPalette::Base);

    m_renderLoopTimer = new QTimer(this);
    m_renderLoopTimer->setInterval( m_refreshRate = 20 );
    connect(m_renderLoopTimer, SIGNAL(timeout()), this, SLOT(update()));
    m_renderLoopTimer->start();
//    grabKeyboard();
    setMouseTracking(true);
    setColors();
}

RenderArea::~RenderArea()
{
    delete m_renderLoopTimer;
}

void RenderArea::cleanArea()
{
	QPainter painter(&pixmap);
	QRect rec;

	rec = painter.viewport();
	painter.setBrush(QColor(255,255,255));	
	painter.drawRect(rec);
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RenderArea::sizeHint() const
{
    return QSize(400, 200);
}

void RenderArea::setShape(Shape shape)
{
    this->shape = shape;
    update();
}

void RenderArea::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void RenderArea::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void RenderArea::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

void RenderArea::setTransformed(bool transformed)
{
    this->transformed = transformed;
    update();
}

void RenderArea::AgentColor()
{
    m_agentColor = QColorDialog::getColor(Qt::green, this);
    setColors();
}

void RenderArea::GoalColor()
{
    m_goalColor = QColorDialog::getColor(Qt::red, this);
    setColors();
}

void setColor(QColor * c, int index)
{
    int h=120,s=255;
    int b=0;
    
    c->setRgb(255,0,0);	
    
    for(h=120;h<359;h++,b++)
        if(b > index)
        {
            c->setHsv(h,s,255);	
            return;
        }

}

float RenderArea::func(int x, int y, int ofx, int ofy, int q)
{
	float z;

	z= (sqrt(pow(x-ofx,2) + pow(y-ofy,2)));

	if(z !=0 ) z =q/z;
	
	if(z > 2000) z = 2000;
	else 
	if(z < 0) z = 0;

	return z;

}

float RenderArea::pot(int x, int y)
{
	float sum=3;

    ObjectGroup<Obstacle*> obstacles = m_simulation->getObstacles();
    for( ObjectGroup<Obstacle*>::iterator it = obstacles.begin();
         it != obstacles.end();
         ++it
       )
    {
        const QPoint* point = (*it)->Center();
        sum += func(x,y,point->x(),point->y(),200);

    }

    // Desenha todos os goals
    ObjectGroup<Goal*> goals = m_simulation->getGoals();
    for( ObjectGroup<Goal*>::iterator it = goals.begin();
         it != goals.end();
         ++it
       )
    {
        const QPoint* point = (*it)->Center();
        sum -= func(x,y,point->x(),point->y(),200);
    }

	return sum;

}

void RenderArea::drawPotential()
{
    if( !m_drawpot ){
        pixmap.fill();
        return;
    }

    if ( m_navegationType != CAMPO_POTENCIAL ) return;

    QPainter painter(&pixmap);
	QRect rec;
	QColor c;

	rec = painter.viewport();

	int x,y,h;
	float z;

	h= 0;
	for(x=rec.left();x<rec.right();x+=10)
		for(y=rec.top();y<rec.bottom();y+=10)
		{
			z = pot(x+5,y+5);
            setColor( &c, log(z)*70 );
			painter.setBrush(c);	
			painter.drawRect(x,y,10,10);
		}
    m_modified = true;
    update();
}

void RenderArea::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(Qt::black);
    QRect rect = painter.viewport();

    if( m_modified )
	    drawPotential();
    painter.drawPixmap( QPoint(0, 0), pixmap );

    // Desenha todos os agentes
    painter.setBrush(m_agentColor);
    m_simulation->getAgents().render(painter);
    
    // Desenha os ladrões
    m_simulation->getThiefs().render(painter);

    // Desenha todos os goals
    painter.setBrush(m_goalColor);
    m_simulation->getGoals().render(painter);

    // Desenha todos os obstáculos
    painter.setBrush(m_obstacleColor);
    m_simulation->getObstacles().render(painter);

    // Desenha os prédios
    m_simulation->getBuildings().render(painter);

    // Desenha a malha de navegação
    m_simulation->getNavMesh().render(painter);

    // Desenha o prédio temporário
    if ( (m_mode == addRect) && (scribbling == true) )
    {
        painter.setBrush(_building_tmp_brush);
        painter.drawRect(tmprect.normalized());
    }

    // Desenha o ICB temporário
    if ( (m_mode == addICB) && (scribbling == true) )
    {
        painter.setBrush(_ICB_tmp_brush);
        painter.drawRect(tmprect.normalized());
    }

    // Desenha arco temporário
    if ( (m_mode == addArc)  && (scribbling == true))
    {
        painter.drawLine(arcFrom, arcTo);
    }
    
    m_modified = false;
    return;
}

void RenderArea::Chasing()
{
	m_navegationType = PERSEGUICAO_FUGA;
}

void RenderArea::CampoPotencial()
{
	m_navegationType = CAMPO_POTENCIAL;
}

void RenderArea::pickElement(QMouseEvent *e)
{
    qreal distance = -1;

    // Goals
    ObjectGroup<Goal*>& goals = m_simulation->getGoals();
        for( ObjectGroup<Goal*>::iterator it = goals.begin();
            it != goals.end();
            ++it
        )
    {
        qreal d = QLineF(e->pos(), *(*it)->Center()).length();
        if ((distance < 0 && d < 12) || d < distance) {
            distance = d;
            _selectedObject = (*it);
            m_mode = movingObject;
        }
    }
    distance = -1;

    // Agents
    ObjectGroup<Agent*>& agents = m_simulation->getAgents();
    for( ObjectGroup<Agent*>::Iterator it = agents.begin();
            it != agents.end();
            ++it
        )
    {
        qreal d = QLineF(e->pos(), *(*it)->Center()).length();
        if ((distance < 0 && d < 12) || d < distance) {
            distance = d;
            _selectedObject = (*it);
            m_mode = movingObject;
        }
    }

    // Obstacles
    distance = -1;
    ObjectGroup<Obstacle*>& obstacles = m_simulation->getObstacles();
    for( ObjectGroup<Obstacle*>::iterator it = obstacles.begin();
            it != obstacles.end();
            ++it
        )
    {
        qreal d = QLineF(e->pos(), *(*it)->Center()).length();
        if ((distance < 0 && d < 12) || d < distance) {
            distance = d;
            _selectedObject = (*it);
            m_mode = movingObject;
        }
    }

    // Buildings
    distance = -1;
    ObjectGroup<Building*>& buildings = m_simulation->getBuildings();
    for( ObjectGroup<Building*>::iterator it = buildings.begin();
         it != buildings.end();
         ++it
       )
    {
        if ( (*it)->_rect.contains(e->pos()) )
        {
            _selectedObject = (*it);
            m_mode = movingObject;
            return;
        }
    }

    // Navigation Mesh Node
    Object* tmp = m_simulation->getNavMesh().find(e->pos());
    if( tmp ) {
        _selectedObject = tmp;
        m_mode = movingObject;
    }
    
}

void RenderArea::mousePressEvent(QMouseEvent *event)
{
    switch( m_mode )
    {
        case pickObject:
            pickElement( event );
        break;

        case addThief:
            m_simulation->addThief( event->pos() );
        break;

        case addAgent:
            m_simulation->addAgent( event->pos() );
        break;

        case addGoal:
            m_simulation->addGoal( event->pos() );
            drawPotential();
        break;
        
        case removeAgent:
            m_simulation->removeAgent( event->pos() );
        break;

        case removeGoal:
            m_simulation->removeGoal( event->pos() );
        break;

        case addObstacle:
            m_simulation->addObstacle( event->pos() );
            drawPotential();
            update();
        break;

        case removeObstacle:
            m_simulation->removeObstacle( event->pos() );
        break;
        
        case addRect:
            if (event->button() == Qt::LeftButton) {
                bottomRight = topLeft = event->pos();
                tmprect = QRect(topLeft, bottomRight);
                scribbling = true;
            }
        break;

        case addICB:
            if (event->button() == Qt::LeftButton) {
                bottomRight = topLeft = event->pos();
                tmprect = QRect(topLeft, bottomRight);
                scribbling = true;
            }
        break;

        case addNodes:
            m_simulation->addNode(event->pos());
        break;
        
        case addArc:
            arcTo = arcFrom = event->pos();
            scribbling = true;
        break;
    }
}

void RenderArea::mouseReleaseEvent(QMouseEvent *event)
{
    if( m_mode == movingObject )
    {
        m_mode = pickObject;
    }
    else if ( m_mode == addRect )
    {
        bottomRight = event->pos();
        QRect rect(topLeft,bottomRight);
        m_simulation->addRectangle( rect.normalized(), false );
        scribbling = false;
        update();
    }
	 else if ( m_mode == addICB )
    {
        bottomRight = event->pos();
        QRect rect(topLeft,bottomRight);
        m_simulation->addRectangle( rect.normalized(), true );
        scribbling = false;
        update();
    } 
    else if( m_mode == addArc )
    {
        m_simulation->addArc(arcFrom, event->pos() );
        scribbling = false;
    }
}

void RenderArea::mouseMoveEvent(QMouseEvent *event)
{
    if( m_mode == movingObject )
    {
        m_modified = true;
        _selectedObject->setPos( event->pos() );
    }
    else if ( (m_mode == addRect) && scribbling )
    {
        bottomRight = event->pos();
        tmprect = QRect(topLeft, bottomRight);
    }
	 else if ( (m_mode == addICB) && scribbling )
    {
        bottomRight = event->pos();
        tmprect = QRect(topLeft, bottomRight);
    }
    else if ((m_mode == addArc)&& scribbling)
    {
        arcTo = event->pos();
    }
    QString msg = QString::number( event->pos().x(), 10  );
    msg += " x ";
    msg += QString::number( event->pos().y(), 10  );
    ((MainWindow*)parentWidget())->setStatusBar(msg);
}



void RenderArea::drawLineTo(const QPoint &endPoint)
{
    QPainter painter(this);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                   Qt::RoundJoin));
    painter.drawLine(lastPoint, endPoint);

    int rad = myPenWidth / 2;
    update(QRect(lastPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
    update();
    lastPoint = endPoint;
}

void RenderArea::resizeEvent(QResizeEvent *event)
{
    if (width() > image.width() || height() > image.height()) {
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizePixmap(&pixmap, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}

void RenderArea::resizePixmap(QPixmap *pixmap, const QSize &newSize)
{
    if (pixmap->size() == newSize)
        return;

    QPixmap newPixmap(newSize);
    newPixmap.fill(qRgb(255, 255, 255));
    QPainter painter(&newPixmap);
    painter.drawPixmap(QPoint(0, 0), *pixmap);
    *pixmap = newPixmap;
}

void RenderArea::setColors()
{
    //m_agentColor;
    //m_goalColor;
    //m_obstacleColor;
    
    // Define as cores dos prédios
    _buildingColor = m_obstacleColor.light(50);

    // Cor dos prédios enquanto eles são desenhados é mais clara e tem transparência
    _building_tmp_brush = m_obstacleColor.light(100);

	 _ICB_tmp_brush = Qt::magenta;
 	 _icbColor = Qt::magenta;

    //_building_tmp_brush.setAlpha(100);
}
