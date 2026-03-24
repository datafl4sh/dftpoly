#pragma once

#include "hist.h"

class PolyDFTWidget : public QWidget
{
    Q_OBJECT

public:
    PolyDFTWidget(QWidget *parent = nullptr);

public slots:
    void setRealData(const QVector<double>&);
    void setImagData(const QVector<double>&);
    void setScale(double);

signals:
    void realValuesChanged(const QVector<double>&);
    void imagValuesChanged(const QVector<double>&);

private:
    HistogramWidget *histRealPart;
    HistogramWidget *histImagPart;
};