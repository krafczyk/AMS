#include "TRD_SimUtil.h"

TrdSimUtil trdSimUtil;
#pragma omp threadprivate(trdSimUtil);


void TrdSimUtil::DefineTubeGas( void ) {
  if(trdTubeGasMaterial)delete trdTubeGasMaterial;
  printf("Enter TrdSimUtil::DefineTubeGas\n");

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
  G4cout << "gas info: " << density /g*cm3 << " " << trdGasTemperature << " " << trdGasPressure/bar << G4endl;
  G4cout << "composition: " << trdGasName.str().c_str() << G4endl;
  
  
  trdTubeGasMaterial =
    new G4Material( "TrdGasUtil", density, ncomponents=3, kStateGas, temperature=trdGasTemperature , pressure=trdGasPressure );
  trdTubeGasMaterial->AddMaterial( XeGas, massFractionXe );
  trdTubeGasMaterial->AddMaterial( ArGas, massFractionAr );
  trdTubeGasMaterial->AddMaterial( CO2,   massFractionCO2 );

  printf("Exit TrdSimUtil::DefineTubeGas\n");
}

void TrdSimUtil::DefineRadiator( void ) {
  printf("Enter TrdSimUtil::DefineRadiator\n");
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

  G4cout<<"mean gas gap in um:  "<<trdMeanGasGap/um<<G4endl;

  G4cout<<"foil thickness um:  "<<trdFleeceFiberDiameter/um<<G4endl;

  trdMeanFoilNumber  = LRP375BK->GetDensity() / trdMeanRadiatorFibreDensity * trdRadiatorThickness  / trdFleeceFiberDiameter;

  G4cout<<"foil number: "<< trdMeanFoilNumber<<G4endl; 

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

    G4double totRadDensity = trdFleeceMaterial->GetDensity() + trdFleeceGasMaterial->GetDensity()*(1.0-fibreGasRatio);
    G4double fractionFibre = trdFleeceMaterial->GetDensity()/totRadDensity;
    G4double fractionGas   = trdFleeceGasMaterial->GetDensity()*(1.0-fibreGasRatio)/totRadDensity;

    trdRadiatorArtificial = new G4Material("LRP 375 BK + Vacuum",totRadDensity,ncomponents = 2 );
    trdRadiatorArtificial->AddMaterial( trdFleeceMaterial, fractionFibre );
    trdRadiatorArtificial->AddMaterial( trdFleeceGasMaterial,      fractionGas  );
  }
  printf("Exit TrdSimUtil::DefineRadiator\n");
}

AMSgmat *TrdSimUtil::GetTubeGasMaterial( void ){
  printf("Enter TrdSimUtil::GetTubeGasMaterial\n");
  const int n=trdTubeGasMaterial->GetElementVector()->size();

  geant a[n],z[n],w[n];
  for(int i=0;i!=n;i++){
    a[i]=trdTubeGasMaterial->GetElementVector()->at(i)->GetA()/g*mole;
    z[i]=trdTubeGasMaterial->GetElementVector()->at(i)->GetZ();
    w[i]=trdTubeGasMaterial->GetFractionVector()[i];
    printf("comp %s - A %.2f Z %.2f W %.2f\n",trdTubeGasMaterial->GetElementVector()->at(i)->GetSymbol().data(),a[i],z[i],w[i]);
  }

  geant density=trdTubeGasMaterial->GetDensity();
  geant temp=   trdTubeGasMaterial->GetTemperature();
  geant radl=   trdTubeGasMaterial->GetRadlen();
  geant absl=   trdTubeGasMaterial->GetNuclearInterLength();

  AMSgmat *thismat=new AMSgmat ("TrdGasGmat", a, z, w, n, density/g*cm3,radl/m,absl/m,temp/kelvin);

  printf("Exit TrdSimUtil::GetTubeGasMaterial\n");
  return thismat;
}

AMSgmat *TrdSimUtil::GetRadiatorArtificialMaterial( void ){
  printf("Enter TrdSimUtil::GetRadiatorArtificialMaterial\n");
  const int n=trdRadiatorArtificial->GetElementVector()->size();

  geant a[n],z[n],w[n];
  for(int i=0;i!=n;i++){
    a[i]=trdRadiatorArtificial->GetElementVector()->at(i)->GetA()/g*mole;
    z[i]=trdRadiatorArtificial->GetElementVector()->at(i)->GetZ();
    w[i]=trdRadiatorArtificial->GetFractionVector()[i];
    printf("comp %s - A %.2f Z %.2f W %.2f\n",trdRadiatorArtificial->GetElementVector()->at(i)->GetSymbol().data(),a[i],z[i],w[i]);
  }

  geant density=trdRadiatorArtificial->GetDensity();
  geant temp=   trdRadiatorArtificial->GetTemperature();
  geant radl=   trdRadiatorArtificial->GetRadlen();
  geant absl=   trdRadiatorArtificial->GetNuclearInterLength();

  AMSgmat *thismat=new AMSgmat ("TrdArtRadGmat", a, z, w, n, density/g*cm3,radl/m,absl/m,temp/kelvin);

  printf("Exit TrdSimUtil::GetRadiatorArtificialMaterial\n");
  return thismat;
}

/*TRD_TrdPhysics *TrdSimUtil::DefineGammaXTR( void ){
  G4ProcessManager * pManager = 0;
  
  G4cout<<"Enter TrdSimUtil::DefineGammaXTR"<<G4endl;

  std::stringstream name;
  name << "GammaXTRadiator" ;

  G4cout<<"In TRD_trdPhysics::ConstructProcess(), check A"<<G4endl;
  G4cout<<"alpha fiber "<<GetAlphaFiber()
	<<" alpha gas "<<GetAlphaGas()
	<<" fleece material "<<GetG4FleeceMaterial()->GetName()
	<<" gas material "<< GetG4TubeGasMaterial()->GetName()
	<<" foil thickness "<< GetTrdFoilThickness()
	<<" gas thickness "<<GetTrdGasThickness()
	<< " nfoils "<<GetTrdFoilNumber()<<G4endl;
  
  TRD_VXTenergyLoss *processXTR = new TRD_GammaXTRadiator( "TRDR",
							   GetAlphaFiber(),
							   GetAlphaGas(),
							   GetG4FleeceMaterial(),
							   GetG4TubeGasMaterial(),
							   
							   GetTrdFoilThickness(),
							   GetTrdGasThickness(),
							   GetTrdFoilNumber(),
							   "GammaXTRadiator" );
  
  if( !processXTR ){
    printf("not xtr process\n");
  }
  else{
    processXTR->SetVerboseLevel(2);
    //       processXTR->G4VProcess::SetVerboseLevel(2);
    
    pManager = G4PionPlus::PionPlus()->GetProcessManager();
    pManager->AddDiscreteProcess(processXTR);
    
    pManager = G4PionMinus::PionMinus()->GetProcessManager();
    pManager->AddDiscreteProcess(processXTR);
    
    pManager = G4Electron::Electron()->GetProcessManager();
    pManager->AddDiscreteProcess(processXTR);
    
    pManager = G4Positron::Positron()->GetProcessManager();
    pManager->AddDiscreteProcess(processXTR);
    
    pManager = G4Proton::Proton()->GetProcessManager();
    pManager->AddDiscreteProcess(processXTR);
  }


  return;
}
*/
