

void geometry(){
    TFile *file = new TFile("geofile_full.newgeo4.root");
//    TFile *file = new TFile("geofile_full.rich.prototype.standard.root");
    TGeoManager *geo = (TGeoManager*) file->Get("FAIRGeom");

    TGeoNode *kw = geo->FindNode(0., 0., 0.+0.0125);
    cout << endl << kw->GetName() << "   " << kw->GetMedium()->GetName() << endl;
    const Double_t local[] = {0., 0., 0.};
    Double_t kw_global[3];
    geo->LocalToMaster(local, kw_global);
    cout << "----- Kapton window -----" << endl;
    cout << kw_global[0] << "   "
	<< kw_global[1] << "   "
	<< kw_global[2] << endl;
    cout << "-------------------------" << endl;

    TGeoNode *pd = geo->FindNode(0., 20.4, 20.-0.025);
    cout << endl << pd->GetName() << "   " << pd->GetMedium()->GetName() << endl;
    Double_t pd_global[3];
    geo->LocalToMaster(local, pd_global);
    cout << "----- Photodetector -----" << endl;
    cout << pd_global[0] << "   "
	<< pd_global[1] << "   "
	<< pd_global[2] << endl;
    cout << "-------------------------" << endl;
    cout << (TMath::ASin(pd->GetMatrix()->GetRotationMatrix()[7])*1e3) << endl;
    cout << "-------------------------" << endl;

    geo->CdTop();
    geo->CdDown(2);
    geo->CdDown(0);
    geo->CdDown(0);
    TGeoNode *m1 = geo->GetCurrentNode();
    cout << endl << m1->GetName() << "   " << m1->GetMedium()->GetName() << endl;
    Double_t m1_global[3];
    geo->LocalToMaster(local, m1_global);
    cout << "----- Mirror 1 -----" << endl;
    cout << m1_global[0] << "   "
	<< m1_global[1] << "   "
	<< m1_global[2] << endl;
    cout << "--------------------" << endl;
    const Double_t a1 = TMath::ASin(m1->GetMatrix()->GetRotationMatrix()[8]);
    cout << (a1*1e3) << endl;
    cout << "-------------------------" << endl;

    m1_global[0] *= 10.;
    m1_global[1] *= 10.;
    m1_global[2] *= 10.;
    const Double_t r = 3000. + 6./2.;
    const Double_t da = (0.05+20.)/300.*TMath::RadToDeg();
    const Double_t theta[] = {
	(90.-da)*TMath::DegToRad(),
	(90.-da)*TMath::DegToRad(),
	(90.+da)*TMath::DegToRad(),
	(90.+da)*TMath::DegToRad()
    };
    const Double_t phi[] = {
	(90.+da)*TMath::DegToRad(),
	(90.-da)*TMath::DegToRad(),
	(90.+da)*TMath::DegToRad(),
	(90.-da)*TMath::DegToRad()
    };
    for(Int_t i = 0; i < 4; i++) {
	Double_t x = -r*TMath::Sin(theta[i])*TMath::Cos(phi[i]);
	Double_t z = r*TMath::Sin(theta[i]+a1)*TMath::Sin(phi[i]) + m1_global[2];
	Double_t y = r*TMath::Cos(theta[i]+a1) + m1_global[1];
	cout << x << "   " << y << "   " << z << endl;
    }
}


