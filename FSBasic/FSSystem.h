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

    static TString getAbsolutePath(TString path, bool useCache = false);

      // **********************************
      //   GET A VECTOR OF ABSOLUTE PATHS TO FILES OR DIRECTORIES
      //    -- expands environment variables
      //    -- expands wildcards (* and ?)
      //    -- recognizes lists of files separated by " " or ","
      //    -- returns existing paths
      // **********************************

    static vector<TString> getAbsolutePaths(TString path, bool useCache = false, bool show = false);

      // **********************************
      //   CHECK IF PATH POINTS TO A PROPER ROOT FILE (OR FILES)
      // **********************************

    static bool checkRootFormat(TString path, bool useCache = false);

      // **********************************
      //   RETURN THE HOST NAME
      // **********************************

    static TString getHostName();

      // **********************************
      //   REPLACE THE BASE OF A PATH WITH A NEW BASE
      //    -- make a new path called newBaseName/oldPath
      //    -- if that points to an existing file or directory, return it
      //    -- if not, remove the lowest directory from oldPath and repeat
      //    -- keep shortening oldPath until an existing path is found, or return empty
      // **********************************

    static TString replaceBaseName(TString newBaseName, TString oldPath, bool useCache = false);


  private:

      // ****************************
      //   HELPER FUNCTIONS
      // ****************************

    static vector<TString> getDirectoryContents(TString directory, TString pattern, bool show = false);
    static vector<TString> getDirectoryStructure(TString path, bool show = false);
    static vector<TString> getAbsolutePathsWildcards(TString path, bool show = false);

    static map< TString, TString >  m_cachePath;
    static map< TString, vector<TString> >  m_cachePaths;
    static map< TString, bool >  m_cacheRoot;

};

#endif
