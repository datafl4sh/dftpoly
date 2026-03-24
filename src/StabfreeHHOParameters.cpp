#include <QLabel>
#include <QGridLayout>

#include "StabfreeHHOParameters.h"

StabfreeHHOParamsWidget::StabfreeHHOParamsWidget(QWidget *parent)
    : QWidget(parent)
{
    hhoVariantCombo = new QComboBox(this);
    hhoVariantCombo->addItem("Mixed high");
    hhoVariantCombo->addItem("Equal order");
    hhoVariantCombo->addItem("Mixed low");

    hhoOrderCombo = new QComboBox(this);
    hhoOrderCombo->addItem("0");
    hhoOrderCombo->addItem("1");
    hhoOrderCombo->addItem("2");
    hhoOrderCombo->addItem("3");

    QGridLayout *layout = new QGridLayout(this);

    layout->addWidget( new QLabel("HHO variant:"), 0, 0);
    layout->addWidget( hhoVariantCombo, 0, 1);
    layout->addWidget( new QLabel("HHO order: "), 1, 0);
    layout->addWidget(hhoOrderCombo, 1, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    setLayout(layout);
}