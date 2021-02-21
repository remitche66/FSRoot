#include "FSAmpToolsAmp/fitConfig.h"

pair<string,string> fitConfig::divide(string decay){
    decay = decay.substr(1,decay.size()-2);
    int copen=0;
    int cclose=0;
    //cout << __FILE__ << " input: " << decay << endl;
	for(string::size_type i=0;i<decay.size();i++){
        if(decay.substr(i,1).compare("(")==0)
            copen++;
        if(decay.substr(i,1).compare(")")==0)
            cclose++;

        if(decay.substr(i,1).compare("{")==0)
            copen++;
        if(decay.substr(i,1).compare("}")==0)
            cclose++;
        
        
        // one ( left open
        if(copen-cclose==0){
            string left_half = decay.substr(0,i+1);
            string right_half = decay.substr(i+2,decay.size()-(left_half.size()+1));
            pair<string,string> ret = make_pair(left_half,right_half);
            //cout << __FILE__ << " returning: " << ret.first << " " << ret.second << endl;
            return ret;
        }
	}
    return make_pair("error","error");
}

vector<res> fitConfig::decayTree(string decay){
    bool m_debug = false;
    int n_fst = count(decay.begin(),decay.end(),',')+1;
    int n_res = count(decay.begin(),decay.end(),'(')+count(decay.begin(),decay.end(),'{');
    vector<pair<string,string>> mypair;

    int count_res = 0;
    int count_fst = 0;
    vector<res> resonances;

    for(unsigned int i=0;count_fst<n_fst;i++){
        if(!count_res){
            pair<string,string> temp_pair = fitConfig::divide(decay);
            if(temp_pair.second.size()>temp_pair.first.size())
                temp_pair = make_pair(temp_pair.second,temp_pair.first);
            mypair.push_back(temp_pair);
            count_res++;
            res resdummy;
            resdummy.index = i;
            vector<int> daug0;
            int ndaug0=0;
            for(string::size_type c=0;c<decay.size();c++){
                if(decay.substr(c,1).compare("(")
                && decay.substr(c,1).compare(")")
                && decay.substr(c,1).compare(",")
                && decay.substr(c,1).compare("{")
                && decay.substr(c,1).compare("}")){
                    ndaug0++;
                    daug0.push_back(atoi(decay.substr(c,1).c_str()));
                }
            }            
            resdummy.n_daug = ndaug0;
            resdummy.i_daug = daug0;
            resdummy.mother_index = -1;
            resdummy.n_ref = -1;
            resdummy.n_rec = -1;
            if(decay.substr(0,1).compare("{")==0 && decay.substr(decay.size()-1,1).compare("}")==0)
                resdummy.is_rad = true;
            else
                resdummy.is_rad = false;
            resonances.push_back(resdummy);
        }
        else{
            if(mypair.at(i-1).first.size()>1){
                pair<string,string> temp_pair = fitConfig::divide(mypair.at(i-1).first);
                if(temp_pair.second.size()>temp_pair.first.size())
                    temp_pair = make_pair(temp_pair.second,temp_pair.first);
                mypair.push_back(temp_pair);
                count_res++;
                res resdummy;
                int ndaug=0;
                vector<int> daug;
                vector<int> recoil;
                for(string::size_type c=0;c<mypair.at(i-1).first.size();c++){
                    if(mypair.at(i-1).first.substr(c,1).compare("(")
                    && mypair.at(i-1).first.substr(c,1).compare(")")
                    && mypair.at(i-1).first.substr(c,1).compare(",")){
                        ndaug++;
                        daug.push_back(atoi(mypair.at(i-1).first.substr(c,1).c_str()));
                    }
                }
                for(string::size_type c=0;c<mypair.at(i-1).second.size();c++){
                    if(mypair.at(i-1).second.substr(c,1).compare("(")
                    && mypair.at(i-1).second.substr(c,1).compare(")")
                    && mypair.at(i-1).second.substr(c,1).compare(",")){
                        recoil.push_back(atoi(mypair.at(i-1).second.substr(c,1).c_str()));
                    }
                }
                if(mypair.at(i-1).first.substr(0,1).compare("{")==0 && mypair.at(i-1).first.substr(mypair.at(i-1).first.size()-1,1).compare("}")==0)
                    resdummy.is_rad = true;
                else
                    resdummy.is_rad = false;
                resdummy.index=resonances.size();
                resdummy.i_daug=daug;
                resdummy.i_rec=recoil;
                resdummy.n_daug = ndaug;
                resdummy.n_rec = recoil.size();
                // loop through the existing list
                // and find the entry for which list of daughters
                // is equal to list of daughters + list of recoils of this one
                // ---> this is then the mother particle
                for(unsigned int r = 0;r<resonances.size();r++){
                    bool found_all = true;
                    if(resonances.at(r).n_daug != resdummy.n_daug + resdummy.n_rec)
                        continue;
                    for(unsigned int d=0;d<daug.size();d++){
                        std::vector<int>::iterator it;
                        it = find(resonances.at(r).i_daug.begin(),resonances.at(r).i_daug.end(),daug.at(d));
                        if(it == resonances.at(r).i_daug.end()){
                            found_all = false;
                        }
                    }
                    for(unsigned int d=0;d<recoil.size();d++){
                        std::vector<int>::iterator it = find(resonances.at(r).i_daug.begin(),resonances.at(r).i_daug.end(),recoil.at(d));
                        if(!(it != resonances.at(r).i_daug.end())){
                            found_all = false;
                        }
                    }
                    // if still true here, we can end the loop because we found the mother particle
                    if(found_all){
                        // save mother index
                        // also save recoil of the mother particle as reference vector
                        resdummy.mother_index = resonances.at(r).index;
                        if(resdummy.mother_index==0)
                            resdummy.n_ref = -1;
                        else{
                            resdummy.n_ref = resonances.at(r).i_rec.size();
                            resdummy.i_ref = resonances.at(r).i_rec;
                        }
                        break;    
                    }      
                }

                resonances.push_back(resdummy);
            }
            else{
                count_fst++;
            }
            if(mypair.at(i-1).second.size()>1){
                pair<string,string> temp_pair = fitConfig::divide(mypair.at(i-1).second);
                if(temp_pair.second.size()>temp_pair.first.size())
                    temp_pair = make_pair(temp_pair.second,temp_pair.first);
                mypair.push_back(temp_pair);
                count_res++;
                res resdummy;
                int ndaug=0;
                vector<int> daug;
                vector<int> recoil;
                for(string::size_type c=0;c<mypair.at(i-1).second.size();c++){
                    if(mypair.at(i-1).second.substr(c,1).compare("(")
                    && mypair.at(i-1).second.substr(c,1).compare(")")
                    && mypair.at(i-1).second.substr(c,1).compare(",")){
                        ndaug++;
                        daug.push_back(atoi(mypair.at(i-1).second.substr(c,1).c_str()));
                    }
                }
                for(string::size_type c=0;c<mypair.at(i-1).first.size();c++){
                    if(mypair.at(i-1).first.substr(c,1).compare("(")
                    && mypair.at(i-1).first.substr(c,1).compare(")")
                    && mypair.at(i-1).first.substr(c,1).compare(",")){
                        recoil.push_back(atoi(mypair.at(i-1).first.substr(c,1).c_str()));
                    }
                }
                resdummy.index=resonances.size();

                if(mypair.at(i-1).second.substr(0,1).compare("{")==0 && mypair.at(i-1).second.substr(mypair.at(i-1).second.size()-1,1).compare("}")==0)
                    resdummy.is_rad = true;
                else
                    resdummy.is_rad = false;
                resdummy.i_daug=daug;
                resdummy.i_rec=recoil;
                resdummy.n_daug = ndaug;
                resdummy.n_rec = recoil.size();
                // loop through the existing list
                // and find the entry for which list of daughters
                // is equal to list of daughters + list of recoils of this one
                // ---> this is then the mother particle
                for(unsigned int r = 0;r<resonances.size();r++){
                    bool found_all = true;
                    if(resonances.at(r).n_daug != resdummy.n_daug + resdummy.n_rec)
                        continue;
                    for(unsigned int d=0;d<daug.size();d++){
                        std::vector<int>::iterator it = find(resonances.at(r).i_daug.begin(),resonances.at(r).i_daug.end(),daug.at(d));
                        if(!(it != resonances.at(r).i_daug.end())){
                            found_all = false;
                        }
                    }
                    for(unsigned int d=0;d<recoil.size();d++){
                        std::vector<int>::iterator it = find(resonances.at(r).i_daug.begin(),resonances.at(r).i_daug.end(),recoil.at(d));
                        if(!(it != resonances.at(r).i_daug.end())){
                            found_all = false;
                        }
                    }
                    // if still true here, we can end the loop because we found the mother particle
                    if(found_all){
                        resdummy.mother_index = resonances.at(r).index;
                        if(resdummy.mother_index==0)
                            resdummy.n_ref = -1;
                        else{
                            resdummy.n_ref = resonances.at(r).i_rec.size();
                            resdummy.i_ref = resonances.at(r).i_rec;
                        }
                        break;    
                    }            
                }

                resonances.push_back(resdummy);
            }
            else{
                count_fst++;
            }
        }
    }

  if(m_debug){
    for(unsigned int r=0;r<resonances.size();r++){
        cout << "res number: " << resonances.at(r).index << " " << resonances.at(r).mother_index << endl;
        cout << "n_daug: " << resonances.at(r).n_daug << endl;
        cout << "daughters: " << endl;
        for(int i=0;i<resonances.at(r).n_daug;i++){
            cout << resonances.at(r).i_daug.at(i) << " ";
        }
        cout << endl << "recoil: " << endl;
        for(int i=0;i<resonances.at(r).n_rec;i++){
            cout << resonances.at(r).i_rec.at(i) << " ";
        }
        cout << endl << "ref: " << endl;
        for(int i=0;i<resonances.at(r).n_ref;i++){
            cout << resonances.at(r).i_ref.at(i) << " ";
        }
        cout << endl;
    }
  }
  return resonances;
}

void fitConfig::printDecayTree(vector<res> tree){
    cout << "this is the decay tree:" << endl;
    for(unsigned int r=0;r<tree.size();r++){
        cout << "resonance: " << tree.at(r).index << " from mother with index " << tree.at(r).mother_index << endl;
        cout << "decays to daughters: " << endl;
        for(int i=0;i<tree.at(r).n_daug;i++){
            cout << tree.at(r).i_daug.at(i) << " ";
        }
        cout << "is radiative decay: " << tree.at(r).is_rad << endl;
        cout << endl << "with recoil: " << endl;
        for(int i=0;i<tree.at(r).n_rec;i++){
            cout << tree.at(r).i_rec.at(i) << " ";
        }
        cout << endl << "and reference vector: " << endl;
        for(int i=0;i<tree.at(r).n_ref;i++){
            cout << tree.at(r).i_ref.at(i) << " ";
        }
        if(r==0){
          cout << "arbitrary" << endl;
        }
        else if(tree.at(r).n_ref<1){
          cout << "beam vector" << endl;
        }
        else
          cout << endl;
    }
}