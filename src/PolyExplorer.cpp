#include <QDockWidget>

#include "PolyExplorer.h"

#include "points.h"

PolyExplorer::PolyExplorer()
{
    setWindowTitle("Polygon Explorer");
    polygonEditorWidget = new PolygonEditorWidget();
    setCentralWidget( polygonEditorWidget );

    QMenuBar *menuBar = this->menuBar();
    QMenu *fileMenu = menuBar->addMenu("File");
    QAction *exitAction = new QAction("Exit", this);
    fileMenu->addAction(exitAction);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    makePolygonMenu();

    viewMenu = menuBar->addMenu("View");

    

    makePolyDFTWidget();

    makeStabfreeHHOWidget();
    hhoModel = new HHOModel();

    QObject::connect(
        polygonEditorWidget, SIGNAL(polygonChanged(const QVector<QPointF>&)),
        polygonEditorWidget, SLOT(processPolygon(const QVector<QPointF>&)) );

    QObject::connect(
        polygonEditorWidget, SIGNAL(realValuesChanged(const QVector<double>&)),
        polyDFTWidget, SLOT(setRealData(const QVector<double>&)) );

    QObject::connect(
        polygonEditorWidget, SIGNAL(imagValuesChanged(const QVector<double>&)),
        polyDFTWidget, SLOT(setImagData(const QVector<double>&)) );

    QObject::connect(polygonEditorWidget, SIGNAL(zoomChanged(double)),
        polyDFTWidget, SLOT(setScale(double)) );

    QObject::connect(
        polyDFTWidget, SIGNAL(realValuesChanged(const QVector<double>&)),
        polygonEditorWidget, SLOT(setRealValues(const QVector<double>&)) );

    QObject::connect(
        polyDFTWidget, SIGNAL(imagValuesChanged(const QVector<double>&)),
        polygonEditorWidget, SLOT(setImagValues(const QVector<double>&)) );

    QObject::connect(
        polygonEditorWidget, SIGNAL(polygonChanged(const QVector<QPointF>&)),
        hhoModel, SLOT(setPolygon(const QVector<QPointF>&)) );

    QObject::connect(
        hhoModel, SIGNAL(minEigenvalueChanged(double)),
        stabfreeWidget, SLOT(setEigenvalue(double)) );
}

void
PolyExplorer::makePolygonMenu()
{
    QMenuBar *menuBar = this->menuBar();

    polygonMenu = menuBar->addMenu("Polygon");
    QAction *polyEnableLabelsAction = new QAction("Vertex labels", this);
    polyEnableLabelsAction->setCheckable(true);
    polygonMenu->addAction(polyEnableLabelsAction);
    connect(polyEnableLabelsAction, SIGNAL(triggered(bool)),
        polygonEditorWidget, SLOT(setShowLabels(bool)));

    QAction *polyEnableIAxesAction = new QAction("Inertia Axes", this);
    polyEnableIAxesAction->setCheckable(true);
    polygonMenu->addAction(polyEnableIAxesAction);
    connect(polyEnableIAxesAction, SIGNAL(triggered(bool)),
        polygonEditorWidget, SLOT(setShowInertiaAxes(bool)));

    QAction *polyEnableBBoxAction = new QAction("Bounding Box", this);
    polyEnableBBoxAction->setCheckable(true);
    polygonMenu->addAction(polyEnableBBoxAction);
    connect(polyEnableBBoxAction, SIGNAL(triggered(bool)),
        polygonEditorWidget, SLOT(setShowBoundingBox(bool)));

    QAction *polyEnableITransAction = new QAction("Inertia transformed polygon", this);
    polyEnableITransAction->setCheckable(true);
    polygonMenu->addAction(polyEnableITransAction);
    connect(polyEnableITransAction, SIGNAL(triggered(bool)),
        polygonEditorWidget, SLOT(setShowInertiaTransformed(bool)));
}

void
PolyExplorer::makePolyDFTWidget()
{
    polyDFTWidget = new PolyDFTWidget(this);
    QDockWidget *dock = new QDockWidget("Fourier Descriptors", this);
    dock->setWidget(polyDFTWidget);
    dock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    viewMenu->addAction(dock->toggleViewAction());
    addDockWidget(Qt::BottomDockWidgetArea, dock);
}

void
PolyExplorer::makeStabfreeHHOWidget()
{
    stabfreeWidget = new StabfreeHHOParamsWidget(this);
    stabfreeWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    QDockWidget *dock = new QDockWidget("Stabfree HHO parameters", this);
    dock->setWidget(stabfreeWidget);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    viewMenu->addAction(dock->toggleViewAction());
    addDockWidget(Qt::RightDockWidgetArea, dock);
}