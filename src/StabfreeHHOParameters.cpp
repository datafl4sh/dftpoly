#include <QObject>
#include <QLabel>
#include <QGridLayout>
#include <QString>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFrame>



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

    hhoIncreaseCombo = new QComboBox(this);
    hhoIncreaseCombo->addItem("0");
    hhoIncreaseCombo->addItem("1");
    hhoIncreaseCombo->addItem("2");
    hhoIncreaseCombo->addItem("3");

    auto createLine = []() {
        QFrame *line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        return line;
    };

    specHHOGradGrad = new SpectrumWidget("Reconstruction eigs");
    specStiffness = new SpectrumWidget("BCCM stiffness eigs");

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget( new QLabel("HHO type:"), 0, 0);
    layout->addWidget( hhoTypeCombo, 0, 1);
    layout->addWidget( new QLabel("HHO variant:"), 1, 0);
    layout->addWidget( hhoVariantCombo, 1, 1);
    layout->addWidget( new QLabel("HHO order: "), 2, 0);
    layout->addWidget(hhoOrderCombo, 2, 1);
    layout->addWidget( new QLabel("Incr. above optimal:"), 3, 0);
    layout->addWidget( hhoIncreaseCombo, 3, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    layout->addWidget(createLine(), 4, 0, 1, 2); 
    layout->addWidget(specHHOGradGrad, 5, 0, 1, 2);
    layout->addWidget(specStiffness, 6, 0, 1, 2);
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

    QObject::connect(
        hhoIncreaseCombo, SIGNAL(currentIndexChanged(int)),
        this, SLOT(priv_hhoIncrChanged(int))
    );
}

void
StabfreeHHOParamsWidget::setGradGradSpectrum(const Eigen::VectorXd& data)
{
    specHHOGradGrad->setSpectrum(data);
}

void
StabfreeHHOParamsWidget::setStiffnessSpectrum(const Eigen::VectorXd& data)
{
    specStiffness->setSpectrum(data);
}

void
StabfreeHHOParamsWidget::priv_hhoTypeChanged(int index)
{
    if (index == 0) {
        hhoIncreaseCombo->setEnabled(false);
        emit hhoTypeChanged(false); /* Use standard */
    }
    else {
        hhoIncreaseCombo->setEnabled(true);
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
StabfreeHHOParamsWidget::priv_hhoIncrChanged(int incr)
{
    emit hhoIncrChanged(incr);
}

