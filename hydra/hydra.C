
/* (c)  oblong industries */

#include "Greenhouse.h"

/*
  hydra

  Hydra is a graphical front-end for two other, long-running
  'worker' programs: imago-worker and wit-worker.

  Instructions:

  1. Build and run imago-worker -- leave it running.
     (Optionally, open a terminal and 'peek imago' to
     observe the pool traffic.)

  2. Run the wit-worker Ruby program -- leave it running.
     (Optionally, open a terminal and 'peek witter' to
     observe the pool traffic.)

  3. Build and run hydra

  4. When you click the text object in hydra, it sends
     a request to wit-worker to make a request to Twitter.
     The results of that request are forwarded back to hydra.

  5. When you click the image object in hydra, it sends
     a request to imago-worker for an image.  The image
     is packed into a protein, which hydra receives and
     displays.

 */


//  An object that puts request proteins into the 'imago' pool,
//  when clicked upon.
//
//  When a response protein arrives on the same pool, displays
//  the image that is contained in that protein.
//
class Imago  :  public Image
{ public:

  Imago (Str filename)  :  Image (filename)
    { SetCornerRadius (0);
      SlapOnFeld ();
      SpaceFeld *f = Feld ();
      SetHeightUniformly (f -> Height () / 2);
      IncTranslation (Vect (f -> Width () * 0.25,
                            f -> Height () * 0.2, 0));

      ParticipateInPool ("imago");
      ListenForDescrip ("image-result");
    }

  void SendImageRequest ()
    { Deposit (ProteinWithDescrip ("image-request"), "imago"); }

  void PointingInsideHarden (PointingEvent *e)
    { Heed (e);
      SendImageRequest ();
      SetAlpha (0.75);
    }

  void PointingMoveEnter (PointingEvent *e)
    { SetAlpha (0.95); }

  void PointingMoveExit (PointingEvent *e)
    { if (IsHeedless ())
        SetAlpha (1.0);
    }

  void PointingInsideSoften (PointingEvent *e)
    { if (IsHeeding (e))
        { SetAlpha (0.95);
          StopHeeding (e);
        }
    }

  void PointingOutsideSoften (PointingEvent *e)
    { if (IsHeeding (e))
        { StopHeeding (e);
          SetAlpha (1.0);
        }
    }

  //  Pressing 'i' sends an image request
  void Blurt (BlurtEvent *e)
    { if (Utters (e, "i"))
        SendImageRequest ();
    }

  void Metabolize (const Protein &p)
    { //INFORM ("Result has ingest 'image-texture'? " +
      //         BOOL (HasIngest (p, "image-texture")));
      SetImage (ImageDataFromIngest (p, "image-texture"));
    }
};


//  An object that puts request proteins into the 'witter' pool,
//  when clicked upon.
//
//  When a response protein arrives on the same pool, displays
//  the text payload that is contained in that protein.
//
class Witter  :  public Text
{ public:

  bool waiting;

  Witter ()  :
      Text ("Click me."),
      waiting (false)
    { SetBackingColor (Color (0, 0, 0, 0.5));
      SlapOnFeld ();
      SpaceFeld *f = Feld ();
      SetFontSize (f -> Height () / 36.0);
      IncTranslation (Vect (f -> Width () * -0.25,
                            f -> Height () * -0.25, 0));

      ParticipateInPool ("witter");
      ListenForDescrip ("wit-result");
    }

  void SendWitRequest (float64 argument)
    { SetString ("Sending request to wit worker");
      waiting = true;
      Protein p = ProteinWithDescrip ("wit-request");
      AppendIngest (p, "argument", argument);
      Deposit (p, "witter");
    }

  void PointingInsideHarden (PointingEvent *e)
    { if (! waiting)
        SendWitRequest (1134.78);
      else
        { if (String () . Matches ("^Patience"))
            SetString (String () + ".");
          else
            SetString ("Patience!  Still waiting for result.");
        }
    }

  //  Pressing 'w' sends a wit request
  void Blurt (BlurtEvent *e)
    { if (Utters (e, "w"))
        SendWitRequest (3.14);
    }

  void Metabolize (const Protein &p)
    { //  We expect to get a slaw list from wit-worker.rb
      Slaw trends_list = p . Ingests () . Find ("trends");
      // INFORM ("Received: \n" + SLAW (trends_list));

      Str to_show = "Current Twitter trends: \n\n";
      for (int64 i = 0 ; i < trends_list . Count () ; i++)
        to_show += trends_list[i] . Find ("name") . ToPrintableString () + "\n";

      SetString (to_show);
      waiting = false;
    }
};


void Setup ()
{ new Imago ("images/heart.png");
  new Witter ();
}
