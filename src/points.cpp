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


void drawArrow(QPainter &painter, QPointF start, QPointF end)
{
    painter.drawLine(start, end);

    const double arrowSize = 10.0;
    QLineF line(start, end);
    auto linelen = std::hypot(line.dx(), line.dy());

    if (linelen < 1.1*arrowSize)
        return;

    
    double angle = std::atan2(-line.dy(), line.dx());

    QPointF arrowP1 = end - QPointF(
        std::sin(angle + M_PI / 3.0) * arrowSize,
        std::cos(angle + M_PI / 3.0) * arrowSize
    );

    QPointF arrowP2 = end - QPointF(
        std::sin(angle + M_PI - M_PI / 3.0) * arrowSize,
        std::cos(angle + M_PI - M_PI / 3.0) * arrowSize
    );

    QPolygonF arrowHead;
    arrowHead << end << arrowP1 << arrowP2;

    painter.setBrush(painter.pen().color());
    painter.drawPolygon(arrowHead);
}

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

QPointF PolygonEditorWidget::barycenter() const
{
    QPointF ret{0.0, 0.0};
    for (const QPointF& p : m_points) {
        ret += p;
    }
    ret /= m_points.size();
    return ret;
}

QVector<QPointF> quadrature_points(const QPointF& a,
    const QPointF& b, const QPointF& c)
{
    QVector<QPointF> ret;
    QPointF qp0 = a/6.0 + b/6.0 + 2.0*c/3.0;
    QPointF qp1 = a/6.0 + 2.0*b/3.0 + c/6.0;
    QPointF qp2 = 2.0*a/3.0 + b/6.0 + c/6.0;
    ret.append(qp0);
    ret.append(qp1);
    ret.append(qp2);
    return ret;
}

float signed_area(const QPointF& a,
    const QPointF& b, const QPointF& c)
{
    QPointF va = b-a;
    QPointF vb = c-a;
    return 0.5*(va.x()*vb.y() - vb.x()*va.y());
}

Eigen::Matrix2d mass(const QPointF p)
{
    Eigen::Matrix2d ret;
    ret << p.x()*p.x(), p.x()*p.y(), p.y()*p.x(), p.y()*p.y();
    return ret;
}

Eigen::Matrix2d inertia(const QPointF p)
{
    Eigen::Matrix2d ret;
    ret << p.y()*p.y(), -p.x()*p.y(), -p.y()*p.x(), p.x()*p.x();
    return ret;
}

Eigen::Matrix2d
PolygonEditorWidget::inertiaMatrix() const
{
    QPointF bar = barycenter();

    Eigen::Matrix2d M = Eigen::Matrix2d::Zero();

    auto N = m_points.size();
    for (int i = 0; i < N; i++) {
        QPointF a = bar;
        QPointF b = m_points[i];
        QPointF c = m_points[(i+1)%N];
        float qw = std::abs(signed_area(a,b,c))/3.0;
        auto qps = quadrature_points(a,b,c);
        for (const auto& qp : qps) {
            auto p = qp - bar;
            M += qw * (mass(p) /* - inertia(p)*/);
        }
    }

    return M;
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
    drawBoundingBox(p);
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

    if (m_points.size() > 3) {
        QPen pen(Qt::lightGray, 1);
        pen.setStyle(Qt::DashLine);
        p.setPen(pen);
        QPointF bar = worldToScreen(barycenter());
        for (int i = 0; i < m_points.size(); i++) {
            p.drawLine(bar, worldToScreen(m_points[i]));
        }
    }
}

void PolygonEditorWidget::drawBoundingBox(QPainter& p)
{
    if (m_points.size() < 3)
        return;

    auto [xmin, ymin] = worldToScreen(m_points[0]);
    auto [xmax, ymax] = worldToScreen(m_points[0]);

    QPolygonF poly;
    for (const QPointF& w : m_points) {
        QPointF ws = worldToScreen(w);
        xmin = std::min(ws.x(), xmin);
        ymin = std::min(ws.y(), ymin);
        xmax = std::max(ws.x(), xmax);
        ymax = std::max(ws.y(), ymax);
    }

    QRectF bbox(xmin, ymin, xmax-xmin, ymax - ymin);

    QPen pen( QColor(70,70,40) , 1);
    p.setBrush(Qt::NoBrush);
    pen.setStyle(Qt::DashLine);
    p.setPen(pen);
    p.drawRect(bbox);
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

    p.setBrush(QColor(0, 150, 50));
    QPointF bar = barycenter();
    QPointF bars = worldToScreen(bar);
    p.drawRect(bars.x()-4, bars.y()-4, 8, 8);

    Eigen::Matrix2d mass = inertiaMatrix();
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix2d> es(mass);

    Eigen::Matrix2d eigvecs = es.eigenvectors();
    Eigen::Vector2d eigvals = es.eigenvalues().cwiseSqrt();
    eigvals /= eigvals.maxCoeff();

    Eigen::Matrix2d d = (eigvecs*eigvals.asDiagonal());

    QPointF v1{ bar.x() + d(0,0), bar.y() + d(1,0) };
    QPointF v1s = worldToScreen(v1);

    QPointF v2{ bar.x() + d(0,1), bar.y() + d(1,1) };
    QPointF v2s = worldToScreen(v2);

    QPen pen(QColor(0, 50, 150), 1);
    pen.setStyle(Qt::DashLine);
    p.setPen(pen);
    //p.drawLine(bars, v1s);
    //p.drawLine(bars, v2s);
    drawArrow(p, bars, v1s);
    drawArrow(p, bars, v2s);
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
