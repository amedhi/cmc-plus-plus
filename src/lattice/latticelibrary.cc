/*---------------------------------------------------------------------------
* Copyright (C) 2015-2016 by Amal Medhi <amedhi@iisertvm.ac.in>.
* All rights reserved.
* Date:   2016-01-17 21:32:15
* Last Modified by:   Amal Medhi, amedhi@macbook
* Last Modified time: 2017-04-17 12:20:46
*----------------------------------------------------------------------------*/
#include <stdexcept>
#include <iomanip>
#include <boost/algorithm/string.hpp>
#include "lattice.h"
//#include "graph.h"

namespace lattice {

// define the lattice
int Lattice::define_lattice(void) 
{
  using pos = Eigen::Vector3i;
  using vec = Eigen::Vector3d;
  unsigned type, ngb, src, tgt;
  vec a1, a2, a3, coord;
  pos src_offset, tgt_offset;

  /*------------- 'SQUARE' lattice--------------*/
  if (lname == "SQUARE") {
    // type
    lid = lattice_id::SQUARE;
    extent[dim3] = Extent{1, boundary_type::open, boundary_type::open};

    // basis vectors
    unitcell.set_basis(a1=vec(1,0,0), a2=vec(0,1,0), a3=vec(0,0,0));

    // add sites
    unitcell.add_site(type=0, coord=vec(0,0,0));

    // add bonds
    unitcell.add_bond(type=0, ngb=1, src=0, src_offset=pos(0,0,0), tgt=0, tgt_offset=pos(1,0,0));
    unitcell.add_bond(type=0, ngb=1, src=0, src_offset=pos(0,0,0), tgt=0, tgt_offset=pos(0,1,0));
  }

  else if (lname == "SIMPLE_CUBIC") {
    // type
    lid = lattice_id::SIMPLECUBIC;

    // basis vectors
    unitcell.set_basis(a1=vec(1,0,0), a2=vec(0,1,0), a3=vec(0,0,1));

    // add sites
    unitcell.add_site(type=0, coord=vec(0,0,0));

    // add bonds
    unitcell.add_bond(type=0, ngb=1, src=0, src_offset=pos(0,0,0), tgt=0, tgt_offset=pos(1,0,0));
    unitcell.add_bond(type=0, ngb=1, src=0, src_offset=pos(0,0,0), tgt=0, tgt_offset=pos(0,1,0));
    unitcell.add_bond(type=0, ngb=1, src=0, src_offset=pos(0,0,0), tgt=0, tgt_offset=pos(0,0,1));
  }

  else if (lname == "SYS_NIMNX") {
    // type
    lid = lattice_id::SYS_NIMNX;

    // basis vectors
    unitcell.set_basis(a1=vec(1,0,0), a2=vec(0,1,0), a3=vec(0,0,1));

    // add sites
    // In-atom
    unsigned In, Mn, Ni1, Ni2;
    In  = unitcell.add_site(type=0, coord=vec(0,0,0));
    // Mn-atom
    Mn  = unitcell.add_site(type=1, coord=vec(0.50,0.50,0.50));
    // Ni1-atom
    Ni1 = unitcell.add_site(type=2, coord=vec(0.25,0.25,0.25));
    // Ni2-atom
    Ni2 = unitcell.add_site(type=2, coord=vec(0.75,0.75,0.75));

    // add bonds
    // intra-cell bonds
    unitcell.add_bond(type=0, ngb=1, src=Ni1, pos(0,0,0), tgt=In,  pos(0,0,0));
    unitcell.add_bond(type=1, ngb=1, src=Mn,  pos(0,0,0), tgt=Ni1, pos(0,0,0));
    unitcell.add_bond(type=1, ngb=1, src=Mn,  pos(0,0,0), tgt=Ni2, pos(0,0,0));
    // inter-cell bonds
    // Mn--others
    unitcell.add_bond(type=1, ngb=1, src=Mn,  pos(0,0,0), tgt=Ni1, pos(1,0,0));
    unitcell.add_bond(type=1, ngb=1, src=Mn,  pos(0,0,0), tgt=Ni1, pos(0,1,0));
    unitcell.add_bond(type=1, ngb=1, src=Mn,  pos(0,0,0), tgt=Ni1, pos(0,0,1));
    // Ni1--others
    unitcell.add_bond(type=0, ngb=1, src=Ni1, pos(0,0,0), tgt=In,  pos(1,0,0));
    unitcell.add_bond(type=0, ngb=1, src=Ni1, pos(0,0,0), tgt=In,  pos(0,1,0));
    unitcell.add_bond(type=0, ngb=1, src=Ni1, pos(0,0,0), tgt=In,  pos(0,0,1));
    // Ni2--others
    unitcell.add_bond(type=2, ngb=1, src=Ni2, pos(0,0,0), tgt=Mn,  pos(1,0,0));
    unitcell.add_bond(type=2, ngb=1, src=Ni2, pos(0,0,0), tgt=Mn,  pos(0,1,0));
    unitcell.add_bond(type=2, ngb=1, src=Ni2, pos(0,0,0), tgt=Mn,  pos(0,0,1));
    unitcell.add_bond(type=0, ngb=1, src=Ni2, pos(0,0,0), tgt=In,  pos(1,1,0));
    unitcell.add_bond(type=0, ngb=1, src=Ni2, pos(0,0,0), tgt=In,  pos(1,0,1));
    unitcell.add_bond(type=0, ngb=1, src=Ni2, pos(0,0,0), tgt=In,  pos(0,1,1));
    unitcell.add_bond(type=0, ngb=1, src=Ni2, pos(0,0,0), tgt=In,  pos(1,1,1));

    //2nd neighbour inter-cell bonds
    /*
    // In is 'non-magnetic' and elastic interaction is only for NN bonds
    unitcell.add_bond(type=3, ngb=2, src=Mn,  cell=pos(0,0,0), tgt=In,  cell=pos(0,0,1));
    unitcell.add_bond(type=3, ngb=2, src=Mn,  cell=pos(0,0,0), tgt=In,  cell=pos(0,1,0));
    unitcell.add_bond(type=3, ngb=2, src=Mn,  cell=pos(0,0,0), tgt=In,  cell=pos(0,1,1));
    unitcell.add_bond(type=3, ngb=2, src=Mn,  cell=pos(0,0,0), tgt=In,  cell=pos(1,0,0));
    unitcell.add_bond(type=3, ngb=2, src=Mn,  cell=pos(0,0,0), tgt=In,  cell=pos(1,0,1));
    unitcell.add_bond(type=3, ngb=2, src=Mn,  cell=pos(0,0,0), tgt=In,  cell=pos(1,1,0));
    */

    /*
    // No Ni1-Ni2 interaction (no elastic also because it is 2nd NN)
    unitcell.add_bond(type=3, ngb=2, src=Ni2, cell=pos(0,0,0), tgt=Ni1,  cell=pos(0,0,1));
    unitcell.add_bond(type=3, ngb=2, src=Ni2, cell=pos(0,0,0), tgt=Ni1,  cell=pos(0,1,0));
    unitcell.add_bond(type=3, ngb=2, src=Ni2, cell=pos(0,0,0), tgt=Ni1,  cell=pos(0,1,1));
    unitcell.add_bond(type=3, ngb=2, src=Ni2, cell=pos(0,0,0), tgt=Ni1,  cell=pos(1,0,0));
    unitcell.add_bond(type=3, ngb=2, src=Ni2, cell=pos(0,0,0), tgt=Ni1,  cell=pos(1,0,1));
    unitcell.add_bond(type=3, ngb=2, src=Ni2, cell=pos(0,0,0), tgt=Ni1,  cell=pos(1,1,0));
    */

    //3rd neighbour inter-cell bonds
    /*
    // In is 'non-magnetic' and elastic interaction is only for NN bonds
    unitcell.add_bond(type=5, ngb=3, src=In, pos(0,0,0), tgt=In,  pos(0,0,1));
    unitcell.add_bond(type=5, ngb=3, src=In, pos(0,0,0), tgt=In,  pos(0,1,0));
    unitcell.add_bond(type=5, ngb=3, src=In, pos(0,0,0), tgt=In,  pos(1,0,0));
    unitcell.add_bond(type=5, ngb=3, src=In, pos(0,0,0), tgt=In,  pos(1,-1,0));
    unitcell.add_bond(type=5, ngb=3, src=In, pos(0,0,0), tgt=In,  pos(0,1,-1));
    unitcell.add_bond(type=5, ngb=3, src=In, pos(0,0,0), tgt=In,  pos(-1,0,1));
    */
    
    unitcell.add_bond(type=3, ngb=3, src=Mn, pos(0,0,0), tgt=Mn,  pos(0,0,1));
    unitcell.add_bond(type=3, ngb=3, src=Mn, pos(0,0,0), tgt=Mn,  pos(0,1,0));
    unitcell.add_bond(type=3, ngb=3, src=Mn, pos(0,0,0), tgt=Mn,  pos(1,0,0));
    unitcell.add_bond(type=3, ngb=3, src=Mn, pos(0,0,0), tgt=Mn,  pos(1,-1,0));
    unitcell.add_bond(type=3, ngb=3, src=Mn, pos(0,0,0), tgt=Mn,  pos(0,1,-1));
    unitcell.add_bond(type=3, ngb=3, src=Mn, pos(0,0,0), tgt=Mn,  pos(-1,0,1));

    /*
    // No Ni1-Ni1 interaction (no elastic also because it is 2nd NN)
    unitcell.add_bond(type=6, ngb=3, src=Ni1, pos(0,0,0), tgt=Ni1,  pos(0,0,1));
    unitcell.add_bond(type=6, ngb=3, src=Ni1, pos(0,0,0), tgt=Ni1,  pos(0,1,0));
    unitcell.add_bond(type=6, ngb=3, src=Ni1, pos(0,0,0), tgt=Ni1,  pos(1,0,0));
    unitcell.add_bond(type=6, ngb=3, src=Ni1, pos(0,0,0), tgt=Ni1,  pos(1,-1,0));
    unitcell.add_bond(type=6, ngb=3, src=Ni1, pos(0,0,0), tgt=Ni1,  pos(0,1,-1));
    unitcell.add_bond(type=6, ngb=3, src=Ni1, pos(0,0,0), tgt=Ni1,  pos(-1,0,1));

    unitcell.add_bond(type=6, ngb=3, src=Ni2, pos(0,0,0), tgt=Ni2,  pos(0,0,1));
    unitcell.add_bond(type=6, ngb=3, src=Ni2, pos(0,0,0), tgt=Ni2,  pos(0,1,0));
    unitcell.add_bond(type=6, ngb=3, src=Ni2, pos(0,0,0), tgt=Ni2,  pos(1,0,0));  
    unitcell.add_bond(type=6, ngb=3, src=Ni2, pos(0,0,0), tgt=Ni2,  pos(1,-1,0));  
    unitcell.add_bond(type=6, ngb=3, src=Ni2, pos(0,0,0), tgt=Ni2,  pos(0,1,-1));  
    unitcell.add_bond(type=6, ngb=3, src=Ni2, pos(0,0,0), tgt=Ni2,  pos(-1,0,1));  
    */

    //6th neighbour Mn-Mn bonds (c/a=1.0)
    unitcell.add_bond(type=4, ngb=6, src=Mn, pos(0,0,0), tgt=Mn,  pos(1,1,-1));
    unitcell.add_bond(type=4, ngb=6, src=Mn, pos(0,0,0), tgt=Mn,  pos(1,-1,1));
    unitcell.add_bond(type=4, ngb=6, src=Mn, pos(0,0,0), tgt=Mn,  pos(-1,1,1));
  }

  /*------------- 'CHAIN' lattice--------------*/
  else if (lname == "CHAIN") {
    lid = lattice_id::CHAIN;
    extent[dim2] = Extent{1, boundary_type::open, boundary_type::open};
    extent[dim3] = Extent{1, boundary_type::open, boundary_type::open};
  }

  /*------------- undefined lattice--------------*/
  else {
    throw std::range_error("error: latticelibrary: undefined lattice");
  }
  return 0;
}

// read lattice parameters
int Lattice::construct(const input::Parameters& parms) 
{

  int info;
  // name
  lname = parms.set_value("lattice", "NULL");
  boost::to_upper(lname);

  // sizes
  for (unsigned dim=dim1; dim<=dim3; ++dim) {
    std::string lsize = "lsize" + std::to_string(dim+1);
    extent[dim].size = parms.set_value(lsize, 1, info);
    if (extent[dim].size<1) throw std::range_error("error: latticelibrary: invalid lattice size");
  }

  // boundary conditions
  std::string bc; 
  for (unsigned dim=dim1; dim<=dim3; ++dim) {
    std::string lbc = "bc" + std::to_string(dim+1);
    bc = parms.set_value(lbc, "open", info);
    extent[dim].periodicity = boundary_condition(bc);
    extent[dim].bc = extent[dim].periodicity;
    if (extent[dim].bc == boundary_type::antiperiodic) extent[dim].bc = boundary_type::periodic;
  }

  // empty unitcell
  unitcell.clear();

  // impurities
  impurity_sites_.clear();
  impurity_bonds_.clear();

  define_lattice();
  finalize_lattice();
  //construct_graph();

  return 0;
}

int Lattice::finalize_lattice(void) 
{
  // Finalize the unit cell
  unitcell.finalize();

  /* Construct 'symmetrized' lattice */
  // copy the user set dimensions
  for (unsigned dim=dim1; dim<=dim3; ++dim) copy_extent[dim] = extent[dim];

  // initially, the 'dim' with periodic bc has size = 1
  spatial_dim = 0;
  Vector3d bvec;
  for (unsigned dim=dim1; dim<=dim3; ++dim) {
    if (extent[dim].bc==boundary_type::periodic) {
      spatial_dim++;
      // temporarily set size = 1 for dim with PBC
      extent[dim].size = 1;
      switch (dim) {
        case dim1: bvec = unitcell.vector_a1(); break;
        case dim2: bvec = unitcell.vector_a2(); break;
        case dim3: bvec = unitcell.vector_a3(); break;
      }
    }
  }
  // if 1 dimensional lattice, rotate the lattice to align 'bvec' along x-direction
  if (spatial_dim == 1) {
    // rotation matrix to do that
    Eigen::Matrix3d matrix = rotation_matrix(bvec, Vector3d(1.0, 0.0, 0.0));
    // rotate the unitcell
    unitcell.rotate_by(matrix);
  }

  // number of unit cells & sites
  num_layer_cells = extent[dim1].size * extent[dim2].size;
  num_total_cells = num_layer_cells * extent[dim3].size;
  num_total_sites = num_total_cells * unitcell.num_sites();

  // Add the sites & the bonds in the symmetrized unitcell
  std::vector<Site> sites;
  std::vector<Bond> bonds;
  //Vector3i src_cell, tgt_cell, src_offset, tgt_offset;
  Unitcell translated_cell;
  Vector3i bravindex(0,0,0);
  for (unsigned i=0; i<num_total_cells; ++i) {
    translated_cell = get_translated_cell(bravindex);
    // collect the sites
    for (unsigned n=0; n<translated_cell.num_sites(); ++n) sites.push_back(translated_cell.site(n));
    // collect the bonds
    for (unsigned n=0; n<translated_cell.num_bonds(); ++n) {
      Bond b = translated_cell.bond(n);
      if (connect_bond(b)) {
        //b.reset_id(bonds.size());
        bonds.push_back(b);
      }
    }
    bravindex = get_next_bravindex(bravindex);
  }

  // Replace the old lists
  unitcell.reset(sites, bonds);

  //std::cout << unitcell.vector_a1() << "\n";
  //std::cout << unitcell.vector_a2() << "\n";
  //std::cout << unitcell.vector_a3() << "\n";

  // extent & basis vectors of the symmetrized lattice
  for (unsigned dim=dim1; dim<=dim3; ++dim) {
    extent[dim] = copy_extent[dim];
    if (extent[dim].bc != boundary_type::periodic) {
      extent[dim].size = 1;
      switch (dim) {
        case dim1: unitcell.reset_a1(Vector3d(0.0,0.0,0.0)); break;
        case dim2: unitcell.reset_a2(Vector3d(0.0,0.0,0.0)); break;
        case dim3: unitcell.reset_a3(Vector3d(0.0,0.0,0.0)); break;
      }
    }
  }
  //std::cout << unitcell.vector_a1() << "\n";
  //std::cout << unitcell.vector_a2() << "\n";
  //std::cout << unitcell.vector_a3() << "\n";

  // number of unit cells & sites
  num_layer_cells = extent[dim1].size * extent[dim2].size;
  num_total_cells = num_layer_cells * extent[dim3].size;
  num_total_sites = num_total_cells * unitcell.num_sites();

  // check
  /*std::cout << "------Sites-------\n";
  for (unsigned i=0; i<unitcell.num_site(); ++i) {
    std::cout << unitcell.site(i) << std::endl;
  }
  std::cout << "------Bonds-------\n";
  for (unsigned i=0; i<unitcell.num_bond(); ++i) {
    std::cout << unitcell.bond(i) << std::endl;
  }*/

  return 0;
}






} // end namespace lattice
