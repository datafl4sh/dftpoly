#include <QWidget>
#include <QPainter>
#include <QVector>
#include <QMouseEvent>

#pragma once

class HistogramWidget : public QWidget
{
    Q_OBJECT

public:
    HistogramWidget(QWidget *parent = nullptr);

signals:
    void valueChanged(int index, double newValue);
    void valuesChanged(const QVector<double>& newValue);

public slots:
    void setValues(const QVector<double>& values);
    void setScale(double);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    int barAtPosition(double x) const;
    double yToValue(double y) const;

    QVector<double> m_values;

    int m_dragIndex = -1;

    double m_maxAbs = 1.0;

    double m_scale = 1.0;

};