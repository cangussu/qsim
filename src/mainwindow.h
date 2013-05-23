#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMainWindow>
#include <QStatusBar>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>

class RenderArea;
class Simulator;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow();

    void setStatusBar( QString& msg ) { statusBar()->showMessage(msg); }

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent ( QKeyEvent * event );


private slots:

    bool save();
    bool open();
    void config();
    void about();
    void documentWasModified();
    void reset();
    void selectMode(QAction*);
    void selectNavigationMode(QAction*);

private:

    void createSimulationActions();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    void readSettings();
    void writeSettings();

    Simulator* m_simulator;
    RenderArea *renderArea;
    QString curFile;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QToolBar *editToolBar;
    
    // Actions
    QAction *exitAct;
    QAction *configAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *saveAct;
    QAction *openAct;

    QAction *agentColorAction;
    QAction *goalColorAction;

    QActionGroup* simulationActions;
    QAction *simStart;
    QAction *simStop;
    QAction *simPause;

    QActionGroup* worldActions;
    QActionGroup* navigationActions;
    QAction *newRectAct;
    QAction *newICBAct;
    QAction *newNodeAct;
    QAction *newArcAct;
    QAction *newAgentAct;
    QAction *newGoalAct;
    QAction *newObstacleAct;
    QAction *pickAgentAct;
    QAction *chasingAct;
    QAction *potFieldAct;
    QAction *newThiefAct;
};

#endif
