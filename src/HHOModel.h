#pragma once

#include <QObject>
#include <QVector>
#include <QPointF>
#include <vector>

#include "diskpp/mesh/mesh.hpp"
#include "diskpp/mesh/meshgen.hpp"
#include "diskpp/methods/hho"
#include "diskpp/bases/bases_utils.hpp"

#define NUM_GRADIENT_DIRECTIONS 8

enum class hho_variant {
    mixed_order_low,
    equal_order,
    mixed_order_high
};
struct config {
    size_t          degree = 0;
    bool            use_stabfree = true;
    hho_variant     variant;
    size_t          incr_above_opt = 0;
};


class HHOModel : public QObject {

    Q_OBJECT

private:
    config hhocfg;
    using mesh_type = disk::generic_mesh<double,2>;
    using point_type = disk::point<double, 2>;
    
    std::vector<point_type> polypts;

    float   gradientDelta = 1;

    void    recompute();
    void    computeGradients(float);

public:
    HHOModel();

public slots:
    void    setPolygon(const QVector<QPointF>&);
    void    setDegree(size_t);
    void    useStabfree(bool);
    void    setIncrAboveOpt(size_t);
    void    setVariant(hho_variant);
    void    setGradientDelta(float);

signals:
    void    minEigenvalueChanged(double);
    void    gradientsChanged(const std::vector<double>&);
};