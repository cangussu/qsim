#include <QStringList>
#include <QList>
#include <QFileDialog>
#include <QMessageBox>

#include "mainwindow.h"
#include "renderarea.h"
#include "simulator.h"

MainWindow::MainWindow()
{
    m_simulator = new Simulator(800,600);
    renderArea = new RenderArea(m_simulator, this);
    m_simulator->setView( renderArea );
    setCentralWidget(renderArea);

    createSimulationActions();
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    readSettings();
}

MainWindow::~MainWindow()
{
    delete m_simulator;
    delete renderArea;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::about()
{
   QMessageBox::about( this, tr("About Qsim"),
                       tr("<b>Qsim</b> é um simulador 2D de agentes.<br>"
                          "Autores:<br>"
                          "Erickson Rangel do Nascimento<br>"
                          "Thiago Cangussu de Castro Gomes<br>"
                          "Wallace Santos Lages"
                         ));
}

void MainWindow::documentWasModified()
{
    setWindowModified(true);
}

//------------------------------------------------------------------------------------------
// Ações disparadas pelos botões
//------------------------------------------------------------------------------------------

void MainWindow::selectNavigationMode(QAction* action)
{
    if( action == potFieldAct )
    {
        renderArea->CampoPotencial();
        renderArea->drawPotential();
    }
    else if ( action == chasingAct ) 
    {
        renderArea->cleanArea();
        renderArea->Chasing();
    }
}

void MainWindow::selectMode(QAction* action)
{
    if(action == pickAgentAct)
        renderArea->setMode( RenderArea::pickObject );
    else if(action == newAgentAct )
        renderArea->setMode( RenderArea::addAgent );
    else if(action == newObstacleAct)
        renderArea->setMode( RenderArea::addObstacle );
    else if(action == newRectAct)
        renderArea->setMode( RenderArea::addRect );
    else if(action == newICBAct)
        renderArea->setMode( RenderArea::addICB );
	 else if(action == newGoalAct)
        renderArea->setMode( RenderArea::addGoal );
    else if(action == newNodeAct)
        renderArea->setMode( RenderArea::addNodes );
    else if(action == newArcAct)
        renderArea->setMode( RenderArea::addArc );
    else if(action == newThiefAct)
        renderArea->setMode( RenderArea::addThief );
}

void MainWindow::reset()
{
	m_simulator->reset();
	renderArea->cleanArea();
	simStart->setChecked(false);
	simStop->setChecked(false);
}

void MainWindow::keyPressEvent( QKeyEvent * event )
{
    if( event->key() == Qt::Key_Escape ) {
        renderArea->setMode( RenderArea::pickObject );
        pickAgentAct->activate(QAction::Trigger);
    }
}

// ----------------------------------------------------------------------------------------
// Actions
// ----------------------------------------------------------------------------------------

void MainWindow::createSimulationActions()
{
    // Ações relacionadas com processo de simulação:
    simStart = new QAction(QIcon(":/images/start.png"), tr("Start"), this);
    simStart->setCheckable(true);
    connect(simStart, SIGNAL(triggered(bool)), m_simulator, SLOT(start()));

    simStop =  new QAction(QIcon(":/images/stop.png"), tr("Stop"), this);
    simStop->setCheckable(true);
    connect(simStop, SIGNAL(triggered(bool)), m_simulator, SLOT(stop()));

    simPause = new QAction(QIcon(":/images/clean.png"), tr("Reset"), this);
    connect(simPause, SIGNAL(triggered(bool)), this, SLOT(reset()));
    
    // Grupo de ações
    simulationActions = new QActionGroup(this);
    simulationActions->addAction(simStart);
    simulationActions->addAction(simStop);

    // Ações relacionadas com o estado da simulação:
    newAgentAct = new QAction(QIcon(":/images/agent.png"), tr("&Agent"), this);
    newAgentAct->setShortcut(tr("Ctrl+A"));
    newAgentAct->setStatusTip(tr("Add an agent"));
    newAgentAct->setCheckable(true);
    
    newThiefAct = new QAction(QIcon(":/images/thief.png"), tr("&Thief"), this);
    newThiefAct->setShortcut(tr("Ctrl+T"));
    newThiefAct->setStatusTip(tr("Add a thief"));
    newThiefAct->setCheckable(true);
    
    newGoalAct = new QAction(QIcon(":/images/goal.png"), tr("&Goal"), this);
    newGoalAct->setShortcut(tr("Ctrl+G"));
    newGoalAct->setStatusTip(tr("Add a goal"));
    newGoalAct->setCheckable(true);

    newObstacleAct = new QAction(QIcon(":/images/obstacle.png"), tr("&Obstacle"), this);
    newObstacleAct->setShortcut(tr("Ctrl+O"));
    newObstacleAct->setStatusTip(tr("Add an obstacle"));
    newObstacleAct->setCheckable(true);
    
    pickAgentAct = new QAction(QIcon(":/images/move.png"), tr("&Move"), this);
    pickAgentAct->setCheckable(true);

    newRectAct = new QAction(QIcon(":/images/building.png"), tr("Predio"), this);
    newRectAct->setStatusTip(tr("Cria um prédio"));
    newRectAct->setCheckable(true);

    newICBAct = new QAction(QIcon(":/images/icb.png"), tr("ICB"), this);
    newICBAct->setStatusTip(tr("Cria um prédio do ICB"));
    newICBAct->setCheckable(true);

    newNodeAct = new QAction(QIcon(":/images/node.png"), tr("Nó de navegação"), this);
    newNodeAct->setStatusTip(tr("Adiciona um nó de navegação"));
    newNodeAct->setCheckable(true);
    
    newArcAct = new QAction(QIcon(":/images/arc.png"), tr("Arco"), this);
    newArcAct->setStatusTip(tr("Adiciona um arco"));
    newArcAct->setCheckable(true);
    

    // Grupo de ações
    worldActions = new QActionGroup(this);
    worldActions->addAction(newAgentAct);
    worldActions->addAction(newGoalAct);
    worldActions->addAction(newObstacleAct);
    worldActions->addAction(newRectAct);
    worldActions->addAction(newICBAct);
    worldActions->addAction(newThiefAct);
    worldActions->addAction(newNodeAct);
    worldActions->addAction(pickAgentAct);
    worldActions->addAction(newArcAct);
    pickAgentAct->setChecked(true);
    connect(worldActions, SIGNAL(triggered(QAction*)), this, SLOT(selectMode(QAction*)));

    // Ações do modo de operação do simulador
    potFieldAct = new QAction(QIcon(":/images/agent_sel.png"), tr("&Campo de Potencial"), this);
    potFieldAct->setStatusTip(tr("Campo de Potencial"));
    potFieldAct->setCheckable(true);

    renderArea->CampoPotencial();

    chasingAct = new QAction(QIcon(":/images/fuga.png"), tr("&Perseguicao e Fuga"), this);
    chasingAct->setStatusTip(tr("Perseguicao e Fuga"));
    chasingAct->setCheckable(true);


    // Grupo de ações
    navigationActions = new QActionGroup(this);
    navigationActions->addAction(potFieldAct);
    navigationActions->addAction(chasingAct);
    potFieldAct->setChecked(true); // default mode

    connect(navigationActions, SIGNAL(triggered(QAction*)), this, SLOT(selectNavigationMode(QAction*)));
}


void MainWindow::createActions()
{
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    agentColorAction = new QAction(QIcon(":/images/agent.png"), tr("&Agent Color"), this);
    agentColorAction->setStatusTip(tr("Agent Color"));
    connect(agentColorAction, SIGNAL(triggered()), renderArea, SLOT(AgentColor()));

    goalColorAction = new QAction(QIcon(":/images/goal.png"), tr("&Goal Color"), this);
    goalColorAction->setStatusTip(tr("Goal Color"));
    connect(goalColorAction, SIGNAL(triggered()), renderArea, SLOT(GoalColor()));

    configAct = new QAction(QIcon(":/images/new.png"), tr("&Config"), this);
    configAct->setStatusTip(tr("Configuração."));
    connect(configAct, SIGNAL(triggered()), this, SLOT(config()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));
    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open"), this);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addSeparator();
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(configAct);
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(agentColorAction);
    editMenu->addAction(goalColorAction);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(newAgentAct);
    editToolBar->addAction(newThiefAct);
    editToolBar->addAction(newGoalAct);
    editToolBar->addAction(newObstacleAct);

    editToolBar->addAction(newRectAct);
    editToolBar->addAction(newICBAct);
    editToolBar->addAction(newNodeAct);
    editToolBar->addAction(newArcAct);
    editToolBar->addAction(pickAgentAct);

    editToolBar->addSeparator();

    editToolBar->addAction(simStart);
    editToolBar->addAction(simStop);
    editToolBar->addAction(simPause);

    editToolBar->addSeparator();

    editToolBar->addAction(potFieldAct);
    editToolBar->addAction(chasingAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

// Configuração
void MainWindow::readSettings()
{
    QSettings settings("ETW", "Qsim");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    renderArea->setDrawPot(settings.value("potential", false).toBool());
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("ETW", "Qsim");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

#include "configdialog.h"

void MainWindow::config()
{
    ConfigDialog confd(this);
    confd.exec();
    renderArea->setDrawPot(confd.drawPotential());
    renderArea->drawPotential();
}

bool MainWindow::save()
{
//    releaseKeyboard();
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;
    
    m_simulator->save(fileName);

    return true;
}

bool MainWindow::open()
{
    //grabKeyboard();
//    releaseKeyboard();
//     QFileDialog *fd = new QFileDialog(this);
// 
//     fd->setFilter( "Mapas (*.map *.*)" );
//     fd->setViewMode(QFileDialog::Detail);
//     QStringList fileNames;
//     if (fd->exec()) {
//         fileNames = fd->selectedFiles();
//         filename = fileNames.at(0);
//     }
//     else
//         return false;
    QString filename = QFileDialog::getOpenFileName(this);
    if (filename.isEmpty())
        return false;
    
    m_simulator->load(filename);

    return true;
}
