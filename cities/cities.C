
/* (c)  oblong industries */

//  Cities data originally from GeoNames <http://www.geonames.org/>
//  http://download.geonames.org/export/dump/cities5000.zip
//  cities5000.zip : all cities with a population > 5000

#include "Greenhouse.h"
#include "DataSet.h"

#define GLOBE_RADIUS 120.0


//  Function to convert latitude, longitude positions to
//  spherical (globe) positions
inline Vect LatLongToSphereSurface (float64 radius, float64 lat, float64 lng)
{ lat *= (M_PI / 180.0);
  lng *= (M_PI / 180.0);
  return Vect (cos (lat) * sin (lng) * radius,
               sin (lat) * radius,
               cos (lat) * cos (lng) * radius);
}


// Object that stores and displays global country border data
class CountryBorders  :  public DataSet
{ public:

  //  We store the dataset's latitudes/longitudes as floats
  Trove <float64> latitude;
  Trove <float64> longitude;

  //  Borders are stored as a continuous line VBO,
  //  0 = invisible, 1 = visible
  Trove <int64> drawitude;

  CountryBorders ()  :  DataSet ()
    { Load ("data/Tissot_indicatrix_world_map_equirectangular_proj_360x180_coords_cleaner2.txt");

      //  Interpret the 0th and 1th column in the data as floats,
      //  and the 2th column as ints
      longitude = FloatColumn (0);
      latitude  = FloatColumn (1);
      drawitude = IntColumn   (2);

      LoadShaders ("shaders/foggy.vert", "shaders/null.frag");

      for (int64 i = 4  ;  i < Count ()  ;  i++)
        { float64 mapped_longitude
            = Range (longitude[i], 0.0, 360.0, -180.0, 180.0) - 0.2;

          //  todo: - .2 because the borders data is a tad off
          float64 mapped_latitude
            = Range (latitude[i], 0.0, 180.0, 90.0, -90.0) + 0.25;

          //  todo: + .25 because the borders data is a tad off

          Vect globe_position = LatLongToSphereSurface (GLOBE_RADIUS - 0.5,
                                                        mapped_latitude,
                                                        mapped_longitude);
          SetPointLocation (i, globe_position.x,
                            globe_position.y,
                            globe_position.z);

          SetPointColor (i, HSB (0.5, 0.0, 0.2, 1.0 * drawitude[i]));
        }

      DataReady ();
      SetDrawMode (GL_LINE_STRIP);

      RotationAnimateChase (0.75);
      TranslationAnimateChase (0.25);
    }

  //  Runs once per render loop; where we provide input to shaders
  void AssignShaderInputs ()
    { Vect viewloc = Feld () -> Camera () -> ViewLoc ();
      SetShaderUniform ("fog_radius", GLOBE_RADIUS);
      SetShaderUniform ("system_distance", Loc () . DistFrom (viewloc));
      SetShaderUniform ("camera_position", viewloc);
    }

  void Blurt (BlurtEvent *e)
    { int64 translation_amount = 40;
      if (Utters (e, "w"))
        { IncTranslation (translation_amount * 2.0 * Feld () -> Norm ()); }
      else if (Utters (e, "s"))
        { IncTranslation (-translation_amount * 2.0 * Feld () -> Norm ()); }
    }

  void PointingHarden (PointingEvent *e)
    { if (IsHeedless ())
        Heed (e);
    }

  void PointingSoften (PointingEvent *e)
    { StopHeeding (e); }

  void PointingMove (PointingEvent *e)
    { //  Drag the object
      if (IsHeeding (e))
        { IncRotation (WrangleRay (Feld () -> Up ()),
                       IntersectionDiff (e, Loc ()) . x / 200);
          IncRotation (WrangleRay (Feld () -> Over ()),
                       - IntersectionDiff (e, Loc ()) . y / 200);
        }
    }

  //  Hantenna pipeline input

  void FistAppear (PointingEvent *e)
    { INFORM ("seismo FistAppear " + e -> Provenance ());
      if (IsHeedless ())
        Heed (e);
    }
  void FistVanish (PointingEvent *e)
    { StopHeeding (e);
      INFORM ("seismo FistVanish " + e -> Provenance ());
    }
  void FistMove (PointingEvent *e)
    { if (IsHeeding (e))
        IncTranslation (6.0 * FistMotion (e, Loc ()));
    }

  void FingerMove (PointingEvent *e)
    { IncRotation (WrangleRay (Feld () -> Up ()),
                   (e -> PhysOrigin () . x - e -> PrevOrigin () . x) / 250);
      IncRotation (WrangleRay (Feld () -> Over ()),
                   -(e -> PhysOrigin () . y - e -> PrevOrigin () . y) / 250);
    }
};


//  Stores and displays the city data
class Cities  :  public DataSet
{ public:

  Trove <Str> city_name;
  Trove <float64> latitude;
  Trove <float64> longitude;

  //  A map of event source names (provenances) to Text labels.
  //  These labels will display information about each cursor's
  //  closest data point
  Dictionary <Str, Text *> labels;

  Cities ()  :  DataSet ()
    { Load ("data/cities/geonames_cities5000.txt");

      city_name = StrColumn   (0);
      latitude  = FloatColumn (1);
      longitude = FloatColumn (2);

      LoadShaders ("shaders/foggy.vert", "shaders/null.frag");

      for (int64 i = 0  ;  i < Count ()  ;  i++)
        { Vect globe_position = LatLongToSphereSurface (GLOBE_RADIUS,
                                                        latitude[i],
                                                        longitude[i]);
          SetPointLocation (i, globe_position.x,
                               globe_position.y,
                               globe_position.z);

          // INFORM ( city_name[i] + ", "
          //        + ToStr (longitude[i]) + ", "
          //        + ToStr (latitude[i]) );

          SetPointColor (i, HSB (0.12, 0.2, 1.0, 1.0));
          SetPointSize (i, 2.0);
        }
      DataReady ();
    }

  //  Runs once per render loop; where we provide input to shaders
  void AssignShaderInputs ()
    { Vect viewloc = Feld () -> Camera () -> ViewLoc ();
      SetShaderUniform ("fog_radius", GLOBE_RADIUS);
      SetShaderUniform ("system_distance", Loc () . DistFrom (viewloc));
      SetShaderUniform ("feld_size", Diag (Feld ()));
      SetShaderUniform ("camera_position", viewloc);
    }

  void UpdateLabel (PointingEvent *e, Str text, Vect loc)
    { //  We want one label per event source (mouse pointer,
      //  wiimote, etc.)  Ensure this source has a label.
      Text *label = labels . Get (e -> Provenance ());
      if (! label)
        { label = new Text ("");
          label -> SetObliviousness (true);
          label -> SetFontSize (Feld () -> Width () / 100.0);
          labels . Put (e -> Provenance (), label);
        }

      label -> SetString (text);
      label -> SetTranslation (loc
                                + ((2.0 + label -> Width () / 2.0)
                                   * Feld () -> Over ())
                                + 10.0 * Feld () -> Norm ());
    }

  void IndividualPointerInteract (PointingEvent *e)
    { int64 last = LastClosestPoint ();
      if (last > -1)
        SetPointSize (last, 2.0);

      int64 closest = FindClosestPoint (e);
      if (closest > -1)
        SetPointSize (closest, 4.0);

      //  todo: document this
      Vect abs_loc = UnWrangleLoc (PointLocation (closest));

      UpdateLabel (e, city_name[closest], abs_loc);
    }

  void PointingMove (PointingEvent *e)
    { IndividualPointerInteract (e); }
};


void Setup ()
{ // Black background color
  SetFeldsColor (0.0, 0.0, 0.0);

  //  Turn off Greenhouse cursors
  HideNeedlePoints ();

  CountryBorders *cb = new CountryBorders ();
  Cities *a = new Cities ();

  //  Make the Cities object a kid of the CountryBorders object,
  //  so that when we move one, the other goes along for the ride.
  //  Parents' movements affect their kids.
  cb -> AppendKid (a);

  //  Center both objects on the screen and orient them like the Feld
  cb -> SlapOnFeld ();
}
