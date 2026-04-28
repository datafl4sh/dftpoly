
#pragma once

#include <QWidget>
#include <Eigen/Dense>

#pragma once

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLogValueAxis>
#include <QCheckBox>
#include <Eigen/Dense>


class BarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BarWidget(QWidget *parent = nullptr);

public slots:
    void setData(const Eigen::VectorXd &data);

private slots:
    void toggleLogScale(bool enabled);

private:
    void updateChart();

    QChart *chart;
    QChartView *view;
    QBarSeries *series;
    QBarSet *barSet;

    QValueAxis *axisYLinear;
    QLogValueAxis *axisYLog;

    QCheckBox *logCheck;
};

