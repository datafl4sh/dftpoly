#include "HHOModel.h"

#include "diskpp/loaders/loader.hpp"
#include "diskpp/loaders/loader_utils.hpp"
#include "diskpp/mesh/meshgen.hpp"
#include "diskpp/output/silo.hpp"


template<disk::mesh_2D Mesh>
void
adjust_stabfree_recdeg(const Mesh& msh, const typename Mesh::cell_type& cl,
    disk::hho_degree_info& hdi, size_t addincr)
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
        hdi.reconstruction_degree(rpd+addincr);
    }
    else {
        /* Every harmonic degree provides 2 additional dofs, therefore
         * we need an increment that it is sufficient to accomodate
         * (from-to) dofs => ((from - to) + (2-1))/2 */
        size_t incr = (from - to + 1)/2;
        hdi.reconstruction_degree(rpd+incr+addincr);
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
        adjust_stabfree_recdeg(msh, cl, hdi, cfg.incr_above_opt);

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

    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es(A);

    Eigen::Matrix<T, Eigen::Dynamic, 1> eigs = es.eigenvalues();

    std::sort(eigs.begin(), eigs.end());

    return eigs;
}


template<disk::mesh_2D Mesh>
auto eig_stiffness_bccm(const Mesh& msh, const config& cfg)
{
    using T = typename Mesh::coordinate_type;

    Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> A;
    
    auto cl = msh[0];
    disk::hho_degree_info hdi(cfg.degree);


    adjust_stabfree_recdeg(msh, cl, hdi, cfg.incr_above_opt);

    auto hb = make_scalar_harmonic_top_basis(msh, cl, hdi.reconstruction_degree());
    hb.maximum_polynomial_degree(hdi.cell_degree()+2);

    A = disk::make_stiffness_matrix(msh, cl, hb);

    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es(A);

    Eigen::Matrix<T, Eigen::Dynamic, 1> eigs = es.eigenvalues();

    std::sort(eigs.begin(), eigs.end());

    return eigs;
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
HHOModel::setIncrAboveOpt(size_t iao)
{
    hhocfg.incr_above_opt = iao;
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
    gradientDelta = delta;
    computeGradients(delta);
}

void
HHOModel::saveEigenFunctions()
{
    if (polypts.size() < 3) {
        return;
    }

    mesh_type msh;
    disk::make_single_elem_mesh_from_points(msh, polypts);

    using T = typename mesh_type::coordinate_type;
    using dm = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;
    using dv = Eigen::Matrix<T, Eigen::Dynamic, 1>;

    dm A, GR;

    Eigen::Matrix<T,mesh_type::dimension,mesh_type::dimension> Id =
        Eigen::Matrix<T,mesh_type::dimension,mesh_type::dimension>::Identity();
    
    auto cl = msh[0];
    disk::hho_degree_info hdi(hhocfg.degree);

    if (hhocfg.use_stabfree) {
        adjust_stabfree_recdeg(msh, cl, hdi, hhocfg.incr_above_opt);

        if (hhocfg.variant == hho_variant::mixed_order_high) {
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

    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> es(A);

    dv eigvals = es.eigenvalues();
    dm eigvecs = es.eigenvectors();

    disk::triangular_mesh<double> ev_msh;
    disk::submesh_via_gmsh(msh, ev_msh, 0.05*disk::average_diameter(msh));
    disk::silo_database db;
    db.create("poly_eigfuns.silo");
    db.add_mesh(ev_msh, "mesh");

    /*
    auto cb = disk::make_scalar_monomial_basis(msh, cl, hdi.cell_degree());

    for (int col = 0; col < eigvecs.cols(); col++) {
        std::vector<T> vals;
        for (size_t i = 0; i < ev_msh.points_size(); i++) {
            auto pt = ev_msh.point_at(i);
            auto phi = cb.eval_functions(pt);
            T val = phi.dot( eigvecs.col(col).segment(0,cb.size()) );
            vals.push_back(val);
        }
        db.add_variable("mesh", "ef_"+std::to_string(col), vals, disk::nodal_variable_t);
    }
    */

    if (hhocfg.use_stabfree) {
        adjust_stabfree_recdeg(msh, cl, hdi, hhocfg.incr_above_opt);
        auto hb = make_scalar_harmonic_top_basis(msh, cl, hdi.reconstruction_degree());
        hb.maximum_polynomial_degree(hdi.cell_degree()+2);

        for (int col = 0; col < eigvecs.cols(); col++) {
            dv r_eigfun = GR*eigvecs.col(col);

            std::vector<T> vals;
            for (size_t i = 0; i < ev_msh.points_size(); i++) {
                auto pt = ev_msh.point_at(i);
                auto phi = hb.eval_functions(pt);
                T val = r_eigfun.dot(phi);
                vals.push_back(val);
            }
            db.add_variable("mesh", "ef_"+std::to_string(col), vals, disk::nodal_variable_t);
        }
    }
}

void
HHOModel::recompute()
{
    if (polypts.size() < 3) {
        return;
    }

    mesh_type msh;
    disk::make_single_elem_mesh_from_points(msh, polypts);

    computeGradients(gradientDelta);

    Eigen::VectorXd eigsGG = test(msh, hhocfg);
    emit eigsGGChanged(eigsGG);

    Eigen::VectorXd eigsStiff = eig_stiffness_bccm(msh, hhocfg);
    emit eigsStiffChanged(eigsStiff);
}

void
HHOModel::computeGradients(float delta)
{
    static const size_t num_grads = NUM_GRADIENT_DIRECTIONS;

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
        double eig_c = test(msh, hhocfg)[1];

        stor->points[point] = {pt_c.x() + delta, pt_c.y()};
        ret[num_grads*point + 0] = (test(msh, hhocfg)[1] - eig_c) / delta;

        stor->points[point] = {pt_c.x() + delta*c, pt_c.y() + delta*s};
        ret[num_grads*point + 1] = (test(msh, hhocfg)[1] - eig_c) / delta;

        stor->points[point] = {pt_c.x(), pt_c.y() + delta};
        ret[num_grads*point + 2] = (test(msh, hhocfg)[1] - eig_c) / delta;

        stor->points[point] = {pt_c.x() - delta*c, pt_c.y() + delta*s};
        ret[num_grads*point + 3] = (test(msh, hhocfg)[1] - eig_c) / delta;

        stor->points[point] = {pt_c.x() - delta, pt_c.y()};
        ret[num_grads*point + 4] = (test(msh, hhocfg)[1] - eig_c) / delta;

        stor->points[point] = {pt_c.x() - delta*c, pt_c.y() - delta*s};
        ret[num_grads*point + 5] = (test(msh, hhocfg)[1] - eig_c) / delta;

        stor->points[point] = {pt_c.x(), pt_c.y() - delta};
        ret[num_grads*point + 6] = (test(msh, hhocfg)[1] - eig_c) / delta;

        stor->points[point] = {pt_c.x() + delta*c, pt_c.y() - delta*s};
        ret[num_grads*point + 7] = (test(msh, hhocfg)[1] - eig_c) / delta;

        stor->points[point] = pt_c;
    }

    emit gradientsChanged(ret);
}