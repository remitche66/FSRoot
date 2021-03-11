#if !defined(FSCANVAS_H)
#define FSCANVAS_H

using namespace std;


class FSCanvas{

  public:

      // ********************************************************
      // DIVIDE A CANVAS INTO NUM PIECES
      // ********************************************************

    static void divideGPad(int num);

    static int divideGPadX(int num);
    static int divideGPadY(int num);


      // ********************************************************
      // CHANGE COLOR SCHEMES
      // ********************************************************

    static void colorScheme(int i);
    static void testColorScheme(int i);


      // ********************************************************
      //  DRAW AN ARROW (size and length are in percent of canvas width)
      // ********************************************************

    static void drawCutArrow(double x, 
                               int color = 1, TString type = "<|", 
                               double size = 0.03, double length = 0.06);


};

#endif
