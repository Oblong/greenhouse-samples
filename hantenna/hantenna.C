/* (c)  Oblong Industries */

#include "Greenhouse.h"


class GestureDetector  :  public Image
{
public:

  Str current_gesture;

  GestureDetector (const Str &s, const Str &g)  :  Image (s)
    { SetName ("GEST_DETECTOR");
      SetHeightUniformly (100);
      SetCornerRadius (0);
      current_gesture = g;
    }

  void Blurt (BlurtEvent *e)
    { if (Utters (e, "\\/\\/-") || Utters (e, "a"))
        { SetImage (FetchImageData ("images/hantenna/h_VV-.png"));
          current_gesture = "\\/\\/-";
        }
      if (Utters (e, "^^\\/>") || Utters (e, "s"))
        { SetImage (FetchImageData ("images/hantenna/h_^^V>.png"));
          current_gesture = "^^\\/>";
        }
      if (Utters (e, "||||-") || Utters (e, "d"))
        { SetImage (FetchImageData ("images/hantenna/h_||||-.png"));
          current_gesture = "||||-";
        }
      if (Utters (e, "^^^|-") || Utters (e, "f"))
        { SetImage (FetchImageData ("images/hantenna/h_^^^|-.png"));
          current_gesture = "^^^|-";
        }
      if (Utters (e, "^^^|>") || Utters (e, "g"))
        { SetImage (FetchImageData ("images/hantenna/h_^^^|>.png"));
          current_gesture = "^^^|>";
        }
      if (Utters (e, "^^^^>") || Utters (e, "h"))
        { SetImage (FetchImageData ("images/hantenna/h_^^^^>.png"));
          current_gesture = "^^^^>";
        }
      if (Utters (e, "_____") || Utters (e, "j"))
        { SetImage (FetchImageData ("images/hantenna/h______.png"));
          current_gesture = "_____";
        }
    }
};


class GestureTarget  :  public Image
{
public:

  Str current_gesture;

  Dictionary <Str, Str> gestures; //create a Trove

  GestureTarget (const Str &s, const Str &g)  :  Image (s)
    { gestures . Put ("\\/\\/-", "images/hantenna/h2_VV-.png");
      gestures . Put ("^^\\/>",  "images/hantenna/h2_^^V>.png");
      gestures . Put ("||||-",   "images/hantenna/h2_||||-.png");
      gestures . Put ("^^^|-",   "images/hantenna/h2_^^^|-.png");
      gestures . Put ("^^^|>",   "images/hantenna/h2_^^^|>.png");
      gestures . Put ("^^^^>",   "images/hantenna/h2_^^^^>.png");

      SetHeightUniformly (100);
      SetCornerRadius (0);
      current_gesture = g;
    }

  void Travail ()
    { GestureDetector *gd = Find ("GEST_DETECTOR") -> As <GestureDetector> ();
      if (current_gesture == gd -> current_gesture)
        { gd -> Heartbeat (.66);
          Heartbeat (.66);

          Node *t = Find ("MATCHER");
          t -> SetAlphaHard (1.0);
          t -> SetAlpha (0);

          int random_gest = int (Random (1) * gestures . Count ());
          current_gesture = gestures . NthKey (random_gest);
          SetImage (FetchImageData (gestures . NthVal (random_gest)));
        }
  }

};


void Setup ()
{ HideNeedlePoints (); //turn off HandiPoints ( ~ Cursor)

  //GESTURE DEFINITIONS
  DoNotRegisterForDefaultEvents (); //turn off default hantenna gestures
  RegisterOneHandBlurt ("\\/\\/-", OpenPalmFingersSpreadPose);
  RegisterOneHandBlurt ("^^\\/>", VictoryPose); // CUSTOM VICTORY BLURT GESTURE
  RegisterOneHandBlurt ("||||-", OpenPalmFingersTogetherPose);
  RegisterOneHandBlurt ("^^^|-", LShapePose);
  RegisterOneHandBlurt ("^^^|>", NumberOnePose); // CUSTOM FINGER BLURT GESTURE
  RegisterOneHandBlurt ("^^^^>", ClosedFistPose); // CUSTOM FIST BLURT GESTURE
  RegisterOneHandBlurt ("_____", NoPose);

  GestureDetector *itu = new GestureDetector ("images/hantenna/h_xxxxx.png",
                                              "xxxxx");
  itu -> SlapOnFeld ();
  itu -> IncTranslation (Feld () -> Over () * Feld () -> Width () * .25);

  Text *t1 = new Text("gesture");
  t1 -> SlapOnFeld ();
  t1 -> SetFontSize (Feld () -> Width () * .025);
  t1 -> IncTranslation (Feld () -> Over () * Feld () -> Width () * .25
                        + Feld () -> Up () * Feld () -> Height () * .35);

  GestureTarget *itt = new GestureTarget ("images/hantenna/h2_^^^|>.png",
                                          "^^^|>");
  itt -> SlapOnFeld ();
  itt -> IncTranslation (Feld () -> Over () * Feld () -> Width () * -.25);

  Text *t2 = new Text("target");
  t2 -> SlapOnFeld ();
  t2 -> SetFontSize (Feld () -> Width () * .025);
  t2 -> IncTranslation (Feld () -> Over () * Feld () -> Width () * -.25
                        + Feld () -> Up () * Feld () -> Height () * .35);

  Text *t3 = new Text("match");
  t3 -> SetName ("MATCHER");
  t3 -> SetAdjColor (Color (1.0, 0, 0));
  t3 -> ColorAnimateQuadratic (2.0); //seconds
  t3 -> SlapOnFeld ();
  t3 -> SetFontSize (Feld () -> Width () * .025);
  t3 -> IncTranslation (Feld () -> Up () * Feld () -> Height () * .35);
  t3 -> SetAlphaHard (0);
}

