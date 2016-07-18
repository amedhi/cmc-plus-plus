/*---------------------------------------------------------------------------
* Copyright (C) 2015-2016 by Amal Medhi <amedhi@iisertvm.ac.in>.
* All rights reserved.
* Author: Amal Medhi
* Date:   2016-01-25 18:05:03
* Last Modified by:   Amal Medhi, amedhi@macbook
* Last Modified time: 2016-03-16 16:47:10
*----------------------------------------------------------------------------*/
#ifndef LATTICE_H
#define LATTICE_H

#include <iostream>
#include <string>
#include <tuple>
#include <stdexcept>
#include "../scheduler/task.h"
#include "../constants.h"
#include <Eigen/Dense>
#include <boost/graph/adjacency_list.hpp>

namespace lattice {

/*---------------lattice types-----------------*/
enum class lattice_id {
  UNDEFINED, SQUARE, CHAIN, HONEYCOMB
};

/*---------------Lattice site class-----------------*/
using Vector3i = Eigen::Vector3i;
using Vector3d = Eigen::Vector3d;
//using BrvaisIdx = Eigen::Matrix<unsigned, 3, 1>;

class Site 
{
public:
  // ctors
  Site() {}
  Site(const unsigned& uid, const unsigned& type, const unsigned& atomid, const Vector3i& bravindex, 
    const Vector3d& coord, const Vector3d& cell_coord);
  ~Site() {}
  // setter functions
  static void reset_count(void) { num_site=0; }
  void reset_uid(const unsigned& uid) { uid_=uid; }
  void reset_bravindex(const Vector3i& idx) { bravindex_=idx; }
  void reset_coord(const Vector3d& v) { coord_=v; }
  void reset_cell_coord(const Vector3d& v) { cell_coord_=v; }
  void translate_by(const int& id_offset, const Vector3i& bravindex_offset, const Vector3d& coord_offset); 

  // getter functions
  int id(void) const {return id_;}
  unsigned uid(void) const {return uid_;}
  unsigned type(void) const {return type_;}
  unsigned atomid(void) const {return atomid_;}
  Vector3i bravindex(void) const { return bravindex_; }
  Vector3d coord(void) const {return coord_;}
  Vector3d cell_coord(void) const {return cell_coord_;}
  // friends
  friend std::ostream& operator<<(std::ostream& os, const Site& site);
private:
  static unsigned num_site;
  int id_ {0};
  unsigned uid_ {0}; // local id within a unitcell
  unsigned type_ {0};
  unsigned atomid_ {0};
  Vector3i bravindex_ {Vector3i(0, 0, 0)};
  Vector3d coord_ {Vector3d(0.0, 0.0, 0.0)};
  Vector3d cell_coord_ {Vector3d(0.0, 0.0, 0.0)};
};

/*---------------Lattice bond class-----------------*/
class Bond 
{
public:
  // ctors
  Bond() {}
  Bond(const unsigned& type, const unsigned& ngb, const Vector3i& bravindex, const unsigned& src_id, 
    const Vector3i& src_offset, const unsigned& tgt_id, const Vector3i& tgt_offset, const int& sign);
  ~Bond() {}
  // setter functions
  static void reset_count(void) { num_bond=0; }
  void reset_id(const unsigned& id) { id_=id; }
  void reset_src_offset(const Vector3i& idx) { src_offset_=idx; }
  void reset_tgt_offset(const Vector3i& idx) { tgt_offset_=idx; }
  void reset_bravindex(const Vector3i& idx) { bravindex_=idx; }
  void shift_target_ids(const int& id_offset) { src_ += id_offset; tgt_ += id_offset; }
  void translate_by(const Vector3i& bravindex_offset) { bravindex_ += bravindex_offset; } 
  void connect(const unsigned& src_id, const Vector3i& src_offset, const unsigned& tgt_id, 
    const Vector3i& tgt_offset, const int& sign);
  // getter functions
  int id(void) const { return id_; }
  unsigned type(void) const {return type_;}
  unsigned src_id(void) const { return src_; }
  unsigned tgt_id(void) const { return tgt_; }
  int sign(void) const { return sign_; }
  Vector3i bravindex(void) const { return bravindex_; }
  Vector3i src_offset(void) const { return src_offset_; }
  Vector3i tgt_offset(void) const { return tgt_offset_; }
  // friends
  friend std::ostream& operator<<(std::ostream& os, const Bond& bond);
private:
  static unsigned num_bond;
  int id_ {0};
  unsigned type_ {0};
  unsigned ngb_ {0};
  unsigned src_ {0}; 
  unsigned tgt_ {0}; 
  int sign_ {1}; // = -1 if across an antiperiodic boundary
  Vector3i bravindex_ {Vector3i(0, 0, 0)};
  Vector3i src_offset_ {Vector3i(0, 0, 0)};
  Vector3i tgt_offset_ {Vector3i(0, 0, 0)};
};

/*---------------Unitcell class-----------------*/
class Unitcell 
{
public:
  // ctors
  Unitcell() {}
  ~Unitcell() {}
  // setter functions
  void clear(void); 
  void set_basis(const Vector3d& av1, const Vector3d& av2, const Vector3d& av3);
  int add_site(const unsigned& type, const unsigned& atomid, const Vector3d& site_coord); 
  int add_site(const unsigned& type, const Vector3d& site_coord); 
  int add_site(const Site& s) { sites.push_back(s); return sites.back().id(); }
  int add_site(const Site& s, const Vector3i& bravindex, const Vector3d& cell_coord);
  int add_bond(const Bond& b) { bonds.push_back(b); return bonds.back().id(); }
  int add_bond(const unsigned& type, const unsigned& ngb, const unsigned& src_id, const Vector3i& src_offset,
    const unsigned& tgt_id, const Vector3i& tgt_offset); 
  int add_bond(const Bond& bond, const Vector3i& src_offset, const Vector3i& tgt_offset);
  void reset_a1(const Vector3d& av) { a1=av; }
  void reset_a2(const Vector3d& av) { a2=av; }
  void reset_a3(const Vector3d& av) { a3=av; }
  void reset(const std::vector<Site>& new_sites, const std::vector<Bond>& new_bonds); 
  // getter functions
  Vector3d vector_a1(void) const { return a1; }
  Vector3d vector_a2(void) const { return a2; }
  Vector3d vector_a3(void) const { return a3; }
  Vector3i bravindex(void) const { return bravindex_; }
  Vector3d coord(void) const {return coord_;}
  unsigned num_sites(void) const { return sites.size(); }
  unsigned num_bonds(void) const { return bonds.size(); }
  void translate_by(const Vector3i& bravindex_offset, const int& cell_id_offset);
  void rotate_by(const Eigen::Matrix3d& matrix);
  Site site(const unsigned& i) const { return sites[i]; }
  Bond bond(const unsigned& i) const { return bonds[i]; }
private:
  int id {0};
  unsigned max_site_type_val {0};
  unsigned max_bond_type_val {0};
  unsigned max_neighb_val {0};
  Vector3d a1 {Vector3d(0.0, 0.0, 0.0)};
  Vector3d a2 {Vector3d(0.0, 0.0, 0.0)};
  Vector3d a3 {Vector3d(0.0, 0.0, 0.0)};
  std::vector<Site> sites;
  std::vector<Bond> bonds;
  Vector3i bravindex_ {Vector3i(0, 0, 0)};
  Vector3d coord_ {Vector3d(0.0, 0.0, 0.0)};
};

/*---------------spatial dimension type-----------------*/
enum class boundary_type {open, periodic, antiperiodic};

/*---------------Lattice class defintion-----------------*/

/*
// lattice graph objects
namespace graph {

// Vertex properties
struct VertexProperties {
  unsigned uid; // unitcell id
  unsigned type; 
  unsigned stype; // symmetry type
  unsigned atomid; 
  Vector3i bravindex;
  Vector3d coord; 
  Vector3d cell_coord;
};

// Edge properties
struct EdgeProperties {
  unsigned type; 
  unsigned stype; 
  int sign; 
  Vector3d vector; 
};

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, 
              VertexProperties, EdgeProperties>;
using vertex = boost::graph_traits<Graph>::vertex_descriptor;
using edge = boost::graph_traits<Graph>::edge_descriptor;
using vertex_iterator = boost::graph_traits<Graph>::vertex_iterator;
using edge_iterator = boost::graph_traits<Graph>::edge_iterator;

} // end namespace 'graph'
*/

class Lattice 
{
public:
  // ctors
  Lattice() {}
  Lattice(const input::Parameters& parms) { construct(parms); }
  ~Lattice() {}
  // setter functions
  int construct(const input::Parameters& parms);
  // getter functions
  std::string name(void) const { return lname; }
  lattice_id id(void) const { return lid; }
  unsigned dimension(void) const { return spatial_dim; }
  unsigned num_sites(void) const { return num_total_sites; }
  unsigned num_basis_sites(void) const { return unitcell.num_sites(); }
  unsigned num_unitcells(void) const { return num_total_cells; }
  Vector3d vector_a1(void) const { return unitcell.vector_a1(); }
  Vector3d vector_a2(void) const { return unitcell.vector_a2(); }
  Vector3d vector_a3(void) const { return unitcell.vector_a3(); }
  int size1(void) const { return static_cast<int>(extent[dim1].size); }
  int size2(void) const { return static_cast<int>(extent[dim2].size); }
  int size3(void) const { return static_cast<int>(extent[dim3].size); }
  boundary_type bc1(void) const { return extent[dim1].bc; }
  boundary_type bc2(void) const { return extent[dim2].bc; }
  boundary_type bc3(void) const { return extent[dim3].bc; }
  //boundary_type bc2(void) const { return extent[dim2].bc; }
  //boundary_type bc3(void) const { return extent[dim3].bc; }
  boundary_type bc1_periodicity(void) const { return extent[dim1].periodicity; }
  boundary_type bc2_periodicity(void) const { return extent[dim2].periodicity; }
  boundary_type bc3_periodicity(void) const { return extent[dim3].periodicity; }

  // other methods 
  //Vector3i boundary_wrap(const Vector3i& cell_idx) const;
  std::pair<Vector3i, int> boundary_wrap(const Vector3i& cell_idx) const;
  Vector3i get_next_bravindex(const Vector3i& current_index) const;
  Unitcell get_translated_cell(const Vector3i& bravindex_offset) const;
  int mapped_site_id(const unsigned& local_id, const Vector3i& bravindex) const;
  bool connect_bond(Bond& bond) const;
private:
  struct Extent {unsigned size; boundary_type bc; boundary_type periodicity;};
  enum Dimension {dim1, dim2, dim3};
  lattice_id lid {lattice_id::UNDEFINED};
  std::string lname {""};
  unsigned spatial_dim {0};

  // unitcell & lattice dimensions
  Unitcell unitcell;
  Extent extent[3] = {Extent{1, boundary_type::open, boundary_type::open}, 
                      Extent{1, boundary_type::open, boundary_type::open},
                      Extent{1, boundary_type::open, boundary_type::open}
                     };

  // copy of initial lattice dimensions
  Extent copy_extent[3] {Extent{1, boundary_type::open, boundary_type::open}, 
                         Extent{1, boundary_type::open, boundary_type::open},
                         Extent{1, boundary_type::open, boundary_type::open}
                        };
  
  // number of unit cells in total and in one layer (for symmetrized lattice)
  unsigned num_total_cells {1};
  unsigned num_layer_cells {1};
  unsigned num_total_sites {0};

  /*
  // graph data
  graph::Graph g;
  graph::vertex_iterator vi_begin, vi_end;
  graph::edge_iterator ei_begin, ei_end;
  boost::property_map<graph::Graph, boost::vertex_index_t>::type vertex_index;
  */

  // helper functions
  int define_lattice(void); 
  int finalize_lattice(void); 
  //int construct_graph(void); 
  boundary_type boundary_condition(std::string& bc) const;
  Eigen::Matrix3d rotation_matrix(const Eigen::Vector3d& r, const Eigen::Vector3d& r_prime);
};


} // end namespace lattice

#endif






