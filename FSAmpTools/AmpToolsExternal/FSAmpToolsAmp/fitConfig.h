#if !defined(FITCONFIG)
#define FITCONFIG


#include <string>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <cctype>
#include <vector>

#pragma once
using namespace std;

struct res{
  int index;
  int n_daug;
  int n_rec;
  int n_ref;
  int mother_index;
  vector<int> i_daug;
  vector<int> i_rec;
  vector<int> i_ref;
  // quantum numbers
  double jr;
  double mr;
  double lr;
  double sr;
  bool is_rad;
  string type;
};


class fitConfig {
  public:
    static pair<string,string> divide(string decay);
    static vector<res> decayTree(string decay);
    static void printDecayTree(vector<res> tree);
  private:
};

#endif
