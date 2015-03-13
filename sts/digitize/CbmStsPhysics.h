/** @file CbmStsPhysics.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 2.12.2014
 **/

#ifndef CBMSTSPHYSICS_H
#define CBMSTSPHYSICS_H 1


#include "Rtypes.h"
#include <iostream>
#include <map>


using std::map;

class CbmStsPhysics {
	public:
		virtual ~CbmStsPhysics();

		static Double_t GetSiCharge() { return fgkSiCharge; }

		static Double_t EnergyLoss(Double_t dz, Double_t mass,
				                       Double_t eKin, Double_t dedx);


		/** Singleton instance **/
		static CbmStsPhysics* Instance();


		/** Stopping power in Silicon
		 ** @param energy      Energy of particle [GeV]
		 ** @param mass        Particle mass [GeV]
		 ** @param charge      Electric charge [e]
		 ** @param isElectron  kTRUE if electron, kFALSE else
		 ** @value             Stopping power [GeV/cm]
		 **
		 ** This function calculates the stopping power
		 ** (average specific energy loss) in Silicon of a particle
		 ** with given mass and charge.
		 **/
		Double_t StoppingPower(Double_t energy, Double_t mass,
				                   Double_t charge, Bool_t isElectron);



	private:

		/** Constructor **/
		CbmStsPhysics();

		/** Copy constructor (not implemented to avoid being executed) **/
    CbmStsPhysics(const CbmStsPhysics&);

    /** Assignment operator (not implemented to avoid being executed) **/
    CbmStsPhysics operator=(const CbmStsPhysics&);

    /** Pointer to singleton instance **/
		static CbmStsPhysics* fgInstance;


		// --- Parameters for energy loss calculation
		static const Double_t fgkSiCharge;  ///< silicon charge
		static const Double_t fgkR;
		static const Double_t fgkEmax;


		// --- Derived parameters
		static const Double_t fgI;
		static const Double_t fgF1;
		static const Double_t fgF2;
		static const Double_t fgE1;
		static const Double_t fgE2;


		// --- Data tables for stopping power
		map<Double_t, Double_t> fStoppingElectron;  ///< E [GeV] -> <-dE/dx> [GeV*g/cm^2]
		map<Double_t, Double_t> fStoppingProton  ;  ///< E [GeV] -> <-dE/dx> [GeV*g/cm^2]


		// --- Read data tables from files
		void ReadDataTables();

};

#endif /* CBMSTSPHYSICS_H_ */
