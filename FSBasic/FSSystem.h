#if !defined(FSSYSTEM_H)
#define FSSYSTEM_H

#include <utility>
#include <vector>
#include <string>
#include <map>
#include "TString.h"

using namespace std;


class FSSystem{

  public:

      // **********************************
      //   MAKE AN ABSOLUTE PATH NAME
      //    -- expands environment variables
      //    -- the path doesn't have to exist
      // **********************************

    static TString makeAbsolutePathName(TString path);

      // **********************************
      //   GET AN ABSOLUTE PATH TO A FILE OR DIRECTORY
      //    -- expands environment variables
      //    -- checks wildcards (* and ?), but doesn't expand them
      //    -- returns empty if the path doesn't exist
      // **********************************

    static TString getAbsolutePath(TString path);

      // **********************************
      //   GET A VECTOR OF ABSOLUTE PATHS TO FILES OR DIRECTORIES
      //    -- expands environment variables
      //    -- expands wildcards (* and ?)
      //    -- returns existing paths
      // **********************************

    static vector<TString> getAbsolutePaths(TString path, bool show = false);




  private:

      // ****************************
      //   HELPER FUNCTIONS
      // ****************************

    static vector<TString> getDirectoryContents(TString directory, TString pattern, bool show = false);
    static vector<TString> getDirectoryStructure(TString path, bool show = false);
    static vector<TString> getAbsolutePathsWildcards(TString path, bool show = false);

};

#endif