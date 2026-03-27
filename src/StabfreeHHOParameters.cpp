#include <QObject>
#include <QLabel>
#include <QGridLayout>
#include <QString>

#include "diskpp/loaders/loader.hpp"
#include "diskpp/loaders/loader_utils.hpp"
#include "diskpp/mesh/meshgen.hpp"



#include "StabfreeHHOParameters.h"

StabfreeHHOParamsWidget::StabfreeHHOParamsWidget(QWidget *parent)
    : QWidget(parent)
{
    hhoTypeCombo = new QComboBox(this);
    hhoTypeCombo->addItem("Standard");
    hhoTypeCombo->addItem("Stabfree");
    hhoTypeCombo->setCurrentIndex(1);

    hhoVariantCombo = new QComboBox(this);
    hhoVariantCombo->addItem("Mixed low");
    hhoVariantCombo->addItem("Equal order");
    hhoVariantCombo->addItem("Mixed high");

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
        this, SLOT(hhoTypeChanged(int))
    );

    QObject::connect(
        hhoVariantCombo, SIGNAL(currentIndexChanged(int)),
        this, SLOT(hhoVariantChanged(int))
    );

    QObject::connect(
        hhoOrderCombo, SIGNAL(currentIndexChanged(int)),
        this, SLOT(hhoOrderChanged(int))
    );
}

void
StabfreeHHOParamsWidget::hhoTypeChanged(int index)
{
    if (index == 0) {
        hhocfg.use_stabfree = false;
    } else {
        hhocfg.use_stabfree = true;
    }

    recompute();
}

void
StabfreeHHOParamsWidget::hhoOrderChanged(int index)
{
    hhocfg.degree = index;
    recompute();
}

void
StabfreeHHOParamsWidget::hhoVariantChanged(int index)
{
    switch (index) {
        case 0: hhocfg.variant = priv::hho_variant::mixed_order_low; break;
        case 1: hhocfg.variant = priv::hho_variant::equal_order; break;
        case 2: hhocfg.variant = priv::hho_variant::mixed_order_high; break;
        default: hhocfg.variant = priv::hho_variant::equal_order; break;
    }
    recompute();
}

void
StabfreeHHOParamsWidget::recompute(void)
{
    if (polypts.size() < 3) {
        smallestEigLabel->setText("--");
        return;
    }

    mesh_type msh;
    disk::make_single_elem_mesh_from_points(msh, polypts);

    double eig = test(msh, hhocfg);
    QString text = QString("%1").arg(eig, 0, 'f', 15);
    smallestEigLabel->setText(text);
}

void
StabfreeHHOParamsWidget::polygonChanged(const QVector<QPointF>& pts)
{
    polypts.clear();
    for (auto& p : pts) {
        polypts.push_back( {p.x(), p.y()} );
    }

    recompute();
}
