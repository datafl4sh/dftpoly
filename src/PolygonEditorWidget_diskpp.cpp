#include <QPoint>

#include "points.h"

#include "diskpp/mesh/mesh.hpp"
#include "diskpp/mesh/meshgen.hpp"

void
PolygonEditorWidget::drawDiskppNormals(QPainter& p)
{
    using mesh_type = disk::generic_mesh<double,2>;
    using point_type = disk::point<double, 2>;

    if (m_points.size() < 3) {
        return;
    }

    std::vector<point_type> polypts;
    for (auto& pt : m_points) {
        polypts.push_back({pt.x(), pt.y()});
    }

    mesh_type msh;
    disk::make_single_elem_mesh_from_points(msh, polypts);
    auto cl = msh.cell_at(0);
    auto h = diameter(msh, cl);

    p.setPen(QPen(QColor(160,100,30), 1));
    auto fcs = faces(msh, cl);
    for (const auto& fc : fcs)
    {
        point_type from = disk::barycenter(msh, fc);
        point_type to = from + (0.05*h*normal(msh, cl, fc)).eval();

        QPointF sfrom = worldToScreen({from.x(), from.y()});
        QPointF sto = worldToScreen({to.x(), to.y()});
        drawArrow(p, sfrom, sto);
    }
    //p.setBrush(Qt::NoPen);

}