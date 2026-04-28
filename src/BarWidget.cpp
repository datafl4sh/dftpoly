#include "BarWidget.h"
#include <QPainter>
#include <algorithm>
#include <cmath>
#include <QGraphicsLayout>

#include "BarWidget.h"

#include <QVBoxLayout>
#include <algorithm>

BarWidget::BarWidget(QWidget *parent)
    : QWidget(parent),
      chart(new QChart()),
      view(new QChartView(chart)),
      series(new QBarSeries()),
      barSet(new QBarSet("values")),
      axisYLinear(new QValueAxis()),
      axisYLog(new QLogValueAxis()),
      logCheck(new QCheckBox("Log scale"))
{
    setMinimumSize(300, 200);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(5,0,0,0);

    layout->addWidget(view);
    layout->addWidget(logCheck);

    chart->addSeries(series);
    series->append(barSet);

    chart->legend()->hide();

    chart->setMargins(QMargins(5, 2, 2, 2));
    chart->setBackgroundRoundness(0);

    view->setContentsMargins(5, 0, 0, 0);
    view->setRenderHint(QPainter::Antialiasing, true);

    view->setRenderHint(QPainter::Antialiasing);
    view->setMinimumHeight(100);

    axisYLinear->setLabelFormat("%.3f");
    axisYLinear->setTickCount(5);

    axisYLog->setBase(10);
    axisYLog->setLabelFormat("%.2e");

    chart->addAxis(axisYLinear, Qt::AlignLeft);
    chart->addAxis(axisYLog, Qt::AlignLeft);
    axisYLog->setVisible(false);

    series->attachAxis(axisYLinear);

    connect(logCheck, &QCheckBox::toggled,
            this, &BarWidget::toggleLogScale);
}

void BarWidget::setData(const Eigen::VectorXd &data)
{
    barSet->remove(0, barSet->count());

    double maxVal = data[ data.size()-1 ];
    double minVal = data[1]/10;

    for (int i = 1; i < data.size(); ++i)
    {
        double v = data[i];
        barSet->append(v);
    }

    if (logCheck->isChecked())
    {
        axisYLog->setRange(minVal, maxVal);
        series->detachAxis(axisYLog);
        series->attachAxis(axisYLinear);
        series->detachAxis(axisYLinear);
        series->attachAxis(axisYLog);
    }
    else
    {
        axisYLinear->setRange(minVal, maxVal);
        series->detachAxis(axisYLinear);
        series->attachAxis(axisYLog);
        series->detachAxis(axisYLog);
        series->attachAxis(axisYLinear);
    }

    chart->layout()->invalidate();
    chart->update();
    view->update();
}

void BarWidget::toggleLogScale(bool enabled)
{
    axisYLinear->setVisible(!enabled);
    axisYLog->setVisible(enabled);

    if (enabled)
    {
        series->detachAxis(axisYLinear);
        series->attachAxis(axisYLog);
    }
    else
    {
        series->detachAxis(axisYLog);
        series->attachAxis(axisYLinear);
    }

    chart->update();
    view->update();
}

