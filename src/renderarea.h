#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QBrush>
#include <QPen>
#include <QColor>
#include <QPixmap>
#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QVector>
#include <QPoint>

#include "object.h"
#include "agent.h"
#include "goal.h"
#include "obstacle.h"
#include "building.h"

class Simulator;
class QTimer;

class RenderArea : public QWidget
{
    Q_OBJECT

public:

    enum Mode {
        pickObject,
        movingObject,
        addAgent,
        addGoal,
        addObstacle,
        addRect,
        addICB,
        removeAgent,
        removeGoal,
        removeObstacle,
        addNodes,
        addArc,
        addThief
    };

	void Chasing();
	void CampoPotencial();

	 enum TIPO_NAVEGACAO {CAMPO_POTENCIAL, PERSEGUICAO_FUGA};

    inline void setMode( Mode mode ) { m_mode = mode; }

    enum Shape { Line, Points, Polyline, Polygon, Rect, RoundRect, Ellipse, Arc,
                 Chord, Pie, Path, Text, Pixmap };

    RenderArea( Simulator* sim, QWidget *parent = 0);
    virtual ~RenderArea();
	void drawPotential();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void setColors();
    
    void setDrawPot(bool state) { m_drawpot = state; }

public slots:
    void cleanArea();
    void setShape(Shape shape);
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setAntialiased(bool antialiased);
    void setTransformed(bool transformed);
    void AgentColor();
    void GoalColor();


protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:

	float pot(int x, int y);
    float func(int x, int y, int ofx, int ofy, int q);


    void pickElement(QMouseEvent *event);
    void drawLineTo(const QPoint &endPoint);
    void resizePixmap(QPixmap *pixmap, const QSize &newSize);
    
    Shape shape;
    QPen pen;
	TIPO_NAVEGACAO m_navegationType;
    QBrush brush;
    
    bool antialiased;
    bool transformed;
    bool m_modified;
    bool scribbling;
    bool m_drawpot;
    int m_refreshRate;
    int myPenWidth;
    int m_pointSize;
    
    QPixmap pixmap;
    QPoint lastPoint, arcFrom, arcTo;
    QPoint topLeft, bottomRight;
    QRect tmprect;
    QImage image;
    QColor myPenColor;
    Mode m_mode;
    
    QColor m_agentColor;
    QColor m_goalColor;
    QColor m_obstacleColor;
    QColor _building_tmp_brush, _buildingColor;
    QColor _ICB_tmp_brush, _icbColor;
    
    QTimer* m_renderLoopTimer;
    
    Simulator* m_simulation;
    
    // Object that is being moved.
    Object* _selectedObject;
};


#endif
