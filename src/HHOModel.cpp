#include "HHOModel.h"

#include "diskpp/loaders/loader.hpp"
#include "diskpp/loaders/loader_utils.hpp"
#include "diskpp/mesh/meshgen.hpp"


template<disk::mesh_2D Mesh>
void
adjust_stabfree_recdeg(const Mesh& msh, const typename Mesh::cell_type& cl,
    disk::hho_degree_info& hdi)
{
    size_t cd = hdi.cell_degree();
    size_t fd = hdi.face_degree();
    bool is_mixed_high = (hdi.cell_degree() > hdi.face_degree());
    size_t n = faces(msh, cl).size();   
    size_t rpd = cd+2;

    /* HHO space dofs */
    size_t from = ((cd+2)*(cd+1))/2 + n*(fd+1);
    /* Reconstruction dofs */
    size_t to = ((rpd+2)*(rpd+1))/2;

    if (from <= to) {
        hdi.reconstruction_degree(rpd);
    }
    else {
        /* Every harmonic degree provides 2 additional dofs, therefore
         * we need an increment that it is sufficient to accomodate
         * (from-to) dofs => ((from - to) + (2-1))/2 */
        size_t incr = (from - to + 1)/2;
        hdi.reconstruction_degree(rpd+incr);
    }
}

template<disk::mesh_2D Mesh>
auto test(const Mesh& msh, const config& cfg)
{
    using T = typename Mesh::coordinate_type;

    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> A;
    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> GR;

    Eigen::Matrix<T,Mesh::dimension,Mesh::dimension> Id =
        Eigen::Matrix<T,Mesh::dimension,Mesh::dimension>::Identity();
    
    auto cl = msh[0];
    disk::hho_degree_info hdi(cfg.degree);

    if (cfg.use_stabfree) {
        adjust_stabfree_recdeg(msh, cl, hdi);

        if (cfg.variant == hho_variant::mixed_order_high) {
            auto oper = make_shl_face_proj_harmonic(msh, cl, hdi, Id);
            A = oper.second;
            GR = oper.first;
        } else {
            auto oper = make_sfl(msh, cl, hdi, Id);
            A = oper.second;
            GR = oper.first;
        }
    }
    else {
        auto oper = make_scalar_hho_laplacian(msh, cl, hdi);
        A = oper.second;
        GR = oper.first;
        A = A + make_scalar_hho_stabilization(msh, cl, GR, hdi);
    }

    Eigen::Matrix<std::complex<T>, Eigen::Dynamic, 1> eigs = A.eigenvalues();
    Eigen::Matrix<T, Eigen::Dynamic, 1> eigsa = eigs.cwiseAbs();

    std::sort(eigsa.begin(), eigsa.end());

    //for (size_t i = 0; i < eigsa.size(); i++)
    //    std::cout << eigsa[i] << " ";
    //std::cout << std::endl;

    auto min_nonzero = eigsa[1];
    for (size_t i = 1; i < eigsa.size(); i++)
        min_nonzero = std::min(min_nonzero, eigsa[i]);

    return min_nonzero;
}


HHOModel::HHOModel()
{
    hhocfg.degree = 0;
    hhocfg.use_stabfree = true;
    hhocfg.variant = hho_variant::equal_order;
}

void
HHOModel::setPolygon(const QVector<QPointF>& points)
{
    polypts.clear();
    polypts.reserve( points.size() );
    for (const auto& pt : points) {
        polypts.push_back({pt.x(), pt.y()});
    }
    recompute();
}

void
HHOModel::setDegree(size_t degree)
{
    hhocfg.degree = degree;
    recompute();
}

void
HHOModel::useStabfree(bool en)
{
    hhocfg.use_stabfree = en;
    recompute();
}

void
HHOModel::setVariant(hho_variant variant)
{
    hhocfg.variant = variant;
    recompute();
}

void
HHOModel::setGradientDelta(float delta)
{
    computeGradients(delta);
}

void
HHOModel::recompute()
{
    if (polypts.size() < 3) {
        return;
    }

    mesh_type msh;
    disk::make_single_elem_mesh_from_points(msh, polypts);

    double eig = test(msh, hhocfg);
    emit minEigenvalueChanged(eig);
}

void
HHOModel::computeGradients(float delta)
{
    static const size_t num_grads = 8;

    if (polypts.size() < 3) {
        return;
    }

    std::vector<double> ret;
    ret.resize(num_grads * polypts.size());

    double c = std::cos(M_PI/4.0);
    double s = std::sin(M_PI/4.0);

    mesh_type msh;
    disk::make_single_elem_mesh_from_points(msh, polypts);
    auto stor = msh.backend_storage();

    for (int point = 0; point < polypts.size(); point++) {

        auto pt_c = stor->points[point];
        double eig_c = test(msh, hhocfg);

        stor->points[point] = {pt_c.x() + delta, pt_c.y()};
        ret[num_grads*point + 0] = (test(msh, hhocfg) - eig_c) / delta;

        stor->points[point] = {pt_c.x() + delta*c, pt_c.y() + delta*s};
        ret[num_grads*point + 1] = (test(msh, hhocfg) - eig_c) / delta;

        stor->points[point] = {pt_c.x(), pt_c.y() + delta};
        ret[num_grads*point + 2] = (test(msh, hhocfg) - eig_c) / delta;

        stor->points[point] = {pt_c.x() - delta*c, pt_c.y() + delta*s};
        ret[num_grads*point + 3] = (test(msh, hhocfg) - eig_c) / delta;

        stor->points[point] = {pt_c.x() - delta, pt_c.y()};
        ret[num_grads*point + 4] = (test(msh, hhocfg) - eig_c) / delta;

        stor->points[point] = {pt_c.x() - delta*c, pt_c.y() - delta*s};
        ret[num_grads*point + 5] = (test(msh, hhocfg) - eig_c) / delta;

        stor->points[point] = {pt_c.x(), pt_c.y() - delta};
        ret[num_grads*point + 6] = (test(msh, hhocfg) - eig_c) / delta;

        stor->points[point] = {pt_c.x() + delta*c, pt_c.y() - delta*s};
        ret[num_grads*point + 7] = (test(msh, hhocfg) - eig_c) / delta;

        stor->points[point] = pt_c;
    }

    emit gradientsChanged(ret);
}