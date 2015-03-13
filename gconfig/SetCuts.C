
/** Configuration macro for setting common cuts and processes for both G3 and G4 (M. Al-Turany 14.11.2007)
   specific cuts and processes to g3 or g4 should be set in the g3Config.C or g4Config.C
*/

void SetCuts()
{
  
  gMC->SetProcess("PAIR",1); /** pair production*/
  gMC->SetProcess("COMP",1); /**Compton scattering*/
  gMC->SetProcess("PHOT",1); /** photo electric effect */
  gMC->SetProcess("PFIS",0); /**photofission*/
  gMC->SetProcess("DRAY",1); /**delta-ray*/
  gMC->SetProcess("ANNI",1); /**annihilation*/
  gMC->SetProcess("BREM",1); /**bremsstrahlung*/
  gMC->SetProcess("HADR",1); /**hadronic process*/
  gMC->SetProcess("MUNU",1); /**muon nuclear interaction*/
  gMC->SetProcess("DCAY",1); /**decay*/
  gMC->SetProcess("LOSS",1); /**energy loss*/
  gMC->SetProcess("MULS",1); /**multiple scattering*/
  Double_t cut1=1.0E-3; //GeV
  Double_t tofmax = 1.0; //seconds

  gMC->SetCut("CUTGAM",cut1);   /** gammas (GeV)*/
  gMC->SetCut("CUTELE",cut1);   /** electrons (GeV)*/
  gMC->SetCut("CUTNEU",cut1);   /** neutral hadrons (GeV)*/
  gMC->SetCut("CUTHAD",cut1);   /**charged hadrons (GeV)*/
  gMC->SetCut("CUTMUO",cut1);   /** muons (GeV)*/
  gMC->SetCut("BCUTE",cut1);    /**electron bremsstrahlung (GeV)*/
  gMC->SetCut("BCUTM",cut1);    /**muon and hadron bremsstrahlung(GeV)*/ 
  gMC->SetCut("DCUTE",cut1);    /**delta-rays by electrons (GeV)*/
  gMC->SetCut("DCUTM",cut1);    /**delta-rays by muons (GeV)*/
  gMC->SetCut("PPCUTM",cut1);   /**direct pair production by muons (GeV)*/
  gMC->SetCut("TOFMAX",tofmax); /**time of flight cut in seconds*/

}
