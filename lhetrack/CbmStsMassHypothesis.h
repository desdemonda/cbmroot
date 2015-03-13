/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//       CbmStsMassHypothesis class 
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

#ifndef CBM_STS_MASS_HYPOTHSESIS_H
#define CBM_STS_MASS_HYPOTHSESIS_H

class CbmStsMassHypothesis {

public:
  CbmStsMassHypothesis() { }
  virtual ~CbmStsMassHypothesis() { }

  Double_t GetTgAlpha()   const { return fTgAlpha;}
  Double_t GetBeta()      const { return fBeta; }

  Double_t GetProton()    const { return fProton; }
  Double_t GetKaon()      const { return fKaon; }
  Double_t GetPion()      const { return fPion; }
  Double_t GetElectron()  const { return fElectron; }


  void SetAngle (Double_t alpha, Double_t beta);
  void SetMomenta (Double_t prot, Double_t kaon,
		   Double_t pion, Double_t elec);

private:

  Double_t fProton;     //  proton momentum from track fitting
  Double_t fKaon;       //  kaon momentum from track fitting
  Double_t fPion;       //  pion momentum from track fitting
  Double_t fElectron;   //  electron momentum from track fitting

  Double_t fTgAlpha;      //  deep (in YZ plane) angle of track
  Double_t fBeta;       //  bending angle (in XZ plane)

  ClassDef(CbmStsMassHypothesis , 1)    // 

};


inline void CbmStsMassHypothesis::SetMomenta(Double_t prot, Double_t kaon,
					     Double_t pion, Double_t elec) {

  fProton = prot;
  fKaon = kaon;
  fPion = pion;
  fElectron = elec;

}

inline void CbmStsMassHypothesis::SetAngle( Double_t alpha, Double_t beta) {

  fTgAlpha = alpha;
  fBeta = beta;

}

#endif

