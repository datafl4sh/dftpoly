#pragma once
#include <QComboBox>
#include <QLabel>

#include "hist.h"

#include "diskpp/mesh/mesh.hpp"
#include "diskpp/mesh/meshgen.hpp"
#include "diskpp/methods/hho"
#include "diskpp/bases/bases_utils.hpp"

namespace priv {

enum class hho_variant {
    mixed_order_low,
    equal_order,
    mixed_order_high
};
struct config {
    size_t          degree = 0;
    bool            use_stabfree = true;
    hho_variant     variant;
};

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
auto test(const Mesh& msh, const priv::config& cfg)
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

        if (cfg.variant == priv::hho_variant::mixed_order_high) {
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

} // namespace priv

class StabfreeHHOParamsWidget : public QWidget
{
    Q_OBJECT

public:
    StabfreeHHOParamsWidget(QWidget *parent = nullptr);

private slots:
    void recompute(void);
    void hhoTypeChanged(int);
    void hhoOrderChanged(int);
    void hhoVariantChanged(int);

public slots:
    void polygonChanged(const QVector<QPointF>&);

private:
    QComboBox   *hhoTypeCombo;
    QComboBox   *hhoVariantCombo;
    QComboBox   *hhoOrderCombo;
    QLabel      *smallestEigLabel;

    using mesh_type = disk::generic_mesh<double,2>;
    using point_type = disk::point<double, 2>;
    mesh_type msh;
    priv::config hhocfg;
    std::vector<point_type> polypts;
};