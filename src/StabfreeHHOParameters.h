#pragma once
#include <QComboBox>
#include <QLabel>

#include "HHOModel.h"
#include "hist.h"

class StabfreeHHOParamsWidget : public QWidget
{
    Q_OBJECT

public:
    StabfreeHHOParamsWidget(QWidget *parent = nullptr);

public slots:
    void setEigenvalue(double);

private slots:
    void priv_hhoTypeChanged(int);
    void priv_hhoOrderChanged(int);
    void priv_hhoVariantChanged(int);
    void priv_hhoIncrChanged(int);

signals:
    void hhoTypeChanged(bool);
    void hhoVariantChanged(hho_variant);
    void hhoOrderChanged(size_t);
    void hhoIncrChanged(size_t);

private:
    QComboBox   *hhoTypeCombo;
    QComboBox   *hhoVariantCombo;
    QComboBox   *hhoOrderCombo;
    QComboBox   *hhoIncreaseCombo;
    QLabel      *smallestEigLabel;
};