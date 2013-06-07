
#include "Greenhouse.h"

class BobRoss  :  public Turtle {
private:
  Vect last;
public:
  BobRoss ()
  { for (int i = 0; i < 4; ++i)
      { for (int j = 0; j < 4; ++j)
          { SetStrokeColor (Color (i * 0.25, .5, j * 0.25));
            SetFillColor (Color (.5, j * 0.25, i * .25));
            DrawRect (Vect (40.0 + -20.0 * i, 40.0 + -20.0 * j, 0.0), 15, 15);
          }
      }
  }

  virtual void PointingMove (PointingEvent *e)
  { SetStrokeColor (Color (1.0, .9));
    SetFillColor (Color (.49, .49, .59, .4));
    DrawEllipse (Intersection (e) - Loc (), 5.0, 5.0);
  } 

  virtual void Blurt (BlurtEvent *evt)
  { if (Utters (evt, "C"))
      Clear ();
  }
};

void Setup ()
{ (new BobRoss) -> SlapOnFeld (); }
