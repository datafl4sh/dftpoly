#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QVector>
#include <QPointF>
#include <QDebug>

#include "points.h"
#include "dft.h"

PolygonEditorWidget::PolygonEditorWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(500, 500);
    setFocusPolicy(Qt::StrongFocus);
    double sX = width() / 2.0;
    double sY = height() / 2.0;

    m_scale = std::min(sX, sY);
}

void PolygonEditorWidget::setShowLabels(bool enabled)
{
    m_showLabels = enabled;
    update();
}

void PolygonEditorWidget::rotatePolygon(double angleRad)
{
    double c = std::cos(angleRad);
    double s = std::sin(angleRad);

    for (QPointF& p : m_points) {
        double x = p.x();
        double y = p.y();

        double xr = x * c - y * s;
        double yr = x * s + y * c;

        p = QPointF(xr, yr);
    }

    emit polygonChanged(m_points);
    update();
}

void PolygonEditorWidget::rescalePolygon(double scale)
{

    for (QPointF& p : m_points) {
        double x = p.x();
        double y = p.y();

        p = QPointF(x*scale, y*scale);
    }

    emit polygonChanged(m_points);
    update();
}

QPointF PolygonEditorWidget::worldToScreen(const QPointF& w) const
{
    double s = m_scale;

    double x = width()  * 0.5 + w.x() * s;
    double y = height() * 0.5 - w.y() * s;

    return QPointF(x, y);
}

QPointF PolygonEditorWidget::screenToWorld(const QPointF& s) const
{
    double scale = m_scale;

    double x = (s.x() - width()  * 0.5) / scale;
    double y = (height() * 0.5 - s.y()) / scale;

    return QPointF(x, y);
}

void PolygonEditorWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    drawAxes(p);
    drawPolygon(p);
    drawPoints(p);
}

void PolygonEditorWidget::drawAxes(QPainter& p)
{
    p.setPen(QPen(Qt::lightGray, 1));
    QPointF origin = worldToScreen(QPointF(0, 0));
    p.drawLine(0, origin.y(), width(), origin.y());
    p.drawLine(origin.x(), 0, origin.x(), height());
}

void PolygonEditorWidget::drawPolygon(QPainter& p)
{
    if (m_points.size() < 2)
        return;

    QPolygonF poly;
    for (const QPointF& w : m_points)
        poly << worldToScreen(w);

    p.setPen(QPen(Qt::blue, 2));
    p.drawPolygon(poly);
}

void PolygonEditorWidget::drawPoints(QPainter& p)
{
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::red);

    for (int i = 0; i < m_points.size(); ++i) {
        QPointF screen = worldToScreen(m_points[i]);
        p.drawEllipse(screen, m_radius, m_radius);

        if (m_showLabels) {
            QString text = QString("(%1, %2)")
                    .arg(m_points[i].x(), 0, 'f', 2)
                    .arg(m_points[i].y(), 0, 'f', 2);

            p.setPen(Qt::black);
            p.drawText(screen + QPointF(8, -8), text);
            p.setPen(Qt::NoPen);
        }
    }
}

void PolygonEditorWidget::resizeEvent(QResizeEvent *)
{
    double sX = width() / 2.0;
    double sY = height() / 2.0;
    m_scale = std::min(sX, sY);
}


void PolygonEditorWidget::mousePressEvent(QMouseEvent *event)
{
    QPointF worldPos = screenToWorld(event->pos());
    int index = findPoint(event->pos());

    if (event->button() == Qt::LeftButton) {
        if (index >= 0) {
            m_dragIndex = index;
        } else {
            m_points.append(worldPos);
            emit polygonChanged(m_points);
            update();
        }
    }
    else if (event->button() == Qt::RightButton && index >= 0) {
        m_points.remove(index);
        emit polygonChanged(m_points);
        update();
    }
}

void PolygonEditorWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragIndex >= 0) {
        QPointF worldPos = screenToWorld(event->pos());
        m_points[m_dragIndex] = worldPos;

        emit pointMoved(m_dragIndex, worldPos);
        emit polygonChanged(m_points);

        update();
    }
}

void PolygonEditorWidget::mouseReleaseEvent(QMouseEvent *)
{
    m_dragIndex = -1;
}

void PolygonEditorWidget::wheelEvent(QWheelEvent *event)
{
    const double zoomFactor = 1.15;
    const double rotationStep = 5.0 * M_PI / 180.0;
    const double scaleFactor = 1.15;

    if (event->modifiers() & Qt::ShiftModifier) {
        if (event->angleDelta().y() > 0)
            rotatePolygon(rotationStep);
        else
            rotatePolygon(-rotationStep);
    } else if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0)
            rescalePolygon(scaleFactor);
        else
            rescalePolygon(1./scaleFactor);
    }
    else {
        if (event->angleDelta().y() > 0) {
            m_scale *= zoomFactor;
            m_zoom *= zoomFactor;
        } else {
            m_scale /= zoomFactor;
            m_zoom /= zoomFactor;
        }
        emit zoomChanged(m_zoom);
    }
    update();
}

void PolygonEditorWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_C) {
        m_points.clear();
        emit polygonChanged(m_points);
        update();
    } else if (event->key() == Qt::Key_U) {
        for (size_t i = 0; i < m_freqs.size(); i++) {
            m_freqs[i] = 0.0;
        }
        if (m_freqs.size() >= 2) {
            m_freqs[1] = 1.0;
        }
        std::vector<std::complex<double>> idftout = idft(m_freqs);
        auto N = m_freqs.size();
        m_points.clear();
        for (int i = 0; i < idftout.size(); i++) {
            m_points.append( {N*std::real(idftout[i]), N*std::imag(idftout[i])} );
        }
        emit polygonChanged(m_points);
        update();
    } else if (event->key() >= Qt::Key_3 && event->key() <= Qt::Key_9) {
        m_points.clear();
        int number = event->key() - Qt::Key_0;
        for (int i = 0; i < number; i++) {
            std::complex<double> angle{0.0, (2.0*M_PI*i)/number};
            std::complex<double> pt = std::exp(angle);
            m_points.append( {std::real(pt), std::imag(pt)} );
        }
        emit polygonChanged(m_points);
        update();
    }
    else {
        QWidget::keyPressEvent(event);
    }
}

int PolygonEditorWidget::findPoint(const QPointF &screenPos)
{
    for (int i = 0; i < m_points.size(); ++i) {
        QPointF sp = worldToScreen(m_points[i]);
        if (QLineF(screenPos, sp).length() <= m_radius + 3)
            return i;
    }
    return -1;
}

void PolygonEditorWidget::processPolygon(const QVector<QPointF>& points)
{
    auto N = points.size();
    std::vector<std::complex<double>> dftin;
    dftin.resize(points.size());
    for (size_t i = 0; i < points.size(); i++) {
        dftin[i] = { points[i].x(), points[i].y() };
    }

    m_freqs = dft(dftin);
    QVector<double> real, imag;
    real.resize(points.size());
    imag.resize(points.size());
    for (size_t i = 0; i < points.size(); i++) {
        real[i] = std::real(m_freqs[i])/N;
        imag[i] = std::imag(m_freqs[i])/N;
    }

    emit realValuesChanged(real);
    emit imagValuesChanged(imag);
}

void PolygonEditorWidget::setRealValues(const QVector<double>& points)
{
    if (points.size() != m_freqs.size()) {
        return;
    }

    auto N = points.size();

    for (size_t i = 0; i < points.size(); i++) {
        auto real = N*points[i];
        auto imag = std::imag(m_freqs[i]);
        m_freqs[i] = {real, imag};
    }

    std::vector<std::complex<double>> idftout = idft(m_freqs);

    m_points.clear();
    for (int i = 0; i < idftout.size(); i++) {
        m_points.append( {std::real(idftout[i]), std::imag(idftout[i])} );
    }

    update();
}

void PolygonEditorWidget::setImagValues(const QVector<double>& points)
{
    if (points.size() != m_freqs.size()) {
        return;
    }

    auto N = points.size();

    for (size_t i = 0; i < points.size(); i++) {
        auto real = std::real(m_freqs[i]);
        auto imag = N*points[i];
        m_freqs[i] = {real, imag};
    }

    std::vector<std::complex<double>> idftout = idft(m_freqs);

    m_points.clear();
    for (int i = 0; i < idftout.size(); i++) {
        m_points.append( {std::real(idftout[i]), std::imag(idftout[i])} );
    }

    update();
}
