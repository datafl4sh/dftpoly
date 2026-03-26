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
    hhoVariantCombo = new QComboBox(this);
    hhoVariantCombo->addItem("Mixed high");
    hhoVariantCombo->addItem("Equal order");
    hhoVariantCombo->addItem("Mixed low");

    hhoOrderCombo = new QComboBox(this);
    hhoOrderCombo->addItem("0");
    hhoOrderCombo->addItem("1");
    hhoOrderCombo->addItem("2");
    hhoOrderCombo->addItem("3");

    smallestEigLabel = new QLabel("--", this);

    QGridLayout *layout = new QGridLayout(this);

    layout->addWidget( new QLabel("HHO variant:"), 0, 0);
    layout->addWidget( hhoVariantCombo, 0, 1);
    layout->addWidget( new QLabel("HHO order: "), 1, 0);
    layout->addWidget(hhoOrderCombo, 1, 1);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 1);
    layout->addWidget(smallestEigLabel, 2,0,1,2);
    setLayout(layout);
}

void
StabfreeHHOParamsWidget::polygonChanged(const QVector<QPointF>& pts)
{
    if (pts.size() < 3) {
        smallestEigLabel->setText("--");
        return;
    }

    std::vector<point_type> polypts;
    for (auto& p : pts) {
        polypts.push_back( {p.x(), p.y()} );
    }
    mesh_type msh;
    disk::make_single_elem_mesh_from_points(msh, polypts);

    double eig = test(msh, hhocfg);
    QString text = QString("%1").arg(eig, 0, 'f', 15);
    smallestEigLabel->setText(text);

}
