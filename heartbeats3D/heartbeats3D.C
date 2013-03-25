
/* (c)  oblong industries */

#include "Greenhouse.h"


//  Hearts inherit from Image, and add interactivity
//  behavior.  They also listen for protein messages.
class Heart  :  public Image
{ public:

  int64 my_time_to_shine;
  float64 original_diag;

  Heart (const Str &image_path, float64 height)  :  Image (image_path)
    { SetAlphaHard (0.25);
      ColorAnimateQuadratic (0.5);    //  When color changes, it will animate
                                      //  (unless we use "hard" option)

      SetHeightUniformly (height);    //  Keep height/width proportional
      original_diag = Size ();

      //  Every time the wall clock says 00::XX seconds, we'll light up, for fun
      my_time_to_shine = Random (9.0, 60.0);
      
      //  This object will listen to a message pool called 'love'.
      ParticipateInPool ("love");
    }

  //  Any protein message that appears in the pool called 'love'
  //  and has 'greenhouse' as one of its descrips will be delivered here.
  //  Internally, we send one of these proteins in PointingSoften().
  //  The protein could also be sent externally, e.g. from the command line:
  //      poke -d greenhouse -d beat -i sender-loc:"[-55,961,-700]" love
  //           ^^^^^^^^^^^^^ ^^^^^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^  ^^^^
  //            a descrip    one more    payload: key/value pair    pool name
  //  Watch pool traffic with
  //      peek love
  //  (No pool yet?)
  //      p-create love
  void Metabolize (const Protein &p)
    { //INFORM ("received protein: " + PROTEIN (p));
      if (HasDescrip (p, "beat"))
        { Vect sender_loc = Ingest <Vect> (p, "sender-loc");
          
          //  Set a single-use timer that will go off later
          //  the farther we are from the source
          float64 dist  = Loc () . DistFrom (sender_loc) / Diag (Feld ());
          float64 delay = Range (dist, 0.0, 5.0, 0.0, 5.0);
          SetFireTimer (delay, 1);
        }
    }

  //  When the fire timer goes off, this is called.
  void Fired ()
    { Heartbeat (0.66);     //  Heart throbs.
      SetAlphaHard (1.0);   //  ....goes (non-animatedly) opaque
      SetAlpha (0.3);       //  ...starts animating back to dim
    }

  void PointingMoveEnter (PointingEvent *e)   
    { Tag ("hovered");  }
  
  void PointingMoveExit  (PointingEvent *e)   
    { UnTag ("hovered"); 
      SetAlpha (0.3);
    }

  void PointingInsideHarden (PointingEvent *e)
    { if (IsHeedless ())
        Heed (e);
      SetSize (original_diag / 2);
    }

  void PointingSoften (PointingEvent *e)
    { //  Ensures that the ptr that's softening matches the hardened one
      if (IsHeeding (e))      
        { SetSize (original_diag);
          StopHeeding (e);

          //  Make a protein message & drop it into the 'love' pool.
          //  This will trigger every all hearts' Metabolize() (& ours)
          //  The 'greenhouse' descrip is included automatically.
          Protein p = ProteinWithDescrip ("beat");
          AppendIngest (p, "sender-loc", Loc ());
          Deposit (p, "love");      
        }
    }

  void Travail ()
    { //  If currently hovered over, we go bright & stay bright
      if (HasTag ("hovered"))
        { SetAlphaHard (1.0); }

      //  Otherwise, if it's our time to randomly shine, do that
      else if (CurTime () > 1  &&
               (int64)CurTime () % my_time_to_shine == 0)
        { SetAlphaHard (1.0);       //  ....goes (non-animatedly) opaque.
          SetAlpha (0.3);           //  ...starts animating back to dim
        }
    }
};


//  A class that inherits from Thing (which has no visual appearance
//  of its own -- just a container).  GridSystem manages
//  pointing interactions with elements in the grid
class GridSystem  :  public Thing
{ public:

  Dictionary <Str, Text*> labels;
  float64 offset;

  GridSystem ()  :  Thing (), 
      offset (0.03 * Feld () -> Height ())
    { float64 o_step = Feld () -> Width  () / 10.0;
      float64 u_step = Feld () -> Height () / 5.0;
      float64 n_step = Feld () -> Height () / 1.85;

      // Size each heart to allow for 10 hearts across plus a 
      // buffer of 25% of the heart's width between each
      float64 heart_height = Feld () -> Width () / 12.75;

      for (float64 over = - ((Feld () -> Width ()) / 2.0)
             ;  over < ((Feld () -> Width ()) / 2.0)
             ;  over += o_step)
        for (float64 up = - ((Feld () -> Height ()) / 2.0)
               ;  up < ((Feld () -> Height ()) / 2.0)
               ;  up += u_step)
          for (float64 norm = 0.0
                 ;  norm > - 6.0 * Feld () -> Height ()
                 ;  norm -= n_step)
            { Heart *g = new Heart ("images/heart.png", heart_height);
              g -> SetTranslation (over * Feld () -> Over () +
                                   up   * Feld () -> Up () +
                                   norm * Feld () -> Norm ());
              AppendKid (g);
            }

    }

  //  Gets triggered on move, harden, or soften within any kid
  void PointerWithinKid (PointingEvent *e, Node *kid) 
    { //  Find a label for this event source (mouse pointer, wiimote, etc).
      //  If one doesn't already exist, create it.  
      Text *label = labels . Get (e -> Provenance ());
      if (! label)
        { label = new Text (Name ());
          label -> SetFontSize (Feld () -> Height () / 30.0);
          label -> SetObliviousness (true);
          label -> ColorAnimateQuadratic (2.0);
          labels . Put (e -> Provenance (), label);
        }

      //  Put the kid's name on the label 
      //  Name() is part of the LocusNode class, not the Node class, so we cast
      label -> SetString (kid -> AsLocus () -> Name ());

      label -> SetAlphaHard (1);  //  ....goes (non-animatedly) opaque.
      label -> SetAlpha (0);      //  ...starts animating back to invisible

      //  Position the label
      Vect loc = Intersection (e) +
                 Vect (offset + label -> Width () / 2.0,
                       offset + label -> Height () / 2.0,
                       0.0);
      label -> SetTranslation (loc);
    }
};


void Setup ()
{ GridSystem *g = new GridSystem ();

  // The whole grid needs to be shifted up and over by half a column and row
  Vect grid_offset = Vect (Feld () -> Width () / 20.0,
                           Feld () -> Height () / 11.0,
                           0.0);
  g -> SetTranslation (Feld () -> Loc () + grid_offset);
}
