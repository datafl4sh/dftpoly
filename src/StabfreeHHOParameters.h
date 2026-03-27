#pragma once
#include <QComboBox>
#include <QLabel>

#include "hist.h"

class StabfreeHHOParamsWidget : public QWidget
{
    Q_OBJECT

public:
    StabfreeHHOParamsWidget(QWidget *parent = nullptr);

public slots:
    void setEigenvalue(double);

private slots:
    void hhoTypeChanged(int);
    void hhoOrderChanged(int);
    void hhoVariantChanged(int);


private:
    QComboBox   *hhoTypeCombo;
    QComboBox   *hhoVariantCombo;
    QComboBox   *hhoOrderCombo;
    QLabel      *smallestEigLabel;
};