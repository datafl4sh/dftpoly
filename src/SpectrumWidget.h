#pragma once

#include <QWidget>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>

#include "BarWidget.h"

class SpectrumWidget : public QWidget
{
    Q_OBJECT

public:
    SpectrumWidget(const QString& title, QWidget *parent = nullptr);

public slots:
    void setSpectrum(const Eigen::VectorXd&);

private:
    QLabel      *smallestEigLabel;
    QLabel      *biggestEigLabel;
    BarWidget   *spectrum;
};