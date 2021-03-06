/*---------------------------------------------------------------------------
* Copyright (C) 2015-2016 by Amal Medhi <amedhi@iisertvm.ac.in>.
* All rights reserved.
* Author: Amal Medhi
*----------------------------------------------------------------------------*/
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <cmc/simulation.h>

// the Simulation class
class MCSimulation : public mc::Simulator
{
public:
  MCSimulation(input::Parameters& parms);
  ~MCSimulation() {}
  int start(input::Parameters& parms);
private:
  void do_measurements(void);
  std::map<std::string, double> print_parms;
};

// implementation
MCSimulation::MCSimulation(input::Parameters& parms) : Simulator(parms) 
{
  // observables as functions of what?
  observables.as_function_of("kB*T");

  // define your observable operators (other than energy)
  if (need_magn) {
    Simulator::set_magn_op("S(i)");
  }
  if (observables.strain() || observables.strain_sq()) {
    Simulator::set_strain_op("sigma(i)");
  }
} 

int MCSimulation::start(input::Parameters& parms)
{
  // update model parameters
  Simulator::update_parameters(parms);

  // observables
  observables.reset();

  /*-----------------Simulation START-----------------*/
  // initial state
  Simulator::init_state_random();
  // warmup runs
  for (int i=0; i<warmup; ++i) Simulator::update_state();
  // measurement
  int count = 0;
  int num_measurement = 0;
  while (num_measurement != measure_samples) {
    Simulator::update_state();
    if (count++ == min_interval) {
      // make measurements
      count = 0;
      num_measurement++;
      do_measurements();
    }
  }
  // output
  //std::cout << "T = " << T << "\n";
  double kB_T = Simulator::kB * Simulator::T;
  observables.print(kB_T);
  /*-----------------Simulation END-----------------*/
  return 0;
}

// measurements
inline void MCSimulation::do_measurements(void)
{
  // energy
  if (need_energy) {
    energy_terms = Simulator::get_energy();
    if (observables.energy_terms()) {
      observables.energy_terms() << energy_terms;
    }
    if (observables.energy()) {
      double e = energy_terms.sum();
      observables.energy() << e;
    }
    if (observables.energy_sq()) {
      double e = energy_terms.sum();
      observables.energy_sq() << e*e;
    }
    if (observables.energy_terms_sq()) {
      observables.energy_terms_sq() << energy_terms.square();
    }
  }

  // magnetization
  if (observables.magn() || observables.magn_sq()) {
    double x = Simulator::get_magnetization();
    if (observables.magn()) observables.magn() << x;
    if (observables.magn_sq()) observables.magn_sq() << x*x;
  }

  // Potts magnetization
  if (observables.potts_magn() || observables.potts_magn_sq()) {
    double x = Simulator::get_potts_magnetization();
    if (observables.potts_magn()) observables.potts_magn() << x;
    if (observables.potts_magn_sq()) observables.potts_magn_sq() << x*x;
  }

  // strain
  if (observables.strain() || observables.strain_sq()) {
    double x = Simulator::get_strain();
    if (observables.strain()) observables.strain() << x;
    if (observables.strain_sq()) observables.strain_sq() << x*x;
  }
}

// main: feed the Simulatior to the scheduler
int main(int argc, const char *argv[])
{
  try {
   return scheduler::start(argc, argv, scheduler::Task<MCSimulation>());
  }
  catch (std::exception& e) {
    std::cout << e.what() << std::endl;
    return -1;
  }
  catch (...) {
    std::cout << "Fatal Error: Unknown Exception!\n";
    return -2;
  }
}

