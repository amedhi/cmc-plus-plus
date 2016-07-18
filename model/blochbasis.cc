/*---------------------------------------------------------------------------
* Copyright (C) 2015-2016 by Amal Medhi <amedhi@iisertvm.ac.in>.
* All rights reserved.
* Author: Amal Medhi
* Date:   2016-03-14 14:48:15
* Last Modified by:   Amal Medhi, amedhi@macbook
* Last Modified time: 2016-03-18 01:08:59
*----------------------------------------------------------------------------*/
#include "../constants.h"
#include "blochbasis.h"

namespace basis {


int BlochBasis::construct(const lattice::Lattice& lattice, const lattice::graph::LatticeGraph& graph)
{
  // reset
  kpoints.resize(lattice.num_unitcells());
  //translation_vectors.clear();
  basis_states.resize(lattice.num_basis_sites());
  state_indices.clear();
  //null_state = lattice.num_sites();
  make_bloch_vectors(lattice);
  make_site_basis(lattice, graph);

  return 0;
}

void BlochBasis::make_bloch_vectors(const lattice::Lattice& lattice)
{

  Vector3d a1, a2, a3;
  double v;
  using bc = lattice::boundary_type;

  // reciprocal lattice vectors 
  a1 = lattice.vector_a1();
  a2 = lattice.vector_a2();
  a3 = lattice.vector_a3();
  b1 = Vector3d(0.0,0.0,0.0);
  b2 = Vector3d(0.0,0.0,0.0);
  b3 = Vector3d(0.0,0.0,0.0);

  unsigned symmetry_type = 0;
  if (lattice.bc1() == bc::periodic) {
    b1 = two_pi() * a1 / a1.dot(a1); 
    symmetry_type = symmetry_type + 1;
  } 

  if (lattice.bc2() == bc::periodic) {
    switch (symmetry_type) {
      case 0:
        b2 = two_pi() * a2 / a2.dot(a2); 
        break;
      case 1:
        a3 = a1.cross(a2);
        v = a1.dot(a2.cross(a3));
        b1 = two_pi() * a2.cross(a3) / v;
        b2 = two_pi() * a3.cross(a1) / v;
        break;
      default: break;
    }
    symmetry_type = symmetry_type + 2;
  } 

  if (lattice.bc3() == bc::periodic) {
    switch (symmetry_type) {
      case 0:
        b3 = two_pi() * a3 / a3.dot(a3); 
        break;
      case 1:
        a2 = a3.cross(a1);
        v = a1.dot(a2.cross(a3));
        b1 = two_pi() * a2.cross(a3) / v;
        b3 = two_pi() * a1.cross(a2) / v;
        break;
      case 2:
        a1 = a2.cross(a3);
        v = a1.dot(a2.cross(a3));
        b2 = two_pi() * a3.cross(a1) / v;
        b3 = two_pi() * a1.cross(a2) / v;
        break;
      case 3:
        v = a1.dot(a2.cross(a3));
        b1 = two_pi() * a2.cross(a3) / v;
        b2 = two_pi() * a3.cross(a1) / v;
        b3 = two_pi() * a1.cross(a2) / v;
        break;
      default: break;
    }
  }

  // antiperiodic boundary condition
  Vector3d antipb_shift(0.0, 0.0, 0.0);
  if (lattice.bc1_periodicity()==bc::antiperiodic) antipb_shift(0) = 0.5/lattice.size1();
  if (lattice.bc2_periodicity()==bc::antiperiodic) antipb_shift(1) = 0.5/lattice.size2();
  if (lattice.bc3_periodicity()==bc::antiperiodic) antipb_shift(2) = 0.5/lattice.size3();

  // k-points & translation vectors
  double x1, x2, x3;
  Vector3i n = {0,0,0};
  Vector3i m = {-lattice.size1()/2, -lattice.size2()/2, -lattice.size3()/2};
  for (unsigned i=0; i<kpoints.size(); i++) {
    x1 = static_cast<double>(m(0)+n(0))/lattice.size1() + antipb_shift(0);
    x2 = static_cast<double>(m(1)+n(1))/lattice.size2() + antipb_shift(1);
    x3 = static_cast<double>(m(2)+n(2))/lattice.size3() + antipb_shift(2);
    kpoints[i] = x1 * b1 + x2 * b2 + x3 * b3;
    //std::cout << i << ": " << kpoints[i](0) << " " << kpoints[i](1) << " " << kpoints[i](2) << "\n";
    //translation_vectors.push_back(n);
    n = lattice.get_next_bravindex(n);
  }
}

void BlochBasis::make_site_basis(const lattice::Lattice& lattice, const lattice::graph::LatticeGraph& graph)
{
  /*
  * The basis states are ! combination of site states connected by lattice 
  * translational symmetry. Among these states in a 'bloch cycle', only the state
  * (site) with smallest 'id' (representative state=rs) is stored. 
  * The basis states are to be regarded as: 
  *   |s> = 1/sqrt(N) \sum_R e^{ik.R} |rs> 
  */

  // basis states & indexing
  lattice::graph::vertex_iterator vi, vi_end;
  basis_state s;
  unsigned i;
  for (i=0; i<basis_states.size(); ++i) {
    s = graph.vertex(i);
    //std::cout << graph.vertex_uid(i) << "\n";
    if (graph.vertex_id(s) != i) {
      throw std::logic_error("*error: blochbasis:: unexpected graph property");
    }
    basis_states[i] = s;
    state_indices.insert({s,i});
    //std::cout << state_index[s] << "\n";
  }
  null_index = basis_states.size();
}

basis_state BlochBasis::representative_state(const basis_state& s, const lattice::graph::LatticeGraph& graph,
    Vector3d& R) const
{
  unsigned rs = graph.vertex_uid(s);
  R = graph.vertex_cellcord(s);
  return graph.vertex(rs);
}


} // end namespace basis