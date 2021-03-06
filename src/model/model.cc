/*---------------------------------------------------------------------------
* Copyright (C) 2015-2016 by Amal Medhi <amedhi@iisertvm.ac.in>.
* All rights reserved.
* Author: Amal Medhi
* Date:   2016-03-09 15:27:50
* Last Modified by:   Amal Medhi, amedhi@macbook
* Last Modified time: 2017-04-08 23:50:19
*----------------------------------------------------------------------------*/
#include "model.h"

namespace model {

unsigned Model::add_sitebasis(SiteBasis& sitebasis)
{
  // it's an error if any 'sitebasis' was already added
  if (basis_.size()>0) 
    throw std::logic_error("Model::add_sitebasis: 'sitebasis' already exists, overwrite not allowed.");
  // the 'sitebasis' is implicitly defined for all site types
  for (const auto& elem : sitetypes_map_) {
    unsigned mapped_type = elem.second;
    basis_.add_sitebasis(mapped_type,sitebasis); 
  }
  return basis_.size();
}

unsigned Model::add_sitebasis(const unsigned& type, SiteBasis& sitebasis)
{
  // add 'sitebasis' of the given 'site type'
  auto it=sitetypes_map_.find(type);
  if (it==sitetypes_map_.end()) 
    throw std::range_error("Model::add_sitebasis: specified 'site type' not found");
  unsigned mapped_type = it->second;
  // it's an error if any 'sitebasis' was already added
  if (!basis_.add_sitebasis(mapped_type,sitebasis)) 
    throw std::logic_error("Model::add_sitebasis: 'sitebasis' already exists, overwrite not allowed.");
  return basis_.size();
}

/*
void Model::def_impurity_bondtype(const unsigned& btype, const unsigned& src_type, 
  const unsigned& tgt_type)
{
  unsigned mapped_btype, mapped_src_type, mapped_tgt_type;
  // if the bondtype already exist
  if (bondtypes_map_.find(btype) != bondtypes_map_.end()) 
    throw std::range_error("Model::add_impurity_bond: 'bond type' already exists");
  // if the sitetypes is not defined yet
  if (sitetypes_map_.find(src_type) == sitetypes_map_.end()) 
    throw std::range_error("Model::add_impurity_bond: 'src type' does not exist");
  else mapped_src_type = sitetypes_map_.at(src_type);
  if (sitetypes_map_.find(tgt_type) == sitetypes_map_.end()) 
    throw std::range_error("Model::add_impurity_bond: 'tgt type' does not exist");
  else mapped_tgt_type = sitetypes_map_.at(tgt_type);
  // add the new bond type & its mapped value
  //impurity_bond_types_.push_back(btype);
  mapped_btype = bondtypes_map_.size();
  bondtypes_map_.insert({btype, mapped_btype});
  // add to bond type-target types map
  bond_sites_map_.insert({mapped_btype, 
    std::make_pair(mapped_src_type, mapped_tgt_type)});
}*/

void Model::add_impurity_bond(const unsigned& id, const unsigned& btype, const unsigned& src_type, 
    const unsigned& tgt_type)
{
  unsigned mapped_btype, mapped_src_type, mapped_tgt_type;
  // 'id' of impurity bondtype must be unique
  if (impurity_bond_types_.find(id) != impurity_bond_types_.end()) 
    throw std::range_error("Model::add_impurity_bond: bond 'id' already exist");
  // if the bondtype already exist
  if (bondtypes_map_.find(btype) != bondtypes_map_.end()) {
    impurity_bond_types_.insert({id, btype});
    return;
  }
  // if the sitetypes is not defined yet
  if (sitetypes_map_.find(src_type) == sitetypes_map_.end()) 
    throw std::range_error("Model::add_impurity_bond: 'src type' does not exist");
  else mapped_src_type = sitetypes_map_.at(src_type);
  if (sitetypes_map_.find(tgt_type) == sitetypes_map_.end()) 
    throw std::range_error("Model::add_impurity_bond: 'tgt type' does not exist");
  else mapped_tgt_type = sitetypes_map_.at(tgt_type);
  // 'btype' is new, hence add its mapped type value
  mapped_btype = bondtypes_map_.size();
  bondtypes_map_.insert({btype, mapped_btype});
  impurity_bond_types_.insert({id, mapped_btype});
  // add to bond type-target types map
  bond_sites_map_.insert({mapped_btype, std::make_pair(mapped_src_type, mapped_tgt_type)});
}

/*
int Model::get_impurity_bondtype(void) const
{
  if (impurity_bond_types_.size()==1) return bondtypes_map_.at(impurity_bond_types_[0]);
  // at present assume only one type of impurity bond 
  return -1;
}
*/

unsigned Model::add_siteterm(const std::string& name, const CouplingConstant& cc,
  const std::string& op_expr, const std::string& site)
{
  // remap site type values in 'cc'
  CouplingConstant cc_remapped = cc;
  cc_remapped.clear_map();
  if (cc.size()==1 && cc.begin()->first==CouplingConstant::global_type) {
    // the 'cc' is implicitly defined for all types
    std::string cc_expr = cc.begin()->second;
    for (const auto& m : sitetypes_map_) {
      cc_remapped.insert({m.second, cc_expr});
    }
  }
  else {
    for (auto it=cc.begin(); it!=cc.end(); ++it) {
      unsigned sitetype = it->first;
      auto it2=sitetypes_map_.find(sitetype);
      if (it2!=sitetypes_map_.end()) {
        unsigned mapped_type = it2->second;
        //std::cout << mapped_type << it->second << "\n";
        cc_remapped.insert({mapped_type, it->second});
      }
      else throw std::range_error("Model::add_siteterm: non-existent 'site type' specified");
    }
  }
  unsigned num_sitetypes = sitetypes_map_.size();
  this->std::vector<SiteTerm>::push_back(SiteTerm(name, cc_remapped, op_expr, site, num_sitetypes));
  return this->std::vector<SiteTerm>::size();
}

unsigned Model::add_bondterm(const std::string& name, const CouplingConstant& cc,
  const std::string& op_expr, const std::string& src, const std::string& tgt)
{
  // remap bond type values in 'cc'
  CouplingConstant cc_remapped = cc;
  cc_remapped.clear_map();
  if (cc.size()==1 && cc.begin()->first==CouplingConstant::global_type) {
    // the 'cc' is implicitly defined for all types
    std::string cc_expr = cc.begin()->second;
    for (const auto& m : bondtypes_map_) {
      //std::cout << "m= " << m.second << " cc_expr = " << cc_expr << "\n";
      cc_remapped.insert({m.second, cc_expr});
    }
  }
  else {
    for (auto it=cc.begin(); it!=cc.end(); ++it) {
      unsigned bondtype = it->first;
      auto it2=bondtypes_map_.find(bondtype);
      if (it2!=bondtypes_map_.end()) {
        unsigned mapped_type = it2->second;
        cc_remapped.insert({mapped_type, it->second});
      }
      else throw std::range_error("Model::add_bondterm: non-existent 'site type' specified");
    }
  }
  unsigned num_bondtypes = bondtypes_map_.size();
  std::vector<BondTerm>::push_back(BondTerm(name, cc_remapped, op_expr, src, tgt, num_bondtypes));
  return std::vector<BondTerm>::size();
}

void Model::finalize(const lattice::Lattice& L)
{
  // check if 'sitebasis' for all 'site types' are defined
  for (const auto& elem : sitetypes_map_) {
    unsigned site_type = elem.second;
    if (basis_.find(site_type) == basis_.end()) 
      throw std::range_error("modellibrary:: 'sitebasis' not defined for all 'site type'-s");
  }

  // finalize the site terms
  for (auto it=std::vector<SiteTerm>::begin(); it!=std::vector<SiteTerm>::end(); ++it) {
    it->build_matrix(basis_); 
    it->eval_coupling_constant(constants_, parms_); 
  }
  has_siteterm_ = (std::vector<SiteTerm>::size()>0);
  st_begin_ = std::vector<SiteTerm>::cbegin();
  st_end_ = std::vector<SiteTerm>::cend();
  // finalize the bond terms
  // map each 'bondtype' to its 'src' & 'tgt' types
  /*BondTerm::BondSiteMap bond_site_map;
  for (unsigned i=0; i<L.num_unitcell_bonds(); ++i) {
    lattice::Bond b = L.unitcell_bond(i);
    lattice::Site src = L.unitcell_site(b.src_id());
    lattice::Site tgt = L.unitcell_site(b.tgt_id());
    bond_site_map.insert({b.type(), std::make_pair(src.type(), tgt.type())});
  }*/
  for (auto it=std::vector<BondTerm>::begin(); it!=std::vector<BondTerm>::end(); ++it) {
    it->build_matrix(basis_, bond_sites_map_); 
    it->eval_coupling_constant(constants_, parms_); 
  }
  has_bondterm_ = (std::vector<BondTerm>::size()>0);
  bt_begin_ = std::vector<BondTerm>::cbegin();
  bt_end_ = std::vector<BondTerm>::cend();


  set_info_string(L);
}

void Model::update_parameters(const input::Parameters& inputs)
{
  // update the parameter values
  for (auto& p : parms_) p.second = inputs.set_value(p.first, p.second);
  // update the model term couping constants
  for (auto it=std::vector<SiteTerm>::begin(); it!=std::vector<SiteTerm>::end(); ++it) {
    it->eval_coupling_constant(constants_, parms_); 
    //std::cout << it->coupling(0) << "\n"; getchar();
  }
  for (auto it=std::vector<BondTerm>::begin(); it!=std::vector<BondTerm>::end(); ++it) {
    it->eval_coupling_constant(constants_, parms_); 
  }
}

void Model::get_term_names(std::vector<std::string>& term_names) const
{
  term_names.clear();
  for (auto it=std::vector<BondTerm>::cbegin(); it!= std::vector<BondTerm>::cend(); ++it) 
    term_names.push_back(it->name());
  for (auto it=std::vector<SiteTerm>::cbegin(); it!= std::vector<SiteTerm>::cend(); ++it) 
    term_names.push_back(it->name());
}

void Model::set_info_string(const lattice::Lattice& L) 
{
  info_str_.clear();
  info_str_ << "# Lattice: " << L.name() << " (";
  info_str_ << "Size="<<L.size1()<<"x"<<L.size2()<<"x"<< L.size3()<<", ";
  info_str_ << "Sites/unitcell="<<L.num_basis_sites()<<", ";
  info_str_ << "Boundary="<<static_cast<int>(L.bc1_periodicity()) << "-"; 
  info_str_ << static_cast<int>(L.bc2_periodicity()) << "-";
  info_str_ << static_cast<int>(L.bc3_periodicity()) << ")\n";
  info_str_ << "# No of sites = " << L.num_sites() << "\n";
  info_str_ << "# Model: " << model_name << "\n";
  info_str_.precision(6);
  info_str_.setf(std::ios_base::fixed);
  for (const auto& p : parms_) 
    info_str_ << "# " << p.first << " = " << p.second << "\n";
}





} // end namespace model
