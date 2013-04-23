
/* (c)  oblong industries */

#include "Greenhouse.h"
#include "LeapPointing.h"

class Squirly : public Thing 
{ 
private:
  Trove <Vect> verts;
  Color c;

public:

  Squirly () : Thing ()
    { c = Color (Random (1), Random (1), Random (1), 1.0); }

  void Heartbeat (float64 amplitude = 1.0, bool dynamic = false)
    { SetScale (Scale () + 0.1); }

  void AppendPoint (Vect p)
    { if (p . IsInvalid ())
        return;
      verts . Insert (p, 0);
      if (verts . Count () > 100)
        verts . RemoveNth (100);
    }

  void DrawSelf ()
    { SetGLColor (c);
      glEnable (GL_LINE_SMOOTH);
      glBegin (GL_LINE_STRIP);
      for (int i = 0; i < verts . Count (); i++)
        glVertex (verts . Nth (i));
      glEnd ();
      glDisable (GL_LINE_SMOOTH);
    }
};


class Round : public Thing 
{
private:

  float64 rad;
  Color col;
  Vect cent;
  float64 line_width;

public:

  Round (float64 r, Vect c) : Thing ()
    { rad = r;
      cent = c;
      line_width = 1.0;
      SetTranslationHard (c);
      col = Color (Random (1), Random (1), Random (1), 1.0);
    }

  void SetLineWidth (float64 lw)
    { line_width = lw; }

  void Heartbeat (float64 amplitude = 1.0, bool dynamic = false)
    { SetScale (Scale () + 0.1); }

  void DrawSelf ()
    { SetGLColor (col);
      glEnable (GL_LINE_SMOOTH);
      glLineWidth (line_width);
      glBegin (GL_LINE_STRIP);
      for (int i = 0; i < 100; i++)
        glVertex (Vect (rad * cos (i / (2 * PI)), rad * sin (i / (2 * PI)), 0));
      glEnd ();
      glLineWidth (1.0);
      glDisable (GL_LINE_SMOOTH);
    }
};


class Splat : public Image 
{
public:

  Splat () : Image ("images/splat" + INT ((int)Random(1, 5)) + ".png")
    { SetHeightUniformly (0.1 * Feld () -> Height ());
      TranslationAnimateQuadratic  (0.5);
    }

  void Heartbeat (float64 amplitude = 1.0, bool dynamic = false)
    { SetScale (Scale () + 0.1); }
};


class Leapy : public Thing, public LeapListener 
{
private:
  Dictionary <Str, Squirly *> squirlies;
  Dictionary <Str, Round *> rounds;
  Trove <Splat *> splats;

public:
  Leapy () : Thing (), LeapListener ()
    { SeedRandomizer ();
      ListenForLeapEvents (this, "leap");
    }

  void PointingMove (PointingEvent *e)
    { Squirly *s = squirlies . Find (e -> Provenance ());
      if (! s)
        { s = new Squirly;
          squirlies . Put (e -> Provenance (), s);
        }
      s -> AppendPoint (Intersection (e));
      s -> SetShouldDraw (true);
    }
  void PointingVanish (PointingEvent *e)
    { Squirly *s = squirlies . Find (e -> Provenance ());
      if (s)
        s -> SetShouldDraw (false);
    }

  void LeapCircle (LMCircle const&c, LMFrame const &f)
    { Vect cent = Intersection (c . center, c . center + c . normal);
      Round *r = rounds . Find (c . prov);
      if ( ! r)
        { r = new Round (c . radius, cent);
          r -> TranslationAnimateQuadratic  (0.5);
          rounds . Put (c . prov, r);
        }
      r -> SetLineWidth (2 * c . progress - 1);
    }

  void LeapSwipe (LMSwipe const&s, LMFrame const &f)
    { Vect move_by = s . speed * s . direction / 100.0;
      for (int i = 0; i < rounds . Count (); i++)
        rounds . NthVal (i) -> IncTranslation (move_by);
      for (int i = 0; i < splats . Count (); i++)
        splats . Nth (i) -> IncTranslation (move_by);
    }

  void LeapScreenTap (LMScreenTap const&s, LMFrame const &f)
    { LMPointer p;
      if (! PointerFromGesture (s, f, p))
        return;

      /// Note: We are using the pointing ray that belongs to the pointer of
      /// this event rather than the event's position and direction since the
      /// values for the latter sent by the leap are inconsistent with what
      /// we would expect
      Vect hit = Intersection (p . phys_origin, p . phys_through);
//    Vect hit = Intersection (s . position, s . position + s . direction);

      Splat *splat = new Splat ();
      splat -> SetTranslationHard (hit);
      splats . Append (splat);
    }

  void LeapKeyTap (LMKeyTap const &s, LMFrame const &f)
    { for (int i = 0; i < rounds . Count (); i++)
        rounds . NthVal (i) -> Heartbeat ();
      for (int i = 0; i < splats . Count (); i++)
        splats . Nth (i) -> Heartbeat ();
    }
};

void Setup () 
{ EnableLeapPointing ();
  new Leapy ();
}
