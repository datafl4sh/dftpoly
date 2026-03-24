#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QVector>
#include <QPointF>
#include <QDebug>
#include <QLabel>

#include <complex>

#include <Eigen/Dense>

#pragma once

class PolygonEditorWidget : public QWidget
{
    Q_OBJECT

public:
    PolygonEditorWidget(QWidget *parent = nullptr);

    QPointF barycenter() const;
    Eigen::Matrix2d inertiaMatrix() const;

signals:
    void pointMoved(int index, QPointF worldPos);
    void polygonChanged(const QVector<QPointF>& points);
    void realValuesChanged(const QVector<double>& vals);
    void imagValuesChanged(const QVector<double>& vals);
    void zoomChanged(double zoom);

public slots:
    void processPolygon(const QVector<QPointF>& points);
    void setRealValues(const QVector<double>& vals);
    void setImagValues(const QVector<double>& vals);

    void setShowLabels(bool enabled);
    void setShowInertiaAxes(bool enabled);
    void setShowBoundingBox(bool enabled);
    void setShowInertiaTransformed(bool enabled);

protected:
    QPointF worldToScreen(const QPointF& w) const;
    QPointF screenToWorld(const QPointF& s) const;

    void paintEvent(QPaintEvent *) override;
    void rotatePolygon(double);
    void rescalePolygon(double);
    void drawAxes(QPainter& p);
    void drawPolygon(QPainter& p);
    void drawPoints(QPainter& p);
    void drawBoundingBox(QPainter& p);
    void resizeEvent(QResizeEvent *) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QVector<QPointF> m_points;
    std::vector<std::complex<double>> m_freqs;
    int m_dragIndex = -1;
    double m_scale = 250.0;
    double m_zoom = 1.0;
    int m_radius = 6;
    bool m_showLabels = false;
    bool m_showInertiaAxes = false;
    bool m_showBoundingBox = false;
    bool m_showInertiaTransformed = false;

    int findPoint(const QPointF &screenPos);
};