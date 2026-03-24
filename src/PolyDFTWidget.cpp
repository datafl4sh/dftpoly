#include <QGridLayout>
#include <QLabel>

#include "hist.h"
#include "PolyDFTWidget.h"

PolyDFTWidget::PolyDFTWidget(QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);

    QLabel *titleReal = new QLabel("Real part");
    QLabel *titleImag = new QLabel("Imaginary part");

    histRealPart = new HistogramWidget(this);
    QObject::connect(
        histRealPart, SIGNAL(valuesChanged(const QVector<double>&)),
        this, SIGNAL(realValuesChanged(const QVector<double>&))
    );
    histImagPart = new HistogramWidget(this);
    QObject::connect(
        histImagPart, SIGNAL(valuesChanged(const QVector<double>&)),
        this, SIGNAL(imagValuesChanged(const QVector<double>&))
    );
    layout->addWidget(titleReal, 0, 0);
    layout->addWidget(titleImag, 0, 1);
    layout->addWidget(histRealPart, 1, 0);
    layout->addWidget(histImagPart, 1, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    setLayout(layout);
}

void
PolyDFTWidget::setRealData(const QVector<double>& rdata)
{
    histRealPart->setValues(rdata);
}

void
PolyDFTWidget::setImagData(const QVector<double>& idata)
{
    histImagPart->setValues(idata);
}

void
PolyDFTWidget::setScale(double s)
{
    histRealPart->setScale(s);
    histImagPart->setScale(s);
}