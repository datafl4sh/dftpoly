#pragma once
#include <QComboBox>

#include "diskpp/mesh/mesh.hpp"

#include "hist.h"

class StabfreeHHOParamsWidget : public QWidget
{
    Q_OBJECT

public:
    StabfreeHHOParamsWidget(QWidget *parent = nullptr);

private:
    QComboBox   *hhoVariantCombo;
    QComboBox   *hhoOrderCombo;
};