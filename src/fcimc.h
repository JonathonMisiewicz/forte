/*
 *@BEGIN LICENSE
 *
 * Libadaptive: an ab initio quantum chemistry software package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *@END LICENSE
 */

#ifndef _fcimc_h_
#define _fcimc_h_

#include <fstream>

#include <liboptions/liboptions.h>
#include <libmints/vector.h>
#include <libmints/matrix.h>
#include <libmints/wavefunction.h>
#include <random>


#include "integrals.h"
#include "bitset_determinant.h"

namespace psi{ namespace forte{

enum SpawnType {random, all, ground_and_random};

typedef std::map<BitsetDeterminant,double> walker_map;

struct ObtCount {
    std::vector<int> naocc, nbocc, navir, nbvir;
};

class FCIQMC : public Wavefunction
{
public:
    // ==> Class Constructor and Destructor <==

    /**
     * Constructor
     * @param wfn The main wavefunction object
     * @param options The main options object
     * @param ints A pointer to an allocated integral object
     */
    FCIQMC(boost::shared_ptr<Wavefunction> wfn, Options &options, ForteIntegrals* ints);

    /// Destructor
    ~FCIQMC();

    // ==> Class Interface <==

    /// Compute the energy
    double compute_energy();

private:
//    /// The wave function symmetry
//    int wavefunction_symmetry_;
//    /// The symmetry of each orbital in Pitzer ordering
//    std::vector<int> mo_symmetry_;
//    /// The symmetry of each orbital in the qt ordering
//    std::vector<int> mo_symmetry_qt_;
//    /// A vector that contains all the frozen core
//    std::vector<int> frzc_;
//    /// A vector that contains all the frozen virtual
//    std::vector<int> frzv_;
//    /// The nuclear repulsion energy
//    double nuclear_repulsion_energy_;

//    /// The determinant with minimum energy
//    StringDeterminant min_energy_determinant_;
//    int compute_pgen(BitsetDeterminant& detI);
    /// The reference determinant
    BitsetDeterminant reference_;
    /// The maximum number of threads
    int num_threads_;
    /// Do we have OpenMP?
    static bool have_omp_;
    /// The molecular integrals required by fcimc
    ForteIntegrals* ints_;

    /// The wave function symmetry
    int wavefunction_symmetry_;
    /// The symmetry of each orbital in Pitzer ordering
    std::vector<int> mo_symmetry_;
    /// The number of correlated molecular orbitals
    int ncmo_;
    /// The number of correlated molecular orbitals per irrep
    Dimension ncmopi_;
    /// The cumulative number of correlated molecular orbitals per irrep
    std::vector<int> cume_ncmopi_;
    /// cume number of irrep combination catagories per alpha beta combination: ab->ab, aiai->ajaj, aiaj->akal, bibi->bjbj, bibj->bkbl, a->a, b->b
    size_t cume_excit_irrep_[7];
    /// Nuclear repulsion energy
    double nuclear_repulsion_energy_;
    /// number of excitations by category
    size_t nsa_,nsb_,ndaa_,ndab_,ndbb_;
    size_t sumgen_;
    size_t cume_sumgen_[5];

    // * Calculation info
    /// spawn type
    SpawnType spawn_type_;
    /// The size of the time step (TAU)
    double time_step_;
    /// The maximum number of FCIQMC steps
    size_t maxiter_;
    /// HartreeForkEnergy
    double Ehf_;
    /// Projectional energy
    double Eproj_, AvgEproj_, ErrEproj_;
    /// Variational energy
    double Evar_;
    /// Start Number of walkers
    double start_num_walkers_;
    /// The shift of energy
    double shift_, AvgShift_, ErrShift_;
    /// Number of walkers
    double nWalkers_;
    /// Number of determinants
    double nDets_;
    /// Shift the Hamiltonian?
    bool do_shift_;
    double shift_num_walkers_;
    int shift_freq_;
    double shift_damp_;
    /// Clone/Death only parents?
    bool death_parent_only_;
    /// Initiator
    bool use_initiator_;
    double initiator_na_;
    /// The frequency of approximate variational estimation of the energy
    int energy_estimate_freq_;
    /// The frequency of print information
    int print_freq_;
    /// number of iterations
    size_t iter_;

    void startup();
    void print_info();

    // adjust shift
    void adjust_shift(double pre_nWalker, size_t pre_iter);

    // Spawning step
    void spawn(walker_map& walkers, walker_map& new_walkers);
    void spawn_generative(walker_map& walkers,walker_map& new_walkers);
    void singleWalkerSpawn(BitsetDeterminant & new_det, const BitsetDeterminant &det, std::tuple<size_t,size_t,size_t,size_t,size_t> pgen, size_t sumgen);
    // Death/Clone step
    void death_clone(walker_map& walkers, double shift);
    void detClone(walker_map& walkers, const BitsetDeterminant& det, double coef, double pDeathClone);
    void detDeath(walker_map& walkers, const BitsetDeterminant& det, double coef, double pDeathClone);
    // Merge step
    void merge(walker_map& walkers,walker_map& new_walkers);
    // Annihilation step
    void annihilate(walker_map& walkers, walker_map& new_walkers);

    // Count the number of allowed single and double excitations
    std::tuple<size_t,size_t,size_t,size_t,size_t> compute_pgen(const BitsetDeterminant &det);
    std::tuple<size_t,size_t,size_t,size_t,size_t> compute_pgen_C1(const BitsetDeterminant &det);
    void compute_excitations(const BitsetDeterminant &det, std::vector<std::tuple<size_t, size_t> > &singleExcitations, std::vector<std::tuple<size_t,size_t,size_t,size_t>>& doubleExcitations);
    void compute_single_excitations(const BitsetDeterminant &det, std::vector<std::tuple<size_t,size_t>>& singleExcitations);
    void compute_double_excitations(const BitsetDeterminant &det, std::vector<std::tuple<size_t,size_t,size_t,size_t>>& doubleExcitations);
    size_t compute_irrep_divided_excitations(const BitsetDeterminant &det, std::vector<size_t> &excitationDivides, std::vector<std::tuple<int, int, int, int> > &excitationType, ObtCount &obtCount);
    bool detSingleRandomExcitation(BitsetDeterminant &new_det, const std::vector<int> &occ, const std::vector<int> &vir, bool isAlpha);
    void detSingleExcitation(BitsetDeterminant &new_det, std::tuple<size_t,size_t>& rand_ext);
    void detDoubleExcitation(BitsetDeterminant &new_det, std::tuple<size_t,size_t,size_t,size_t>& rand_ext);
    bool detDoubleSoloSpinRandomExcitation(BitsetDeterminant &new_det, const std::vector<int> &occ, const std::vector<int> &vir, bool isAlpha);
    bool detDoubleMixSpinRandomExcitation(BitsetDeterminant &new_det, const std::vector<int> &aocc, const std::vector<int> &bocc, const std::vector<int> &avir, const std::vector<int> &bvir);
    void detExcitation(BitsetDeterminant &new_det, size_t rand_ext,  std::vector<size_t> &excitationDivides, std::vector<std::tuple<int, int, int, int> > &excitationType, ObtCount &obtCount);
    double count_walkers(walker_map& walkers);
    double compute_proj_energy(BitsetDeterminant& ref, walker_map& walkers);
    double compute_var_energy(walker_map& walkers);
    void print_iter_info(size_t iter);
    void compute_avg_Eproj(std::vector<double> Eprojs);
    void compute_err_Eproj(std::vector<double> Eprojs);
    void compute_avg_shift(std::vector<double> shifts);
    void compute_err_shift(std::vector<double> shifts);
};

}} // End Namespaces

#endif // _fcimc_h_