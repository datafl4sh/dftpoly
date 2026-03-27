#include <QObject>
#include <QLabel>
#include <QGridLayout>
#include <QString>



#include "HHOModel.h"
#include "StabfreeHHOParameters.h"

StabfreeHHOParamsWidget::StabfreeHHOParamsWidget(QWidget *parent)
    : QWidget(parent)
{
    hhoTypeCombo = new QComboBox(this);
    hhoTypeCombo->addItem("Standard");
    hhoTypeCombo->addItem("Stabfree");
    hhoTypeCombo->setCurrentIndex(1);

    hhoVariantCombo = new QComboBox(this);
    hhoVariantCombo->addItem("Equal order");
    hhoVariantCombo->addItem("Mixed high");
    //hhoVariantCombo->addItem("Mixed low");

    hhoOrderCombo = new QComboBox(this);
    hhoOrderCombo->addItem("0");
    hhoOrderCombo->addItem("1");
    hhoOrderCombo->addItem("2");
    hhoOrderCombo->addItem("3");

    smallestEigLabel = new QLabel("--", this);

    QGridLayout *layout = new QGridLayout(this);

    layout->addWidget( new QLabel("HHO type:"), 0, 0);
    layout->addWidget( hhoTypeCombo, 0, 1);
    layout->addWidget( new QLabel("HHO variant:"), 1, 0);
    layout->addWidget( hhoVariantCombo, 1, 1);
    layout->addWidget( new QLabel("HHO order: "), 2, 0);
    layout->addWidget(hhoOrderCombo, 2, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    layout->addWidget(smallestEigLabel, 3,0,1,2);
    setLayout(layout);

    QObject::connect(
        hhoTypeCombo, SIGNAL(currentIndexChanged(int)),
        this, SLOT(priv_hhoTypeChanged(int))
    );

    QObject::connect(
        hhoVariantCombo, SIGNAL(currentIndexChanged(int)),
        this, SLOT(priv_hhoVariantChanged(int))
    );

    QObject::connect(
        hhoOrderCombo, SIGNAL(currentIndexChanged(int)),
        this, SLOT(priv_hhoOrderChanged(int))
    );
}

void
StabfreeHHOParamsWidget::priv_hhoTypeChanged(int index)
{
    if (index == 0) {
        emit hhoTypeChanged(false); /* Use standard */
    }
    else {
        emit hhoTypeChanged(true); /* Use stabfree */
    }
}

void
StabfreeHHOParamsWidget::priv_hhoOrderChanged(int index)
{
    emit hhoOrderChanged(index);
}

void
StabfreeHHOParamsWidget::priv_hhoVariantChanged(int index)
{
    switch (index) {
        case 0: emit hhoVariantChanged(hho_variant::equal_order); break;
        case 1: emit hhoVariantChanged(hho_variant::mixed_order_high); break;
        case 2: emit hhoVariantChanged(hho_variant::mixed_order_low); break;
    }
}

void
StabfreeHHOParamsWidget::setEigenvalue(double eig)
{
    QString text = QString("%1").arg(eig, 0, 'f', 15);
    smallestEigLabel->setText(text);
}

//void
//StabfreeHHOParamsWidget::recompute(void)
//{
//    
//}

