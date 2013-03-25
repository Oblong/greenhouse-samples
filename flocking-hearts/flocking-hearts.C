
/* (c)  oblong industries */

#include "Greenhouse.h"


//  Object that represents one heart-bird
class HeartBird  :  public Image
{ public:
 
  Vect velocity;
  Vect acceleration;
  float64 maxforce;    // maximum steering force
  float64 maxspeed;    // maximum speed

  HeartBird (const Str &image_path, const Vect &v)  :  Image (image_path)
    { SetScale ( 0.05 );
      velocity = Vect (Random (-20.0, 20.0), Random (-20.0, 20.0), 0.0);
      acceleration = ZeroVect;
      maxspeed = Feld () -> Width () / 200.0;
      maxforce = 0.3;

      RotationAnimateQuadratic (0.05);
      SetTranslationHard (v);
    }

  Vect Separation ()
    { Vect separation = ZeroVect;
      int64 separation_count = 0;

      for (int64 i = 0  ;  i < Parent () -> KidCount ()  ;  ++i)
        { Vect sub = Translation () - Parent () -> NthKid (i) -> Translation ();
          float64 dist = sub . Mag ();

          if (dist > 0  &&  dist < Feld () -> Width () / 45.0)
            { sub = sub . Norm () / dist;
              separation += sub;
              separation_count++;
            }
        }

      if (separation_count > 0)
        separation /= separation_count;

      if (separation . Mag ()  >  0.0)
        { separation = separation . Norm ();
          separation = separation * maxspeed;
          separation = separation - velocity;
          separation = Limit (separation, maxforce);
        }

      return separation;
    }

  Vect Alignment ()
    { Vect alignment = ZeroVect;
      int64 alignment_count = 0;

      //  Iterate over all the parent's kids (all our bird brethren)
      for (int64 i = 0  ;  i < Parent () -> KidCount ()  ;  ++i)
        { //  Here the type of NthKid(i) is generic Node.
          float64 dist = (Parent () -> NthKid (i) -> Translation () - 
                          Translation ())
                         . Mag ();
          
          if (dist > 0  &&  dist < Feld () -> Width () / 27.0)
            { //  Here we have to specify that we want the HeartBird type 
              //  with NthKid<HeartBird> because 'velocity' is a member 
              //  of HeartBird -- it's not included in generic Nodes
              alignment += Parent () -> NthKid <HeartBird> (i) -> velocity;
              alignment_count++;
            }
        }

      if (alignment_count > 0)
        { alignment /= alignment_count;
          alignment = alignment . Norm ();
          alignment *= maxspeed;
          alignment -= velocity;
          alignment = Limit (alignment, maxforce);
        }

      return alignment;
    }

  Vect Cohesion ()
    { int64 cohesion_count = 0;

      //  Determine the target
      Vect cohesion_target = ZeroVect;

      for (int64 i = 0  ;  i < Parent () -> KidCount ()  ;  ++i)
        { float64 dist = (Parent () -> NthKid (i) -> Translation () - 
                          Translation ()) 
                          . Mag ();

          if (dist > 0  &&  dist < Feld () -> Width () / 34)
            { cohesion_target += Parent () -> NthKid (i) -> Translation ();
              cohesion_count++;
            }
        }

      if (cohesion_count > 0)
        cohesion_target /= cohesion_count;

      //  Steer to the target
      Vect steer = cohesion_target - Translation ();
      steer = steer . Norm ();
      steer *= maxspeed;
      steer -= velocity;      // Slow down as it approaches the target
      return Limit (steer, maxforce);
    }

  Vect Limit (Vect limited, const float64 &max_acceleration)
    { float64 mag = limited . Mag ();
      if (mag > max_acceleration)
        { float64 factor = max_acceleration / mag;
          limited = limited * factor;
        }
      return limited;
    }

  void Travail ()
    { //  Update velocity
      acceleration += 1.5 * Separation ();
      acceleration += 1.0 * Alignment ();
      acceleration += 1.0 * Cohesion ();

      velocity += acceleration;
      velocity = Limit (velocity, maxspeed);
      velocity.z = 0.0;
      IncTranslation (velocity);

      //  Set acceleration to 0
      acceleration *= 0;

      //  Update rotation
      float64 rotation = 0.0;
      rotation = atan (velocity.y / velocity.x);
      if (velocity . x <= 0)
        rotation += PI * 3 / 2;
      else
        rotation += PI / 2;
      SetRotation (Vect (0.0, 0.0, 1.0), rotation);

      // Wrap around the edges
      if (Translation () . x > Feld () -> Width () / 2.0 + 30.0)
        { Vect wrapped_translation = Translation ();
          wrapped_translation.x *= -1.0;
          wrapped_translation.x += 1.0;
          SetTranslationHard (wrapped_translation);
        }
      else if (Translation () . x < -Feld () -> Width () / 2.0 - 30.0)
        { Vect wrapped_translation = Translation ();
          wrapped_translation.x *= -1.0;
          wrapped_translation.x -= 1.0;
          SetTranslationHard (wrapped_translation);
        }
      else if (Translation () . y > Feld () -> Height () / 2.0 + 30.0)
        { Vect wrapped_translation = Translation ();
          wrapped_translation.y *= -1.0;
          wrapped_translation.y += 1.0;
          SetTranslationHard (wrapped_translation);
        }
      else if (Translation ().y < - Feld () -> Height () / 2.0 - 30.0)
        { Vect wrapped_translation = Translation ();
          wrapped_translation.y *= -1.0;
          wrapped_translation . y += -1.0;
          SetTranslationHard (wrapped_translation);
        }
    }
};


//  A container object that holds all the birds
class Flock  :  public Thing
{ public:
  
  Dictionary <Str, Vect> hardens;

  Flock ()  :  Thing ()
    { SlapOnFeld ();

      for (int64 i = 0  ;  i < 100  ;  i++)
        AppendKid (new HeartBird ("images/heart.png", ZeroVect));
    }

  void Travail ()
    { for (int64 j = 0  ;  j < hardens . Count ()  ;  j++)
        { Vect pos = hardens . NthVal (j);
          for (int64 i = 0  ;  i < KidCount ()  ;  ++i)
            { Vect kid_dist = pos - NthKid (i) -> Translation ();
              if (kid_dist . Mag ()  <  80.0)
                NthKid <HeartBird> (i) -> velocity +=  0.01 * kid_dist;
            }
        }
    }

  Vect OffsetFromFeldCenter (PointingEvent *e) const
    { Vect v = Intersection (e, Feld () -> Loc ()) - 
               Feld () -> Loc (); 
      v.z = 0;
      return v;
    }

  void PointingHarden (PointingEvent *e)
    { //  A harden (or click) means that a mouse button is down, a wiimote 
      //  button is pressed, etc.  Add it to the list.  Use the (unique) source 
      //  of the event (aka its provenance) as the key.
      hardens . Put (e -> Provenance (), 
                     OffsetFromFeldCenter (e)); 
    }

  void PointingSoften (PointingEvent *e)
    { hardens . Remove (e -> Provenance ()); }

  void PointingMove (PointingEvent *e)
    { //  If it's a move event from a hardened source, update the list
      if (IsHardened (e))
        hardens . Put (e -> Provenance (), 
                       OffsetFromFeldCenter (e)); 
    }
};


void Setup ()
{ new Flock (); }
