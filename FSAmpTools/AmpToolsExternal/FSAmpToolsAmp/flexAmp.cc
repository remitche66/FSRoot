#include <cassert>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cmath>

#include "TMath.h"
#include "IUAmpTools/Kinematics.h"
#include "FSAmpToolsAmp/angDist.h"
#include "FSAmpToolsAmp/flexAmp.h"
#include "FSAmpToolsAmp/fitConfig.h"


flexAmp::flexAmp(const vector<string> &args) :
  UserAmplitude<flexAmp>(args){
m_ryandebug = 0;

	// example:
	// DECAY (((1,2),3),(4,5)) BEAM {x,y,z,e} QNRES jm::m::l_::s_m jr1::lr1::ls1 jr2::lr2::ls2 jr3::lr3::ls3 QNDAUG jd1 md1 jd2 md2 jd3 md3 jd4 md4 jd5 md5


	if(args[0].compare("DECAY")!=0){
		cout << "please declare DECAY first using: DECAY (((1,2),3),(4,5)) etc." << endl;
		exit(0);
	}
	list_of_resonances = fitConfig::decayTree(args[1]);
	if(m_print)
		fitConfig::printDecayTree(list_of_resonances);


	string s_beam;
	int i_qnres=-2500;
	int i_qnd=-2500;
	int i_fac=-2500;
	b_useFactor = false;
	qn_mother_rad = "NORAD";
	for(int i=0;i<10;i++)
		qn_res_rad[i] = "NORAD";
	for(unsigned int i=2;i<args.size();i++){
		if(args[i-1].compare("BEAM")==0){
			s_beam = args[i];
			continue;
		}
		if(args[i].compare("QNRES")==0){
			i_qnres=i;
			continue;
		}
		if(i>i_qnres && i<=i_qnres+3*list_of_resonances.size()+1){
			// assert should make sure one always gives correct number of arguments for each keyword
			assert(args[i].find("Q")==string::npos && args[i].find("B")==string::npos && args[i].find("F")==string::npos);
			if(i<=i_qnres+4){
				qn_mother.push_back(atof(args[i].c_str()));
				if(args[i].compare("E")==0 || args[i].compare("M")==0)
					qn_mother_rad = args[i];
			}
			else{
				qn_res[(i-(i_qnres+5))/3].push_back(atof(args[i].c_str()));
				if(args[i].compare("E")==0 || args[i].compare("M")==0)
					qn_res_rad[(i-(i_qnres+5))/3] = args[i];
			}
			continue;
		}
		if(args[i].compare("QNDAUG")==0){
			i_qnd=i;
			continue;
		}
		if(i>i_qnd && i<=i_qnd+2*list_of_resonances.at(0).n_daug){
			assert(args[i].find("Q")==string::npos && args[i].find("B")==string::npos && args[i].find("F")==string::npos);
			qn_daug[(i-(i_qnd+1))/2].push_back(atof(args[i].c_str()));
			continue;
		}
		if(args[i].compare("FACTOR")==0){
			i_fac=i;
			b_useFactor = true;
			FACTORF fac = {-1,AmpParameter("1.0"),AmpParameter("0.0")};
			extra_factors.push_back(fac);
			continue;
		}
		if(i==i_fac+1){
			extra_factors.at(extra_factors.size()-1).index = atoi(args[i].c_str());
			continue;
		}
		if(i>i_fac+1 && i<=i_fac+3){
			assert(args[i].find("Q")==string::npos && args[i].find("B")==string::npos && args[i].find("F")==string::npos);
			if(i==i_fac+2)
				extra_factors.at(extra_factors.size()-1).real = AmpParameter(args[i]);
			else
				extra_factors.at(extra_factors.size()-1).imag = AmpParameter(args[i]);
			continue;
		}
	}

	// if no daughter spins and helicities were given, assume daughters are spin-0
	if(i_qnd==-1){
		cout << "warning, you did not declare QNDAUG, assuming all daughters are spin-0" << endl;
		for(unsigned int i=0;i<2*list_of_resonances.at(0).n_daug;i++)
			qn_daug[i/2].push_back(0.);
	}

	for(unsigned int i=0;i<list_of_resonances.size();i++){
		if(!i){
			list_of_resonances.at(i).jr = qn_mother.at(0);
			list_of_resonances.at(i).mr = qn_mother.at(1);
			list_of_resonances.at(i).lr = qn_mother.at(2);
			list_of_resonances.at(i).sr = qn_mother.at(3);
			list_of_resonances.at(i).type = qn_mother_rad;
		}
		else{
			list_of_resonances.at(i).jr = qn_res[i-1].at(0);
			list_of_resonances.at(i).lr = qn_res[i-1].at(1);
			list_of_resonances.at(i).sr = qn_res[i-1].at(2);
			list_of_resonances.at(i).mr = -222; // this is looped over later in any case
			list_of_resonances.at(i).type = qn_res_rad[i-1];
		}
	}

	// make some checks:
	assert(qn_mother.size()==4);
	for(unsigned int i=0;i<list_of_resonances.size()-1;i++)
		assert(qn_res[i].size()==3);
	for(unsigned int i=0;i<list_of_resonances.at(0).n_daug;i++)
		assert(qn_daug[i].size()==2);

	// set beam vector
	if(s_beam.size()>0){
		s_beam = s_beam.substr(1,s_beam.size()-2);
		istringstream ss(s_beam);
		string token;
		int count=0;
		double p[4];
		while(getline(ss, token, ',') && count<4) {
			p[count] = atof(token.c_str());
			count++;
		}
		p4Ep = TLorentzVector(p[0],p[1],p[2],p[3]);
	}
	else{
		cout << "warning, no BEAM vector was given, using e+ from BESIII J/psi data" << endl;
    	p4Ep = TLorentzVector(0.017034145545817156, 0, 1.548496226945871, 1.5485899999999999);
	}


	// print some stuff
	if(m_print){
		cout << "beam: " << p4Ep.Px() << " " << p4Ep.Py() << " " << p4Ep.Pz() << " " << p4Ep.E() << endl;
		cout << "qn mother:" << endl;
		for(unsigned int i=0;i<qn_mother.size();i++)
			cout << " " << qn_mother.at(i);
		cout << endl;
		for(unsigned int j=0;j<10 && j<list_of_resonances.size()-1;j++){
			cout << "qn res" << j << endl;
			for(unsigned int i=0;i<qn_res[j].size();i++)
				cout << " " << qn_res[j].at(i);
			cout << endl;
		}
		for(unsigned int j=0;j<20 && j<list_of_resonances.at(0).n_daug;j++){
			cout << "qn daug" << j << endl;
			for(unsigned int i=0;i<qn_daug[j].size();i++)
				cout << " " << qn_daug[j].at(i);
			cout << endl;
		}
		if(b_useFactor){
			for(unsigned int j=0;j<extra_factors.size();j++){
				assert(extra_factors.at(j).index>0 && extra_factors.at(j).index<list_of_resonances.size());
				cout << "extra factor for resonance " << extra_factors.at(j).index << " " << extra_factors.at(j).real << " " << extra_factors.at(j).imag << endl;
			}
		}
	

		for(unsigned int j=0;j<10 && j<list_of_resonances.size()-1;j++){
			cout << "qn res" << j << endl;
			for(unsigned int i=0;i<qn_res[j].size();i++)
				cout << " " << qn_res[j].at(i);
			cout << endl;
		}	
	}


	type = -222;
	if(list_of_resonances.at(0).n_daug==1){
		cout << "amplitude should be at least two-body, aborting..." << endl;
		exit(0);
	}
	else if(list_of_resonances.at(0).n_daug==2)  // this is 2-body, only 1 amplitude
		type = 0;
	else if(list_of_resonances.at(0).n_daug==3)  // this is 3-body, again only 1 amplitude
		type = 1;
	else if(list_of_resonances.at(0).n_daug==4){ // this is 4 body, two different cases
		// case (a) two daughters on each side
		if(list_of_resonances.at(1).n_daug == list_of_resonances.at(2).n_daug)
			type = 2;
		// case (b) three daughters on left side, 1 daughter on right side
		else
			type = 3;
	}
	else if(list_of_resonances.at(0).n_daug==5){
		// two cases: 3 daughters on left side, 2 daughters on right side
		if(list_of_resonances.at(1).n_daug == list_of_resonances.at(2).n_daug+1)
			type = 4;
		if(list_of_resonances.at(1).n_daug == 4 && list_of_resonances.at(2).n_daug == 2)
			type = 5; // this is new
		else if(list_of_resonances.at(1).n_daug == 4 && list_of_resonances.at(2).n_daug == 3)
			type = 6;
		else{
			cout << "I am stupid and missed a case here" << endl;
			exit(0);
		}
		
	}
	else if(list_of_resonances.at(0).n_daug==6){
		// three cases:
		// (a) 3 daughters on each side
		if(list_of_resonances.at(1).n_daug == list_of_resonances.at(2).n_daug)
			type = 7;
		// (b) 4 daughters on left side, 2 daughters on right side
		else if(list_of_resonances.at(1).n_daug == list_of_resonances.at(2).n_daug+2)
			type = 8;
		// (c) 5 daughters on left side, 1 daughter on right side
		else if(list_of_resonances.at(1).n_daug==5 && list_of_resonances.at(3).n_daug==2)
			type = 9; // this is new
		else if(list_of_resonances.at(1).n_daug==5 && list_of_resonances.at(3).n_daug==3)
			type = 10;
		else{
			cout << "I am stupid and missed a case here" << endl;
			exit(0);
		}
	}

	typeToDecay[0] = "M --> A+B";
	typeToDecay[1] = "M --> R1+A\nR1 --> B+C";
	typeToDecay[2] = "M --> R1+R2\nR1 --> A+B\nR2 --> C+D";
	typeToDecay[3] = "M --> R1+A\nR1 --> R2+B\nR2 --> C+D";
	typeToDecay[4] = "M --> R1+R2\nR1 --> R3+A\nR2 --> B+C\nR3 --> D+E";
	typeToDecay[5] = "M --> R1+A\nR1 --> R2+R3\nR2 --> B+C\nR3 --> D+E";
	typeToDecay[6] = "M --> R1+A\nR1 --> R2+B\nR2 --> R3+C\nR3 --> D+E";
	typeToDecay[7] = "M --> R1+R2\nR1 --> R3+A\nR2 --> R4+B\nR3 --> C+D\nR4 --> E+F";
	typeToDecay[8] = "M --> R1+R2\nR1 --> R3+A\nR2 --> B+C\nR3 --> R4+D\nR4 --> E+F";
	typeToDecay[9] = "M --> R1+A\nR1 --> R2+B\nR2 --> R3+R4\nR3 --> C+D\nR4 --> E+F";
	typeToDecay[10] = "M --> R1+A\nR1 --> R2+B\nR2 --> R3+C\nR3 --> R4+D\nR4 --> E+F";

	assert(type>=0 && type<11);

	if(m_print)
		cout << "using amplitude for decay of the type" << endl << typeToDecay[type] << endl;

	// if necessary, register parameters
	if(b_useFactor){
		if(m_print)
			cout << "registering extra parameters" << endl;
		for(unsigned int j=0;j<extra_factors.size();j++){
			registerParameter(extra_factors.at(j).real);
			registerParameter(extra_factors.at(j).imag);
		}
	}

}

complex< GDouble >
  flexAmp::calcAmplitude( GDouble** pKin, GDouble *userVars ) const {

  	complex<GDouble> totAmp(0, 0);
  	complex<GDouble> MotherAmp(0, 0);
  	complex<GDouble> R1Amp(0, 0);
  	complex<GDouble> R2Amp(0, 0);
  	complex<GDouble> R3Amp(0, 0);
  	complex<GDouble> R4Amp(0, 0);
	TLorentzVector p4[10];
	TLorentzVector p4R[10];
	TLorentzVector p4Rec[10];
	TLorentzVector p4Ref[10];

	// in case there is no additional amplitude factor needed, the full amplitude can be cached
	if(!b_useFactor){
		totAmp = complex<GDouble>(userVars[kReal], userVars[kImag]);
		return totAmp;
	}
	// otherwise, amplitude needs to be recalculated
	else{
		for(int i=0;i<list_of_resonances.at(0).n_daug;i++)
			p4[i] = TLorentzVector(pKin[i][1], pKin[i][2], pKin[i][3], pKin[i][0]);

		for(int i=1;i<list_of_resonances.size();i++){
			p4R[i-1] = TLorentzVector(0,0,0,0);
			p4Rec[i-1] = TLorentzVector(0,0,0,0);
			p4Ref[i-1] = TLorentzVector(0,0,0,0);
			for(int j=0;j<list_of_resonances.at(i).n_daug;j++){
				p4R[i-1] += p4[list_of_resonances.at(i).i_daug.at(j)-1];
			}
			for(int j=0;j<list_of_resonances.at(i).n_rec;j++){
				p4Rec[i-1] += p4[list_of_resonances.at(i).i_rec.at(j)-1];	
			}
			if(list_of_resonances.at(i).n_ref > 0){
				for(int j=0;j<list_of_resonances.at(i).n_ref;j++){
					p4Ref[i-1] += p4[list_of_resonances.at(i).i_ref.at(j)-1];	
				}
			}
			else
			{
				p4Ref[i-1] = p4Ep;
			}
		}

		int i_recoil, i_daug1, i_daug2;
		int ir1_recoil;	
		int ir2_recoil;
		int ir3_recoil;
		int ir4_recoil;
		int ir5_recoil;
		int ir1_daug1, ir1_daug2;
		int ir2_daug1, ir2_daug2;
		int ir3_daug1, ir3_daug2;
		int ir4_daug1, ir4_daug2;
		int ir5_daug1, ir5_daug2;

		switch (type){
			case 0:
				/// full amplitude for e+ e- ---> psi(mother) ---> A1 + B1
				if(list_of_resonances.at(0).is_rad){
					// reads QNRES JR::MR::TYPE::JGAM
					// so list_of_resonances.at(0).sr is JGAM
					totAmp = angDist::multipoleProd(p4[0],p4[1],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,qn_daug[0].at(0),qn_daug[0].at(1),list_of_resonances.at(0).sr,qn_daug[1].at(1),list_of_resonances.at(0).type);
				}
				else{
					totAmp = angDist::ampProd(p4[0],p4[1],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,qn_daug[0].at(0),qn_daug[0].at(1),qn_daug[1].at(0),qn_daug[1].at(1),list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
				}
				break;
			case 1:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + A2
				///                                         with   R1 --> A1 + B1
				i_recoil = list_of_resonances.at(1).i_rec.at(0)-1;
				i_daug1 = list_of_resonances.at(1).i_daug.at(0)-1;
				i_daug2 = list_of_resonances.at(1).i_daug.at(1)-1;
				for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2; m_Mr1++){
					if(list_of_resonances.at(0).is_rad){
						MotherAmp = angDist::multipoleProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(0).sr,qn_daug[i_recoil].at(1),list_of_resonances.at(0).type);
					}
					else{
						MotherAmp = angDist::ampProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,qn_daug[i_recoil].at(0),qn_daug[i_recoil].at(1),list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
					}
					if(list_of_resonances.at(0).is_rad){
						R1Amp = angDist::multipoleDecay(p4[i_daug1],p4[i_daug2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,qn_daug[i_daug1].at(0),qn_daug[i_daug1].at(1),list_of_resonances.at(1).sr,qn_daug[i_daug2].at(1),list_of_resonances.at(1).type);
					}
					else{
						R1Amp = angDist::ampDecay(p4[i_daug1],p4[i_daug2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,qn_daug[i_daug1].at(0),qn_daug[i_daug1].at(1),qn_daug[i_daug2].at(0),qn_daug[i_daug2].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
					}
					if(extra_factors.at(0).index==1)
						R1Amp *= complex<GDouble>(extra_factors.at(0).real,extra_factors.at(0).imag);
					totAmp += MotherAmp*R1Amp;
				}	
				break;
				// tested, seems to work

			case 2:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + R2
				/// 										with   R1 ---> A1 + B1
				///										    and    R2 ---> A2 + B2
				ir1_daug1 = list_of_resonances.at(1).i_daug.at(0)-1;
				ir1_daug2 = list_of_resonances.at(1).i_daug.at(1)-1;
				ir2_daug1 = list_of_resonances.at(2).i_daug.at(0)-1;
				ir2_daug2 = list_of_resonances.at(2).i_daug.at(1)-1;

				// sum over R2 spin projections
				for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2; m_Mr2++){
					if(list_of_resonances.at(2).is_rad){
						R2Amp = angDist::multipoleDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),list_of_resonances.at(2).sr,qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).type);
					}
					else{
						R2Amp = angDist::ampDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),qn_daug[ir2_daug2].at(0),qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
					}
					// sum over R1 spin projections
					for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2; m_Mr1++){
						
						MotherAmp = angDist::ampProd(p4R[0],p4R[1],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
						
						if(list_of_resonances.at(1).is_rad){
							R1Amp = angDist::multipoleDecay(p4[ir1_daug1],p4[ir1_daug2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,qn_daug[ir1_daug1].at(0),qn_daug[ir1_daug1].at(1),list_of_resonances.at(1).sr,qn_daug[ir1_daug2].at(1),list_of_resonances.at(1).type);
						}
						else{
							R1Amp = angDist::ampDecay(p4[ir1_daug1],p4[ir1_daug2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,qn_daug[ir1_daug1].at(0),qn_daug[ir1_daug1].at(1),qn_daug[ir1_daug2].at(0),qn_daug[ir1_daug2].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
						}
						for(unsigned int i=0;i<extra_factors.size();i++){
							if(extra_factors.at(i).index==1)
								R1Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
							else if(extra_factors.at(i).index==2)
								R2Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
						}
						totAmp += MotherAmp*R1Amp*R2Amp;
					}
				}
				break;
				// tested, seems to work
			
			case 3:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + B2
				/// 										with   R1 ---> R2 + A2
				///										    and    R2 ---> A1 + B1
				ir1_recoil = list_of_resonances.at(1).i_rec.at(0)-1;
				ir2_recoil = list_of_resonances.at(2).i_rec.at(0)-1;
				ir2_daug1 = list_of_resonances.at(2).i_daug.at(0)-1;
				ir2_daug2 = list_of_resonances.at(2).i_daug.at(1)-1;
				// sum over R2 spin projections
				for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2; m_Mr2++){
					if(list_of_resonances.at(2).is_rad){
						R2Amp = angDist::multipoleDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Rec[0],list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),list_of_resonances.at(2).sr,qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).type);
					}
					else{
						R2Amp = angDist::ampDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Rec[0],list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),qn_daug[ir2_daug2].at(0),qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
					}

					for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2; m_Mr1++){
						if(list_of_resonances.at(0).is_rad){
							MotherAmp = angDist::multipoleProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(0).sr,qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).type);
						}
						else{
							MotherAmp = angDist::ampProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,qn_daug[ir1_recoil].at(0),qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
						}

						if(list_of_resonances.at(1).is_rad){
							R1Amp = angDist::multipoleDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(1).sr,qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).type);
						}
						else{
							R1Amp = angDist::ampDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_recoil].at(0),qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
						}

						for(unsigned int i=0;i<extra_factors.size();i++){
							if(extra_factors.at(i).index==1)
								R1Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
							else if(extra_factors.at(i).index==2)
								R2Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
						}

						totAmp += MotherAmp*R1Amp*R2Amp;
					}			
				}
				break;
				// tested, seems to work
				
			case 4:
				/// 5 body cases:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + R2
				/// 										with   R2 ---> A1 + B1
				///											with   R1 ---> R3 + A3
				///											with   R3 ---> A2 + B2	

				ir3_recoil = list_of_resonances.at(3).i_rec.at(0)-1;
				ir2_daug1 = list_of_resonances.at(2).i_daug.at(0)-1;
				ir2_daug2 = list_of_resonances.at(2).i_daug.at(1)-1;
				ir3_daug1 = list_of_resonances.at(3).i_daug.at(0)-1;
				ir3_daug2 = list_of_resonances.at(3).i_daug.at(1)-1;

				// sum over R3 spin projections
				for(double m_Mr3=-list_of_resonances.at(3).jr; m_Mr3<=list_of_resonances.at(3).jr+0.2; m_Mr3++){
					if(list_of_resonances.at(3).is_rad){
						R3Amp = angDist::multipoleDecay(p4[ir3_daug1],p4[ir3_daug1],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),list_of_resonances.at(3).sr,qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).type);
					}
					else{
						R3Amp = angDist::ampDecay(p4[ir3_daug1],p4[ir3_daug1],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),qn_daug[ir3_daug2].at(0),qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).lr,list_of_resonances.at(3).sr);
					}
					// sum over R2 spin projections
					for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2;m_Mr2++){
						if(list_of_resonances.at(2).is_rad){
							R2Amp = angDist::multipoleDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),list_of_resonances.at(2).sr,qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).type);
						}
						else{
							R2Amp = angDist::ampDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),qn_daug[ir2_daug2].at(0),qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
						}
						for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2;m_Mr1++){
							if(list_of_resonances.at(1).is_rad){
								R1Amp = angDist::multipoleDecay(p4R[2],p4Rec[2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(1).sr,qn_daug[ir3_recoil].at(1),list_of_resonances.at(1).type);
							}
							else{
								R1Amp = angDist::ampDecay(p4R[2],p4Rec[2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_recoil].at(0),qn_daug[ir3_recoil].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
							}
							MotherAmp = angDist::ampProd(p4R[0],p4R[1],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);

							for(unsigned int i=0;i<extra_factors.size();i++){
								if(extra_factors.at(i).index==1)
									R1Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
								else if(extra_factors.at(i).index==2)
									R2Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
								else if(extra_factors.at(i).index==3)
									R3Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
							}

							totAmp += MotherAmp*R1Amp*R2Amp*R3Amp;
						}
					}
				}	
				// not tested yet
				break;

			case 5:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + A3
				/// 										with   R1 ---> R2 + R3
				///											with   R2 ---> A2 + B2
				///											with   R3 ---> A1 + B1	

				ir1_recoil = list_of_resonances.at(1).i_rec.at(0)-1;
				ir2_daug1 = list_of_resonances.at(2).i_daug.at(0)-1;
				ir2_daug2 = list_of_resonances.at(2).i_daug.at(1)-1;
				ir3_daug1 = list_of_resonances.at(3).i_daug.at(0)-1;
				ir3_daug2 = list_of_resonances.at(3).i_daug.at(1)-1;
				// sum over R3 spin projections
				for(double m_Mr3=-list_of_resonances.at(3).jr; m_Mr3<=list_of_resonances.at(3).jr+0.2; m_Mr3++){
					if(list_of_resonances.at(3).is_rad){
						R3Amp = angDist::multipoleDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),list_of_resonances.at(3).sr,qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).type);
					}
					else{
						R3Amp = angDist::ampDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),qn_daug[ir3_daug2].at(0),qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).lr,list_of_resonances.at(3).sr);
					}
					for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2; m_Mr2++){
						if(list_of_resonances.at(2).is_rad){
							R2Amp = angDist::multipoleDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Ref[1],list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),list_of_resonances.at(2).sr,qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).type);
						}
						else{
							R2Amp = angDist::ampDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Ref[1],list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),qn_daug[ir2_daug2].at(0),qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
						}
						for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2;m_Mr1++){
							R1Amp = angDist::ampDecay(p4R[1],p4R[2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
							if(list_of_resonances.at(0).is_rad){
								MotherAmp = angDist::multipoleProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(0).sr,qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).type);
							}
							else{
								MotherAmp = angDist::ampProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,qn_daug[ir1_recoil].at(0),qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
							}

							for(unsigned int i=0;i<extra_factors.size();i++){
								if(extra_factors.at(i).index==1)
									R1Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
								else if(extra_factors.at(i).index==2)
									R2Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
								else if(extra_factors.at(i).index==3)
									R3Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
							}

							totAmp += MotherAmp*R1Amp*R2Amp*R3Amp;
						}
					}
				}
				// not tested yet
				break;	
			
			case 6:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + A3
				/// 										with   R1 ---> R2 + B2
				///											with   R2 ---> R3 + A2
				///											with   R3 ---> A1 + B1	

				ir3_recoil = list_of_resonances.at(3).i_rec.at(0)-1;
				ir2_recoil = list_of_resonances.at(2).i_rec.at(0)-1;
				ir1_recoil = list_of_resonances.at(1).i_rec.at(0)-1;
				ir3_daug1 = list_of_resonances.at(3).i_daug.at(0)-1;
				ir3_daug2 = list_of_resonances.at(3).i_daug.at(1)-1;

				// sum over R3 spin projections
				for(double m_Mr3=-list_of_resonances.at(3).jr; m_Mr3<=list_of_resonances.at(3).jr+0.2; m_Mr3++){
					if(list_of_resonances.at(3).is_rad){
						R3Amp = angDist::multipoleDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),list_of_resonances.at(3).sr,qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).type);
					}
					else{
						R3Amp = angDist::ampDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),qn_daug[ir3_daug2].at(0),qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).lr,list_of_resonances.at(3).sr);
					}
					for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2;m_Mr2++){
						if(list_of_resonances.at(2).is_rad){
							R2Amp = angDist::multipoleDecay(p4R[2],p4Rec[2],p4Rec[1],p4Ref[1],list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(2).sr,qn_daug[ir3_recoil].at(1),list_of_resonances.at(2).type);
						}
						else{
							R2Amp = angDist::ampDecay(p4R[2],p4Rec[2],p4Rec[1],p4Ref[1],list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_recoil].at(0),qn_daug[ir3_recoil].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
						}
						for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2;m_Mr1++){
							if(list_of_resonances.at(1).is_rad){
								R1Amp = angDist::multipoleDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(1).sr,qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).type);
							}
							else{
								R1Amp = angDist::ampDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_recoil].at(0),qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
							}
							if(list_of_resonances.at(0).is_rad){
								MotherAmp = angDist::multipoleProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(0).sr,qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).type);
							}
							else{
								MotherAmp = angDist::ampProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,qn_daug[ir1_recoil].at(0),qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
							}

							for(unsigned int i=0;i<extra_factors.size();i++){
								if(extra_factors.at(i).index==1)
									R1Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
								else if(extra_factors.at(i).index==2)
									R2Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
								else if(extra_factors.at(i).index==3)
									R3Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
							}

							totAmp += MotherAmp*R1Amp*R2Amp*R3Amp;
						}
					}
				}
				// not tested yet
				break;	

				
			case 7:
				// 6 body cases
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + R2
				/// 										with   R1 ---> R3 + A3
				///											with   R2 ---> R4 + B3
				///											with   R3 ---> A2 + B2	
				///											with   R4 ---> A1 + B1		

				ir3_recoil = list_of_resonances.at(3).i_rec.at(0)-1;
				ir4_recoil = list_of_resonances.at(4).i_rec.at(0)-1;
				ir3_daug1 = list_of_resonances.at(3).i_daug.at(0)-1;
				ir3_daug2 = list_of_resonances.at(3).i_daug.at(1)-1;
				ir4_daug1 = list_of_resonances.at(4).i_daug.at(0)-1;
				ir4_daug2 = list_of_resonances.at(4).i_daug.at(1)-1;
				// sum over R4 spin projections
				for(double m_Mr4=-list_of_resonances.at(4).jr; m_Mr4<=list_of_resonances.at(4).jr+0.2; m_Mr4++){
					if(list_of_resonances.at(4).is_rad){
						R4Amp = angDist::multipoleDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),list_of_resonances.at(4).sr,qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).type);
					}
					else{
						R4Amp = angDist::ampDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),qn_daug[ir4_daug2].at(0),qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).lr,list_of_resonances.at(4).sr);
					}
					// sum over R3 spin projections
					for(double m_Mr3=-list_of_resonances.at(3).jr; m_Mr3<=list_of_resonances.at(3).jr+0.2; m_Mr3++){
						if(list_of_resonances.at(3).is_rad){
							R3Amp = angDist::multipoleDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),list_of_resonances.at(3).sr,qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).type);
						}
						else{
							R3Amp = angDist::ampDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),qn_daug[ir3_daug2].at(0),qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).lr,list_of_resonances.at(3).sr);
						}
						// sum over R2 spin projections
						for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2; m_Mr2++){
							if(list_of_resonances.at(2).is_rad){
								R2Amp = angDist::multipoleDecay(p4R[3],p4Rec[3],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(4).jr,m_Mr4,list_of_resonances.at(2).sr,qn_daug[ir4_recoil].at(1),list_of_resonances.at(2).type);
							}
							else{
								R2Amp = angDist::ampDecay(p4R[3],p4Rec[3],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_recoil].at(0),qn_daug[ir4_recoil].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
							}
							// sum over R1 spin projections
							for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2; m_Mr1++){
								if(list_of_resonances.at(1).is_rad){
									R1Amp = angDist::multipoleDecay(p4R[2],p4Rec[2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(1).sr,qn_daug[ir3_recoil].at(1),list_of_resonances.at(1).type);
								}
								else{
									R1Amp = angDist::ampDecay(p4R[2],p4Rec[2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_recoil].at(0),qn_daug[ir3_recoil].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
								}
								MotherAmp = angDist::ampProd(p4R[0],p4R[1],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);

								for(unsigned int i=0;i<extra_factors.size();i++){
									if(extra_factors.at(i).index==1)
										R1Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
									else if(extra_factors.at(i).index==2)
										R2Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
									else if(extra_factors.at(i).index==3)
										R3Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
									else if(extra_factors.at(i).index==4)
										R4Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
								}

								totAmp += MotherAmp*R1Amp*R2Amp*R3Amp*R4Amp;
							}
						}
					}
				}
				// not tested yet
				break;

			case 8:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + R2
				/// 										with   R1 ---> R3 + A1
				///											with   R3 ---> R4 + B1
				///											with   R4 ---> A2 + B2
				///											with   R2 ---> A3 + B3	
				ir3_recoil = list_of_resonances.at(3).i_rec.at(0)-1;
				ir4_recoil = list_of_resonances.at(4).i_rec.at(0)-1;
				ir2_daug1 = list_of_resonances.at(2).i_daug.at(0)-1;
				ir2_daug2 = list_of_resonances.at(2).i_daug.at(1)-1;
				ir4_daug1 = list_of_resonances.at(4).i_daug.at(0)-1;
				ir4_daug2 = list_of_resonances.at(4).i_daug.at(1)-1;

				// sum over R4 spin projections
				for(double m_Mr4=-list_of_resonances.at(4).jr; m_Mr4<=list_of_resonances.at(4).jr+0.2; m_Mr4++){
					if(list_of_resonances.at(4).is_rad){
						R4Amp = angDist::multipoleDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),list_of_resonances.at(4).sr,qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).type);
					}
					else{
						R4Amp = angDist::ampDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),qn_daug[ir4_daug2].at(0),qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).lr,list_of_resonances.at(4).sr);
					}
					// sum over R3 spin projections
					for(double m_Mr3=-list_of_resonances.at(3).jr; m_Mr3<=list_of_resonances.at(3).jr+0.2; m_Mr3++){
						if(list_of_resonances.at(3).is_rad){
							R3Amp = angDist::multipoleDecay(p4R[3],p4Rec[3],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(4).jr,m_Mr4,list_of_resonances.at(3).sr,qn_daug[ir4_recoil].at(1),list_of_resonances.at(3).type);
						}
						else{
							R3Amp = angDist::ampDecay(p4R[3],p4Rec[3],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_recoil].at(0),qn_daug[ir4_recoil].at(1),list_of_resonances.at(3).lr,list_of_resonances.at(3).sr);
						}
						// sum over R1 spin projections
						for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2; m_Mr2++){
							if(list_of_resonances.at(2).is_rad){
								R2Amp = angDist::multipoleDecay(p4[ir2_daug1],p4[ir2_daug1],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),list_of_resonances.at(2).sr,qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).type);
							}
							else{
								R2Amp = angDist::ampDecay(p4[ir2_daug1],p4[ir2_daug1],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),qn_daug[ir2_daug2].at(0),qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
							}
							// sum over R4 spin projections
							for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2; m_Mr1++){
								if(list_of_resonances.at(1).is_rad){
									R1Amp = angDist::multipoleDecay(p4R[2],p4Rec[2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(1).sr,qn_daug[ir3_recoil].at(1),list_of_resonances.at(1).type);
								}
								else{
									R1Amp = angDist::ampDecay(p4R[2],p4Rec[2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_recoil].at(0),qn_daug[ir3_recoil].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
								}
								MotherAmp = angDist::ampProd(p4R[0],p4R[1],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);

								for(unsigned int i=0;i<extra_factors.size();i++){
									if(extra_factors.at(i).index==1)
										R1Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
									else if(extra_factors.at(i).index==2)
										R2Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
									else if(extra_factors.at(i).index==3)
										R3Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
									else if(extra_factors.at(i).index==4)
										R4Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
								}

								totAmp += MotherAmp*R1Amp*R2Amp*R3Amp*R4Amp;
							}
						}
					}
				}
				// not tested yet
				break;
			case 9:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + A1
				/// 										with   R1 ---> R2 + B1
				///											with   R2 ---> R3 + R4
				///											with   R3 ---> A2 + B2
				///											with   R4 ---> A3 + B3	
				ir1_recoil = list_of_resonances.at(1).i_rec.at(0)-1;
				ir2_recoil = list_of_resonances.at(2).i_rec.at(0)-1;
				ir3_daug1 = list_of_resonances.at(3).i_daug.at(0)-1;
				ir3_daug2 = list_of_resonances.at(3).i_daug.at(1)-1;
				ir4_daug1 = list_of_resonances.at(4).i_daug.at(0)-1;
				ir4_daug2 = list_of_resonances.at(4).i_daug.at(1)-1;
				// sum over R4 spin projections
				for(double m_Mr4=-list_of_resonances.at(4).jr; m_Mr4<=list_of_resonances.at(4).jr+0.2; m_Mr4++){
					if(list_of_resonances.at(4).is_rad){
						R4Amp = angDist::multipoleDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),list_of_resonances.at(4).sr,qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).type);
					}
					else{
						R4Amp = angDist::ampDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),qn_daug[ir4_daug2].at(0),qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).lr,list_of_resonances.at(4).sr);
					}	
				// sum over R4 spin projections
					for(double m_Mr3=-list_of_resonances.at(3).jr; m_Mr3<=list_of_resonances.at(3).jr+0.2; m_Mr3++){
						if(list_of_resonances.at(3).is_rad){
							R3Amp = angDist::multipoleDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),list_of_resonances.at(3).sr,qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).type);	
						}
						else{
							R3Amp = angDist::ampDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),qn_daug[ir3_daug2].at(0),qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).lr,list_of_resonances.at(3).sr);	
						}
						// sum over R2 spin projections
						for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2; m_Mr2++){
							R2Amp = angDist::ampDecay(p4R[2],p4R[3],p4Rec[1],p4Ref[1],list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(4).jr,m_Mr4,list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);	
							// sum over R1 spin projections
							for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2; m_Mr1++){
								if(list_of_resonances.at(1).is_rad){
									R1Amp = angDist::multipoleDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(1).sr,qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).type);
								}
								else{
									R1Amp = angDist::ampDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_recoil].at(0),qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
								}
								if(list_of_resonances.at(0).is_rad){
									MotherAmp = angDist::multipoleProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(0).sr,qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).type);
								}
								else{
									MotherAmp = angDist::ampProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,qn_daug[ir1_recoil].at(0),qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
								}

								for(unsigned int i=0;i<extra_factors.size();i++){
									if(extra_factors.at(i).index==1)
										R1Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
									else if(extra_factors.at(i).index==2)
										R2Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
									else if(extra_factors.at(i).index==3)
										R3Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
									else if(extra_factors.at(i).index==4)
										R4Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
								}

								totAmp += MotherAmp*R1Amp*R2Amp*R3Amp*R4Amp;
							}
						}
					}
				}
				// not tested yet
				break;												
				
			case 10:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + A1
				/// 										with   R1 ---> R2 + B1
				///											with   R2 ---> R3 + A2
				///											with   R3 ---> R4 + B2
				///											with   R4 ---> A3 + B3	
				ir1_recoil = list_of_resonances.at(1).i_rec.at(0)-1;
				ir2_recoil = list_of_resonances.at(2).i_rec.at(0)-1;
				ir3_recoil = list_of_resonances.at(3).i_rec.at(0)-1;
				ir4_recoil = list_of_resonances.at(4).i_rec.at(0)-1;
				ir4_daug1 = list_of_resonances.at(4).i_daug.at(0)-1;
				ir4_daug2 = list_of_resonances.at(4).i_daug.at(1)-1;

				// sum over R4 spin projections
				for(double m_Mr4=-list_of_resonances.at(4).jr; m_Mr4<=list_of_resonances.at(4).jr+0.2; m_Mr4++){
					if(list_of_resonances.at(4).is_rad){
						R4Amp = angDist::multipoleDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),list_of_resonances.at(4).sr,qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).type);
					}
					else{
						R4Amp = angDist::ampDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),qn_daug[ir4_daug2].at(0),qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).lr,list_of_resonances.at(4).sr);
					}
					// sum over R3 spin projections
					for(double m_Mr3=-list_of_resonances.at(3).jr; m_Mr3<=list_of_resonances.at(3).jr+0.2; m_Mr3++){
						if(list_of_resonances.at(3).is_rad){
							R3Amp = angDist::multipoleDecay(p4R[3],p4Rec[3],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(4).jr,m_Mr4,list_of_resonances.at(3).sr,qn_daug[ir4_recoil].at(1),list_of_resonances.at(3).type);
						}
						else{
							R3Amp = angDist::ampDecay(p4R[3],p4Rec[3],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_recoil].at(0),qn_daug[ir4_recoil].at(1),list_of_resonances.at(3).lr,list_of_resonances.at(3).sr);
						}
						// sum over R2 spin projections
						for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2; m_Mr2++){
							if(list_of_resonances.at(2).is_rad){
								R2Amp = angDist::multipoleDecay(p4R[2],p4Rec[2],p4Rec[1],p4Ref[1],list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(2).sr,qn_daug[ir3_recoil].at(1),list_of_resonances.at(2).type);
							}
							else{
								R2Amp = angDist::ampDecay(p4R[2],p4Rec[2],p4Rec[1],p4Ref[1],list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_recoil].at(0),qn_daug[ir3_recoil].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
							}
							// sum over R1 spin projections
							for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2; m_Mr1++){
								if(list_of_resonances.at(1).is_rad){
									R1Amp = angDist::multipoleDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(1).sr,qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).type);
								}
								else{
									R1Amp = angDist::ampDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_recoil].at(0),qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
								}
								if(list_of_resonances.at(0).is_rad){
									MotherAmp = angDist::multipoleProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(0).sr,qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).type);
								}
								else{
									MotherAmp = angDist::ampProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,qn_daug[ir1_recoil].at(0),qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
								}

								for(unsigned int i=0;i<extra_factors.size();i++){
									if(extra_factors.at(i).index==1)
										R1Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
									else if(extra_factors.at(i).index==2)
										R2Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
									else if(extra_factors.at(i).index==3)
										R3Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
									else if(extra_factors.at(i).index==4)
										R4Amp *= complex<GDouble>(extra_factors.at(i).real,extra_factors.at(i).imag);
								}

								totAmp += MotherAmp*R1Amp*R2Amp*R3Amp*R4Amp;
							}
						}
					}
				}
				// not tested yet
				break;
		}	

		if(TMath::IsNaN(totAmp.real()) || TMath::IsNaN(totAmp.imag())){
			cout << "error, angular amplitude is NaN" << endl;
			for(unsigned int i=0;i<extra_factors.size();i++){
				cout << extra_factors.at(i).index << " " << extra_factors.at(i).real << " " << extra_factors.at(i).imag << endl;
			}
			exit(0);
		}
		return totAmp;
	}

	return totAmp;
}

int flexAmp::m_ryandebug;

void flexAmp::calcUserVars( GDouble** pKin, GDouble* userVars) const {
m_ryandebug++;
	// if there is an additional complex factor in the amplitude,
	// we can not cache it, and will thus cache (and do) nothing here
	if(b_useFactor)
		return;
if (m_ryandebug%10000 == 0){
cout << "   FLEXAMP::calcUserVars  continuing";
cout << "   FLEXAMP::calcUserVars number = " << m_ryandebug << endl;
}

  	complex<GDouble> totAmp(0, 0);
  	complex<GDouble> MotherAmp(0, 0);
  	complex<GDouble> R1Amp(0, 0);
  	complex<GDouble> R2Amp(0, 0);
  	complex<GDouble> R3Amp(0, 0);
  	complex<GDouble> R4Amp(0, 0);
	TLorentzVector p4[10];
	TLorentzVector p4R[10];
	TLorentzVector p4Rec[10];
	TLorentzVector p4Ref[10];
	for(int i=0;i<list_of_resonances.at(0).n_daug;i++)
		p4[i] = TLorentzVector(pKin[i][1], pKin[i][2], pKin[i][3], pKin[i][0]);


	for(int i=1;i<list_of_resonances.size();i++){
		p4R[i-1] = TLorentzVector(0,0,0,0);
		p4Rec[i-1] = TLorentzVector(0,0,0,0);
		p4Ref[i-1] = TLorentzVector(0,0,0,0);
		for(int j=0;j<list_of_resonances.at(i).n_daug;j++){
			p4R[i-1] += p4[list_of_resonances.at(i).i_daug.at(j)-1];
		}
		for(int j=0;j<list_of_resonances.at(i).n_rec;j++){
			p4Rec[i-1] += p4[list_of_resonances.at(i).i_rec.at(j)-1];	
		}
		if(list_of_resonances.at(i).n_ref > 0){
			for(int j=0;j<list_of_resonances.at(i).n_ref;j++){
				p4Ref[i-1] += p4[list_of_resonances.at(i).i_ref.at(j)-1];	
			}
		}
		else
		{
			p4Ref[i-1] = p4Ep;
		}
	}

	int i_recoil, i_daug1, i_daug2;
	int ir1_recoil;	
	int ir2_recoil;
	int ir3_recoil;
	int ir4_recoil;
	int ir5_recoil;
	int ir1_daug1, ir1_daug2;
	int ir2_daug1, ir2_daug2;
	int ir3_daug1, ir3_daug2;
	int ir4_daug1, ir4_daug2;
	int ir5_daug1, ir5_daug2;

	switch (type){
			case 0:
				/// full amplitude for e+ e- ---> psi(mother) ---> A1 + B1
				if(list_of_resonances.at(0).is_rad){
					// reads QNRES JR::MR::TYPE::JGAM
					// so list_of_resonances.at(0).sr is JGAM
					totAmp = angDist::multipoleProd(p4[0],p4[1],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,qn_daug[0].at(0),qn_daug[0].at(1),list_of_resonances.at(0).sr,qn_daug[1].at(1),list_of_resonances.at(0).type);
				}
				else{
					totAmp = angDist::ampProd(p4[0],p4[1],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,qn_daug[0].at(0),qn_daug[0].at(1),qn_daug[1].at(0),qn_daug[1].at(1),list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
				}
				break;
			case 1:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + A2
				///                                         with   R1 --> A1 + B1
				i_recoil = list_of_resonances.at(1).i_rec.at(0)-1;
				i_daug1 = list_of_resonances.at(1).i_daug.at(0)-1;
				i_daug2 = list_of_resonances.at(1).i_daug.at(1)-1;
				for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2; m_Mr1++){
					if(list_of_resonances.at(0).is_rad){
						MotherAmp = angDist::multipoleProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(0).sr,qn_daug[i_recoil].at(1),list_of_resonances.at(0).type);
					}
					else{
						MotherAmp = angDist::ampProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,qn_daug[i_recoil].at(0),qn_daug[i_recoil].at(1),list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
					}
					if(list_of_resonances.at(0).is_rad){
						R1Amp = angDist::multipoleDecay(p4[i_daug1],p4[i_daug2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,qn_daug[i_daug1].at(0),qn_daug[i_daug1].at(1),list_of_resonances.at(1).sr,qn_daug[i_daug2].at(1),list_of_resonances.at(1).type);
					}
					else{
						R1Amp = angDist::ampDecay(p4[i_daug1],p4[i_daug2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,qn_daug[i_daug1].at(0),qn_daug[i_daug1].at(1),qn_daug[i_daug2].at(0),qn_daug[i_daug2].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
					}

					totAmp += MotherAmp*R1Amp;
				}	
				break;
				// tested, seems to work

			case 2:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + R2
				/// 										with   R1 ---> A1 + B1
				///										    and    R2 ---> A2 + B2
				ir1_daug1 = list_of_resonances.at(1).i_daug.at(0)-1;
				ir1_daug2 = list_of_resonances.at(1).i_daug.at(1)-1;
				ir2_daug1 = list_of_resonances.at(2).i_daug.at(0)-1;
				ir2_daug2 = list_of_resonances.at(2).i_daug.at(1)-1;

				// sum over R2 spin projections
				for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2; m_Mr2++){
					if(list_of_resonances.at(2).is_rad){
						R2Amp = angDist::multipoleDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),list_of_resonances.at(2).sr,qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).type);
					}
					else{
						R2Amp = angDist::ampDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),qn_daug[ir2_daug2].at(0),qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
					}
					// sum over R1 spin projections
					for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2; m_Mr1++){
						
						MotherAmp = angDist::ampProd(p4R[0],p4R[1],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
						
						if(list_of_resonances.at(1).is_rad){
							R1Amp = angDist::multipoleDecay(p4[ir1_daug1],p4[ir1_daug2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,qn_daug[ir1_daug1].at(0),qn_daug[ir1_daug1].at(1),list_of_resonances.at(1).sr,qn_daug[ir1_daug2].at(1),list_of_resonances.at(1).type);
						}
						else{
							R1Amp = angDist::ampDecay(p4[ir1_daug1],p4[ir1_daug2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,qn_daug[ir1_daug1].at(0),qn_daug[ir1_daug1].at(1),qn_daug[ir1_daug2].at(0),qn_daug[ir1_daug2].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
						}

						totAmp += MotherAmp*R1Amp*R2Amp;
					}
				}
				break;
				// tested, seems to work
			
			case 3:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + B2
				/// 										with   R1 ---> R2 + A2
				///										    and    R2 ---> A1 + B1
				ir1_recoil = list_of_resonances.at(1).i_rec.at(0)-1;
				ir2_recoil = list_of_resonances.at(2).i_rec.at(0)-1;
				ir2_daug1 = list_of_resonances.at(2).i_daug.at(0)-1;
				ir2_daug2 = list_of_resonances.at(2).i_daug.at(1)-1;
				// sum over R2 spin projections
				for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2; m_Mr2++){
//if (m_ryandebug > 122372){ cout << "here start" << endl; }
					if(list_of_resonances.at(2).is_rad){
//if (m_ryandebug > 122372){ cout << "mult start"; }
						R2Amp = angDist::multipoleDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Rec[0],list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),list_of_resonances.at(2).sr,qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).type);
//if (m_ryandebug > 122372){ cout << "   mult end" << endl; }
					}
					else{
						R2Amp = angDist::ampDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Rec[0],list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),qn_daug[ir2_daug2].at(0),qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
					}

					for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2; m_Mr1++){
						if(list_of_resonances.at(0).is_rad){
							MotherAmp = angDist::multipoleProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(0).sr,qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).type);
						}
						else{
							MotherAmp = angDist::ampProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,qn_daug[ir1_recoil].at(0),qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
						}

						if(list_of_resonances.at(1).is_rad){
							R1Amp = angDist::multipoleDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(1).sr,qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).type);
						}
						else{
							R1Amp = angDist::ampDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_recoil].at(0),qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
						}

						totAmp += MotherAmp*R1Amp*R2Amp;
					}			
				}
				break;
				// tested, seems to work
				
			case 4:
				/// 5 body cases:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + R2
				/// 										with   R2 ---> A1 + B1
				///											with   R1 ---> R3 + A3
				///											with   R3 ---> A2 + B2	

				ir3_recoil = list_of_resonances.at(3).i_rec.at(0)-1;
				ir2_daug1 = list_of_resonances.at(2).i_daug.at(0)-1;
				ir2_daug2 = list_of_resonances.at(2).i_daug.at(1)-1;
				ir3_daug1 = list_of_resonances.at(3).i_daug.at(0)-1;
				ir3_daug2 = list_of_resonances.at(3).i_daug.at(1)-1;

				// sum over R3 spin projections
				for(double m_Mr3=-list_of_resonances.at(3).jr; m_Mr3<=list_of_resonances.at(3).jr+0.2; m_Mr3++){
					if(list_of_resonances.at(3).is_rad){
						R3Amp = angDist::multipoleDecay(p4[ir3_daug1],p4[ir3_daug1],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),list_of_resonances.at(3).sr,qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).type);
					}
					else{
						R3Amp = angDist::ampDecay(p4[ir3_daug1],p4[ir3_daug1],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),qn_daug[ir3_daug2].at(0),qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).lr,list_of_resonances.at(3).sr);
					}
					// sum over R2 spin projections
					for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2;m_Mr2++){
						if(list_of_resonances.at(2).is_rad){
							R2Amp = angDist::multipoleDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),list_of_resonances.at(2).sr,qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).type);
						}
						else{
							R2Amp = angDist::ampDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),qn_daug[ir2_daug2].at(0),qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
						}
						for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2;m_Mr1++){
							if(list_of_resonances.at(1).is_rad){
								R1Amp = angDist::multipoleDecay(p4R[2],p4Rec[2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(1).sr,qn_daug[ir3_recoil].at(1),list_of_resonances.at(1).type);
							}
							else{
								R1Amp = angDist::ampDecay(p4R[2],p4Rec[2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_recoil].at(0),qn_daug[ir3_recoil].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
							}
							MotherAmp = angDist::ampProd(p4R[0],p4R[1],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);

							totAmp += MotherAmp*R1Amp*R2Amp*R3Amp;
						}
					}
				}	
				// not tested yet
				break;

			case 5:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + A3
				/// 										with   R1 ---> R2 + R3
				///											with   R2 ---> A2 + B2
				///											with   R3 ---> A1 + B1	

				ir1_recoil = list_of_resonances.at(1).i_rec.at(0)-1;
				ir2_daug1 = list_of_resonances.at(2).i_daug.at(0)-1;
				ir2_daug2 = list_of_resonances.at(2).i_daug.at(1)-1;
				ir3_daug1 = list_of_resonances.at(3).i_daug.at(0)-1;
				ir3_daug2 = list_of_resonances.at(3).i_daug.at(1)-1;
				// sum over R3 spin projections
				for(double m_Mr3=-list_of_resonances.at(3).jr; m_Mr3<=list_of_resonances.at(3).jr+0.2; m_Mr3++){
					if(list_of_resonances.at(3).is_rad){
						R3Amp = angDist::multipoleDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),list_of_resonances.at(3).sr,qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).type);
					}
					else{
						R3Amp = angDist::ampDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),qn_daug[ir3_daug2].at(0),qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).lr,list_of_resonances.at(3).sr);
					}
					for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2; m_Mr2++){
						if(list_of_resonances.at(2).is_rad){
							R2Amp = angDist::multipoleDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Ref[1],list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),list_of_resonances.at(2).sr,qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).type);
						}
						else{
							R2Amp = angDist::ampDecay(p4[ir2_daug1],p4[ir2_daug2],p4Rec[1],p4Ref[1],list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),qn_daug[ir2_daug2].at(0),qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
						}
						for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2;m_Mr1++){
							R1Amp = angDist::ampDecay(p4R[1],p4R[2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
							if(list_of_resonances.at(0).is_rad){
								MotherAmp = angDist::multipoleProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(0).sr,qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).type);
							}
							else{
								MotherAmp = angDist::ampProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,qn_daug[ir1_recoil].at(0),qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
							}

							totAmp += MotherAmp*R1Amp*R2Amp*R3Amp;
						}
					}
				}
				// not tested yet
				break;	
			
			case 6:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + A3
				/// 										with   R1 ---> R2 + B2
				///											with   R2 ---> R3 + A2
				///											with   R3 ---> A1 + B1	

				ir3_recoil = list_of_resonances.at(3).i_rec.at(0)-1;
				ir2_recoil = list_of_resonances.at(2).i_rec.at(0)-1;
				ir1_recoil = list_of_resonances.at(1).i_rec.at(0)-1;
				ir3_daug1 = list_of_resonances.at(3).i_daug.at(0)-1;
				ir3_daug2 = list_of_resonances.at(3).i_daug.at(1)-1;

				// sum over R3 spin projections
				for(double m_Mr3=-list_of_resonances.at(3).jr; m_Mr3<=list_of_resonances.at(3).jr+0.2; m_Mr3++){
					if(list_of_resonances.at(3).is_rad){
						R3Amp = angDist::multipoleDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),list_of_resonances.at(3).sr,qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).type);
					}
					else{
						R3Amp = angDist::ampDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),qn_daug[ir3_daug2].at(0),qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).lr,list_of_resonances.at(3).sr);
					}
					for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2;m_Mr2++){
						if(list_of_resonances.at(2).is_rad){
							R2Amp = angDist::multipoleDecay(p4R[2],p4Rec[2],p4Rec[1],p4Ref[1],list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(2).sr,qn_daug[ir3_recoil].at(1),list_of_resonances.at(2).type);
						}
						else{
							R2Amp = angDist::ampDecay(p4R[2],p4Rec[2],p4Rec[1],p4Ref[1],list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_recoil].at(0),qn_daug[ir3_recoil].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
						}
						for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2;m_Mr1++){
							if(list_of_resonances.at(1).is_rad){
								R1Amp = angDist::multipoleDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(1).sr,qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).type);
							}
							else{
								R1Amp = angDist::ampDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_recoil].at(0),qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
							}
							if(list_of_resonances.at(0).is_rad){
								MotherAmp = angDist::multipoleProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(0).sr,qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).type);
							}
							else{
								MotherAmp = angDist::ampProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,qn_daug[ir1_recoil].at(0),qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
							}

							totAmp += MotherAmp*R1Amp*R2Amp*R3Amp;
						}
					}
				}
				// not tested yet
				break;	

				
			case 7:
				// 6 body cases
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + R2
				/// 										with   R1 ---> R3 + A3
				///											with   R2 ---> R4 + B3
				///											with   R3 ---> A2 + B2	
				///											with   R4 ---> A1 + B1		

				ir3_recoil = list_of_resonances.at(3).i_rec.at(0)-1;
				ir4_recoil = list_of_resonances.at(4).i_rec.at(0)-1;
				ir3_daug1 = list_of_resonances.at(3).i_daug.at(0)-1;
				ir3_daug2 = list_of_resonances.at(3).i_daug.at(1)-1;
				ir4_daug1 = list_of_resonances.at(4).i_daug.at(0)-1;
				ir4_daug2 = list_of_resonances.at(4).i_daug.at(1)-1;
				// sum over R4 spin projections
				for(double m_Mr4=-list_of_resonances.at(4).jr; m_Mr4<=list_of_resonances.at(4).jr+0.2; m_Mr4++){
					if(list_of_resonances.at(4).is_rad){
						R4Amp = angDist::multipoleDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),list_of_resonances.at(4).sr,qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).type);
					}
					else{
						R4Amp = angDist::ampDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),qn_daug[ir4_daug2].at(0),qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).lr,list_of_resonances.at(4).sr);
					}
					// sum over R3 spin projections
					for(double m_Mr3=-list_of_resonances.at(3).jr; m_Mr3<=list_of_resonances.at(3).jr+0.2; m_Mr3++){
						if(list_of_resonances.at(3).is_rad){
							R3Amp = angDist::multipoleDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),list_of_resonances.at(3).sr,qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).type);
						}
						else{
							R3Amp = angDist::ampDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),qn_daug[ir3_daug2].at(0),qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).lr,list_of_resonances.at(3).sr);
						}
						// sum over R2 spin projections
						for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2; m_Mr2++){
							if(list_of_resonances.at(2).is_rad){
								R2Amp = angDist::multipoleDecay(p4R[3],p4Rec[3],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(4).jr,m_Mr4,list_of_resonances.at(2).sr,qn_daug[ir4_recoil].at(1),list_of_resonances.at(2).type);
							}
							else{
								R2Amp = angDist::ampDecay(p4R[3],p4Rec[3],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_recoil].at(0),qn_daug[ir4_recoil].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
							}
							// sum over R1 spin projections
							for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2; m_Mr1++){
								if(list_of_resonances.at(1).is_rad){
									R1Amp = angDist::multipoleDecay(p4R[2],p4Rec[2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(1).sr,qn_daug[ir3_recoil].at(1),list_of_resonances.at(1).type);
								}
								else{
									R1Amp = angDist::ampDecay(p4R[2],p4Rec[2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_recoil].at(0),qn_daug[ir3_recoil].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
								}
								MotherAmp = angDist::ampProd(p4R[0],p4R[1],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);

								totAmp += MotherAmp*R1Amp*R2Amp*R3Amp*R4Amp;
							}
						}
					}
				}
				// not tested yet
				break;

			case 8:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + R2
				/// 										with   R1 ---> R3 + A1
				///											with   R3 ---> R4 + B1
				///											with   R4 ---> A2 + B2
				///											with   R2 ---> A3 + B3	
				ir3_recoil = list_of_resonances.at(3).i_rec.at(0)-1;
				ir4_recoil = list_of_resonances.at(4).i_rec.at(0)-1;
				ir2_daug1 = list_of_resonances.at(2).i_daug.at(0)-1;
				ir2_daug2 = list_of_resonances.at(2).i_daug.at(1)-1;
				ir4_daug1 = list_of_resonances.at(4).i_daug.at(0)-1;
				ir4_daug2 = list_of_resonances.at(4).i_daug.at(1)-1;

				// sum over R4 spin projections
				for(double m_Mr4=-list_of_resonances.at(4).jr; m_Mr4<=list_of_resonances.at(4).jr+0.2; m_Mr4++){
					if(list_of_resonances.at(4).is_rad){
						R4Amp = angDist::multipoleDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),list_of_resonances.at(4).sr,qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).type);
					}
					else{
						R4Amp = angDist::ampDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),qn_daug[ir4_daug2].at(0),qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).lr,list_of_resonances.at(4).sr);
					}
					// sum over R3 spin projections
					for(double m_Mr3=-list_of_resonances.at(3).jr; m_Mr3<=list_of_resonances.at(3).jr+0.2; m_Mr3++){
						if(list_of_resonances.at(3).is_rad){
							R3Amp = angDist::multipoleDecay(p4R[3],p4Rec[3],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(4).jr,m_Mr4,list_of_resonances.at(3).sr,qn_daug[ir4_recoil].at(1),list_of_resonances.at(3).type);
						}
						else{
							R3Amp = angDist::ampDecay(p4R[3],p4Rec[3],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_recoil].at(0),qn_daug[ir4_recoil].at(1),list_of_resonances.at(3).lr,list_of_resonances.at(3).sr);
						}
						// sum over R1 spin projections
						for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2; m_Mr2++){
							if(list_of_resonances.at(2).is_rad){
								R2Amp = angDist::multipoleDecay(p4[ir2_daug1],p4[ir2_daug1],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),list_of_resonances.at(2).sr,qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).type);
							}
							else{
								R2Amp = angDist::ampDecay(p4[ir2_daug1],p4[ir2_daug1],p4Rec[1],p4Ep,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_daug1].at(0),qn_daug[ir2_daug1].at(1),qn_daug[ir2_daug2].at(0),qn_daug[ir2_daug2].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
							}
							// sum over R4 spin projections
							for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2; m_Mr1++){
								if(list_of_resonances.at(1).is_rad){
									R1Amp = angDist::multipoleDecay(p4R[2],p4Rec[2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(1).sr,qn_daug[ir3_recoil].at(1),list_of_resonances.at(1).type);
								}
								else{
									R1Amp = angDist::ampDecay(p4R[2],p4Rec[2],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_recoil].at(0),qn_daug[ir3_recoil].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
								}
								MotherAmp = angDist::ampProd(p4R[0],p4R[1],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);

								totAmp += MotherAmp*R1Amp*R2Amp*R3Amp*R4Amp;
							}
						}
					}
				}
				// not tested yet
				break;
			case 9:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + A1
				/// 										with   R1 ---> R2 + B1
				///											with   R2 ---> R3 + R4
				///											with   R3 ---> A2 + B2
				///											with   R4 ---> A3 + B3	
				ir1_recoil = list_of_resonances.at(1).i_rec.at(0)-1;
				ir2_recoil = list_of_resonances.at(2).i_rec.at(0)-1;
				ir3_daug1 = list_of_resonances.at(3).i_daug.at(0)-1;
				ir3_daug2 = list_of_resonances.at(3).i_daug.at(1)-1;
				ir4_daug1 = list_of_resonances.at(4).i_daug.at(0)-1;
				ir4_daug2 = list_of_resonances.at(4).i_daug.at(1)-1;
				// sum over R4 spin projections
				for(double m_Mr4=-list_of_resonances.at(4).jr; m_Mr4<=list_of_resonances.at(4).jr+0.2; m_Mr4++){
					if(list_of_resonances.at(4).is_rad){
						R4Amp = angDist::multipoleDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),list_of_resonances.at(4).sr,qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).type);
					}
					else{
						R4Amp = angDist::ampDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),qn_daug[ir4_daug2].at(0),qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).lr,list_of_resonances.at(4).sr);
					}	
				// sum over R4 spin projections
					for(double m_Mr3=-list_of_resonances.at(3).jr; m_Mr3<=list_of_resonances.at(3).jr+0.2; m_Mr3++){
						if(list_of_resonances.at(3).is_rad){
							R3Amp = angDist::multipoleDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),list_of_resonances.at(3).sr,qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).type);	
						}
						else{
							R3Amp = angDist::ampDecay(p4[ir3_daug1],p4[ir3_daug2],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_daug1].at(0),qn_daug[ir3_daug1].at(1),qn_daug[ir3_daug2].at(0),qn_daug[ir3_daug2].at(1),list_of_resonances.at(3).lr,list_of_resonances.at(3).sr);	
						}
						// sum over R2 spin projections
						for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2; m_Mr2++){
							R2Amp = angDist::ampDecay(p4R[2],p4R[3],p4Rec[1],p4Ref[1],list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(4).jr,m_Mr4,list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);	
							// sum over R1 spin projections
							for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2; m_Mr1++){
								if(list_of_resonances.at(1).is_rad){
									R1Amp = angDist::multipoleDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(1).sr,qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).type);
								}
								else{
									R1Amp = angDist::ampDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_recoil].at(0),qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
								}
								if(list_of_resonances.at(0).is_rad){
									MotherAmp = angDist::multipoleProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(0).sr,qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).type);
								}
								else{
									MotherAmp = angDist::ampProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,qn_daug[ir1_recoil].at(0),qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
								}

								totAmp += MotherAmp*R1Amp*R2Amp*R3Amp*R4Amp;
							}
						}
					}
				}
				// not tested yet
				break;												
				
			case 10:
				/// full amplitude for e+ e- ---> psi(mother) ---> R1 + A1
				/// 										with   R1 ---> R2 + B1
				///											with   R2 ---> R3 + A2
				///											with   R3 ---> R4 + B2
				///											with   R4 ---> A3 + B3	
				ir1_recoil = list_of_resonances.at(1).i_rec.at(0)-1;
				ir2_recoil = list_of_resonances.at(2).i_rec.at(0)-1;
				ir3_recoil = list_of_resonances.at(3).i_rec.at(0)-1;
				ir4_recoil = list_of_resonances.at(4).i_rec.at(0)-1;
				ir4_daug1 = list_of_resonances.at(4).i_daug.at(0)-1;
				ir4_daug2 = list_of_resonances.at(4).i_daug.at(1)-1;

				// sum over R4 spin projections
				for(double m_Mr4=-list_of_resonances.at(4).jr; m_Mr4<=list_of_resonances.at(4).jr+0.2; m_Mr4++){
					if(list_of_resonances.at(4).is_rad){
						R4Amp = angDist::multipoleDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),list_of_resonances.at(4).sr,qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).type);
					}
					else{
						R4Amp = angDist::ampDecay(p4[ir4_daug1],p4[ir4_daug2],p4Rec[3],p4Ref[3],list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_daug1].at(0),qn_daug[ir4_daug1].at(1),qn_daug[ir4_daug2].at(0),qn_daug[ir4_daug2].at(1),list_of_resonances.at(4).lr,list_of_resonances.at(4).sr);
					}
					// sum over R3 spin projections
					for(double m_Mr3=-list_of_resonances.at(3).jr; m_Mr3<=list_of_resonances.at(3).jr+0.2; m_Mr3++){
						if(list_of_resonances.at(3).is_rad){
							R3Amp = angDist::multipoleDecay(p4R[3],p4Rec[3],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(4).jr,m_Mr4,list_of_resonances.at(3).sr,qn_daug[ir4_recoil].at(1),list_of_resonances.at(3).type);
						}
						else{
							R3Amp = angDist::ampDecay(p4R[3],p4Rec[3],p4Rec[2],p4Ref[2],list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(4).jr,m_Mr4,qn_daug[ir4_recoil].at(0),qn_daug[ir4_recoil].at(1),list_of_resonances.at(3).lr,list_of_resonances.at(3).sr);
						}
						// sum over R2 spin projections
						for(double m_Mr2=-list_of_resonances.at(2).jr; m_Mr2<=list_of_resonances.at(2).jr+0.2; m_Mr2++){
							if(list_of_resonances.at(2).is_rad){
								R2Amp = angDist::multipoleDecay(p4R[2],p4Rec[2],p4Rec[1],p4Ref[1],list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(3).jr,m_Mr3,list_of_resonances.at(2).sr,qn_daug[ir3_recoil].at(1),list_of_resonances.at(2).type);
							}
							else{
								R2Amp = angDist::ampDecay(p4R[2],p4Rec[2],p4Rec[1],p4Ref[1],list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(3).jr,m_Mr3,qn_daug[ir3_recoil].at(0),qn_daug[ir3_recoil].at(1),list_of_resonances.at(2).lr,list_of_resonances.at(2).sr);
							}
							// sum over R1 spin projections
							for(double m_Mr1=-list_of_resonances.at(1).jr; m_Mr1<=list_of_resonances.at(1).jr+0.2; m_Mr1++){
								if(list_of_resonances.at(1).is_rad){
									R1Amp = angDist::multipoleDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,list_of_resonances.at(1).sr,qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).type);
								}
								else{
									R1Amp = angDist::ampDecay(p4R[1],p4Rec[1],p4Rec[0],p4Ep,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(2).jr,m_Mr2,qn_daug[ir2_recoil].at(0),qn_daug[ir2_recoil].at(1),list_of_resonances.at(1).lr,list_of_resonances.at(1).sr);
								}
								if(list_of_resonances.at(0).is_rad){
									MotherAmp = angDist::multipoleProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,list_of_resonances.at(0).sr,qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).type);
								}
								else{
									MotherAmp = angDist::ampProd(p4R[0],p4Rec[0],p4Ep,list_of_resonances.at(0).jr,list_of_resonances.at(0).mr,list_of_resonances.at(1).jr,m_Mr1,qn_daug[ir1_recoil].at(0),qn_daug[ir1_recoil].at(1),list_of_resonances.at(0).lr,list_of_resonances.at(0).sr);
								}

								totAmp += MotherAmp*R1Amp*R2Amp*R3Amp*R4Amp;
							}
						}
					}
				}
				// not tested yet
				break;
		}	

	if(TMath::IsNaN(totAmp.real()) || TMath::IsNaN(totAmp.imag())){
		cout << "error, angular amplitude is NaN" << endl;
		exit(0);
	}

	userVars[kReal] = totAmp.real();
	userVars[kImag] = totAmp.imag();

}

#ifdef GPU_ACCELERATION
void
anglePhiFJ::launchGPUKernel( dim3 dimGrid, dim3 dimBlock, GPU_AMP_PROTO ) const {
  
  anglePhiFJ_exec( dimGrid,  dimBlock, GPU_AMP_ARGS, 
                       m_Mjpsi, m_l, m_s, m_daughter1, m_daughter2 );

}
#endif //GPU_ACCELERATION
