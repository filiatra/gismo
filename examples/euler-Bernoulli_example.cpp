/** @file poisson2_example.cpp

    @brief Tutorial on how to use expression assembler to solve the Poisson equation

    This file is part of the G+Smo library.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.

    Author(s): A. Mantzaflaris
*/

//! [Include namespace]
#include <typeinfo>
#include <gismo.h>

#  define MatExprType  auto

namespace gismo{
namespace expr{

// Comments for normal:
// - TODO: dimensionm indep. later on
template<class E>
class Gcon_expr : public _expr<Gcon_expr<E> >
{
public:
    typedef E Scalar;

private:

    typename gsGeometryMap<Scalar>::Nested_t _G;

public:
    Gcon_expr(const gsGeometryMap<Scalar> & G) : _G(G) { }

    mutable gsVector<Scalar,3> Gcov;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    MatExprType eval(const index_t k) const
    {
        Gcov = _G.data().values[1].col(k);
        return Gcov/(Gcov.norm()*Gcov.norm());
    }

    index_t rows() const { return 3; }
    index_t cols() const { return 1; }

    void setFlag() const
    {
        _G.data().flags |= NEED_DERIV;
    }

    void parse(gsSortedVector<const gsFunctionSet<Scalar>*> & evList) const
    {
        evList.push_sorted_unique(&_G.source());
        _G.data().flags |= NEED_DERIV;
    }

    const gsFeSpace<Scalar> & rowVar() const { return gsNullExpr<Scalar>::get(); }
    const gsFeSpace<Scalar> & colVar() const {return gsNullExpr<Scalar>::get();}
    index_t cardinality_impl() const { return 1; }

    static constexpr bool rowSpan() {return false; }
    static constexpr bool colSpan() {return false;}

    void print(std::ostream &os) const { os << "Gcon("; _G.print(os); os <<")"; }
};

template<class E>
class curveNormal_expr : public _expr<curveNormal_expr<E> >
{
public:
    typedef E Scalar;

private:

    typename gsGeometryMap<Scalar>::Nested_t _G;

public:
    curveNormal_expr(const gsGeometryMap<Scalar> & G) : _G(G) { }

    mutable gsVector<Scalar,3> cDer1, cDer2, b;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    MatExprType eval(const index_t k) const
    {
        const size_t domainDim = _G.data().dim.first;
        const size_t targetDim = _G.data().dim.second;

        cDer1 = _G.data().values[1].reshapeCol(k,targetDim,domainDim); // gives a vector of 3 rows and 1 column for a 3D domain
        cDer2 = _G.data().values[2].reshapeCol(k,targetDim,domainDim); // gives a vector of 3 rows and 1 column for a 3D domain
        b = cDer1.cross(cDer2).normalized();
        return b.cross(cDer1).normalized();
    }

    index_t rows() const { return 3; }
    index_t cols() const { return 1; }

    void setFlag() const
    {
        _G.data().flags |= NEED_NORMAL | NEED_DERIV | NEED_2ND_DER;
    }

    void parse(gsSortedVector<const gsFunctionSet<Scalar>*> & evList) const
    {
        evList.push_sorted_unique(&_G.source());
        _G.data().flags |= NEED_NORMAL | NEED_DERIV | NEED_2ND_DER;
    }

    const gsFeSpace<Scalar> & rowVar() const { return gsNullExpr<Scalar>::get(); }
    const gsFeSpace<Scalar> & colVar() const {return gsNullExpr<Scalar>::get();}
    index_t cardinality_impl() const { return 1; }

    static constexpr bool rowSpan() {return false; }
    static constexpr bool colSpan() {return false;}

    void print(std::ostream &os) const { os << "normal("; _G.print(os); os <<")"; }
};


// Comments for normal:
// - TODO: dimensionm indep. later on
template<class E>
class curveBinormal_expr : public _expr<curveBinormal_expr<E> >
{
public:
    typedef E Scalar;

private:

    typename gsGeometryMap<Scalar>::Nested_t _G;

public:
    curveBinormal_expr(const gsGeometryMap<Scalar> & G) : _G(G) { }

    mutable gsVector<Scalar,3> cDer1, cDer2;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    MatExprType eval(const index_t k) const
    {
        const size_t domainDim = _G.data().dim.first;
        const size_t targetDim = _G.data().dim.second;

        cDer1 = _G.data().values[1].reshapeCol(k,targetDim,domainDim); // gives a vector of 3 rows and 1 column for a 3D domain
        cDer2 = _G.data().values[2].reshapeCol(k,targetDim,domainDim); // gives a vector of 3 rows and 1 column for a 3D domain
        return cDer1.cross(cDer2).normalized();
    }

    index_t rows() const { return 3; }
    index_t cols() const { return 1; }

    void setFlag() const
    {
        _G.data().flags |= NEED_NORMAL | NEED_DERIV | NEED_2ND_DER;
    }

    void parse(gsSortedVector<const gsFunctionSet<Scalar>*> & evList) const
    {
        evList.push_sorted_unique(&_G.source());
        _G.data().flags |= NEED_NORMAL | NEED_DERIV | NEED_2ND_DER;
    }

    const gsFeSpace<Scalar> & rowVar() const { return gsNullExpr<Scalar>::get(); }
    const gsFeSpace<Scalar> & colVar() const {return gsNullExpr<Scalar>::get();}
    index_t cardinality_impl() const { return 1; }

    static constexpr bool rowSpan() {return false; }
    static constexpr bool colSpan() {return false;}

    void print(std::ostream &os) const { os << "binormal("; _G.print(os); os <<")"; }
};

// Comments for var1:
// - TODO: dimensionm indep. later on
template<class E>
class var1_expr : public _expr<var1_expr<E> >
{
public:
    typedef typename E::Scalar Scalar;

private:

    typename E::Nested_t _u;
    typename gsGeometryMap<Scalar>::Nested_t _G;

public:
    enum {ColBlocks = E::rowSpan() };
    enum{ Space = E::Space };

    var1_expr(const E & u, const gsGeometryMap<Scalar> & G) : _u(u), _G(G) { }

    mutable gsMatrix<Scalar> res;

    mutable gsMatrix<Scalar> bGrads, bDer2s;//, cJac, cDer2;
    mutable gsVector<Scalar,3> cDer1, cDer2, normal, b, b_der, n_der;
    mutable gsMatrix<Scalar,3,3> eye, A, B;
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    // helper function
    static inline gsVector<Scalar,3> vecFun(index_t pos, Scalar val)
    {
        gsVector<Scalar,3> result = gsVector<Scalar,3>::Zero();
        result[pos] = val;
        return result;
    }

    MatExprType eval(const index_t k) const
    {
        const index_t numAct = _u.cardinality()/_u.targetDim();
        const size_t domainDim = _G.data().dim.first;
        const size_t targetDim = _G.data().dim.second;
        res.resize(rows(), _u.cardinality()); // rows()*

        bGrads = _u.data().values[1].reshapeCol(k, _u.data().dim.first, numAct ); // gives a vector of 1 row (since we have scalar basis function), and numAct columns
        bDer2s = _u.data().values[2].reshapeCol(k, _u.data().dim.first, numAct ); // gives a vector of 1 row (since we have scalar basis function), and numAct columns
        cDer1 = _G.data().values[1].reshapeCol(k,targetDim,domainDim); // gives a vector of 3 rows and 1 column for a 3D domain
        cDer2 = _G.data().values[2].reshapeCol(k,targetDim,domainDim); // gives a vector of 3 rows and 1 column for a 3D domain

        // gsDebugVar(measure);
        // gsVector<Scalar,3> Gcov = cDer1;
        // gsVector<Scalar,3> Gcon = Gcov/(Gcov.norm()*Gcov.norm());

        // gsDebugVar(math::pow(Gcon.norm(),2));

        eye.setIdentity();
        b = cDer1.cross(cDer2).normalized();
        normal = b.cross(cDer1).normalized();

        A = (eye - normal*normal.transpose()) / (b.cross(cDer1)).norm();
        B = (eye - b*b.transpose()) / (cDer1.cross(cDer2)).norm();

        for (index_t d = 0; d!= _u.dim(); ++d) // for all basis function components
        {
            const short_t s = d*numAct;
            for (index_t j = 0; j!= numAct; ++j) // for all actives
            {
                // Bi-normal vector
                b_der = B * ( vecFun(d, bGrads.at(2*j  ) ).cross(cDer2) + cDer1.cross(vecFun(d, bDer2s.at(2*j  ) ) ));

                // ---------------  First variation of the normal
                n_der = A * ( b_der.cross(cDer1) + b.cross(vecFun(d, bGrads.at(2*j)) ) );

                // res.row(s+j).noalias() = (m_v - ( normal.dot(m_v) ) * normal).transpose();
                res.col(s+j).noalias() = n_der; // outer-product version
            }
        }
        return res;
    }

    index_t rows() const { return _u.dim(); }
    index_t cols() const { return 1; }

    void setFlag() const
    {
        _u.data().flags |= NEED_GRAD | NEED_2ND_DER;
        _G.data().flags |= NEED_NORMAL | NEED_DERIV | NEED_2ND_DER;
    }

    void parse(gsSortedVector<const gsFunctionSet<Scalar>*> & evList) const
    {
        evList.push_sorted_unique(&_u.source());
        _u.data().flags |= NEED_GRAD | NEED_2ND_DER;
        _G.data().flags |= NEED_NORMAL | NEED_DERIV | NEED_2ND_DER;
    }

    const gsFeSpace<Scalar> & rowVar() const { return _u.rowVar(); }
    const gsFeSpace<Scalar> & colVar() const {return gsNullExpr<Scalar>::get();}
    index_t cardinality_impl() const { return _u.cardinality_impl(); }

    static constexpr bool rowSpan() {return E::rowSpan(); }
    static constexpr bool colSpan() {return false;}

    void print(std::ostream &os) const { os << "var1("; _u.print(os); os <<")"; }
};

// Comments for var2:
// - TODO: dimensionm indep. later on
// - TODO: how to structure this matrix
template<class E1, class E2, class E3>
class var2_expr : public _expr<var2_expr<E1,E2,E3> >
{
public:
    typedef typename E1::Scalar Scalar;

private:

    typename E1::Nested_t _u;
    typename E2::Nested_t _v;
    typename gsGeometryMap<Scalar>::Nested_t _G;
    typename E3::Nested_t _Ef;

public:
    enum{ Space = E1::Space };

    var2_expr( const E1 & u, const E2 & v, const gsGeometryMap<Scalar> & G, _expr<E3> const& Ef) : _u(u),_v(v), _G(G), _Ef(Ef) { }

    mutable gsMatrix<Scalar> res;

    mutable gsMatrix<Scalar> uGrads, vGrads, cJac, cDer2, evEf, result;
    mutable gsVector<Scalar> m_u, m_v, normal, m_uv, m_u_der, n_der, n_der2, tmp; // memomry leaks when gsVector<T,3>, i.e. fixed dimension
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    // helper function
    static inline gsVector<Scalar,3> vecFun(index_t pos, Scalar val)
    {
        gsVector<Scalar,3> result = gsVector<Scalar,3>::Zero();
        result[pos] = val;
        return result;
    }

    const gsMatrix<Scalar> & eval(const index_t k) const
    {
        res.resize(_u.cardinality(), _u.cardinality());

        normal = _G.data().normal(k);
        normal.normalize();
        uGrads = _u.data().values[1].col(k);
        vGrads = _v.data().values[1].col(k);
        cJac = _G.data().values[1].reshapeCol(k, _G.data().dim.first, _G.data().dim.second).transpose();
        cDer2 = _G.data().values[2].reshapeCol(k, _G.data().dim.second, _G.data().dim.second);

        const index_t cardU = _u.data().values[0].rows(); // number of actives per component of u
        const index_t cardV = _v.data().values[0].rows(); // number of actives per component of v
        const Scalar measure =  _G.data().measures.at(k);

        evEf = _Ef.eval(k);

        for (index_t j = 0; j!= cardU; ++j) // for all basis functions u (1)
        {
            for (index_t i = 0; i!= cardV; ++i) // for all basis functions v (1)
            {
                for (index_t d = 0; d!= _u.dim(); ++d) // for all basis functions u (2)
                {
                    m_u.noalias() = ( vecFun(d, uGrads.at(2*j  ) ).cross( cJac.col(1).template head<3>() )
                                     -vecFun(d, uGrads.at(2*j+1) ).cross( cJac.col(0).template head<3>() ))
                                    / measure;

                    const short_t s = d*cardU;

                    for (index_t c = 0; c!= _v.dim(); ++c) // for all basis functions v (2)
                    {
                        const short_t r = c*cardV;
                        m_v.noalias() = ( vecFun(c, vGrads.at(2*i  ) ).cross( cJac.col(1).template head<3>() )
                                         -vecFun(c, vGrads.at(2*i+1) ).cross( cJac.col(0).template head<3>() ))
                                        / measure;

                        // n_der.noalias() = (m_v - ( normal.dot(m_v) ) * normal);
                        n_der.noalias() = (m_v - ( normal*m_v.transpose() ) * normal); // outer-product version

                        m_uv.noalias() = ( vecFun(d, uGrads.at(2*j  ) ).cross( vecFun(c, vGrads.at(2*i+1) ) )
                                          +vecFun(c, vGrads.at(2*i  ) ).cross( vecFun(d, uGrads.at(2*j+1) ) ))
                                          / measure; //check

                        m_u_der.noalias() = (m_uv - ( normal.dot(m_v) ) * m_u);
                        // m_u_der.noalias() = (m_uv - ( normal*m_v.transpose() ) * m_u); // outer-product version TODO

                        // ---------------  Second variation of the normal
                        tmp = m_u_der - (m_u.dot(n_der) + normal.dot(m_u_der) ) * normal - (normal.dot(m_u) ) * n_der;
                        // tmp = m_u_der - (m_u.dot(n_der) + normal.dot(m_u_der) ) * normal - (normal.dot(m_u) ) * n_der;

                        // Evaluate the product
                        tmp = cDer2 * tmp; // E_f_der2, last component
                        tmp.row(2) *= 2.0;
                        result = evEf * tmp;

                        res(s + j, r + i ) = result(0,0);
                    }
                }
            }
        }
        return res;
    }

    index_t rows() const
    {
        return 1; // because the resulting matrix has scalar entries for every combination of active basis functions
    }

    index_t cols() const
    {
        return 1; // because the resulting matrix has scalar entries for every combination of active basis functions
    }

    void setFlag() const
    {
        _u.data().flags |= NEED_GRAD;
        _G.data().flags |= NEED_NORMAL | NEED_DERIV | NEED_2ND_DER | NEED_MEASURE;
        _Ef.setFlag();
    }

    void parse(gsSortedVector<const gsFunctionSet<Scalar>*> & evList) const
    {
        evList.push_sorted_unique(&_u.source());
        _u.data().flags |= NEED_GRAD;
        _G.data().flags |=  NEED_NORMAL | NEED_DERIV | NEED_2ND_DER | NEED_MEASURE;
        _Ef.setFlag();
    }

    const gsFeSpace<Scalar> & rowVar() const { return _u.rowVar(); }
    const gsFeSpace<Scalar> & colVar() const { return _v.rowVar(); }

    static constexpr bool rowSpan() {return true; }
    static constexpr bool colSpan() {return true; }

    void print(std::ostream &os) const { os << "var2("; _u.print(os); os <<")"; }
};


template<class E1, class E2>
class deriv2dot_expr : public _expr<deriv2dot_expr<E1, E2> >
{
public:
    typedef typename E1::Scalar Scalar;

private:

    typename E1::Nested_t _u;
    typename E2::Nested_t _v;

public:
    enum{ Space = E1::Space };

    deriv2dot_expr(const E1 & u, const E2 & v) : _u(u), _v(v) { }

    mutable gsMatrix<Scalar> res,tmp, vEv;

    const gsMatrix<Scalar> & eval(const index_t k) const {return eval_impl(_u,k); }

    index_t rows() const
    {
        return 1; //since the product with another vector is computed
    }

    index_t cols() const
    {
        return _u.source().domainDim() * ( _u.source().domainDim() + 1 ) / 2;
    }

    void setFlag() const
    {
        _u.data().flags |= NEED_DERIV2;
        _v.setFlag();
    }

    void parse(gsSortedVector<const gsFunctionSet<Scalar>*> & evList) const
    {
        evList.push_sorted_unique(&_u.source());
        _u.data().flags |= NEED_DERIV2;
    }

    const gsFeSpace<Scalar> & rowVar() const { return _u.rowVar(); }
    const gsFeSpace<Scalar> & colVar() const { return _v.rowVar(); }

    static constexpr bool rowSpan() {return E1::rowSpan(); }
    static constexpr bool colSpan() {return E2::rowSpan(); }

    void print(std::ostream &os) const { os << "deriv2("; _u.print(os); _v.print(os); os <<")"; }

private:
    template<class U> inline
    typename util::enable_if< util::is_same<U,gsGeometryMap<Scalar> >::value, const gsMatrix<Scalar> & >::type
    eval_impl(const U & u, const index_t k)  const
    {
        /*
            Here, we multiply the hessian of the geometry map by a vector, which possibly has multiple actives.
            The hessian of the geometry map c has the form: hess(c)
            [d11 c1, d11 c2, d11 c3]
            And we want to compute [d11 c .v; d22 c .v;  d12 c .v] ( . denotes a dot product and c and v are both vectors)
            So we simply evaluate for every active basis function v_k the product hess(c).v_k
        */


        // evaluate the geometry map of U
        tmp =_u.data().values[2].reshapeCol(k, cols(), _u.data().dim.second );
        vEv = _v.eval(k);
        res = vEv * tmp.transpose();
        return res;
    }

    template<class U> inline
    typename util::enable_if<util::is_same<U,gsFeSpace<Scalar> >::value, const gsMatrix<Scalar> & >::type
    eval_impl(const U & u, const index_t k) const
    {
        /*
            We assume that the basis has the form v*e_i where e_i is the unit vector with 1 on index i and 0 elsewhere
            This implies that hess(v) = [hess(v_1), hess(v_2), hess(v_3)] only has nonzero entries in column i. Hence,
            hess(v) . normal = hess(v_i) * n_i (vector-scalar multiplication. The result is then of the form
            [hess(v_1)*n_1 .., hess(v_1)*n_1 .., hess(v_1)*n_1 ..]. Here, the dots .. represent the active basis functions.
        */
        const index_t numAct = u.data().values[0].rows();   // number of actives of a basis function
        const index_t cardinality = u.cardinality();        // total number of actives (=3*numAct)
        res.resize(rows()*cardinality, cols() );
        tmp.transpose() =_u.data().values[2].reshapeCol(k, cols(), numAct );
        vEv = _v.eval(k);

        for (index_t i = 0; i!=_u.dim(); i++)
            res.block(i*numAct, 0, numAct, cols() ) = tmp * vEv.at(i);

        return res;
    }

};


/*
    DESCRIPTION
**/
template<class E>
class deriv2G_expr : public _expr<deriv2G_expr<E> >
{
        typename gsGeometryMap<E>::Nested_t _G;
public:
    typedef E Scalar;

public:

    deriv2G_expr(const gsGeometryMap<E> & G) : _G(G) { }

    mutable gsMatrix<Scalar> res, tmp;

    const gsMatrix<Scalar> & eval(const index_t k) const
    {
        /*
            Here, we compute the hessian of the geometry map.
            The hessian of the geometry map c has the form: hess(c)
            [d11 c1, d11 c2, d11 c3]
            [d22 c1, d22 c2, d22 c3]
            [d12 c1, d12 c2, d12 c3]

            The geometry map has components c=[c1,c2,c3]
        */
        // evaluate the geometry map of U
        res = _G.data().values[2].reshapeCol(k, rows(), cols());
        return res;
    }

    index_t rows() const //(components)
    {
        return _G.source().targetDim(); // number of derivatives
    }

    index_t cols() const
    {
        return 1; // number of components
    }

    void setFlag() const
    {
        _G.data().flags |= NEED_DERIV2|NEED_ACTIVE;
    }

    void parse(gsSortedVector<const gsFunctionSet<Scalar>*> & evList) const
    {
        evList.push_sorted_unique(&_G.source());
        _G.data().flags |= NEED_DERIV2;
    }

    const gsFeSpace<Scalar> & rowVar() const { return gsNullExpr<Scalar>::get(); }
    const gsFeSpace<Scalar> & colVar() const { return gsNullExpr<Scalar>::get(); }

    static constexpr bool rowSpan() {return false;}
    static constexpr bool colSpan() {return false;}

    void print(std::ostream &os) const { os << "deriv2("; _G.print(os); os <<")"; }
};

/*
    DESCRIPTION
**/
template<class E>
class deriv2_expr : public _expr<deriv2_expr<E> >
{
public:
    typedef typename E::Scalar Scalar;

private:

    typename E::Nested_t _u;

public:
    enum {ColBlocks = E::rowSpan() };
    enum{ Space = E::Space };

    deriv2_expr(const E & u) : _u(u) { }

    mutable gsMatrix<Scalar> res, tmp;

    const gsMatrix<Scalar> & eval(const index_t k) const
    {
        /*
            HOW IS IT STRUCTURED?
        */
        const index_t numAct = _u.data().values[0].rows();

        res.resize(rows(),_u.cardinality());
        res.setZero();

        tmp = _u.data().values[2].reshapeCol(k, 1, numAct ); // put everything in 1 row, since we only have 1 derivative per active
        for (index_t d = 0; d != _u.dim(); ++d)
        {
            const index_t s = d*numAct;
            for (index_t j = 0; j != numAct; ++j)
                    res(d,s+j) = tmp(0,j);
        }

        // res = _u.data().values[2].col(k).transpose().blockDiag(_u.targetDim()); // analoguous to jacobian..
        // res = res.transpose();
        // gsDebugVar(res);
        return res;

    }

    index_t rows() const //(components)
    {
        return _u.dim(); // number of components
    }

    index_t cols() const
    {
        return 1; // number of derivatives
    }

    void setFlag() const
    {
        _u.data().flags |= NEED_DERIV2|NEED_ACTIVE;
    }

    void parse(gsSortedVector<const gsFunctionSet<Scalar>*> & evList) const
    {
        evList.push_sorted_unique(&_u.source());
        _u.data().flags |= NEED_DERIV2;
    }

    const gsFeSpace<Scalar> & rowVar() const { return _u.rowVar(); }
    const gsFeSpace<Scalar> & colVar() const { return _u.colVar(); }

    index_t cardinality_impl() const { return _u.cardinality_impl(); }

    static constexpr bool rowSpan() {return E::rowSpan();}
    static constexpr bool colSpan() {return E::colSpan();}

    void print(std::ostream &os) const { os << "deriv2("; _u.print(os); os <<")"; }
};


// template<class E1, class E2>
// class hessdot_expr : public _expr<hessdot_expr<E1,E2> >
// {
//     typename E1::Nested_t _u;
//     typename E2::Nested_t _v;

// public:
//     enum{ Space = E1::Space };

//     typedef typename E1::Scalar Scalar;

//     hessdot_expr(const E1 & u, const E2 & v) : _u(u), _v(v) {}

//     mutable gsMatrix<Scalar> res, hess, tmp;
//     mutable gsMatrix<Scalar> normalMat;

//     MatExprType eval(const index_t k) const
//     {
//         const gsFuncData<Scalar> & udata = _u.data(); // udata.values[2].col(k)
//         const index_t numAct = udata.values[0].rows();
//         const gsAsConstMatrix<Scalar> ders(udata.values[2].col(k).data(), 3, numAct );

//         tmp = _v.eval(k);

//         res.resize(rows(), cols() );


//             for (index_t i = 0; i!=tmp.rows(); ++i)
//             {
//                 res.block(i*numAct, 0, numAct, 3).noalias() = ders.transpose() * tmp.at(i);
//             }

//         return res;
//     }

//     index_t rows() const
//     {
//         return _u.dim() * _u.data().values[0].rows();
//     }

//     index_t cols() const
//     {
//         return // ( E2::rowSpan() ? rows() : 1 ) *
//             _u.data().values[2].rows() / _u.data().values[0].rows();//=3
//     }

//     void setFlag() const
//     {
//         _u.data().flags |= NEED_2ND_DER;
//         _v.setFlag();
//     }

//     void parse(gsSortedVector<const gsFunctionSet<Scalar>*> & evList) const
//     {
//         //GISMO_ASSERT(NULL!=m_fd, "FeVariable: FuncData member not registered");
//         evList.push_sorted_unique(&_u.source());
//         _u.data().flags |= NEED_2ND_DER;
//     }

//     const gsFeSpace<Scalar> & rowVar() const { return _u.rowVar(); }
//     const gsFeSpace<Scalar> & colVar() const { return _v.rowVar(); }

//     static constexpr bool rowSpan() {return E1::rowSpan(); }
//     static constexpr bool colSpan() {return E2::rowSpan(); }

//     void print(std::ostream &os) const { os << "hessdot("; _u.print(os); os <<")"; }
// };


/**
   TO ADD
 */
template<class E1, class E2, class E3>
class flatdot_expr  : public _expr<flatdot_expr<E1,E2,E3> >
{
public:
    typedef typename E1::Scalar Scalar;
    enum {ScalarValued = 0, Space = E1::Space};
private:
    typename E1::Nested_t _A;
    typename E2::Nested_t _B;
    typename E3::Nested_t _C;
    mutable gsMatrix<Scalar> eA, eB, eC, tmp, res;

public:

    flatdot_expr(_expr<E1> const& A, _expr<E2> const& B, _expr<E3> const& C) : _A(A),_B(B),_C(C)
    {
        //GISMO_ASSERT( _u.rows()*_u.cols() == _n*_m, "Wrong dimension"); //
    }

    const gsMatrix<Scalar> & eval(const index_t k) const
    {
        const index_t Ac = _A.cols();
        const index_t An = _A.cardinality();
        const index_t Bc = _B.cols();
        const index_t Bn = _B.cardinality();

        eA = _A.eval(k);
        eB = _B.eval(k);
        eC = _C.eval(k);

        // gsDebugVar(eA);
        // gsDebugVar(eB);
        // gsDebugVar(eC);

        // gsDebugVar(eA.rows());
        // gsDebugVar(eA.cols());
        // gsDebugVar(_A.rows());
        // gsDebugVar(_A.cols());
        // gsDebugVar(eB.rows());
        // gsDebugVar(eB.cols());
        // gsDebugVar(_B.rows());
        // gsDebugVar(_B.cols());

        GISMO_ASSERT(Bc==_A.rows(), "Dimensions: "<<Bc<<","<< _A.rows()<< "do not match");
        GISMO_ASSERT(_A.rowSpan(), "First entry should be rowSpan");
        GISMO_ASSERT(_B.colSpan(), "Second entry should be colSpan.");

        res.resize(An, Bn);
        for (index_t i = 0; i!=An; ++i)
            for (index_t j = 0; j!=Bn; ++j)
            {
                tmp.noalias() = eB.middleCols(i*Bc,Bc) * eA.middleCols(j*Ac,Ac);
                tmp(0,0) *= eC.at(0);
                tmp(0,1) *= eC.at(2);
                tmp(1,0) *= eC.at(2);
                tmp(1,1) *= eC.at(1);
                res(i,j) = tmp.sum();
            }
        return res;
    }

    index_t rows() const { return 1; }
    index_t cols() const { return 1; }
    void setFlag() const { _A.setFlag();_B.setFlag();_C.setFlag(); }

    void parse(gsSortedVector<const gsFunctionSet<Scalar>*> & evList) const
    { _A.parse(evList);_B.parse(evList);_C.parse(evList); }

    const gsFeSpace<Scalar> & rowVar() const { return _A.rowVar(); }
    const gsFeSpace<Scalar> & colVar() const { return _B.colVar(); }
    index_t cardinality_impl() const { return _A.cardinality_impl(); }

    static constexpr bool rowSpan() {return E1::rowSpan();}
    static constexpr bool colSpan() {return E2::colSpan();}

    void print(std::ostream &os) const { os << "flatdot("; _A.print(os);_B.print(os);_C.print(os); os<<")"; }
};

/**
   To Do:
   *    Improve by inputting u instead of deriv2(u)
 */
template<class E1, class E2, class E3>
class flatdot2_expr  : public _expr<flatdot2_expr<E1,E2,E3> >
{
public:
    typedef typename E1::Scalar Scalar;
    enum {ScalarValued = 0, Space = E1::Space};
    enum {ColBlocks = 0 };

private:
    typename E1::Nested_t _A;
    typename E2::Nested_t _B;
    typename E3::Nested_t _C;
    mutable gsMatrix<Scalar> eA, eB, eC, res, tmp;

public:

    flatdot2_expr(_expr<E1> const& A, _expr<E2> const& B, _expr<E3> const& C) : _A(A),_B(B),_C(C)
    {
        //GISMO_ASSERT( _u.rows()*_u.cols() == _n*_m, "Wrong dimension"); //
    }

    const gsMatrix<Scalar> & eval(const index_t k) const
    {
        const index_t Ac = _A.cols();
        const index_t An = _A.cardinality();
        const index_t Bn = _B.cardinality();

        eA = _A.eval(k);
        eB = _B.eval(k);
        eC = _C.eval(k);

        // gsDebugVar(eA);
        // gsDebugVar(eB);
        // gsDebugVar(eC);

        // gsDebugVar(eA.rows());
        // gsDebugVar(eA.cols());
        // gsDebugVar(_A.rows());
        // gsDebugVar(_A.cols());
        // gsDebugVar(eB.rows());
        // gsDebugVar(eB.cols());
        // gsDebugVar(_B.rows());
        // gsDebugVar(_B.cols());

        GISMO_ASSERT(_B.rows()==_A.cols(), "Dimensions: "<<_B.rows()<<","<< _A.cols()<< "do not match");
        GISMO_ASSERT(_A.rowSpan(), "First entry should be rowSpan");
        GISMO_ASSERT(_B.colSpan(), "Second entry should be colSpan.");
        GISMO_ASSERT(_C.cols()==_B.rows(), "Dimensions: "<<_C.rows()<<","<< _B.rows()<< "do not match");

        // NOTE: product moved to the loop since that is more consistent with the formulations
        // for (index_t i = 0; i!=An; ++i)
        //     for (index_t j = 0; j!=Ac; ++j)
        //         eA.middleCols(i*Ac,Ac).row(j) *= eC(j);

        res.resize(An, Bn);
        for (index_t i = 0; i!=An; ++i)
            for (index_t j = 0; j!=Bn; ++j)
            {
                tmp = eA.middleCols(i*Ac,Ac) * eB.col(j);   // E_f_der2
                tmp.row(2) *= 2.0;                          // multiply the third row of E_f_der2 by 2 for voight notation
                res(i,j) = eC.row(0) * tmp.col(0);          // E_f^T * mm * E_f_der2
            }
        return res;
    }

    index_t rows() const { return 1; }
    index_t cols() const { return 1; }
    void setFlag() const { _A.setFlag();_B.setFlag();_C.setFlag(); }

    void parse(gsSortedVector<const gsFunctionSet<Scalar>*> & evList) const
    { _A.parse(evList);_B.parse(evList);_C.parse(evList); }

    const gsFeSpace<Scalar> & rowVar() const { return _A.rowVar(); }
    const gsFeSpace<Scalar> & colVar() const { return _B.colVar(); }
    index_t cardinality_impl() const { return _A.cardinality_impl(); }

    static constexpr bool rowSpan() {return E1::rowSpan();}
    static constexpr bool colSpan() {return E2::colSpan();}

    void print(std::ostream &os) const { os << "flatdot2("; _A.print(os);_B.print(os);_C.print(os); os<<")"; }
};

/*
   Expression for the transformation matrix between local cartesian and covariant bases, based on a geometry map
 */
template<class T> class cartcovinv_expr ;

template<class T>
class cartcov_expr : public _expr<cartcov_expr<T> >
{
    typename gsGeometryMap<T>::Nested_t _G;

public:
    typedef T Scalar;

    cartcov_expr(const gsGeometryMap<T> & G) : _G(G) { }

    mutable gsMatrix<Scalar,3,3> covBasis, conBasis, covMetric, conMetric, cartBasis, result;
    mutable gsVector<Scalar,3> normal, tmp;
    mutable gsVector<Scalar,3> e1, e2, a1, a2;

    MatExprType eval(const index_t k) const
    {
        // Compute covariant bases in deformed and undeformed configuration
        normal = _G.data().normals.col(k);
        normal.normalize();
        covBasis.leftCols(2) = _G.data().jacobian(k);
        covBasis.col(2)      = normal;
        covMetric = covBasis.transpose() * covBasis;

        conMetric = covMetric.inverse();

        // conBasis.col(0) = conMetric(0,0)*covBasis.col(0)+conMetric(0,1)*covBasis.col(1)+conMetric(0,2)*covBasis.col(2);
        conBasis.col(1) = conMetric(1,0)*covBasis.col(0)+conMetric(1,1)*covBasis.col(1)+conMetric(1,2)*covBasis.col(2);
        // conBasis.col(2) = conMetric(2,0)*covBasis.col(0)+conMetric(2,1)*covBasis.col(1)+conMetric(2,2)*covBasis.col(2);

        e1 = covBasis.col(0); e1.normalize();
        e2 = conBasis.col(1); e2.normalize();
        // e3 = normal;

        a1 = covBasis.col(0);
        a2 = covBasis.col(1);

        result(0,0) = (e1.dot(a1))*(a1.dot(e1));
        result(0,1) = (e1.dot(a2))*(a2.dot(e2));
        result(0,2) = 2*(e1.dot(a1))*(a2.dot(e1));
        // Row 1
        result(1,0) = (e2.dot(a1))*(a1.dot(e2));
        result(1,1) = (e2.dot(a2))*(a2.dot(e2));
        result(1,2) = 2*(e2.dot(a1))*(a2.dot(e2));
        // Row 2
        result(2,0) = (e1.dot(a1))*(a1.dot(e2));
        result(2,1) = (e1.dot(a2))*(a2.dot(e2));
        result(2,2) = (e1.dot(a1))*(a2.dot(e2)) + (e1.dot(a2))*(a1.dot(e2));

        // return result.inverse(); // !!!!
        return result;
    }

    cartcovinv_expr<T> inv() const
    {
        GISMO_ASSERT(rows() == cols(), "The Jacobian matrix is not square");
        return cartcovinv_expr<T>(_G);
    }

    index_t rows() const { return 3; }

    index_t cols() const { return 3; }

    static constexpr bool rowSpan() {return false; }
    static bool colSpan() {return false;}

    static const gsFeSpace<Scalar> & rowVar() { return gsNullExpr<Scalar>::get(); }
    static const gsFeSpace<Scalar> & colVar() { return gsNullExpr<Scalar>::get(); }

    void setFlag() const { _G.data().flags |= NEED_NORMAL|NEED_DERIV; }

    void parse(gsSortedVector<const gsFunctionSet<Scalar>*> & evList) const
    {
        //GISMO_ASSERT(NULL!=m_fd, "FeVariable: FuncData member not registered");
        evList.push_sorted_unique(&_G.source());
        _G.data().flags |= NEED_NORMAL|NEED_DERIV;
    }

    void print(std::ostream &os) const { os << "cartcov("; _G.print(os); os <<")"; }
};

template<class T>
class cartcovinv_expr : public _expr<cartcovinv_expr<T> >
{
    typename gsGeometryMap<T>::Nested_t _G;

public:
    typedef T Scalar;

    cartcovinv_expr(const gsGeometryMap<T> & G) : _G(G) { }

    mutable gsMatrix<T> temp;

    MatExprType eval(const index_t k) const
    {
        return  (cartcov_expr(_G).eval(k)).reshape(3,3).inverse();
    }

    index_t rows() const { return 3; }

    index_t cols() const { return 3; }

    static constexpr bool rowSpan() {return false; }
    static bool colSpan() {return false;}

    static const gsFeSpace<Scalar> & rowVar() { return gsNullExpr<Scalar>::get(); }
    static const gsFeSpace<Scalar> & colVar() { return gsNullExpr<Scalar>::get(); }

    void setFlag() const { _G.data().flags |= NEED_NORMAL|NEED_DERIV; }

    void parse(gsSortedVector<const gsFunctionSet<Scalar>*> & evList) const
    {
        //GISMO_ASSERT(NULL!=m_fd, "FeVariable: FuncData member not registered");
        evList.push_sorted_unique(&_G.source());
        _G.data().flags |= NEED_NORMAL|NEED_DERIV;
    }

    void print(std::ostream &os) const { os << "cartcovinv("; _G.print(os); os <<")"; }
};


/*
   Expression for the transformation matrix between local cartesian and contravariant bases, based on a geometry map
 */
template<class T> class cartconinv_expr ;

template<class T>
class cartcon_expr : public _expr<cartcon_expr<T> >
{
    typename gsGeometryMap<T>::Nested_t _G;

public:
    typedef T Scalar;

    cartcon_expr(const gsGeometryMap<T> & G) : _G(G) { }

    mutable gsMatrix<Scalar,3,3> covBasis, conBasis, covMetric, conMetric, cartBasis, result;
    mutable gsVector<Scalar,3> normal, tmp;
    mutable gsVector<Scalar,3> e1, e2, ac1, ac2;

    MatExprType eval(const index_t k) const
    {
        // Compute covariant bases in deformed and undeformed configuration
        normal = _G.data().normals.col(k);
        normal.normalize();
        covBasis.leftCols(2) = _G.data().jacobian(k);
        covBasis.col(2)      = normal;
        covMetric = covBasis.transpose() * covBasis;

        conMetric = covMetric.inverse();

        conBasis.col(0) = conMetric(0,0)*covBasis.col(0)+conMetric(0,1)*covBasis.col(1)+conMetric(0,2)*covBasis.col(2);
        conBasis.col(1) = conMetric(1,0)*covBasis.col(0)+conMetric(1,1)*covBasis.col(1)+conMetric(1,2)*covBasis.col(2);
        // conBasis.col(2) = conMetric(2,0)*covBasis.col(0)+conMetric(2,1)*covBasis.col(1)+conMetric(2,2)*covBasis.col(2);

        e1 = covBasis.col(0); e1.normalize();
        e2 = conBasis.col(1); e2.normalize();
        // e3 = normal;

        ac1 = conBasis.col(0);
        ac2 = conBasis.col(1);

        result(0,0) = (e1.dot(ac1))*(ac1.dot(e1));
        result(0,1) = (e1.dot(ac2))*(ac2.dot(e2));
        result(0,2) = 2*(e1.dot(ac1))*(ac2.dot(e1));
        // Row 1
        result(1,0) = (e2.dot(ac1))*(ac1.dot(e2));
        result(1,1) = (e2.dot(ac2))*(ac2.dot(e2));
        result(1,2) = 2*(e2.dot(ac1))*(ac2.dot(e2));
        // Row 2
        result(2,0) = (e1.dot(ac1))*(ac1.dot(e2));
        result(2,1) = (e1.dot(ac2))*(ac2.dot(e2));
        result(2,2) = (e1.dot(ac1))*(ac2.dot(e2)) + (e1.dot(ac2))*(ac1.dot(e2));

        return result;
    }

    cartconinv_expr<T> inv() const
    {
        GISMO_ASSERT(rows() == cols(), "The Jacobian matrix is not square");
        return cartconinv_expr<T>(_G);
    }

    index_t rows() const { return 3; }

    index_t cols() const { return 3; }

    static constexpr bool rowSpan() {return false; }
    static bool colSpan() {return false;}

    static const gsFeSpace<Scalar> & rowVar() { return gsNullExpr<Scalar>::get(); }
    static const gsFeSpace<Scalar> & colVar() { return gsNullExpr<Scalar>::get(); }

    void setFlag() const { _G.data().flags |= NEED_NORMAL|NEED_DERIV; }

    void parse(gsSortedVector<const gsFunctionSet<Scalar>*> & evList) const
    {
        //GISMO_ASSERT(NULL!=m_fd, "FeVariable: FuncData member not registered");
        evList.push_sorted_unique(&_G.source());
        _G.data().flags |= NEED_NORMAL|NEED_DERIV;
    }

    void print(std::ostream &os) const { os << "cartcon("; _G.print(os); os <<")"; }
};

template<class T>
class cartconinv_expr : public _expr<cartconinv_expr<T> >
{
    typename gsGeometryMap<T>::Nested_t _G;

public:
    typedef T Scalar;

    cartconinv_expr(const gsGeometryMap<T> & G) : _G(G) { }

    mutable gsMatrix<T> temp;

    MatExprType eval(const index_t k) const
    {
        return  (cartcon_expr(_G).eval(k)).reshape(3,3).inverse();
    }

    index_t rows() const { return 3; }

    index_t cols() const { return 3; }

    static constexpr bool rowSpan() {return false; }
    static bool colSpan() {return false;}

    static const gsFeSpace<Scalar> & rowVar() { return gsNullExpr<Scalar>::get(); }
    static const gsFeSpace<Scalar> & colVar() { return gsNullExpr<Scalar>::get(); }

    void setFlag() const { _G.data().flags |= NEED_NORMAL|NEED_DERIV; }

    void parse(gsSortedVector<const gsFunctionSet<Scalar>*> & evList) const
    {
        //GISMO_ASSERT(NULL!=m_fd, "FeVariable: FuncData member not registered");
        evList.push_sorted_unique(&_G.source());
        _G.data().flags |= NEED_NORMAL|NEED_DERIV;
    }

    void print(std::ostream &os) const { os << "cartconinv("; _G.print(os); os <<")"; }
};

template<class E> EIGEN_STRONG_INLINE
Gcon_expr<E> Gcon(const gsGeometryMap<E> & G) { return Gcon_expr<E>(G); }

template<class E> EIGEN_STRONG_INLINE
curveBinormal_expr<E> bn(const gsGeometryMap<E> & G) { return curveBinormal_expr<E>(G); }

template<class E> EIGEN_STRONG_INLINE
curveNormal_expr<E> cn(const gsGeometryMap<E> & G) { return curveNormal_expr<E>(G); }

template<class E> EIGEN_STRONG_INLINE
var1_expr<E> var1(const E & u, const gsGeometryMap<typename E::Scalar> & G) { return var1_expr<E>(u, G); }

template<class E1, class E2, class E3> EIGEN_STRONG_INLINE
var2_expr<E1,E2,E3> var2(const E1 & u, const E2 & v, const gsGeometryMap<typename E1::Scalar> & G, const E3 & Ef)
{ return var2_expr<E1,E2,E3>(u,v, G, Ef); }

// template<class E1, class E2> EIGEN_STRONG_INLINE
// hessdot_expr<E1,E2> hessdot(const E1 & u, const E2 & v) { return hessdot_expr<E1,E2>(u, v); }

template<class E> EIGEN_STRONG_INLINE
deriv2_expr<E> deriv2(const E & u) { return deriv2_expr<E>(u); }

template<class E> EIGEN_STRONG_INLINE
deriv2G_expr<E> deriv2(const gsGeometryMap<E> & G) { return deriv2G_expr<E>(G); }

template<class E1, class E2> EIGEN_STRONG_INLINE
deriv2dot_expr<E1, E2> deriv2(const E1 & u, const E2 & v) { return deriv2dot_expr<E1, E2>(u,v); }

template<class E1, class E2, class E3> EIGEN_STRONG_INLINE
flatdot_expr<E1,E2,E3> flatdot(const E1 & u, const E2 & v, const E3 & w)
{ return flatdot_expr<E1,E2,E3>(u, v, w); }

template<class E1, class E2, class E3> EIGEN_STRONG_INLINE
flatdot2_expr<E1,E2,E3> flatdot2(const E1 & u, const E2 & v, const E3 & w)
{ return flatdot2_expr<E1,E2,E3>(u, v, w); }

template<class E> EIGEN_STRONG_INLINE
cartcov_expr<E> cartcov(const gsGeometryMap<E> & G) { return cartcov_expr<E>(G); }

template<class E> EIGEN_STRONG_INLINE
cartcon_expr<E> cartcon(const gsGeometryMap<E> & G) { return cartcon_expr<E>(G); }

}
}


using namespace gismo;

template <class T>
void evaluateFunction(gsExprEvaluator<T> ev, auto expression, gsVector<T> pt);
template <class T>
void evaluateFunction(gsExprEvaluator<T> ev, auto expression, gsMatrix<T> pt);
template <class T>
void constructSolution(gsExprAssembler<T> assembler, gsMultiPatch<T> mp_def);

template <typename T1, typename T2, typename T3 > using var2_t = gismo::expr::var2_expr<T1,T2,T3>;
template <typename T1, typename T2, typename T3 > using flatdot_t = gismo::expr::flatdot_expr<T1,T2,T3 >;
template <typename T1, typename T2, typename T3 > using flatdot2_t= gismo::expr::flatdot2_expr<T1,T2,T3 >;
template <typename T1, typename T2  > using mult_t = gismo::expr::mult_expr<T1,T2,false >;
template <typename T1, typename T2  > using divide_t= gismo::expr::divide_expr<T1,T2 >;
template <typename T1, typename T2  > using add_t  = gismo::expr::add_expr<T1,T2>;
template <typename T1, typename T2  > using sub_t  = gismo::expr::sub_expr<T1,T2>;
template <typename T1, typename T2  > using der2d_t= gismo::expr::deriv2dot_expr<T1,T2>;

template <typename T> using jacG_t      = gismo::expr::jacG_expr<T>;
template <typename T> using jac_t       = gismo::expr::jac_expr<T>;
template <typename T> using sn_t        = gismo::expr::normal_expr<T>;
template <typename T> using var1_t      = gismo::expr::var1_expr<T>;
template <typename T> using der2_t      = gismo::expr::deriv2_expr<T>;
template <typename T> using normalized_t= gismo::expr::normalized_expr<T>;
template <typename T> using symmetrize_t= gismo::expr::symmetrize_expr<T>;
template <typename T> using flat_t      = gismo::expr::flat_expr<T>;
template <typename T> using tr_t        = gismo::expr::tr_expr<T>;
template <typename T> using u_t         = gismo::expr::gsFeSpace<T>;
template <typename T> using G_t         = gismo::expr::gsGeometryMap<T>;
template <typename T> using var_t       = gismo::expr::gsFeVariable<T>;
template <typename T> using val_t       = gismo::expr::value_expr<T>;
template <typename T> using reshape_t   = gismo::expr::reshape_expr<T>;

template <typename T> using E_m_t  =
mult_t
< T,
    sub_t
    <
        flat_t
        <
            mult_t< tr_t< jacG_t<T> >, jacG_t<T> >
        >
        ,
        flat_t
        <
            mult_t< tr_t< jacG_t<T> >, jacG_t<T> >
        >
    >
>;

template <typename T> using S_m_t  =
mult_t
<
    E_m_t<T>
    ,
    reshape_t< var_t <T> >
>;

template <typename T> using N_t  =
mult_t< val_t<var_t<T>>, S_m_t<T> >;

template <typename T> using E_m_der_t  =
flat_t
<
    mult_t< tr_t< jacG_t<T> >, jac_t<u_t<T>> >
>;

template <typename T> using S_m_der_t  =
mult_t
<
    E_m_der_t<T>
    ,
    reshape_t< var_t <T> >
>;

template <typename T> using N_der_t  =
mult_t< val_t<var_t<T>>, S_m_der_t<T> >;

template <typename T> using E_m_der2_t  =
flatdot_t
<
    jac_t< u_t<T> >,
    tr_t< jac_t< u_t<T> > >,
    N_t<T>
>;

template <typename T> using E_f_t  =
mult_t
<
    sub_t
    <
        der2d_t<G_t<T>, tr_t< normalized_t< sn_t<T> > > >
        ,
        der2d_t<G_t<T>, tr_t< normalized_t< sn_t<T> > > >
    >
    ,
    reshape_t< var_t<T> >
>;

template <typename T> using S_f_t  =
mult_t
<
    E_f_t<T>
    ,
    reshape_t< var_t <T> >
>;

template <typename T> using M_t  =
mult_t
<
    divide_t
    <
        mult_t
        <
            mult_t
            <
                val_t<var_t<T>>
                ,
                val_t<var_t<T>>
            >
            ,
            val_t<var_t<T>>
        >
    ,
    T
    >
    ,
    S_f_t<T>
>;

template <typename T> using E_f_der_t  =
mult_t
<
    add_t
    <
        der2d_t<u_t<T>, tr_t< normalized_t< sn_t<T> > > >
        ,
        der2d_t<G_t<T>, var1_t< u_t<T> > >
    >
    ,
    reshape_t< var_t<T> >
>;

template <typename T> using S_f_der_t  =
mult_t
<
    E_f_der_t<T>
    ,
    reshape_t< var_t <T> >
>;

template <typename T> using M_der_t  =
mult_t
<
    divide_t
    <
        mult_t
        <
            mult_t
            <
                val_t<var_t<T>>
                ,
                val_t<var_t<T>>
            >
            ,
            val_t<var_t<T>>
        >
    ,
    T
    >
    ,
    S_f_der_t<T>
>;


template <typename T> using E_f_der2_t  =
add_t
<
    symmetrize_t
    <
        flatdot2_t
        <
            der2_t< u_t<T> >,
            tr_t< var1_t<u_t<T> > >,
            M_t<T>
        >
    >
    ,
    var2_t< u_t<T>, u_t<T>, M_t<T> >
>;

template <typename T> using force_t = var_t<T>;

int main(int argc, char *argv[])
{
    //! [Parse command line]
    bool plot = false;
    index_t numRefine  = 1;
    index_t numElevate = 1;
    index_t testCase = 1;
    bool nonlinear = false;
    std::string fn("pde/poisson2d_bvp.xml");

    real_t E_modulus = 1.0;
    real_t Radius = 0.0;

    gsCmdLine cmd("Tutorial on solving a Poisson problem.");
    cmd.addInt( "e", "degreeElevation",
                "Number of degree elevation steps to perform before solving (0: equalize degree in all directions)", numElevate );
    cmd.addInt( "r", "uniformRefine", "Number of Uniform h-refinement steps to perform before solving",  numRefine );
    cmd.addInt( "t", "testCase", "Test case to run: 1 = unit square; 2 = Scordelis Lo Roof",  testCase );
    cmd.addString( "f", "file", "Input XML file", fn );
    cmd.addSwitch("nl", "Solve nonlinear problem", nonlinear);
    cmd.addSwitch("plot", "Create a ParaView visualization file with the solution", plot);

    try { cmd.getValues(argc,argv); } catch (int rv) { return rv; }
    //! [Parse command line]

    //! [Read input file]
    gsMultiPatch<> mp;
    gsMultiPatch<> mp_def;
    if (testCase==1)
    {
        fn = "curves3d/quarter_circle.xml";
        gsReadFile<>(fn, mp);
        // mp.embed(3);
        E_modulus = 1.0;

    }
    //! [Read input file]

    // p-refine
    if (numElevate!=0)
        mp.degreeElevate(numElevate);

    // h-refine
    for (int r =0; r < numRefine; ++r)
        mp.uniformRefine();

    mp_def = mp;

    //! [Refinement]
    gsMultiBasis<> dbasis(mp);

    gsInfo << "Patches: "<< mp.nPatches() <<", degree: "<< dbasis.minCwiseDegree() <<"\n";
    gsInfo << dbasis.basis(0)<<"\n";

    gsBoundaryConditions<> bc;
    gsVector<> tmp(3);
    tmp << 0, 0, 0;

    gsVector<> neu(3);
    neu << 0, 0, 0;

    gsFunctionExpr<> displ("1",3);

    gsConstantFunction<> neuData(neu,3);
    if (testCase == 1)
    {
        for (index_t i=0; i!=3; ++i)
        {
            bc.addCondition(boundary::east, condition_type::dirichlet, 0, i ); // unknown 1 - y
            bc.addCondition(boundary::west, condition_type::dirichlet, 0, i ); // unknown 2 - z
        }
        tmp << 0,0,-1;
    }

    //! [Problem setup]
    gsExprAssembler<> A;

    //gsInfo<<"Active options:\n"<< A.options() <<"\n";
    typedef gsExprAssembler<>::geometryMap geometryMap;
    typedef gsExprAssembler<>::variable    variable;
    typedef gsExprAssembler<>::space       space;
    typedef gsExprAssembler<>::solution    solution;

    // Elements used for numerical integration
    A.setIntegrationElements(dbasis);
    gsExprEvaluator<> ev(A);

    // Set the geometry map
    geometryMap G = A.getMap(mp); // the last map counts
    geometryMap defG = A.getMap(mp_def);


    // Set the discretization space
    space u = A.getSpace(dbasis, 3);
    u.setInterfaceCont(0); // todo: 1 (smooth basis)
    u.addBc( bc.get("Dirichlet") ); // (!) must be called only once

    // Solution vector and solution variable
    gsMatrix<> random;
    solution u_sol = A.getSolution(u,random);

    // gsFunctionExpr<> materialMat("1","0","0","0","1","0","0","0","1",3);
    // variable mm = A.getCoeff(materialMat, G);
    gsFunctionExpr<> E(std::to_string(E_modulus),3);
    variable Emod = A.getCoeff(E, G); // evaluates in the physical domain

    gsFunctionExpr<> R(std::to_string(Radius),3);
    variable Radi = A.getCoeff(R, G); // evaluates in the physical domain

    // gsFunctionExpr<> force("0","0","1", 3);
    gsConstantFunction<> force(tmp,3);
    variable ff = A.getCoeff(force,G); // evaluates in the physical domain

    gsSparseSolver<>::CGDiagonal solver;

    //! [Problem setup]

    //! [Solver loop]

    // Set Dirichlet values
    A.options().setInt("DirichletValues", dirichlet::homogeneous);

    // // Initialize the system
    A.initSystem();

    gsInfo<< A.numDofs() <<"\n"<<std::flush;

    /*
        We provide the following functions:                         checked with previous assembler
            E_m         membrane strain tensor.                             V
            E_m_der     first variation of E_m.                             V
            E_m_der2    second variation of E_m MULTIPLIED BY S_m.          V
            E_f         flexural strain tensor.                             V
            E_f_der     second variation of E_f.                            V
            E_f_der2    second variation of E_f MULTIPLIED BY S_f.          X NOTE: var1 in E_f_der2 in previous assembler is computed with both G and defG

        Where:
            G       the undeformed geometry,
            defG    the deformed geometry,
            mm      the material matrix,
            m2      an auxillary matrix to multiply the last row of a tensor with 2
    **/

    // mult_t< real_t, flat_t< jacG_t<real_t> >> E_mtest = 0.5 * flat(jac(G));

    // // Membrane components
    auto A0 = 3.1415926536*pow(Radi.val(),2);
    auto I0 = 3.1415926536/4*pow(Radi.val(),4);


    auto E_m = 0.5 * ( jac(defG).tr()*jac(defG) - jac(G).tr()* jac(G) ) * pow(Gcon(G).norm(),2); //[checked]
    auto S_m = E_m * Emod.val() * pow(Gcon(G).norm(),2);
    auto N       = A0 * S_m;

    auto E_m_der = pow(Gcon(G).norm(),2) * (jac(defG).tr() * jac(u)); //[checked]
    auto S_m_der = Emod.val() * pow(Gcon(G).norm(),2) * E_m_der;
    auto N_der   = S_m_der;

    // auto E_m_der2 = flatdot( jac(u),jac(u).tr(), N ); //[checked]

    // Flexural components
    auto E_f = pow(Gcon(G).norm(),2)*( deriv2(defG).tr()*cn(defG) - deriv2(defG).tr()*cn(defG)); //[checked]
    auto S_f = Emod.val() * pow(Gcon(G).norm(),2) * E_f;
    auto M   = I0 * S_f;

    // TO FIX
    // auto E_f_der = pow(Gcon(G).norm(),2)*( cn(G).tr()*deriv2(u) + deriv2(G).tr()*var1(u,G) ); //[checked]
    auto E_f_der = pow(Gcon(G).norm(),2)*( cn(G).tr()*deriv2(u).tr() + deriv2(G).tr()*var1(u,G) ); //[checked]
    auto S_f_der = Emod.val() * pow(Gcon(G).norm(),2) * E_f_der;
    auto M_der   = I0 * S_f_der;

    // auto E_f_der2 = flatdot2( deriv2(u), var1(u,defG).tr(), M  ).symmetrize() + var2(u,u,defG, M );

    auto F        = ff;

    // auto That       = cartcon(G);
    // auto Ttilde     = cartcov(G).inv(); // IS INVERTED
    // // auto TtildeInv  = cartcov(G).inv(); // DOES NOT WORK!!
    // auto D = Ttilde*reshape(mmD,3,3)*That; // NOTE: That = Ttilde.inv()

    // auto C = reshape(mm,3,3);

    // auto S_m2 = (tt.val() * E_m * C ) * Ttilde;
    // auto S_f2 = (pow(tt.val(),3)/12.0 * E_f * C ) * Ttilde;

    gsVector<> pt(1); pt.setConstant(0.7);
    // evaluateFunction(ev, G, pt); // evaluates an expression on a point
    // evaluateFunction(ev, jac(u), pt); // evaluates an expression on a point
    // evaluateFunction(ev, jac(u).tr(), pt); // evaluates an expression on a point
    // evaluateFunction(ev, cn(G), pt); // evaluates an expression on a point
    // evaluateFunction(ev, bn(G), pt); // evaluates an expression on a point
    // evaluateFunction(ev, var1(u,G), pt); // evaluates an expression on a point
    // evaluateFunction(ev, var1(u,G).tr(), pt); // evaluates an expression on a point
    // evaluateFunction(ev, deriv2(G), pt); // evaluates an expression on a point
    // evaluateFunction(ev, deriv2(G).tr(), pt); // evaluates an expression on a point
    // evaluateFunction(ev, deriv2(u), pt); // evaluates an expression on a point

    // evaluateFunction(ev, deriv2(u).tr()*cn(G), pt);
    // evaluateFunction(ev, cn(G).tr()*deriv2(u), pt);





    // evaluateFunction(ev, E_m, pt); // evaluates an expression on a point
    // evaluateFunction(ev, E_m_der, pt); // evaluates an expression on a point
    // evaluateFunction(ev, E_m_der.tr(), pt); // evaluates an expression on a point
    // evaluateFunction(ev, N_der, pt); // evaluates an expression on a point
    // evaluateFunction(ev, N_der.tr(), pt); // evaluates an expression on a point
    evaluateFunction(ev, N_der.tr() * E_m_der, pt); // evaluates an expression on a point
    evaluateFunction(ev, N_der * E_m_der.tr(), pt); // evaluates an expression on a point
    evaluateFunction(ev, E_m_der, pt); // evaluates an expression on a point
    evaluateFunction(ev, E_m_der.tr(), pt); // evaluates an expression on a point

    evaluateFunction(ev, (jac(defG).tr() * jac(u)) * (jac(defG).tr() * jac(u)).tr(), pt); // evaluates an expression on a point



    gsDebugVar(((jac(defG).tr() * jac(u)) * (jac(defG).tr() * jac(u)).tr()).isMatrix());
    gsDebugVar(((jac(defG).tr() * jac(u)) * (jac(defG).tr() * jac(u)).tr()).colSpan());
    gsDebugVar(((jac(defG).tr() * jac(u)) * (jac(defG).tr() * jac(u)).tr()).rowSpan());
    gsDebugVar((jac(defG).tr() * jac(u)).colSpan());
    gsDebugVar((jac(defG).tr() * jac(u)).rowSpan());
    gsDebugVar(((jac(defG).tr() * jac(u)).tr()).colSpan());
    gsDebugVar(((jac(defG).tr() * jac(u)).tr()).rowSpan());

    // evaluateFunction(ev, E_f, pt); // evaluates an expression on a point
    // evaluateFunction(ev, E_f_der, pt); // evaluates an expression on a point
    // evaluateFunction(ev, E_f_der.tr(), pt); // evaluates an expression on a point
    // evaluateFunction(ev, M_der, pt); // evaluates an expression on a point
    // evaluateFunction(ev, M_der.tr(), pt); // evaluates an expression on a point
    // evaluateFunction(ev, M_der.tr() * E_f_der, pt); // evaluates an expression on a point

    // ! [Solve linear problem]

    // assemble system
    A.assemble(
        (
            E_m_der * E_m_der.tr()
            // +
            // M_der * E_f_der.tr()
            ) //* meas(G)
        // ,
        // u * F  * meas(G)
        );

    // // For Neumann (same for Dirichlet/Nitsche) conditions
    // variable g_N = A.getBdrFunction();
    // A.assembleRhsBc(u * g_N, bc.container("Neumann") );

    // // for weak dirichlet (DOES THIS HANDLE COMPONENTS?)
    // real_t alpha_d = 1e3;
    // A.assembleLhsRhsBc
    // (
    //     alpha_d * u * u.tr()
    //     ,
    //     alpha_d * u * (defG - G - g_N).tr()
    //     ,
    //     bc.container("dirichlet weak")
    // );

    // // for weak clamped
    // real_t alpha_r = 1e3;
    // A.assembleLhsRhsBc
    // (
    //     // alpha_r * ( sn(defG).tr()*sn(G) - 1.0 ) * ( flatdot2??? )
    //     // +
    //     alpha_r * ( var1(u,defG) * sn(G).tr() ).symmetrize()
    //     ,
    //     alpha_r * ( sn(defG)*sn(G).tr() - sn(G)*sn(G).tr() ) * ( var1(u,defG) * sn(G).tr() )
    //     ,
    //     bc.container("clamped weak")
    // );



    // solve system
    solver.compute( A.matrix() );
    gsMatrix<> solVector = solver.solve(A.rhs());

    gsInfo<<A.matrix().toDense()<<"\n";
    gsInfo<<A.rhs()<<"\n";
    gsInfo<<solVector<<"\n";


    // update deformed patch
    gsMatrix<> cc;

    u_sol.setSolutionVector(solVector);
    for ( size_t k =0; k!=mp_def.nPatches(); ++k) // Deform the geometry
    {
        // // extract deformed geometry
        u_sol.extract(cc, k);
        mp_def.patch(k).coefs() += cc;  // defG points to mp_def, therefore updated
    }
    /*Something with Dirichlet homogenization*/

    // auto sol1 = G + u_sol;
    // auto sol2 = defG;

    // auto nsol1= sn(G);
    // // defG.registerData(G.source(), G.data())
    // auto nsol2= sn(defG);

    // evaluateFunction(ev, sol1, pt); // evaluates an expression on a point
    // evaluateFunction(ev, sol2, pt); // evaluates an expression on a point
    // evaluateFunction(ev, nsol1, pt); // evaluates an expression on a point
    // evaluateFunction(ev, nsol2, pt); // evaluates an expression on a point

    // // ! [Solve linear problem]

    // // ! [Solve nonlinear problem]
    // real_t residual = A.rhs().norm();
    // if (nonlinear)
    // {
    //     index_t itMax = 10;
    //     real_t tol = 1e-8;
    //     for (index_t it = 0; it != itMax; ++it)
    //     {
    //         A.initSystem();
    //         // assemble system
    //         A.assemble(
    //             ( N_der * E_m_der.tr() + E_m_der2 + M_der * E_f_der.tr() - E_f_der2 ) * meas(G)
    //             , u * F * meas(G) - ( ( N * E_m_der.tr() - M * E_f_der.tr() ) * meas(G) ).tr()
    //             );

    //         // For Neumann (same for Dirichlet/Nitche) conditions
    //         variable g_N = A.getBdrFunction();
    //         A.assembleRhsBc(u * g_N, bc.neumannSides() );

    //         // A.assemble(tt.val() * tt.val() * tt.val() / 3.0 * E_f_der2);
    //         // solve system
    //         solver.compute( A.matrix() );
    //         gsMatrix<> updateVector = solver.solve(A.rhs()); // this is the UPDATE
    //         solVector += updateVector;
    //         residual = A.rhs().norm();

    //         gsInfo<<"Iteration: "<< it
    //                <<", residue: "<< residual
    //                <<", update norm: "<<updateVector.norm()
    //                <<"\n";

    //         // update deformed patch
    //         u_sol.setSolutionVector(updateVector);
    //         for ( size_t k =0; k!=mp_def.nPatches(); ++k) // Deform the geometry
    //         {
    //             // // extract deformed geometry
    //             u_sol.extract(cc, k);
    //             mp_def.patch(k).coefs() += cc;  // defG points to mp_def, therefore updated
    //         }


    //         if (residual < tol)
    //             break;
    //     }
    // }


    // // ! [Solve nonlinear problem]

    // // Penalize the matrix? (we need values for the DoFs to be enforced..
    // // function/call:  penalize_matrix(DoF_indices, DoF_values)
    // // otherwise: should we tag the DoFs inside "u" ?

    // // gsInfo<<"RHS rows = "<<A.rhs().rows()<<"\n";
    // // gsInfo<<"RHS cols = "<<A.rhs().cols()<<"\n";
    // // gsInfo<<"MAT rows = "<<A.matrix().rows()<<"\n";
    // // gsInfo<<"MAT cols = "<<A.matrix().cols()<<"\n";

    // // gsInfo<< A.rhs().transpose() <<"\n";
    // // gsInfo<< A.matrix().toDense()<<"\n";


    //! [Export visualization in ParaView]
    if (plot)
    {
        u_sol.setSolutionVector(solVector);
        mp_def = mp;
        for ( size_t k =0; k!=mp.nPatches(); ++k) // Deform the geometry
        {
            // // extract deformed geometry
            u_sol.extract(cc, k);
            mp_def.patch(k).coefs() += cc;  // defG points to mp_def, therefore updated
        }

        gsMultiPatch<> deformation = mp_def;
        for (index_t k = 0; k != mp_def.nPatches(); ++k)
            deformation.patch(k).coefs() -= mp.patch(k).coefs();

        gsField<> solField(mp, deformation);
        gsInfo<<"Plotting in Paraview...\n";
        gsWriteParaview<>( solField, "solution", 1000, true);

        // ev.options().setSwitch("plot.elements", true);
        // ev.writeParaview( S_f2, G, "stress");
        // evaluateFunction(ev, S_f2[0], pt); // evaluates an expression on a point

        // gsFileManager::open("solution.pvd");
    }

    return EXIT_SUCCESS;

}// end main

template <class T>
void evaluateFunction(gsExprEvaluator<T> ev, auto expression, gsVector<T> pt)
{
    gsMatrix<T> evresult = ev.eval( expression,pt );
    gsInfo << "Eval on point ("<<pt.at(0)<<") :\n"<< evresult;
    gsInfo << "\nEnd ("<< evresult.rows()<< " x "<<evresult.cols()<<")\n";
};

template <class T>
void evaluateFunction(gsExprEvaluator<T> ev, auto expression, gsMatrix<T> pt)
{
    gsVector<T> tmp;
    gsMatrix<T> evresult;
    for (index_t k=0; k!= pt.cols(); ++k)
    {
        tmp = pt.col(k);
        evresult = ev.eval( expression,tmp );
        gsInfo<<evresult<<"\n";
    }
};

/*
template<class T>
void gsShellAssembler<T>::applyLoads()
{
    gsMatrix<T>        bVals;
    gsMatrix<unsigned> acts,globalActs;

    for (size_t i = 0; i< m_pLoads.numLoads(); ++i )
    {
        if ( m_pLoads[i].parametric )
        {
            m_bases.front().basis(m_pLoads[i].patch).active_into( m_pLoads[i].point, acts );
            m_bases.front().basis(m_pLoads[i].patch).eval_into  ( m_pLoads[i].point, bVals);
        }
        else
        {
            gsMatrix<> forcePoint;
            m_patches.patch(m_pLoads[i].patch).invertPoints(m_pLoads[i].point,forcePoint);
            u.source().piece(m_pLoads[i].patch).active_into( forcePoint, acts );
            u.source().piece(m_pLoads[i].patch).active_into( forcePoint, bVals);
        }

        // translate patch-local indices to global dof indices
        for (size_t j = 0; j< 3; ++j)
        {
            if (m_pLoads[i].value[j] != 0.0)
            {
                u.dofMappers[j].localToGlobal(acts, m_pLoads[i].patch, globalActs);

                for (index_t k=0; k < globalActs.rows(); ++k)
                {
                    if (int(globalActs(k,0)) < m_dofs)
                        m_rhs(globalActs(k,0), 0) += bVals(k,0) * m_pLoads[i].value[j];
                }
            }
        }
    }
}
*/