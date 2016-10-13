/*---------------------------------------------------------------------------
* Copyright (C) 2015-2016 by Amal Medhi <amedhi@iisertvm.ac.in>.
* All rights reserved.
* Author: Amal Medhi
* Date:   2016-03-11 13:02:35
* Last Modified by:   Amal Medhi, amedhi@macbook
* Last Modified time: 2016-03-17 23:51:58
*----------------------------------------------------------------------------*/
#include "model.h"
#include <boost/algorithm/string.hpp>

namespace model {

int Model::define_model(const input::Parameters& inputs, const lattice::Lattice& lattice)
{
  //int info;
  //unsigned ntypes;
  //std::vector<MatrixElement> matrix_elem(20);
  double defval;
  unsigned sitetype, change, type, src_type, tgt_type;
  std::string name, matrixelem, op, qn, site, src, tgt, fact;
  SiteBasis site_basis;
  BasisDescriptor basis;
  QuantumNumber::value_type min, max, step;
  CouplingConstant cc;
  //QuantumOperator q_op0, q_op1, q_op2;

  // define the models 
  model_name = inputs.set_value("model", "ISING");
  boost::to_upper(model_name);

  if (model_name == "ISING") {
    // site basis
    site_basis.clear();
    site_basis.add_qn(qn="S", min=-1, max=1, step=2);
    site_basis.add_operator(op="S", matrixelem="S", qn="S", change=0);
    add_sitebasis(site_basis);
    // model parameters
    add_parameter(name="J", defval=1.0, inputs);
    // bond operator terms
    add_bondterm(name="Exchange", cc="-J", op="S(i)*S(j)", src="i", tgt="j");
  }

  else if (model_name == "BEG_POTTS_NI2MNX") {
    switch (lattice.id()) {
      /*------------- 'SQUARE' lattice--------------*/
      case lattice::lattice_id::SIMPLECUBIC:
        //site_basis
        site_basis.clear();
        site_basis.add_qn(qn="S", min=-2, max=2, step=1);
        site_basis.add_qn(qn="sigma", min=-1, max=1, step=1);
        site_basis.add_operator(op="S", matrixelem="S", qn="S");
        site_basis.add_operator(op="sigma", matrixelem="sigma", qn="sigma");
        add_sitebasis(site_basis, sitetype=0);

        // model parameters
        add_parameter(name="T", defval=1.0, inputs);
        add_parameter(name="kB", defval=1.0, inputs);
        add_parameter(name="J", defval=1.0, inputs);
        add_parameter(name="J_fm", defval=1.0, inputs);
        add_parameter(name="J_afm", defval=1.0, inputs);
        add_parameter(name="H", defval=0.0, inputs);
        add_parameter(name="U", defval=0.0, inputs);
        add_parameter(name="K", defval=0.0, inputs);
        add_parameter(name="T_afm", defval=1.0, inputs);

        // this model has terms for impurity 'bond' 
        def_impurity_bondtype(type=1, src_type=0, tgt_type=0);

        // site operator term
        // magnetic field along +z direction (S=+2) 
        add_siteterm("H_field", cc="-H/J", op="cron(S(i),2)", site="i");
        add_siteterm("sigma", cc="-0.693147*kB*T/J", op="1-sigma(i)*sigma(i)", site="i");

        // bond operator term
        cc = CouplingConstant({0, "-J_fm/J"}, {1, "-J_afm/J*min(1.0,(T/T_afm-1.0))"});
        add_bondterm("Potts", cc, op="cron(S(i),S(j))", src="i", tgt="j");
        add_bondterm("Tetra", cc="-1.0", op="sigma(i)*sigma(j)", src="i", tgt="j");
        add_bondterm("Cubic", cc="-K/J", op="(1-sigma(i)*sigma(i))*(1-sigma(j)*sigma(j))", src="i", tgt="j");
        add_bondterm("Interaction", cc="U/J", 
          op="cron(S(i),S(j))*((1-sigma(i)*sigma(i))*(1-sigma(j)*sigma(j))-0.5)", src="i", tgt="j");
        break;
      default:
        throw std::range_error("*error: modellibrary: model not defined for the given lattice");
    }
  }

  else if (model_name == "BEG") {
    switch (lattice.id()) {
      /*------------- 'SQUARE' lattice--------------*/
      case lattice::lattice_id::SQUARE:
        //site_basis
        site_basis.clear();
        site_basis.add_qn(qn="Sz", min=-2, max=2, step=2);
        site_basis.add_qn(qn="El", min=-1, max=1, step=1);
        site_basis.add_operator(name="Sz", matrixelem="Sz", qn="Sz", change=0);
        site_basis.add_operator(name="sigma", matrixelem="El", qn="El", change=0);
        add_sitebasis(site_basis, sitetype=0);
        // model parameters
        add_parameter(name="J", defval=1.0, inputs);
        add_parameter(name="B", defval=0.0, inputs);
        // site operator term
        add_siteterm("MagField", cc="B", op="2*Sz(i)*sigma(i)", site="i");
        // bond operator term
        add_bondterm("Exchange", cc="-J", op="Sz(i)*Sz(j)", src="i", tgt="j");
        break;
      default:
        throw std::range_error("*error: modellibrary: model not defined for the given lattice");
    }
  }

  /*------------- undefined lattice--------------*/
  else {
    throw std::range_error("*error: modellibrary: undefined model");
  }

  return 0;
}

int Model::construct(const input::Parameters& inputs, const lattice::Lattice& lattice)
{
  // reset
  parms_.clear();
  //operators.clear();
  // maps of site & bond type values (to contigous type values)
  sitetypes_map_ = lattice.sitetypes_map();
  bondtypes_map_ = lattice.bondtypes_map();
  // maps of a given bond type to the types of its target
  bond_sites_map_.clear();
  for (unsigned i=0; i<lattice.num_unitcell_bonds(); ++i) {
    lattice::Bond b = lattice.unitcell_bond(i);
    lattice::Site src = lattice.unitcell_site(b.src_id());
    lattice::Site tgt = lattice.unitcell_site(b.tgt_id());
    bond_sites_map_.insert({b.type(), std::make_pair(src.type(), tgt.type())});
  }
  impurity_bond_types_.clear();

  define_model(inputs, lattice);
  finalize(lattice);
  return 0;
}


} // end namespace model