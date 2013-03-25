
/* (c)  oblong industries */

#include "Greenhouse.h"


const Str holder_text ("I'm the Parent.\nDrag me or my kids.");


//  KidHolder is a Text object, but is also acts as a container for 
//  other objects.  It handles click-n-drag user events.
class KidHolder  :  public Text
{ public:

  KidHolder ()  :  Text (holder_text)
    { }

  void PointingInsideHarden (PointingEvent *e)
    { //  If already currently tracking (heeding) some other mouse or 
      //  pointer source, we'd ignore this one.
      if (IsHeedless ())
        { Heed (e);
          SetString ("heeding '" + e -> Provenance () + "'");
          SetAdjColor (Color (1.0, 0.0, 0.0));  //  turn red
        }
    }

  void PointingMove (PointingEvent *e)
    { //  If this move event is from the same source we're already heeding,
      //  move along with it.  
      //  We only start heeding on harden events.
      //  We only stop heeding on soften events.
      if (IsHeeding (e))
        { //  IntersectionDiff tells us how far between 
          //     (a) the place where this pointing event intersects the plane
          //     (b) the last such point from the same source
          //  Optional 3rd argument is the plane; here, the main feld
          //  Loc() is the location of this, the KidHolder object
          Vect delta = IntersectionDiff (e, Loc ()); 
          IncTranslation (delta);
        }
    }

  void PointingSoften (PointingEvent *e)
    { if (IsHeeding (e))
        { StopHeeding (e);
          SetString (holder_text);
          SetAdjColor (Color (1.0, 1.0, 1.0));
        }
    }

  //  Gets triggered on move, harden, or soften within any kid
  void PointerWithinKid (PointingEvent *e, Node *kid)
    { //  The tag lets us watch one kid and ignore the other one.
      if (kid -> HasTag ("watchme"))
        { //  Check to see if event source is in a 'hardened' (clicked) state
          if (IsHardened (e))
            PointingInsideHarden (e);
          else
            PointingSoften (e);
        }
    }
};


void Setup ()
{ KidHolder *kh = new KidHolder ();
  //  put at the same location as the Feld('s center), & match its orientation
  kh -> SlapOnFeld ();   

  //  Make some generic text objects and add them as kids of KidHolder
  Text *kidA = new Text ("I'm the clickable kid");
  kidA -> Tag ("watchme");    //  This one is tagged; the other kid is not
  kidA -> SetFontSize (Feld () -> Height () / 40.0);
  kidA -> IncTranslation (Vect (Feld () -> Width () * -0.25,
                                 Feld () -> Height () * -0.25, 0));
  kh -> AppendKid (kidA);

  Text *kidB = new Text ("I'm not");
  kidB -> SetFontSize (Feld () -> Height () / 40.0);
  kidB -> IncTranslation (Vect (Feld () -> Width () * 0.25,
                                 Feld () -> Height () * -0.25, 0));
  kh -> AppendKid (kidB);
}
