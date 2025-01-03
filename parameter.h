/**
 * @author      Christoph Schaefer cm.schaefer@gmail.com and Thomas I. Maindl
 *
 * @section     LICENSE
 * Copyright (c) 2019 Christoph Schaefer
 *
 * This file is part of miluphcuda.
 *
 * miluphcuda is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * miluphcuda is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with miluphcuda.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef _PARAMETER_H
#define _PARAMETER_H

// Dimension of the problem
#define DIM 3

// Basic physical model, choose one of the following:
// SOLID solves continuum mechanics with material strength, and stress tensor \sigma^{\alpha \beta} = -p \delta^{\alpha \beta} + S^{\alpha \beta}
// HYDRO solves only the Euler equation, and there is only (scalar) pressure
#define SOLID 1
#define HYDRO 0
// set additionally p to 0 if p < 0
#define REAL_HYDRO 0

// add additional point masses to the simulation, read from file <filename>.mass
// format is location velocities mass r_min r_max, where location and velocities are vectors with size DIM and
// r_min/r_max are min/max distances of sph particles to the bodies before they are taken out of the simulation
#define GRAVITATING_POINT_MASSES 1

// sink particles (set point masses to be sink particles)
#define PARTICLE_ACCRETION 1 // check if particle is bound to one of the sink particles (crossed the accretion radius, rmin); if also UPDATE_SINK_VALUES 1: particle is accreted and ignored afterwards, else: continues orbiting without being accreted
#define UPDATE_SINK_VALUES 1 // add to sink the quantities of the accreted particle: mass, velocity and COM

// integrate the energy equation
// when setting up a SOLID simulation with Tillotson or ANEOS, it must be set to 1
#define INTEGRATE_ENERGY 1

// integrate the continuity equation
// if set to 0, the density will be calculated using the standard SPH sum \sum_i m_j W_ij
#define INTEGRATE_DENSITY 1

// adds viscosity to the Euler equation
#define NAVIER_STOKES 0
// choose between two different viscosity models
#define SHAKURA_SUNYAEV_ALPHA 0
#define CONSTANT_KINEMATIC_VISCOSITY 0
// artificial bulk viscosity according to Schaefer et al. (2004)
#define KLEY_VISCOSITY 0

// Grady-Kipp fragmentation/damage model, following Benz & Asphaug (1995). Set FRAGMENTATION to activate it.
// Damage acts always on (negative) pressure, but only on deviatoric stresses if DAMAGE_ACTS_ON_S is set.
// It depends on the use case and the plasticity model (set below) whether this is desired/reasonable.
// Note: The damage model needs distribution of activation thresholds in the input file.
#define FRAGMENTATION 0
#define DAMAGE_ACTS_ON_S 0

// Choose the SPH representation to solve the momentum and energy equation:
// SPH_EQU_VERSION 1: original version with HYDRO dv_a/dt ~ - (p_a/rho_a**2 + p_b/rho_b**2)  \nabla_a W_ab
//                                     SOLID dv_a/dt ~ (sigma_a/rho_a**2 + sigma_b/rho_b**2) \nabla_a W_ab
// SPH_EQU_VERSION 2: slighty different version with
//                                     HYDRO dv_a/dt ~ - (p_a+p_b)/(rho_a*rho_b)  \nabla_a W_ab
//                                     SOLID dv_a/dt ~ (sigma_a+sigma_b)/(rho_a*rho_b)  \nabla_a W_ab
// If you do not know what to do, choose SPH_EQU_VERSION 1.
#define SPH_EQU_VERSION 1

// for the tensile instability fix
// you do not need this
#define ARTIFICIAL_STRESS 0

// standard SPH alpha/beta viscosity
#define ARTIFICIAL_VISCOSITY 1
// Balsara switch: lowers the artificial viscosity in regions without shocks
#define BALSARA_SWITCH 1

// INVISCID SPH (see Cullen & Dehnen paper)
#define INVISCID_SPH 0

// consistency switches
// for zeroth order consistency, careful if you integrate the continuity equation (INTEGRATE_DENSITY=1)
#define SHEPARD_CORRECTION 0
// for linear consistency
// add tensorial correction tensor to dSdt calculation -> better conservation of angular momentum
#define TENSORIAL_CORRECTION 1


// Available plastic flow conditions:
// (if you do not know what this is, choose (1) or nothing)

//   (1) Simple von Mises plasticity with a constant yield strength
#define VON_MISES_PLASTICITY 0

//   (2) Drucker-Prager yield criterion
//       -> yield strength is given by the condition \sqrt(J_2) + A * I_1 + B = 0
//          I_1: first invariant of stress tensor
//          J_2: second invariant of stress tensor
//          A, B: Drucker-Prager constants, which are calculated from angle of internal friction and cohesion
//       -> intended for granular-like materials, therefore the yield strength decreases to zero for p < 0
//       -> you can additionally use (1) to set an upper limit for the yield stress
//       -> negative pressures can get arbitrarily large (i.e., no negative-pressure cap)
#define DRUCKER_PRAGER_PLASTICITY 0

//   (3) Mohr-Coulomb yield criterion
//       -> for p > 0: yield strength = tan(friction_angle) \times pressure + cohesion
//       -> intended for granular-like materials, therefore the yield strength decreases to zero for p < 0:
//          yield strength = pressure + cohesion (i.e., slope = 1)
//       -> you can additionally use (1) to set an upper limit for the yield stress
//       -> negative-pressure cap: negative pressures are limited to zero of yield strength curve (at -cohesion)
#define MOHR_COULOMB_PLASTICITY 0

//   (4) Pressure dependent yield strength following Collins et al. (2004) and the implementation in Jutzi (2015)
//       -> yield strength is different for damaged (Y_d) and intact material (Y_i), and averaged mean (Y) in between:
//              P > 0: Y_i = cohesion + \mu P / (1 + \mu P / (yield_stress - cohesion) )
//              P < 0: Y_i = cohesion
//                  *cohesion* is the yield strength for P = 0 and *yield_stress* the asymptotic limit for P = \infty
//                  \mu is the coefficient of internal friction (= tan(friction_angle))
//              P > 0: Y_d = cohesion_damaged + \mu_d \times P
//              P < 0: Y_d = cohesion_damaged + P (i.e., slope = 1)
//                  where \mu_d is the coefficient of friction of the *damaged* material
//              combined/final yield strength:  Y = (1-damage)*Y_i + damage*Y_d
//                                              Y is limited to <= Y_i
//              negative-pressure release by damage:
//                  foremost via (1-damage), in line with the Grady-Kipp model (FRAGMENTATION), but only up to the
//                  residual tensile strength the material retains even when fully damaged, assumed to be -y_0_d
//       Note: - If FRAGMENTATION (damage model) is not activated only Y_i is used.
//             - DAMAGE_ACTS_ON_S is not reasonable for this model, since the limiting of S already depends on damage.
//       If you want to additionally model the influence of some (single) melt energy on the yield strength, then activate
//       COLLINS_PLASTICITY_INCLUDE_MELT_ENERGY, which adds a factor (1-e/e_melt) to the yield strength.
#define COLLINS_PLASTICITY 1
#define COLLINS_PLASTICITY_INCLUDE_MELT_ENERGY 1

//   (5) Simplified version of COLLINS_PLASTICITY, which uses only the Lundborg strength representation (Y_i above).
//       For more detailed modeling including crack growth (FRAGMENTATION) use the regular COLLINS_PLASTICITY above.
//       Unlike in (4), Y decreases to zero for p < 0 (with slope = 1, i.e., zero at -cohesion).
//       In addition, a negative-pressure cap limits negative pressures to the zero of the yield strength curve (at -cohesion).
#define COLLINS_PLASTICITY_SIMPLE 0

// Additional strength reduction for low-density states (below the reference density). For most plasticity models this
// is done by reducing the cohesion, and by that the whole yield envelope. For COLLINS_PLASTICITY only the damaged
// cohesion is reduced, vor VON_MISES_PLASTICITY all the (constant) yield strength is reduced.
// Strength reduction increases with decreasing density, where the functional form is set by several parameters in
// the material config file (see there). Works for all plasticity models above (not the experimental ones below).
#define LOW_DENSITY_WEAKENING 0

// model regolith as viscous fluid (warning: experimental)
#define VISCOUS_REGOLITH 0
// use Bui model for regolith (warning: experimental)
#define PURE_REGOLITH 0
// use Johnson-Cook plasticity model (warning: experimental)
#define JC_PLASTICITY 0

// Porosity models:
// p-alpha model implemented following Jutzi (200x); if in doubt activate both of the following options
#define PALPHA_POROSITY 0         // pressure depends on distention
#define STRESS_PALPHA_POROSITY 0  // deviatoric stress is also affected by distention
// Sirono model modified by Geretshauser (2009/10)
#define SIRONO_POROSITY 0
// eps-alpha model implemented following Wuennemann
#define EPSALPHA_POROSITY 0

// max number of activation thresholds per particle, only required for FRAGMENTATION, otherwise set to 1
#define MAX_NUM_FLAWS 1
// maximum number of interactions per particle -> fixed array size
#define MAX_NUM_INTERACTIONS 512

// if VARIABLE_SML is set, the smoothing length (sml) is not fixed in time - choose either:
//   FIXED_NOI for a fixed number of interaction partners, following the ansatz by Hernquist & Katz (1989)
//   or
//   INTEGRATE_SML if you want to additionally integrate an ODE for the sml, following the ansatz by Benz:
//                 d sml / dt  = sml/DIM * 1/rho  \nabla velocity
#define VARIABLE_SML 1
#define FIXED_NOI 0
#define INTEGRATE_SML 1
// read sml for each particle from input file (instead of using a single, material-specific one from material.cfg)
// (if VARIABLE_SML is not set the individual smls remain constant)
#define READ_INITIAL_SML_FROM_PARTICLE_FILE 0

// correction terms for sml calculation (warning: experimental)
// adds gradient of the smoothing length to continuity equation, equation of motion, energy equation
#define SML_CORRECTION 0

// if set to 0, h = (h_i + h_j)/2  is used to calculate W_ij
// if set to 1, W_ij = ( W(h_i) + W(h_j) ) / 2
#define AVERAGE_KERNELS 0

// important switch: if the simulations yields at some point too many interactions for
// one particle (given by MAX_NUM_INTERACTIONS), then its smoothing length will be set to 0
// and the simulation continues. It will be announced on *stdout* when this happens
// if set to 0, the simulation stops in such a case unless DEAL_WITH_TOO_MANY_INTERACTIONS is used
#define TOO_MANY_INTERACTIONS_KILL_PARTICLE 0
// important switch: if the simulations yields at some point too many interactions for
// one particle (given by MAX_NUM_INTERACTIONS), then its smoothing length will be lowered until
// the interactions are lower than MAX_NUM_INTERACTIONS
#define DEAL_WITH_TOO_MANY_INTERACTIONS 0

// additional smoothing of the velocity field
// hinders particle penetration (see Morris & Monaghan, 1984)
#define XSPH 0

// boundary conditions (warning: experimental)
// note: see additionally boundaries.cu with functions beforeRHS and afterRHS for boundary conditions
#define BOUNDARY_PARTICLE_ID -1
#define GHOST_BOUNDARIES 0

// IO options
#define HDF5IO 1    // use HDF5 (needs libhdf5-dev and libhdf5)
#define MORE_OUTPUT 1   //produce additional output to HDF5 files: p_max, p_min, rho_max, rho_min
#define MORE_ANEOS_OUTPUT 1 // produce additional output to HDF5 files: T, cs, entropy, phase-flag; set only if you use the ANEOS EoS; currently not supported for porosity + ANEOS
#define OUTPUT_GRAV_ENERGY 0    // compute and output gravitational energy (at times when output files are written); of all SPH particles (and also w.r.t. gravitating point masses and between them); direct particle-particle summation, not tree; option exists to control costly computation for high particle numbers
#define BINARY_INFO 0   // generates additional output file (binary_system.log) with info regarding binary system: semi-major axis, eccentricity if GRAVITATING_POINT_MASSES == 1

#endif
