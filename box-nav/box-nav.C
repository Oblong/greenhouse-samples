
/* (c)  oblong industries */

#include "Greenhouse.h"
#include "Box.h"


#define number_of_boxes 100

// SystemNodes inherit from Box and add interactivity
// behavior.  They also listen for protein messages.
class SystemNode  :  public Box
{ public:

  int64 random_timer;
  Str harden_prov;
  bool am_tickled;
  Str tickle_prov;
  Text * tickle_label;
  bool should_beat;
  bool should_rotate;
  Vect rotate_node_axis;
  float64 rotate_node_angle;
  Trove <int64> connected_boxes;

  SystemNode (const float64 &connection_sensitivity)  :  Box (),
      am_tickled (false),
      should_beat (false),
      should_rotate (false)
    { for (int64 j = 0; j < 6; j++)
        AppendTexture ("images/box_0" + INT (j + 1) + ".png");
      InitTextures ();

      ColorAnimateQuadratic (0.5); // When the color changes, it will animate
      RotationAnimateChase (0.75); // When the rotation changes, it will animate
      TranslationAnimateChase (.5); // When the translation changes, animate it
      SetScale (.2);
      SetAlpha (.375);

      // Based off this timer, the object will light up randomly
      // (between 3 and 60 seconds).
      random_timer = Random (3,60);
      rotate_node_axis = new Vect (0, 0, 1);
      rotate_node_angle = PI / 2;

      // Create a text object with the name of this SystemNode object
      Text *t = new Text (Name ());
      t -> SetObliviousness (true);      // Make the text non-receptive to
                                         // PointingInside events.
      t -> ColorAnimateQuadratic (0.25);
      t -> SetAlpha (0);                 // Hide initially.
      t -> SetAlignmentLeft ();
      t -> SetScale (Feld () -> Width () / 1000);
      tickle_label = t;

      // This object will listen to a message pool called 'love'.
      ParticipateInPool ("love");

      // Populate the connected_boxes trove
      for (int64 i = 0; i < number_of_boxes; i++)
        { float64 random_value = Random (0, 1);
            if (random_value > connection_sensitivity)
                connected_boxes.Append (i);
        }
    }

  bool IsTickledBy (PointingEvent *e) const
    { return (am_tickled  &&  e -> Provenance () == tickle_prov); }

  // Extract data from the ingests (payload) of the protein message with the
  // Ingest function and providing the ingest key name.
  float64 CalculateDelay (const Protein &p) const
    { Vect sender_loc = Ingest <Vect> (p, "sender-loc");
      float64 dist = PhysLoc () . DistFrom (sender_loc)
                  / Diag (Feld ());
      float64 delay = Range (dist, 0.0, 20.0, 0.0, 4.0);
      return delay;
    }

  bool ProteinHardensMe (const Protein &p) const
    { return (Ingest <Str> (p, "hardened-node") == Name ()); }

  void SetRotationDirection (const Protein &p)
    { rotate_node_axis  = Ingest <Vect> (p, "rotate-axis");
      rotate_node_angle = Ingest <float64> (p, "rotate-angle");
    }

  //  Any protein message that appears in the pool called 'love'
  //  and has 'greenhouse' as one of its descrips will be delivered here.
  void Metabolize (const Protein &p)
    { if (HasDescrip (p, "beat-hardencheck-please"))
        { ProteinHardensMe (p)  ?  Tag ("hardened")  :  UnTag ("hardened");
          SetFireTimer (CalculateDelay (p), 1);
          should_beat = true;
        }

      if (HasDescrip (p, "rotate-please"))
        { SetRotationDirection (p);
          SetFireTimer (CalculateDelay (p), 1);
          should_rotate = true;
        }
    }

  // This callback function is triggered after SetFireTimer is called with a
  // given period of time.
  void Fired ()
    { SetAlphaHard (1.0);
      if (should_rotate)
        { IncRotation (rotate_node_axis, rotate_node_angle);
          should_rotate = false;
        }
      if (should_beat)
        { Heartbeat (.66);
          should_beat = false;
        }
    }

  void TickleBox (PointingEvent *e)
    { // I'm already tickled by someone else
      if (am_tickled  &&  tickle_prov != e -> Provenance ())
        return;
      am_tickled = true;
      tickle_prov = e -> Provenance ();
      tickle_label -> SetAlpha (1.0);
    }

  void UntickleBox (PointingEvent *e)
    { //  only untickle if the person untickling is the one that
      //  was tickling
      if (IsTickledBy (e))
        { am_tickled = false;
          tickle_prov . SetToEmpty ();
          tickle_label -> SetAlpha (0.0);
        }
    }

  void PointingInsideMove (PointingEvent *e)
    { TickleBox (e); }

  void PointingOutsideMove (PointingEvent *e)
    { UntickleBox (e); }

  void PointingInsideHarden (PointingEvent *e)
    { // Make a protein message & drop it into the 'love' pool.
      // This will trigger Metabolize ().  Attach the physical location and the
      // name of the node to the ingests (payload) of the protein message.
      Protein p = ProteinWithDescrip ("beat-hardencheck-please");
      AppendIngest (p, "sender-loc", PhysLoc ());
      AppendIngest (p, "hardened-node", Name ());
      Deposit (p, "love");

      Tag ("hardened");
      harden_prov = e -> Provenance ();
    }

  void PointingVanish (PointingEvent *e)
    { if (HasTag ("hardened")  &&  e -> Provenance () == harden_prov)
          { UnTag ("hardened");
            harden_prov . SetToEmpty ();
          }
      if (IsTickledBy (e))
        UntickleBox (e);
    }

  void Travail ()
    { if (am_tickled)
        { SetAlpha (1.0);
          tickle_label -> SetAlpha (1.0);
        }
      else if (!HasTag ("randomly-lit"))
        { SetAlpha (0.375); }

      if ((int64)CurTime () % random_timer == 0)
        { SetAlpha (1.0);
          Tag ("randomly-lit");
        }
      else
        { UnTag ("randomly-lit"); }
    }
};


//  A class that extends Thing and manages
//  PointingEvent interactions with the SystemNodes
class DataSystem  :  public Thing
{ public:

  DataSystem ()  :  Thing ()
    { SlapOnFeld ();
      float64 w = Feld () -> Width  () * .65;
      float64 h = Feld () -> Height () * .65;
      float64 wmax = Feld () -> Width  () * .65;
      float64 hmax = Feld () -> Height () * 4.0;

      // create and position an assortment of SystemNodes
      for (int64 i = 0; i < number_of_boxes; i++)
        { SystemNode *g = new SystemNode (Random (.2, .5));

          if (i < 20)
            g -> SetTranslation (Random (-w, w) * Feld () -> Over () +
                                 Random (-h, h) * Feld () -> Up () +
                                 Random (-w, w) * Feld () -> Norm ());
          else
            g -> SetTranslation (Random (-wmax, wmax)     * Feld () -> Over () +
                                 Random (-hmax/2, hmax/2) * Feld () -> Up () +
                                 Random (-wmax, wmax)     * Feld () -> Norm ());
          AppendKid (g);
        }

      RotationAnimateChase (1.05); // When the rotation changes, it will animate
      TranslationAnimateChase (.25); // When the translation changes, it will
                                     // animate
    }

  void PointingMove (PointingEvent *e)
    { if (IsHeeding (e))
        { IncRotation (UnWrangleRay (Feld () -> Up ()),
                       IntersectionDiff (e, PhysLoc ()) . x / 200);
          IncRotation (UnWrangleRay (Feld () -> Over ()),
                       - IntersectionDiff (e, PhysLoc ()) . y / 200);
        }

      // update the position of the tickled SystemNode's text label to the
      // intersection location of the pointing event and feld plus an offset
      for (int64 i = 0; i < KidCount (); ++i)
        { SystemNode *kid = NthKid <SystemNode> (i);
          if (kid  &&  kid -> IsTickledBy (e))
            { Vect target_loc = Intersection (e);
                target_loc += Vect (10, 10, 1);
                kid -> tickle_label -> SetTranslation (target_loc);
            }
        }
    }

  void PointingHarden (PointingEvent *e)
    { if (IsHeedless ())
        Heed (e);
    }

  void PointingSoften (PointingEvent *e)
    { if (IsHeeding (e))
        StopHeeding (e);
    }

  void PointingVanish (PointingEvent *e)
    { if (IsHeeding (e))
        StopHeeding (e);
    }

  //  Draw connections to every other kid in its connected_boxes trove
  void DrawConnectionsFromNode (SystemNode *n)
    { for (int64 j = 0; j < n -> connected_boxes . Count (); j++)
        { int64 temp_connected_kid = n -> connected_boxes[j];
          Node *kid_other = NthKid (temp_connected_kid);
          if (! kid_other) continue;

          //  Line thickness is based on the z position
          float64 line_width =
              Range (kid_other -> PhysLoc () . z,
                     -Feld () -> Width () * 4 + Feld () -> PhysLoc () . z,
                      Feld () -> Width () * 4 + Feld () -> PhysLoc () . z,
                      1, 3);

          glLineWidth (line_width);
          if (kid_other -> HasTag ("randomly-lit"))
            glColor4f (0.34, 0.57, 0.71, 1.0);
          else
            glColor4f (0.34, 0.57, 0.71, .25);

          glBegin (GL_LINES);
            glVertex (UnWrangleLoc (n -> PhysLoc ()));
            glVertex (UnWrangleLoc (kid_other -> PhysLoc ()));
          glEnd();
      }
    }

  void DrawSelf ()
    { for (int64 i = 0  ;  i < KidCount ()  ;  ++i)
        { if (kids[i] -> HasTag ("hardened"))
            { kids[i] -> SetAlphaHard (1.0);
              DrawConnectionsFromNode (NthKid <SystemNode> (i));
            }
        }
    }

  void RotateSystem (const Vect &r_axis, const float64 &r_angle)
    { for (int64 i = 0  ; i < KidCount ()  ; ++i)
        { SystemNode *kid = NthKid <SystemNode> (i);
          if (kid  &&  kid -> HasTag ("hardened"))
            { Protein p = ProteinWithDescrip ("rotate-please");
              AppendIngest (p, "sender-loc", kid -> PhysLoc ());
                                AppendIngest (p, "rotate-axis", r_axis);
                                AppendIngest (p, "rotate-angle", r_angle);
              Deposit (p, "love");
              return;
            }
        }

      for (int64 i = 0  ; i < KidCount ()  ; ++i)
        { if (SystemNode *kid = NthKid <SystemNode> (i))
            { kid -> SetAlphaHard (1.0);
              kid -> IncRotation (r_axis, r_angle);
              kid -> Heartbeat (.66);
            }
        }
    }

  void ZoomToHardenedKid ()
    { Vect diff = ZeroVect;
      for (int64 i = 0  ;  i < KidCount ()  ;  ++i)
        { if (kids[i] -> HasTag ("hardened"))
            { diff = UnWrangleLoc (kids[i] -> PhysLoc ()) -
                     UnWrangleLoc (Feld () -> Loc ());
            }
        }

      for (int64 j = 0  ;  j < KidCount ()  ;  ++j)
        kids[j] -> IncTranslation (-diff);
    }

  void RightTheSystem ()
    { //  Correct horizontal angle
      float64 correction_angle = UnWrangleRay (Up ())
                                 . AngleWith (Feld () -> Up());
      Vect correction_vector = Up ()
                               . Cross (UnWrangleRay (Up ()));

      IncRotation (correction_vector, correction_angle);
    }

  void Blurt (BlurtEvent *e)
    { //  Rotate kid
      if (Utters (e, "j") || Utters (e, "LLCOOLJ"))
        { Str genus = e -> Genus ();
          if (Utters (e, "j") || genus . Match ("right"))
            { RotateSystem ( Vect (0, 1, 0), - PI / 2); }
        }
      if (Utters (e, "l"))
        { RotateSystem ( Vect (0, 1, 0), PI / 2); }

      //  Beat
      if (Utters (e, "b") )//|| Utters (e, "boink"))
        { bool found_a_harden = false;
          for (int64 i = 0; i < KidCount (); ++i)
            { if (kids[i] -> HasTag ("hardened"))
                { Protein p = ProteinWithDescrip ("beat-please");
                  AppendIngest (p, "sender-loc", kids[i] -> PhysLoc ());
                  Deposit (p, "love");
                  found_a_harden = true;
                }
            }
          if (! found_a_harden)
            { for (int64 i = 0; i < KidCount (); ++i)
                { kids[i] -> SetAlphaHard (1.0);
                  kids[i] -> Heartbeat (.66);
                }
            }
        }

      //  Zoom
      if (Utters (e, "e") || Utters (e, "VICTORY"))
        { ZoomToHardenedKid (); }

      //  Orient up
      if (Utters (e, "w")|| Utters (e, "WHATSUP"))
        { RightTheSystem (); }

      if (Utters (e, "q") || Utters (e, "LLCOOLJ"))
        { Str genus = e -> Genus ();
          if (Utters (e, "q") || genus . Match ("left"))
            { IncRotation (Feld () -> Up(), PI); }
        }
    }
};


void Setup ()
{ // Prevent any of the default events from registering and instead register
  // several of our own for receiving a "VICTORY" blurt event with the
  // Victory pose, "LLCOOLJ" blurt event with a single L-shape pose, and a
  // "WHATSUP" blurt event with a two handed L-shape pose.
  DoNotRegisterForDefaultEvents ();
  RegisterTwoHandBlurt ("WHATSUP", LShapePose, LShapePose);
  RegisterOneHandBlurt ("LLCOOLJ", LShapePose);
  RegisterOneHandBlurt ("VICTORY", VictoryPose);

  SetFeldsColor (0.0, 0.0, 0.0);

  new DataSystem ();
}
