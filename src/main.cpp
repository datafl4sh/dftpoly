#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QFrame>

#include "points.h"
#include "hist.h"

#include "PolyExplorer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    /*
    QWidget window;

    QGridLayout* layout = new QGridLayout(&window);

    QWidget* polyW   = new PolygonEditorWidget();
    QWidget* real_histW  = new HistogramWidget();
    QWidget* imag_histW = new HistogramWidget();

    layout->addWidget(polyW,   0, 0, 1, 2);
    layout->addWidget(real_histW,  1, 0);
    layout->addWidget(imag_histW, 1, 1);

    QObject::connect(polyW, SIGNAL(polygonChanged(const QVector<QPointF>&)),
        polyW, SLOT(processPolygon(const QVector<QPointF>&)) );

    QObject::connect(polyW, SIGNAL(realValuesChanged(const QVector<double>&)),
        real_histW, SLOT(setValues(const QVector<double>&)) );

    QObject::connect(polyW, SIGNAL(zoomChanged(double)),
        real_histW, SLOT(setScale(double)) );

    QObject::connect(real_histW, SIGNAL(valuesChanged(const QVector<double>&)),
        polyW, SLOT(setRealValues(const QVector<double>&)) );


    QObject::connect(polyW, SIGNAL(imagValuesChanged(const QVector<double>&)),
        imag_histW, SLOT(setValues(const QVector<double>&)) );

    QObject::connect(polyW, SIGNAL(zoomChanged(double)),
        imag_histW, SLOT(setScale(double)) );

    QObject::connect(imag_histW, SIGNAL(valuesChanged(const QVector<double>&)),
        polyW, SLOT(setImagValues(const QVector<double>&)) );

    layout->setRowStretch(0, 2);
    layout->setRowStretch(1, 1);

    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);

    window.resize(600, 400);
    window.show();
    */

    PolyExplorer pe;
    pe.show();

    return app.exec();
}