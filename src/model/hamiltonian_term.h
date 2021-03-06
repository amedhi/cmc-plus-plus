/*---------------------------------------------------------------------------
* Copyright (C) 2015-2016 by Amal Medhi <amedhi@iisertvm.ac.in>.
* All rights reserved.
* Author: Amal Medhi
*----------------------------------------------------------------------------*/
#ifndef HAMILTONIAN_TERM_H
#define HAMILTONIAN_TERM_H

#include <string>
#include <vector>
#include <array>
#include <map>
//#include <unordered_map>
#include <stdexcept>
#include <Eigen/Core>
#include "sitebasis.h"
#include "modelparams.h"
#include "../lattice/lattice.h"
#include "../expression/expression.h"

namespace model {

class CouplingConstant : public std::unordered_map<int, std::string>
{
public:
  using super_type = std::unordered_map<int, std::string>;
  using iterator = super_type::iterator;
  using const_iterator = super_type::const_iterator;
  using value_type = super_type::value_type;

  CouplingConstant() {}
  CouplingConstant(const std::string& expr); 
  CouplingConstant(const value_type& type0, const value_type& type1={0,"_null_"}, 
    const value_type& type2={0,"_null_"}, const value_type& type3={0,"_null_"}, 
    const value_type& type4={0,"_null_"}, const value_type& type5={0,"_null_"});
  ~CouplingConstant() {}
  CouplingConstant& operator=(const std::string expr); 
  std::pair<iterator, bool> insert(const value_type& val);
  void create(const unsigned& num_type);
  void create(const value_type& type0, const value_type& type1={0,"_null_"}, 
    const value_type& type2={0,"_null_"}, const value_type& type3={0,"_null_"}, 
    const value_type& type4={0,"_null_"}, const value_type& type5={0,"_null_"});
  void add_type(const unsigned& type, const std::string& expr);
  void add_type(const value_type& val);
  void clear(void); 
  void clear_map(void) { super_type::clear(); } 
  const bool& valid(void) const { return valid_; } 
  const std::string& expression(const unsigned& type) const;

  static const int global_type;
private:
  int num_types_{0}; 
  bool valid_{false};
};

class SiteOperator
{
public:
  SiteOperator() : op_expr_{""}, site_{""} {}
  SiteOperator(const std::string& op_expr, const std::string& site);
  ~SiteOperator() {}
  int build_matrix(const SiteBasis& basis);
  int build_matrix(const SiteBasis& basis, expr::Expression::variables& vars);
  //int build_matrix(const SiteBasis& basis, expr::Expression::variables& vars);
  const double& matrix_element(const unsigned& idx) const { return matrix_(idx); }
private:
  std::string op_expr_;
  std::string site_;
  //std::vector<int> matrix_; // matrix elements, diagonal ones only
  Eigen::VectorXd matrix_; // matrix elements, diagonal ones only
};

class SiteOperatorTerm: public SiteOperator
{
public:
  SiteOperatorTerm() : SiteOperator(), name_{""}, cc_expr_{""}, cc_value_{0.0} {}
  //SiteOperatorTerm(const std::string& name, const double& coupling, 
  //  const std::string& site, const std::string& op_expr);
  SiteOperatorTerm(const std::string& name, const std::string& cc_expr, 
    const std::string& op_expr, const std::string& site);
  ~SiteOperatorTerm() {}
  int eval_coupling_constant(const ModelParams& cvals, const ModelParams& pvals);
  const double& coupling(void) const { return cc_value_; }
private:
  std::string name_{""};
  std::string cc_expr_{""};
  double cc_value_{0.0};
};

//class SiteTerm : public std::unordered_map<unsigned, SiteOperatorTerm>
class SiteTerm : public std::array<SiteOperatorTerm, lattice::MAX_SITE_TYPES>
{
public:
  //using super_type = std::unordered_map<unsigned, SiteOperatorTerm>;
  using super_type = std::array<SiteOperatorTerm, lattice::MAX_SITE_TYPES>;
  using iterator = super_type::iterator;
  using const_iterator = super_type::const_iterator;

  //SiteTerm() {}
  SiteTerm(const std::string& name, const CouplingConstant& cc, 
    const std::string& site, const std::string& op_expr, const unsigned& size_);
  ~SiteTerm() {}
  void build_matrix(const BasisDescriptor& basis);
  void eval_coupling_constant(const ModelParams& cvals, const ModelParams& pvals);
  //const int& matrix_element(const unsigned& const unsigned& idx) const { return matrix_[idx]; }
  const double& matrix_element(const unsigned& site_type, const unsigned& state_idx) const; 
  const double& coupling(const unsigned& site_type) const; 
  const std::string& name(void) const { return name_; }
private:
  std::string name_;
  //double null_element_{0.0};
  //double null_coupling_{0.0};
  unsigned size_{1};
};

//---------------------Bond Terms-------------------
class BondOperator
{
public:
  BondOperator() : op_expr_{""}, source_{""}, target_{""} {}
  BondOperator(const std::string& op_expr, const std::string& source, const std::string& target);
  ~BondOperator() {}
  int build_matrix(const SiteBasis& source_basis, const SiteBasis& target_basis);
  const double& matrix_element(const unsigned& src_idx, const unsigned& tgt_idx) const 
    { return matrix_(src_idx, tgt_idx); }
private:
  std::string op_expr_{""};
  std::string source_{""};
  std::string target_{""};
  Eigen::MatrixXd matrix_;
};

class BondOperatorTerm: public BondOperator
{
public:
  BondOperatorTerm() : BondOperator(), name_{""}, cc_expr_{""}, cc_value_{0.0} {}
  BondOperatorTerm(const std::string& name, const std::string& cc_expr, 
  const std::string& op_expr, const std::string& source, const std::string& target);
  ~BondOperatorTerm() {}
  int eval_coupling_constant(const ModelParams& cvals, const ModelParams& pvals);
  const double& coupling(void) const { return cc_value_; }
private:
  std::string name_{""};
  std::string cc_expr_{""};
  double cc_value_{0.0};
};

//class BondTerm : public std::unordered_map<unsigned, BondOperatorTerm>
class BondTerm : public std::array<BondOperatorTerm, lattice::MAX_BOND_TYPES>
{
public:
  //using super_type = std::unordered_map<unsigned, BondOperatorTerm>;
  using super_type = std::array<BondOperatorTerm, lattice::MAX_BOND_TYPES>;
  using iterator = super_type::iterator;
  using const_iterator = super_type::const_iterator;
  using BondSiteMap = std::map<unsigned, std::pair<unsigned, unsigned> >;
  //BondTerm() {}
  BondTerm(const std::string& name, const CouplingConstant& cc, const std::string& op_expr, 
    const std::string& src, const std::string& tgt, const unsigned& size);
  ~BondTerm() {}
  void build_matrix(const BasisDescriptor& basis, const BondSiteMap& bondtypes);
  const double& matrix_element(const unsigned& bond_type, const unsigned& src_idx, 
    const unsigned& tgt_idx) const;
  void eval_coupling_constant(const ModelParams& cvals, const ModelParams& pvals);
  const double& coupling(const unsigned& bond_type) const; 
  const std::string& name(void) const { return name_; }
private:
  std::string name_;
  //double null_element_{0.0};
  //double null_coupling_{0.0};
  unsigned size_{1};
};


} // end namespace model

#endif
