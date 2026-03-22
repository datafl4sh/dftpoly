#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QVector>
#include <QDebug>
#include <QTimer>

#include "hist.h"

HistogramWidget::HistogramWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(300, 150);
}

void HistogramWidget::setValues(const QVector<double>& values)
{
    m_values = values;
    update();
}

int HistogramWidget::barAtPosition(double x) const
{
    if (m_values.isEmpty())
        return -1;

    int n = m_values.size();
    double barFullWidth = width() / (double)n;

    int index = static_cast<int>(x / barFullWidth);

    if (index < 0 || index >= n)
        return -1;

    return index;
}

double HistogramWidget::yToValue(double y) const
{
    double zeroY = height() / 2.0;
    double scaleY = m_scale * (height() / 2.0) / m_maxAbs;

    return (zeroY - y) / scaleY;
}

void HistogramWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int idx = barAtPosition(event->pos().x());
        if (idx >= 0) {
            m_dragIndex = idx;

            double v = yToValue(event->pos().y());
            m_values[idx] = v;

            emit valueChanged(idx, v);
            emit valuesChanged(m_values);
            update();
        }
    }

    if (event->button() == Qt::RightButton) {
        int idx = barAtPosition(event->pos().x());
        if (idx >= 0) {
            m_values[idx] = 0.0;
            emit valueChanged(idx, 0.0);
            emit valuesChanged(m_values);
            update();
        }
    }
}

void HistogramWidget::mouseMoveEvent(QMouseEvent *event) 
{
    if (m_dragIndex >= 0) {
        double v = yToValue(event->pos().y());
        m_values[m_dragIndex] = v;

        emit valueChanged(m_dragIndex, v);
        emit valuesChanged(m_values);
        update();
    }
}

void HistogramWidget::mouseReleaseEvent(QMouseEvent *)
{
    m_dragIndex = -1;
    update();
}

void HistogramWidget::setScale(double scale)
{
    m_scale = scale;
    update();
}

void HistogramWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect(rect(), Qt::white);

    if (m_values.isEmpty())
        return;

    double maxAbs = m_maxAbs;

    int n = m_values.size();
    double spacingRatio = 0.2; // 20% spacing between bars

    double totalWidth = width();
    double barFullWidth = totalWidth / n;
    double barWidth = barFullWidth * (1.0 - spacingRatio);
    double spacing = barFullWidth * spacingRatio;

    double zeroY = height() / 2.0;
    double scaleY = m_scale * ( (height()-50) / 2.0) / maxAbs;

    p.setPen(QPen(Qt::black, 1));
    p.drawLine(0, zeroY, width(), zeroY);

    p.setPen(Qt::NoPen);
    
    for (int i = 0; i < n; ++i) {
        if (i == m_dragIndex)
            p.setBrush(QColor(255, 100, 100));
        else
            p.setBrush(QColor(100, 150, 255));

        double v = m_values[i];

        double x = i * barFullWidth + spacing / 2.0;
        double h = v * scaleY;

        QRectF bar;

        if (v >= 0) {
            bar = QRectF(x, zeroY - h, barWidth, h);
        } else {
            bar = QRectF(x, zeroY, barWidth, -h);
        }

        p.setPen(Qt::black);
        p.drawText(bar.topLeft() + QPointF(0, -2), QString::number(v, 'f', 2));
        p.setPen(Qt::NoPen);

        p.drawRect(bar);
    }
}

