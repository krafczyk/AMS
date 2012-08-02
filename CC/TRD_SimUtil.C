#include "TRD_SimUtil.h"

TrdSimUtil trdSimUtil;
#pragma omp threadprivate(trdSimUtil)


void TrdSimUtil::DefineTubeGas( void ) {
  if(trdTubeGasMaterial)delete trdTubeGasMaterial;
  if(TRDMCFFKEY.debug)printf("Enter TrdSimUtil::DefineTubeGas\n");

  G4String symbol;             //a=mass of a mole;
  G4double a, z, density;      //z=mean number of protons;
  G4int ncomponents, natoms;
  G4double temperature, pressure;
  const G4double R = 8.314472673 * joule / mole / kelvin;

  G4Element* C  = new G4Element("Carbon"  ,symbol="C" , z= 6., a= 12.01*g/mole);
  G4Element* O  = new G4Element("Oxygen"  ,symbol="O" , z= 8., a= 16.00*g/mole);
  G4Element* Xe = new G4Element("Xenon"   ,symbol="Xe", z=54., a= 131.29*g/mole);
  G4Element* Ar = new G4Element("Argon",   symbol="Ar", z=18, a=  39.95*g/mole);

  // C02 gas
  G4double molarMassCO2 = C->GetA() + 2.*O->GetA();
  density = trdGasPressure * molarMassCO2 / R / trdGasTemperature;
  G4Material* CO2 =
    new G4Material("CO2", density, ncomponents=2, kStateGas, temperature=trdGasTemperature , pressure=trdGasPressure );
  CO2->AddElement( C , natoms=1 );
  CO2->AddElement( O , natoms=2 );

  // Xe gas
  G4double molarMassXe = Xe->GetA();
  density = trdGasPressure * molarMassXe / R / trdGasTemperature;
  G4Material* XeGas =
    new G4Material("XenonGas", density, ncomponents=1, kStateGas, temperature=trdGasTemperature , pressure=trdGasPressure );
  XeGas->AddElement( Xe, natoms=1 );

  // Ar gas
  G4double molarMassAr = Ar->GetA();
  density = trdGasPressure * molarMassAr / R / trdGasTemperature;
  G4Material* ArGas =
    new G4Material("ArgonGas", density, ncomponents=1, kStateGas, temperature=trdGasTemperature , pressure=trdGasPressure );
  ArGas->AddElement( Ar, natoms=1 );

  // Xenon/CO2 gas 80:20
  G4double numberFractionXe  = fTrdNumberFractionXe;
  G4double numberFractionCO2 = fTrdNumberFractionCO2;
  G4double numberFractionAr = fTrdNumberFractionAr;

  G4double massFractionXe  = numberFractionXe*XeGas->GetDensity() / ( numberFractionXe*XeGas->GetDensity() + numberFractionCO2*CO2->GetDensity() + numberFractionAr*ArGas->GetDensity());
  G4double massFractionAr  = numberFractionAr*ArGas->GetDensity() / ( numberFractionXe*XeGas->GetDensity() + numberFractionCO2*CO2->GetDensity() + numberFractionAr*ArGas->GetDensity());
  G4double massFractionCO2  = numberFractionCO2*CO2->GetDensity() / ( numberFractionXe*XeGas->GetDensity() + numberFractionCO2*CO2->GetDensity() + numberFractionAr*ArGas->GetDensity());

  density = numberFractionXe * XeGas->GetDensity() + numberFractionCO2 * CO2->GetDensity() + numberFractionAr * ArGas->GetDensity();
  
  std::stringstream trdGasName;
  trdGasName << "Xe/Ar/CO2 " << numberFractionXe*100 << ":" << numberFractionAr*100 << ":" << numberFractionCO2*100;
  if(TRDMCFFKEY.debug)G4cout << "gas info: " << density /g*cm3 << " " << trdGasTemperature << " " << trdGasPressure/bar << G4endl;
  if(TRDMCFFKEY.debug)G4cout << "composition: " << trdGasName.str().c_str() << G4endl;
  
  
  trdTubeGasMaterial =
    new G4Material( "TrdGasUtil", density, ncomponents=3, kStateGas, temperature=trdGasTemperature , pressure=trdGasPressure );
  trdTubeGasMaterial->AddMaterial( XeGas, massFractionXe );
  trdTubeGasMaterial->AddMaterial( ArGas, massFractionAr );
  trdTubeGasMaterial->AddMaterial( CO2,   massFractionCO2 );

  if(TRDMCFFKEY.debug)printf("Exit TrdSimUtil::DefineTubeGas\n");
}

void TrdSimUtil::DefineRadiator( void ) {
  if(TRDMCFFKEY.debug)printf("Enter TrdSimUtil::DefineRadiator\n");
  G4String symbol;             //a=mass of a mole;
  G4double a, z, density;      //z=mean number of protons;
  G4int ncomponents, natoms;

  if(trdFleeceMaterial)delete trdFleeceMaterial;
  

  G4Element* H  = new G4Element("Hydrogen",symbol="H" , z= 1., a= 1.01*g/mole);
  G4Element* C  = new G4Element("Carbon"  ,symbol="C" , z= 6., a= 12.01*g/mole);
  
  // Polyethylene
  G4Material* Polyethylene = new G4Material("Polyethylene", density = 0.92*g/cm3, ncomponents=2);
  Polyethylene->AddElement(C, natoms=2);
  Polyethylene->AddElement(H, natoms=4);

  // Polypropylene
  G4Material* Polypropylene = new G4Material("Polypropylene", density = 0.85*g/cm3, ncomponents=2);
  Polypropylene->AddElement(C, natoms=3);
  Polypropylene->AddElement(H, natoms=6);

  // Radiator
  G4double numberFractionPE = 0.85;
  G4double numberFractionPP = 0.15;
  G4double massFractionPE = numberFractionPE*Polyethylene->GetDensity()  / ( numberFractionPE*Polyethylene->GetDensity() + numberFractionPP*Polypropylene->GetDensity() );
  G4double massFractionPP = numberFractionPP*Polypropylene->GetDensity() / ( numberFractionPE*Polyethylene->GetDensity() + numberFractionPP*Polypropylene->GetDensity() );
  trdMeanRadiatorFibreDensity = numberFractionPE*Polyethylene->GetDensity() + numberFractionPP*Polypropylene->GetDensity();
  
  trdFleeceMaterial= new G4Material("TrdRadUtil", density=trdMeanRadiatorFibreDensity, ncomponents=2);
  trdFleeceMaterial->AddMaterial( Polyethylene,  massFractionPE );
  trdFleeceMaterial->AddMaterial( Polypropylene, massFractionPP );

  G4Material *LRP375BK = new G4Material("LRP 375 BK", density=0.06*g/cm3, ncomponents=2);
  LRP375BK->AddMaterial( Polyethylene,  massFractionPE );
  LRP375BK->AddMaterial( Polypropylene, massFractionPP );
  
  trdMeanGasGap = trdFleeceFiberDiameter * ( trdMeanRadiatorFibreDensity/LRP375BK->GetDensity() - 1.0 );

  if(TRDMCFFKEY.debug)G4cout<<"mean gas gap in um:  "<<trdMeanGasGap/um<<G4endl;

  if(TRDMCFFKEY.debug)G4cout<<"foil thickness um:  "<<trdFleeceFiberDiameter/um<<G4endl;

  trdMeanFoilNumber  = LRP375BK->GetDensity() / trdMeanRadiatorFibreDensity * trdRadiatorThickness  / trdFleeceFiberDiameter;

  if(TRDMCFFKEY.debug)G4cout<<"foil number: "<< trdMeanFoilNumber<<G4endl; 

  // artificial radiator material (including gas)
  trdRadiatorArtificial = 0;
  G4double fibreGasRatio = trdRadiatorThickness/(trdRadiatorThickness+trdMeanGasGap);
  
  if(trdFleeceGasConf==0){
    G4Element* N  = new G4Element("Nitrogen",symbol="N" , z= 7., a= 14.01*g/mole);
    G4Element* O  = new G4Element("Oxygen"  ,symbol="O" , z= 8., a= 16.00*g/mole);
    
    trdFleeceGasMaterial= new G4Material("Air"  , density= 1.290*mg/cm3, ncomponents=2,kStateGas, trdGasTemperature);
    trdFleeceGasMaterial->AddElement(N, 0.7);
    trdFleeceGasMaterial->AddElement(O, 0.3);
    
    G4double totRadDensity = LRP375BK->GetDensity() + trdFleeceGasMaterial->GetDensity()*(1.0-fibreGasRatio);
    G4double fractionFibre = LRP375BK->GetDensity()/totRadDensity;
    G4double fractionGas   = trdFleeceGasMaterial->GetDensity()*(1.0-fibreGasRatio)/totRadDensity;
    
    trdRadiatorArtificial = new G4Material("LRP 375 BK + Air",totRadDensity,ncomponents = 2 );
    trdRadiatorArtificial->AddMaterial( LRP375BK, fractionFibre );
    trdRadiatorArtificial->AddMaterial( trdFleeceGasMaterial,      fractionGas  );
  }
  else if (trdFleeceGasConf==1){
    trdFleeceGasMaterial = new G4Material("Galactic", z=1., a=1.01*g/mole,density=universe_mean_density,kStateGas, 3.e-18*pascal, trdGasTemperature);

    G4double totRadDensity = LRP375BK->GetDensity() + trdFleeceGasMaterial->GetDensity()*(1.0-fibreGasRatio);
    G4double fractionFibre = LRP375BK->GetDensity()/totRadDensity;
    G4double fractionGas   = trdFleeceGasMaterial->GetDensity()*(1.0-fibreGasRatio)/totRadDensity;

    trdRadiatorArtificial = new G4Material("LRP 375 BK + Vacuum",totRadDensity,ncomponents = 2 );
    trdRadiatorArtificial->AddMaterial( LRP375BK, fractionFibre );
    trdRadiatorArtificial->AddMaterial( trdFleeceGasMaterial,      fractionGas  );
  }
  if(TRDMCFFKEY.debug)printf("Exit TrdSimUtil::DefineRadiator\n");
}

AMSgmat *TrdSimUtil::GetTubeGasMaterial( void ){
  if(TRDMCFFKEY.debug)printf("Enter TrdSimUtil::GetTubeGasMaterial\n");
  const int n=trdTubeGasMaterial->GetElementVector()->size();

  geant a[n],z[n],w[n];
  for(int i=0;i!=n;i++){
    a[i]=trdTubeGasMaterial->GetElementVector()->at(i)->GetA()/g*mole;
    z[i]=trdTubeGasMaterial->GetElementVector()->at(i)->GetZ();
    w[i]=trdTubeGasMaterial->GetFractionVector()[i]*1.e8;
    if(TRDMCFFKEY.debug)printf("comp %s - A %.2f Z %.2f W %.2f\n",trdTubeGasMaterial->GetElementVector()->at(i)->GetSymbol().data(),a[i],z[i],w[i]);
  }

  geant density=trdTubeGasMaterial->GetDensity();
  geant temp=   trdTubeGasMaterial->GetTemperature();
  geant radl=   trdTubeGasMaterial->GetRadlen();
  geant absl=   trdTubeGasMaterial->GetNuclearInterLength();

  AMSgmat *thismat=new AMSgmat ("TrdGasGmat", a, z, w, n, density/g*cm3,radl/m,absl/m,temp/kelvin);

  if(TRDMCFFKEY.debug)printf("Exit TrdSimUtil::GetTubeGasMaterial\n");
  return thismat;
}

AMSgmat *TrdSimUtil::GetRadiatorArtificialMaterial( void ){
  if(TRDMCFFKEY.debug)printf("Enter TrdSimUtil::GetRadiatorArtificialMaterial\n");
  const int n=trdRadiatorArtificial->GetElementVector()->size();

  geant a[n],z[n],w[n];
  for(int i=0;i!=n;i++){
    a[i]=trdRadiatorArtificial->GetElementVector()->at(i)->GetA()/g*mole;
    z[i]=trdRadiatorArtificial->GetElementVector()->at(i)->GetZ();
    w[i]=trdRadiatorArtificial->GetFractionVector()[i]*4.e8;
    if(TRDMCFFKEY.debug)printf("comp %s - A %.2f Z %.2f W %.2f\n",trdRadiatorArtificial->GetElementVector()->at(i)->GetSymbol().data(),a[i],z[i],w[i]);
  }

  geant density=trdRadiatorArtificial->GetDensity();
  geant temp=   trdRadiatorArtificial->GetTemperature();
  geant radl=   trdRadiatorArtificial->GetRadlen();
  geant absl=   trdRadiatorArtificial->GetNuclearInterLength();

  AMSgmat *thismat=new AMSgmat ("TrdArtRadGmat", a, z, w, n, density/g*cm3,radl/m,absl/m,temp/kelvin);

  if(TRDMCFFKEY.debug)printf("Exit TrdSimUtil::GetRadiatorArtificialMaterial\n");
  return thismat;
}

AMSgmat *TrdSimUtil::GetKaptonMaterial( void ){
  if(TRDMCFFKEY.debug)printf("Enter TrdSimUtil::GetRadiatorArtificialMaterial\n");

  G4Element* H  = new G4Element("Hydrogen","H" , 1., 1.01*g/mole);
  G4Element* C  = new G4Element("Carbon"  ,"C" , 6., 12.01*g/mole);
  G4Element* O  = new G4Element("Oxygen"  ,"O" , 8., 16.00*g/mole);
  G4Element* N  = new G4Element("Nitrogen","N" , 7., 14.01*g/mole);

  trdKaptonMaterial  = new G4Material("Kapton", 1.42*g/cm3, 4);
  trdKaptonMaterial->AddElement(C, 22);
  trdKaptonMaterial->AddElement(H, 10);
  trdKaptonMaterial->AddElement(N, 2);
  trdKaptonMaterial->AddElement(O, 5);
  
  const int n=trdKaptonMaterial->GetElementVector()->size();
  geant a[n],z[n],w[n];
  for(int i=0;i!=n;i++){
    a[i]=trdKaptonMaterial->GetElementVector()->at(i)->GetA()/g*mole;
    z[i]=trdKaptonMaterial->GetElementVector()->at(i)->GetZ();
    w[i]=trdKaptonMaterial->GetFractionVector()[i]*4.e8;
    if(TRDMCFFKEY.debug)printf("comp %s - A %.2f Z %.2f W %.2f\n",trdKaptonMaterial->GetElementVector()->at(i)->GetSymbol().data(),a[i],z[i],w[i]);
  }

  geant density=trdKaptonMaterial->GetDensity();
  geant temp=   trdKaptonMaterial->GetTemperature();
  geant radl=   trdKaptonMaterial->GetRadlen();
  geant absl=   trdKaptonMaterial->GetNuclearInterLength();

  //  delete H,C,N,O;
  AMSgmat *thismat=new AMSgmat ("Kapton", a, z, w, n, density/g*cm3,radl/m,absl/m,temp/kelvin);
  
  if(TRDMCFFKEY.debug)printf("Exit TrdSimUtil::GetKaptonMaterial\n");
  return thismat;
}

void TrdSimUtil::UpdateGas ( void ) {
  G4double cut = TRDMCFFKEY.GasStep*mm;
  trdGasTemperature=TRDMCFFKEY.Tmean*kelvin;
  trdGasPressure=TRDMCFFKEY.Pmean/1000.*bar;
  trdFleeceFiberDiameter=TRDMCFFKEY.FibreDiam*um;
  trdRadiatorThickness=TRDMCFFKEY.RadThick*mm;
  fAlphaFiber=TRDMCFFKEY.AlphaFibre;
  fAlphaGas=TRDMCFFKEY.AlphaGas;
  trdFleeceGasConf=TRDMCFFKEY.FleeceGas;
  fTrdNumberFractionXe=TRDMCFFKEY.XenonFraction;
  fTrdNumberFractionCO2=TRDMCFFKEY.CO2Fraction;
  fTrdNumberFractionAr=TRDMCFFKEY.ArgonFraction;


  fTrdGasRegionCuts = new G4ProductionCuts();
  fTrdGasRegionCuts->SetProductionCut(cut, idxG4GammaCut);
  fTrdGasRegionCuts->SetProductionCut(cut, idxG4ElectronCut);
  fTrdGasRegionCuts->SetProductionCut(cut, idxG4PositronCut);
  fTrdGasRegionCuts->SetProductionCut(cut, idxG4ProtonCut);
  
  fTrdRadRegionCuts = new G4ProductionCuts();
  fTrdRadRegionCuts->SetProductionCut(1*mm, idxG4GammaCut);
  fTrdRadRegionCuts->SetProductionCut(1*mm, idxG4ElectronCut);
  fTrdRadRegionCuts->SetProductionCut(1*mm, idxG4PositronCut);
  fTrdRadRegionCuts->SetProductionCut(1*mm, idxG4ProtonCut);
  
  //Should not define region before "World" is created, "World" need to be the first instance in G4RegionStore
//   if(!gasregion){
//     gasregion=new G4Region("TrdGasRegion");
//     (G4RegionStore::GetInstance())->Register(gasregion);
//   }
//   if(!radregion){
//     radregion=new G4Region("TrdRadRegion");
//     (G4RegionStore::GetInstance())->Register(radregion);
//   }

  if(TRDMCFFKEY.debug){
    G4cout << "TRDMC datacards :" << G4endl;
    G4cout << "PAIModel " << TRDMCFFKEY.PAIModel << G4endl;
    G4cout << "debug " << TRDMCFFKEY.debug << G4endl;
    G4cout << "GasStep " << TRDMCFFKEY.GasStep << G4endl;

    G4cout << "Radiator Parameters:" << G4endl;
    G4cout << " FibreDiam " << TRDMCFFKEY.FibreDiam << G4endl;
    G4cout << " RadThick " << TRDMCFFKEY.RadThick << G4endl;
    G4cout << " AlphaFibre " <<  TRDMCFFKEY.AlphaFibre << G4endl;
    G4cout << " AlphaGas " << TRDMCFFKEY.AlphaGas << G4endl;
    G4cout << " FleeceGas " << TRDMCFFKEY.FleeceGas << G4endl;
    
    G4cout << "Gas Parameters:"<<G4endl;
    G4cout << " Tmean " << TRDMCFFKEY.Tmean << G4endl;
    G4cout << " Pmean " << TRDMCFFKEY.Pmean << G4endl;
    G4cout << " Xe/Ar/CO2 " << TRDMCFFKEY.XenonFraction << "/" << TRDMCFFKEY.ArgonFraction << "/" << TRDMCFFKEY.CO2Fraction << G4endl;
    
    
  }

}

TrdSimUtil::TrdSimUtil(){
  trdTubeGasMaterial=0;
  trdFleeceMaterial=0;
  trdFleeceGasMaterial=0;
  trdRadiatorArtificial=0;
  trdKaptonMaterial=0;

  trdFleeceFiberDiameter=0.;
  trdRadiatorThickness=0.;
  trdGasTemperature=0.;
  trdGasPressure=0.;
  trdFoilThickness=0.;
  trdMeanFoilNumber=0.;
  trdMeanRadiatorFibreDensity=0.;
  trdMeanGasGap=0.;

  fTrdNumberFractionXe=0.;
  fTrdNumberFractionAr=0.;
  fTrdNumberFractionCO2=0.;

  fAlphaFiber=0.;
  fAlphaGas=0.;
  
  trdFleeceGasConf=0;
  fTrdGasRegionCuts=0;
  fTrdRadRegionCuts=0;
  gasregion=0;
  radregion=0;
  radlv=0;
}

void TrdSimUtil::EndOfRun( void ) {
  (G4RegionStore::GetInstance())->DeRegister(gasregion);
  (G4RegionStore::GetInstance())->DeRegister(radregion);
}
