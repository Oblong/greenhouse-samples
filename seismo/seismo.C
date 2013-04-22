/* (c)  Oblong Industries */

//  Earthquake data originally from the USGS
//  < http://earthquake.usgs.gov/earthquakes/eqarchives/epic/epic_global.php >

#include "Greenhouse.h"
#include "DataSet.h"

//  GLOBAL VARIABLES
#define GLOBE_RADIUS 100.0
float64 distFromCamera;
bool viewDataAsGlobe;
bool stratifiedByMagnitude;
bool explodedByDepth;

//  COMMON CALCULATION METHODS
//  Function to calculate the earthquake data GL point size
float ComputePointSize (float m)
  { float output = m * m * m * m;
     output = Range (output, 0, 10000, 0, 50);
     if (output < 1.5)
       output = 1.5;
     return output;
  }

//  Function to convert latitude, longitude positions to
//  spherical (globe) positions
Vect LatLongToSphereSurface (float64 radius, float64 lat, float64 lng)
{ lat *= (M_PI / 180.0);
  lng *= (M_PI / 180.0);
  Vect v = Vect (cos (lat) * sin (lng) * radius,
                 sin (lat) * radius,
                 cos (lat) * cos (lng) * radius);
  return v;
}


//  CLASSES
//  The Selector object stores and displays the cube graphics that
//  are displayed arounf individual quake data points
class Selector  :  public Thing //  subclassed from Thing
{
  public:

  Str provenance; //  name of the Pointer associated with this Selector

  Trove <Vect> opengl_lines; // holds all the points for the GL_LINES
  Trove <float64> opengl_lines_sizes;  // holds all the line weights
  Trove <float64> opengl_lines_alphas; // holds all the line alphas

  Selector (float point_radius)  :  Thing ()
    { SetObliviousness (true); } // ignore Pointer interactions

  //  Function to rescale the size of the Selector cube graphics
  void RebuildScale (const float &f)
    { // LINES
      opengl_lines . Empty ();
      opengl_lines_sizes . Empty ();
      opengl_lines_alphas . Empty ();

      float64 corner_line_length = f / 2; // half of const float &f
     // BACK
     // BOTTOM LEFT
      opengl_lines . Append (Vect ( -corner_line_length+corner_line_length/4,
                                    -corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    -corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    -corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    -corner_line_length+corner_line_length/4,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    -corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    -corner_line_length,
                                    -corner_line_length+corner_line_length/4 ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );


      // BOTTOM RIGHT
      opengl_lines . Append (Vect ( corner_line_length-corner_line_length/4,
                                    -corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    -corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    -corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    -corner_line_length+corner_line_length/4,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    -corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    -corner_line_length,
                                    -corner_line_length+corner_line_length/4 ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );


      // TOP RIGHT
      opengl_lines . Append (Vect ( corner_line_length-corner_line_length/4,
                                    corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    corner_line_length-corner_line_length/4,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    corner_line_length,
                                    -corner_line_length+corner_line_length/4 ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );


      // TOP LEFT
      opengl_lines . Append (Vect ( -corner_line_length+corner_line_length/4,
                                    corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    corner_line_length-corner_line_length/4,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    corner_line_length,
                                    -corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    corner_line_length,
                                    -corner_line_length+corner_line_length/4 ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );


      // FRONT
     // BOTTOM LEFT
      opengl_lines . Append (Vect ( -corner_line_length+corner_line_length/4,
                                    -corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    -corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    -corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    -corner_line_length+corner_line_length/4,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    -corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    -corner_line_length,
                                    corner_line_length-corner_line_length/4 ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      // BOTTOM RIGHT
      opengl_lines . Append (Vect ( corner_line_length-corner_line_length/4,
                                    -corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    -corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    -corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    -corner_line_length+corner_line_length/4,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    -corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    -corner_line_length,
                                    corner_line_length-corner_line_length/4 ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );


      // TOP RIGHT
      opengl_lines . Append (Vect ( corner_line_length-corner_line_length/4,
                                    corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    corner_line_length-corner_line_length/4,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( corner_line_length,
                                    corner_line_length,
                                    corner_line_length-corner_line_length/4 ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );


      // TOP LEFT
      opengl_lines . Append (Vect ( -corner_line_length+corner_line_length/4,
                                    corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    corner_line_length-corner_line_length/4,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    corner_line_length,
                                    corner_line_length ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

      opengl_lines . Append (Vect ( -corner_line_length,
                                    corner_line_length,
                                    corner_line_length-corner_line_length/4 ));
      opengl_lines_sizes . Append (1.0);
      opengl_lines_alphas . Append ( 1.0 );

    }

  void Travail ()
    { IncRotation (Vect (0, 1, 0), .015); } //  rotate around Feld -> Up ()

  void DrawSelf ()
    { // LINES
      for (int i = 0; i < opengl_lines . Count() - 1; i+=2)
        { glLineWidth(opengl_lines_sizes . Nth (i));
          glBegin(GL_LINES);
          glColor4f(1.0, 1.0, 1.0, opengl_lines_alphas . Nth (i));
          Vect line_start = opengl_lines . Nth (i);
          glVertex (line_start);
          Vect line_stop = opengl_lines . Nth (i+1);
          glVertex (line_stop);
          glEnd();
        }
    }
};


// Object that stores and displays global country border data
class CountryBorders  :  public DataSet
{
 public:

  //  We store the dataset's latitudes/longitudes as floats
  Trove <float64> latitude;
  Trove <float64> longitude;

  //  Borders are stored as a continuous line VBO,
  //  0 = invisible, 1 = visible
  Trove <int64> drawitude;

  CountryBorders ()  :  DataSet ()
    { SetName ("country_borders_dataset");
      Load ("data/Tissot_indicatrix_world_map_equirectangular_proj_"
            "360x180_coords_cleaner2.txt");

      //  Interpret the 0th and 1th column in the data as floats,
      //  and the 2th column as ints
      longitude = FloatColumn (0);
      latitude = FloatColumn (1);
      drawitude = IntColumn (2);

      LoadShaders ("shaders/foggy.vert", "shaders/null.frag");
      UpdatePointLocations();
      UpdatePointColors ();

      DataReady ();
      SetDrawMode (GL_LINE_STRIP);
    }

    //  Function to the VBO's vertex positions
    void UpdatePointLocations ()
      { for (int64 i = 4; i < Count (); i++)
          { float64 mapped_longitude
              = Range (longitude . Nth (i), 0, 360, -180, 180) - 0.2;
              //  todo: - .2 because the borders data is a tad off

            float64 mapped_latitude
              = Range (latitude . Nth (i), 0, 180, 90, -90) + 0.25;
              //  todo: + .25 because the borders data is a tad off

            if(viewDataAsGlobe)
              { Vect globe_position = LatLongToSphereSurface (GLOBE_RADIUS,
                                                              mapped_latitude,
                                                              mapped_longitude);

                SetPointLocation (i, globe_position . x,
                                     globe_position . y,
                                     globe_position . z);
              }
            else
              { SetPointLocation (i, mapped_longitude,
                                  mapped_latitude,
                                  0 );
              }
          }
      }

    //  Function to the VBO's point colors
    void UpdatePointColors ()
      { for (int64 i = 4; i < Count (); i++)
          { if (viewDataAsGlobe)
              { if (drawitude . Nth (i) == 2)
                  { SetPointColor (i, HSB (.5, 0, 1, 0)); }
                else
                  { SetPointColor (i, HSB (.5, 0, 1, 1.0 * drawitude . Nth (i))); }
              }
            else // if !viewDataAsGlobe
              { if(drawitude . Nth (i) == 2)
                  { SetPointColor (i, HSB (.5, 0, 1, 1)); }
                else
                  { SetPointColor (i, HSB (.5, 0, 1, 1.0 * drawitude . Nth (i))); }
              }
          }
      }

    //  Runs once per render loop; where we provide input to shaders
    void AssignShaderInputs ()
      { if (viewDataAsGlobe)
          { SetShaderUniform ("fog_radius", GLOBE_RADIUS);
            SetShaderUniform ("system_distance", distFromCamera);
            // INFORM ("system_distance / distFromCamera = " + FLOAT(distFromCamera));
            SetShaderUniform ("camera_position", Feld () -> Camera () -> ViewLoc ());
          }
        else
          { SetShaderUniform ("fog_radius", GLOBE_RADIUS * 4);
            SetShaderUniform ("system_distance", distFromCamera);
            SetShaderUniform ("camera_position", Feld () -> Camera () -> ViewLoc ());
          }
      }
};


//  Earthquakes class stores and displays the earthquake data
class Earthquakes  :  public DataSet
{
 public:

  //  Troves to store the data from the text file
  Trove <float64> time_year;
  Trove <float64> time_month;
  Trove <float64> time_day;
  Trove <float64> magnitude;
  Trove <float64> latitude;
  Trove <float64> longitude;
  Trove <float64> depth;

  //  Variables that provide info on the Troves
  float64 quake_magnitude_max, quake_magnitude_min, quake_magnitude_avg;
  float64 quake_latitude_max, quake_latitude_min;
  float64 quake_longitude_max, quake_longitude_min;
  float64 quake_depth_avg, quake_depth_factor, quake_depth_factor_max;
  int64 quake_depth_max, quake_depth_min;

  //  Variables used to filter the Trove data
  int64 system_start_year, system_current_year;
  int64 system_current_month, system_current_day;
  bool auto_play;           //  True plays temporally through the data
  bool all_z_visible;       //  True removes temporal filtering
  float64 system_depth;     //  Controls depth plane sectioning of the data
  Vect victory_displacement_previous; //  stores the previous displacement
                            //  Record of the vect added to the system_depth
  float64 system_magnitude; //  Controls magnitude filtering of the data
  Str draw_mode;  //  GL point color drawn w "depth" or "magnitude"

  //  Variables used to stratify the Trove data
  float64 stratified_by_magnitude_factor; // 0 - 1.0 value of each quakes
                            //  stratifiedByMagnitudeFactorTarget value
  Trove <float64> stratifiedByMagnitudeFactorTarget; // Stores the
                            //  stratification band for each quake
  float64 stratifiedByMagnitudeOffset; //  mm distance for band increment

  //  Display text for feedback
  Text *t_system_depth;
  Text *t_quakes_within_range;
  Text *t_system_time;
  Text *t_system_magnitude;

  //  Store and display Pointer info for the closest data point
  Dictionary <Str, Text*> labels;
  Dictionary <Str, Selector*> labels_graphics;

  Earthquakes ()  :  DataSet ()
    { SetName ("earthquake_dataset");

      // LOAD DATASET AND SET SYSTEM TIME VARIABLES
      system_start_year = 2007;
      Load ("data/seismo/USGS_NEIC_" + INT (system_start_year) + "0101_20120906.txt");

      system_current_year = system_start_year;
      system_current_month = 01;
      system_current_day = 01;

      auto_play = false;
      all_z_visible = true;

      // ASSIGN TROVE VALUES
      time_year = FloatColumn (0);
      time_month = FloatColumn (1);
      time_day = FloatColumn (2);
      magnitude = FloatColumn (6);
      latitude = FloatColumn (4);
      longitude = FloatColumn (5);
      depth = FloatColumn (7);

      quake_depth_factor = 0.0; // 0.36;
      draw_mode = "depth";

      quake_depth_max = Max (depth);
      quake_depth_min = Min (depth);

      quake_magnitude_max = Max (magnitude);
      quake_magnitude_min = Min (magnitude);

      quake_latitude_max = Max (latitude);
      quake_latitude_min = Min (latitude);

      quake_longitude_max = Max (longitude);
      quake_longitude_min = Min (longitude);
      quake_depth_avg = Avg (depth);
      quake_magnitude_avg = Avg (magnitude);

      system_depth = 0;
      system_magnitude = 1; //  Sets all data to a transparent value

      // STRATEFICATION VALUES
      if (stratifiedByMagnitude)
        stratified_by_magnitude_factor = 1.0;
      else
        stratified_by_magnitude_factor = 0.0;

      stratifiedByMagnitudeOffset = 10; //  distance in mm

      //  Assign a stratification layer based on quake magnitude
      for (int64 i = 0; i < Count (); i++)
        { if (magnitude . Nth (i) < 1.5)
            stratifiedByMagnitudeFactorTarget . Append (0);
          else if (magnitude . Nth (i) >= 1.5 && magnitude . Nth (i) < 2.5)
            stratifiedByMagnitudeFactorTarget . Append (.25);
          else if (magnitude . Nth (i) >= 2.5 && magnitude . Nth (i) < 3.5)
            stratifiedByMagnitudeFactorTarget . Append (.4375);
          else if (magnitude . Nth (i) >= 3.5 && magnitude . Nth (i) < 4.5)
            stratifiedByMagnitudeFactorTarget . Append (.765625);
          else if (magnitude . Nth (i) >= 4.5 && magnitude . Nth (i) < 5.5)
            stratifiedByMagnitudeFactorTarget . Append (1.34);
          else if (magnitude . Nth (i) >= 5.5 && magnitude . Nth (i) < 6.5)
            stratifiedByMagnitudeFactorTarget . Append (2.34);
          else if (magnitude . Nth (i) >= 6.5 && magnitude . Nth (i) < 7.5)
            stratifiedByMagnitudeFactorTarget . Append (4.1);
          else if (magnitude . Nth (i) >= 7.5 && magnitude . Nth (i) < 8.5)
            stratifiedByMagnitudeFactorTarget . Append (7.18);
          else if (magnitude . Nth (i) >= 8.5 && magnitude . Nth (i) < 9.5)
            stratifiedByMagnitudeFactorTarget . Append (12.57);
          else if (magnitude . Nth (i) >= 9.5 && magnitude . Nth (i) <= 10)
            stratifiedByMagnitudeFactorTarget . Append (22);
        }

      //  EXPLODED BY DEPTH VALUES
      quake_depth_factor_max = .55;
      if (explodedByDepth)
        quake_depth_factor = quake_depth_factor_max;
      else
        quake_depth_factor = 0;

      victory_displacement_previous = Vect ();

      //  Set and update the VBO values
      LoadShaders ("shaders/pointsizefog.vert", "shaders/null.frag");
      UpdatePointLocations ();
      UpdatePointColors (true);
      updatePointSize ();
      DataReady ();

      INFORM (INT (Count ()) + " EARTHQUAKES LOADED / quake_depth_max = " +
            INT (quake_depth_max) + ", quake_depth_min = " + INT (quake_depth_min) +
            ", quake_depth_avg = " + FLOAT (quake_depth_avg) +
            " / quake_magnitude_max = " + FLOAT (quake_magnitude_max) +
            ", quake_magnitude_min = " + FLOAT (quake_magnitude_min) +
            ", quake_magnitude_avg = " + FLOAT (quake_magnitude_avg) +
            " / quake_longitude_max = " + FLOAT (quake_longitude_max) +
            ", quake_longitude_min = " + FLOAT (quake_longitude_min) +
            " / quake_latitude_max = " + FLOAT (quake_latitude_max) +
            ", quake_latitude_min = " + FLOAT (quake_latitude_min) );

      //  Set the UI text elements' values
      t_system_magnitude = new Text ("t_system_magnitude"); // + FLOAT (system_magnitude_max));
      t_system_magnitude -> SetTranslation (Inside (Feld (), Vect (.475, -.39, -.01)));
      t_system_magnitude -> SetFontSize (Feld () -> Width () / 80);
      t_system_magnitude -> SetAlignmentRight ();
      t_system_magnitude -> SetString ("system_magnitude = 0.0-10.0");

      t_system_depth = new Text ("t_system_depth"); // + FLOAT (system_magnitude_max));
      t_system_depth -> SetTranslation (Inside (Feld (), Vect (.475, -.41, -.01)));
      t_system_depth -> SetFontSize (Feld () -> Width () / 80);
      t_system_depth -> SetAlignmentRight ();
      t_system_depth -> SetString ("system_depth = " + INT (system_depth) + "km");

      t_quakes_within_range = new Text ("t_quakes_within_range");
                                        // + FLOAT (system_magnitude_max));
      t_quakes_within_range -> SetTranslation (Inside (Feld (), Vect (.475, -.43, -.01)));
      t_quakes_within_range -> SetFontSize (Feld () -> Width () / 80);
      t_quakes_within_range -> SetAlignmentRight ();
      int64 quakes_within_range = 0;
          for (int64 i = 0; i < Count (); i++)
            if ( Abs (depth . Nth (i) - system_depth) < 10)
              quakes_within_range++;
      t_quakes_within_range -> SetString ("quakes_within_range (+/- 10km) = " + INT (quakes_within_range));

      t_system_time = new Text ("t_system_time"); // + FLOAT (system_magnitude_max));
      t_system_time -> SetTranslation (Inside (Feld (), Vect (.475, .45, -.01)));
      t_system_time -> SetFontSize (Feld () -> Width () / 60);
      t_system_time -> SetAlignmentRight ();
      // t_system_time -> SetString ("system_current_year = " + INT (system_current_year) + " / " + "system_current_month = " + INT (system_current_month));
      if (all_z_visible)
        t_system_time -> SetString (INT (system_start_year) + " - 2012");
      else if (system_current_month < 10)
        t_system_time -> SetString ("0" + INT (system_current_month) + " / " + INT (system_current_year));
      else
        t_system_time -> SetString (INT (system_current_month) + " / " + INT (system_current_year));

      //  Set the rate of Fired ()
      SetFireTimer (1.0);
    }


  void UpdatePointLocations ()
    { for (int64 i = 0; i < Count (); i++)
        { if (viewDataAsGlobe)
            { Vect globe_position = LatLongToSphereSurface (GLOBE_RADIUS  - depth . Nth (i) * quake_depth_factor / 6 +
                                                        stratifiedByMagnitudeOffset * stratifiedByMagnitudeFactorTarget . Nth (i) * stratified_by_magnitude_factor,
                                                        latitude . Nth (i), longitude . Nth (i));

              SetPointLocation (i, globe_position . x,
                                globe_position . y,
                                globe_position . z);
            }
          else
            { SetPointLocation (i, longitude . Nth (i),
                                latitude . Nth (i),
                                -depth . Nth (i) * quake_depth_factor / 4 +
                                    stratifiedByMagnitudeOffset * stratifiedByMagnitudeFactorTarget . Nth (i) * stratified_by_magnitude_factor);
            }
        }
    }

  //  Function to set the VBO's point colors
  void UpdatePointColors (bool updateHue = false)
    { Color c;
      for (int64 i = 0; i < Count (); i++)
        { float64 a;
          float64 a_on = .75;
          float64 a_transparent = 0.25;
          float64 a_off = .02;
          // DEAL W SYSTEM_DEPTH && SYSTEM_MAGNITUDE
          if (system_depth == 0) // if the system depth is 0
            if (system_magnitude == 0) // and the system magnitude is 0
              a = a_on; // a_mid; // set all the alphas to .15
            else if (system_magnitude == 1) // and the system magnitude is 0
              a = a_transparent; // a_mid; // set all the alphas to .15
            else // if the system magnitude is not 0
              if (Abs (magnitude . Nth (i) - system_magnitude) < .5) // and the quake mag is within .5 of the system magnitude
                a = a_on; // set all the alphas to .75
              else // and the quake mag is greater than a .5 diff from the system magnitude
                a = a_off; // set all the alphas to .15
          else // if the system depth is not 0
            if (system_magnitude == 0) // and the system magnitude is 0
              if ( Abs (depth . Nth (i) - system_depth) < 10) // and if the quake depth is within 10 of the system depth
                a = a_on; // this was used to turn on all quakes when system_depth was within 10 of 0
              else
                a = a_off; // a_mid;
            else // if the system magnitude is not 0
              if ( Abs (depth . Nth (i) - system_depth) < 10) // if the quake depth is within 10 of the system depth
                if (Abs (magnitude . Nth (i) - system_magnitude) < .5) // and the quake mag is within .5 of the system magnitude
                  a = a_on;
                else // and the quake mag is greater than a .5 diff from the system magnitude
                  a = a_off; //
              else // if the quake depth is not within 10 of the system depth
                a = a_off; //
          // OVERRIDE SYSTEM_DEPTH && SYSTEM_MAGNITUDE W TIME DATA
          // if (auto_play &&
          if (!all_z_visible && (time_year . Nth (i) != system_current_year ||
              time_month . Nth (i) != system_current_month))
            a = a_off;
          float64 h = 0.0;
          if (draw_mode == "depth")
            h = Range (depth . Nth (i), quake_depth_min, quake_depth_max/4.0, 0.0, 0.77);
          else if (draw_mode == "magnitude")
            h = Range (magnitude . Nth (i), 3.0, 5.0, 0.77, 0.0);
          if (updateHue)
            SetPointColor (i, HSB (h, 1, 1, a));
          else
            SetPointAlpha (i, a);
        }
    }

  //  Function to set the VBO's point sizes
  void updatePointSize ()
    { for (int64 i = 0; i < Count (); i++)
        { float64 m =  ComputePointSize (magnitude . Nth (i));
          SetPointSize (i, m);
        }
    }

  //  Function to update the Shader's uniforms
  void AssignShaderInputs ()
      { if (viewDataAsGlobe)
          { SetShaderUniform ("fog_radius", GLOBE_RADIUS);
            SetShaderUniform ("system_distance", distFromCamera);
            SetShaderUniform ("feld_size", Diag (Feld ())*2);
            // INFORM ("system_distance / distFromCamera = " + FLOAT(distFromCamera));
            // INFORM ("Diag (Feld ()) = " + FLOAT (Diag (Feld ())));
            SetShaderUniform ("camera_position", Feld () -> Camera () -> ViewLoc ());
          }
        else
          { SetShaderUniform ("fog_radius", GLOBE_RADIUS * 4);
            SetShaderUniform ("system_distance", distFromCamera);
            SetShaderUniform ("feld_size", Diag (Feld ())*2);
            SetShaderUniform ("camera_position", Feld () -> Camera () -> ViewLoc ());
          }
      }

  //  Function to set system_magnitude (quake magnitude filter)
  void SetSystemMagnitude (int num)
    { system_magnitude = num;
      UpdatePointColors ();
      t_system_magnitude -> SetString ("system_magnitude (+/- 0.5) = " + FLOAT (system_magnitude));
    }

  //  Function to set system_magnitude (quake magnitude filter)
  void SetSystemMagnitude (BlurtEvent *e)
    { SetSystemMagnitude (Number (e -> Utterance ())); }

  //  Travail handles all expanding by depth and straification animations
  void Travail ()
    { if (HasTag ("Expand"))
        { if (quake_depth_factor < quake_depth_factor_max)
            { quake_depth_factor += .085 * (quake_depth_factor_max - quake_depth_factor);
              UpdatePointLocations ();
              if (quake_depth_factor > (quake_depth_factor_max - .008)) quake_depth_factor = quake_depth_factor_max;
            }
          else
            { quake_depth_factor = quake_depth_factor_max;
              UpdatePointLocations ();
              UnTag ("Expand");
            }
        }

      if (HasTag ("Contract"))
        { if (quake_depth_factor > 0)
            { quake_depth_factor -= .085 * (quake_depth_factor);
              UpdatePointLocations ();
              if (quake_depth_factor < .008) quake_depth_factor = 0;
            }
          else
            { quake_depth_factor = 0;
              UpdatePointLocations ();
              UnTag ("Contract");
            }
        }

      if (HasTag ("Stratify"))
        { if (stratified_by_magnitude_factor < 1.0)
            { stratified_by_magnitude_factor += .1 * (1.0 - stratified_by_magnitude_factor);
              if ((1.0 - stratified_by_magnitude_factor) < .01)
                { stratified_by_magnitude_factor = 1;
                  UnTag ("Stratify");
                }
              UpdatePointLocations ();
            }
        }

      if (HasTag ("Unstratify"))
        { if (stratified_by_magnitude_factor > 0.0)
            { stratified_by_magnitude_factor -= .1 * (stratified_by_magnitude_factor);
              if (stratified_by_magnitude_factor < .01)
                { stratified_by_magnitude_factor = 0;
                  UnTag ("Unstratify");
                }
              UpdatePointLocations ();
            }
        }
    }

  //  Fired handles auto_play filtering by time
  void Fired ()
    { if (auto_play)
        { system_current_month++;
          if (system_current_month == 13)
            { system_current_month = 1;
              system_current_year++;
            }
          if (system_current_year == 2013)
            system_current_year = system_start_year;
          UpdatePointColors ();

          if (system_current_month < 10)
            t_system_time -> SetString ("0" + INT (system_current_month) + " / " + INT (system_current_year));
          else
            t_system_time -> SetString (INT (system_current_month) + " / " + INT (system_current_year));
        }

      // label turn off code moved from Travail ()
      for (int64 k = 0; k < labels . Count (); k++)
        { Text *t_update = labels . NthVal (k);
          Selector *s_update = labels_graphics . NthVal (k);

          // check if the pointer label is moving
          if (t_update -> HasTag ("pointer_moving"))
            { t_update -> UnTag ("pointer_moving"); }
          else // if not moving
            { // turn the text label off
              t_update -> ColorAnimateQuadratic (.25);
              t_update -> SetAlpha (0);

              // and remove the label graphcis (rotating box)
              s_update -> opengl_lines . Empty ();
              s_update -> opengl_lines_sizes . Empty ();
              s_update -> opengl_lines_alphas . Empty ();
            }
        }
    }

  //  Function to handle Pointer interactions with individual data points
  void IndividualQuakeInteract (PointingEvent *e)
    { //  If the pointer is currently being followed (aka it's new)
      if (!labels . KeyIsPresent (e -> Provenance ()))
        { // Create a Text for the Pointer
          Text *t = new Text (Name ());
          t -> SetObliviousness (true);
          t -> SetFontSize (Feld () -> Width () / 80.0);
          labels . Put (e -> Provenance (), t);

          // Create a Selector for the Pointer
          Selector *s = new Selector ( 0 );
          s->provenance = e -> Provenance ();
          s -> SetObliviousness (true);
          labels_graphics . Put (e -> Provenance (), s);
        }

      int closest = FindClosestPoint (e, true);
      if (closest < 1  || closest > (magnitude . Count () - 1))
        { return; } //  returning because closest is possibly out of bounds

      //  Find the magnitude of the closest point and
      //  use it with/to ComputePointSize ()
      float m = 0;
      Vect quake_abs_loc;
      quake_abs_loc . SetInvalid ();
      if (closest > -1)
        { //  Find the location of the quake in the Eathquakes object
          //  and then use WrangleLoc () to determine the absolute location
          //  which takes into consideration translations and rotations
          //  applied to the parent object (the DataSet class)
          quake_abs_loc = WrangleLoc (PointLocation (closest));
          m = ComputePointSize (magnitude . Nth (closest));
          m *= .3;
        }

      //  Update the Text label based on the closest point
      Text *t_update = labels . ValFromKey (e -> Provenance ());
      t_update -> SetString ("M" + Compose ("%.1f", magnitude . Nth (closest)) + ", " +
                              // INT (depth . Nth (closest)) + "km, " +
                              INT (time_month . Nth (closest)) + "/" + INT (time_day . Nth (closest)) +
                              "/" + INT (time_year . Nth (closest)));
      t_update -> SetAlignmentLeft ();
      t_update -> Tag ("pointer_moving");
      t_update -> ColorAnimateQuadratic (0);
      t_update -> SetAlpha (1);

      //  Advanced : Create a new Pointing Event from the Camera to the quake data point
      PointingEvent *pe = new PointingEvent ();
      pe -> SetPhysOrigAndThrough (Feld () -> Camera () -> ViewLoc (),
                                   Feld () -> Camera () -> ViewLoc ()
                                   + (quake_abs_loc - Feld () -> Camera () -> ViewLoc ()) . Norm ());
      //  Find the pe pointer intersection with the Feld ()
      Vect label_loc = Intersection (pe);
      //  Translate the text over to the right based on the Feld size
      Vect trans_amt = label_loc + Feld () -> Over () * (Diag (Feld ()) / 100);
      t_update -> SetTranslation (trans_amt);

      //  Update the Selector graphics based on the closest point location and magnitude
      Selector *s_update = labels_graphics . ValFromKey (e -> Provenance ());
      s_update -> RebuildScale ( m ); //  Based on the quake magnitude
      s_update -> SetTranslation (quake_abs_loc);

    }

  //  GESTURE INTERACTIVITY WITH THE EARTHQUAKE DATA
  //  Finger interactions to get individual quake data
  void FingerMove (PointingEvent *e)
    { if(!viewDataAsGlobe) //  Only in the equirectangular mode
        { IndividualQuakeInteract (e); }
    }

  //  Pointer harden (click) interactions to stratify / expand the data
  void PointingHarden (PointingEvent *e)
    { Str pointing_provenance = e -> Provenance ();
      Str search_string = "iOS"; //  For the iOS device
      if (pointing_provenance.Match(search_string))
        { if (stratifiedByMagnitude)
            { if (HasTag ("Stratify")) UnTag ("Stratify");
              Tag ("Unstratify");
              stratifiedByMagnitude = false;

              if (HasTag ("Contract")) UnTag("Contract");
              if (explodedByDepth) Tag ("Expand");
            }
          else // if !stratifiedByMagnitude
            { if (HasTag ("Unstratify")) UnTag ("Unstratify");
              Tag ("Stratify");
              stratifiedByMagnitude = true;

              if (HasTag ("Expand")) UnTag("Expand");
              if (explodedByDepth) Tag ("Contract");
            }
        }
    }

  //  Victory gesture interactions to filter the data by quake depth plane
  void Victory (const Vect &displacement)
    { //  If the data has not been expanded by depth, do so now
      if (quake_depth_factor == 0 && !stratifiedByMagnitude)
        { Tag ("Expand");
          explodedByDepth = true;
        }
      //  If the data has been filtered by depth or magnitude,
      if (system_depth != 0 && system_magnitude != 0)
        { SetSystemMagnitude (0); } //  make the visible points brighter
                                    //  by setting the magnitude to 0

      //  Update the system based on Victory gesture Norm () movement,
      if (system_depth == 0 && displacement . Mag () < 40)
        { }
      else //  only if a displacement of over 40mm is detected (a quick push)
        { system_depth += .4 * displacement . Dot (- Feld () -> Norm ());
          INFORM ("Victory displacement . Mag () is " + FLOAT (displacement . Mag ()));
        }

      //  Reset system_depth to 0 if < 0
      if (system_depth <= 0 && system_depth != 0)
        { system_depth = 0;
          SetSystemMagnitude (1);
        }
      //  Reset system_depth to quake_depth_max if > quake_depth_max
      else if (system_depth > quake_depth_max)
        { system_depth = quake_depth_max; }

      //  Count the number of quakes within the depth range
      int quakes_within_range = 0;
      for (int i = 0; i < Count (); i++)
        if (Abs (depth . Nth (i) - system_depth) < 10)
          quakes_within_range++;

      //  Only update the VBO + Text when there has been a change in the displacement
      if (victory_displacement_previous != displacement)
        { UpdatePointColors ();
          t_system_depth -> SetString ("system_depth = " + INT (system_depth) + "km");
          t_quakes_within_range -> SetString ("quakes_within_range (+/- 10km) = "
                                              + INT (quakes_within_range));
        }

      victory_displacement_previous = displacement;
    }

  // KEYBOARD + MOUSE INTERACTIONS WITH THE EARTHQUAKE DATA
  //  Pointer interactions (i.e. the mouse)
  void PointingMove (PointingEvent *e)
    { IndividualQuakeInteract (e); }

  //  Keyboard interactions
  void Blurt (BlurtEvent *e)
    { //  Mimics pushing the depth plane with the Victory gesture
      if (Utters (e, "+"))
        { if (system_magnitude != 0)
            SetSystemMagnitude (0);

          if(system_depth <= (quake_depth_max-20.0))
            system_depth += 10.0;
          int64 quakes_within_range = 0;
          for (int64 i = 0; i < Count (); i++)
            if (Abs (depth . Nth (i) - system_depth) < 10)
              quakes_within_range++;
          UpdatePointColors ();
          t_system_depth -> SetString ("system_depth = " + INT (system_depth) + "km");
          t_quakes_within_range -> SetString ("quakes_within_range (+/- 10km) = "
                                              + INT (quakes_within_range));
        }

      //  Mimics pulling the depth plane with the Victory gesture
      else if (Utters (e, "_"))
        { if (system_magnitude != 0) SetSystemMagnitude (0);

          if(system_depth >= (quake_depth_min + 10.0))
            system_depth -= 10.0;
          int64 quakes_within_range = 0;
          for (int64 i = 0; i < Count (); i++)
            if ( Abs (depth . Nth (i) - system_depth) < 10)
              quakes_within_range++;
          UpdatePointColors ();
          t_system_depth -> SetString ("system_depth = " + INT (system_depth) + "km");
          t_quakes_within_range -> SetString ("quakes_within_range (+/- 10km) = "
                                              + INT (quakes_within_range));
        }

      //  Toggle on and off explosion of data by depth
      else if (Utters (e, "="))
        { if (explodedByDepth)
            { if (HasTag ("Expand"))
                UnTag ("Expand");
              Tag ("Contract");
              explodedByDepth = false;

              if (stratifiedByMagnitude)
                { if (HasTag ("Unstratify"))
                    UnTag ("Unstratify");
                  Tag ("Stratify");
                }
            }
          else // if !explodedByDepth
            { if (HasTag ("Contract"))
                UnTag ("Contract");
              Tag ("Expand");
              explodedByDepth = true;

              if (stratifiedByMagnitude)
                { if (HasTag ("Stratify"))
                    UnTag ("Stratify");
                  Tag ("Unstratify");
                }
            }
        }

      //  Toggle on and off stratification of data by magnitude
      else if (Utters (e, "x"))
        { if (stratifiedByMagnitude)
            { if (HasTag ("Stratify")) UnTag ("Stratify");
              Tag ("Unstratify");
              stratifiedByMagnitude = false;

              if (explodedByDepth) {
                if (HasTag ("Contract")) UnTag("Contract");
                Tag ("Expand");
              }
            }
          else // if !stratifiedByMagnitude
            { if (HasTag ("Unstratify")) UnTag ("Unstratify");
              Tag ("Stratify");
              stratifiedByMagnitude = true;

              if (explodedByDepth) {
                if (HasTag ("Expand")) UnTag("Expand");
                Tag ("Contract");
              }
            }
        }

      //  Plays through the data month by month
      else if (Utters (e, "p"))
        { auto_play = true;
          all_z_visible = false;
          system_magnitude = 0;
          UpdatePointColors ();
          t_system_magnitude -> SetString ("system_magnitude (+/- 0.5) = " +
                                            FLOAT (system_magnitude));
        }

      //  Stops automatic playthrough
      else if (Utters (e, "o"))
        { auto_play = false;
          UpdatePointColors ();
        }

      //  Toggles between all months of data and an individual month
      else if (Utters (e, "z"))
        { if (all_z_visible)
            { all_z_visible = false;
              t_system_time -> SetString (INT (system_current_month) +
                                          " / " + INT (system_current_year));
              system_magnitude = 0;
            }
          else
            { all_z_visible = true;
              t_system_time -> SetString (INT (system_start_year) + " - 2012");
              system_magnitude = 1;
            }
            UpdatePointColors ();
        }

      //  Set the system_magnitude value to the numeric key press
      else if (Utters (e, "0"))
        { SetSystemMagnitude (e); }
      else if (Utters (e, "1"))
        { SetSystemMagnitude (e); }
      else if (Utters (e, "2"))
        { if (system_magnitude != 0) SetSystemMagnitude (0);
          SetSystemMagnitude (e);
        }
      else if (Utters (e, "3"))
        { if (system_magnitude != 0) SetSystemMagnitude (0);
          SetSystemMagnitude (e);
        }
      else if (Utters (e, "4"))
        { if (system_magnitude != 0) SetSystemMagnitude (0);
          SetSystemMagnitude (e);
        }
      else if (Utters (e, "5"))
        { if (system_magnitude != 0) SetSystemMagnitude (0);
          SetSystemMagnitude (e);
        }
      else if (Utters (e, "6"))
        { if (system_magnitude != 0) SetSystemMagnitude (0);
          SetSystemMagnitude (e);
        }
      else if (Utters (e, "7"))
        { if (system_magnitude != 0) SetSystemMagnitude (0);
          SetSystemMagnitude (e);
        }
      else if (Utters (e, "8"))
        { if (system_magnitude != 0) SetSystemMagnitude (0);
          SetSystemMagnitude (e);
        }
      else if (Utters (e, "9"))
        { if (system_magnitude != 0) SetSystemMagnitude (0);
          SetSystemMagnitude (e);
        }

      //  Draw the earthquake data VBO point colors according to quake depth
      else if (Utters (e, "n"))
        { draw_mode = "depth";
          UpdatePointColors (true);
        }

      //  Draw the earthquake data VBO point colors according to quake magnitude
      else if (Utters (e, "m"))
        { draw_mode = "magnitude";
          UpdatePointColors (true);
        }
    }
};

//  Earthquakes class stores and displays the earthquake data
//  via attached CountryBorders and Earthquakes children.
//  The class also controls global manipulations to the system
//  such as rotation and translation.
class DataSystem  :  public Thing
{
 public:

  DataSystem ()  :  Thing ()
    { AppendKid (new CountryBorders ());
      AppendKid (new Earthquakes ());

      //  To dampen the 3D scanner noise
      //  apply a soften animation to the rotation
      RotationAnimateChase (0.75);
      //  and to the translation of the globe
      TranslationAnimateChase (.25);
    }

  //  Function to reset Dataset to it's original settings
  void OriginalSettings ()
    { //  DataSystem settings
      SetTranslationHard (Feld () -> Loc ());
      RotationAnimateChase (0.75);
      SetRotation (Vect (0, 1, 0), 0);
      SetRotation (Vect (1, 0, 0), 0);

      //  Eathquakes class settings
      Earthquakes *eq = Find ("earthquake_dataset") -> As <Earthquakes> ();
      eq -> system_depth = 0;
      eq -> system_magnitude = 1;
      // show all
      if(!eq -> all_z_visible)
        { eq -> system_current_year = eq -> system_start_year;
          eq -> system_current_month = 01;
          eq -> system_current_day = 01;
          eq -> auto_play = false;
          eq -> all_z_visible = true;
          eq -> t_system_time -> SetString (INT (eq -> system_start_year) + " - 2012");
        }
      eq -> SetSystemMagnitude (1);
      eq -> UpdatePointColors ();

      if(explodedByDepth)
        { if (eq -> HasTag ("Expand"))
            eq -> UnTag ("Expand");
          eq -> Tag ("Contract");
          explodedByDepth = false;
        }

      if (stratifiedByMagnitude)
        {
          if (eq -> HasTag ("Stratify"))
            eq -> UnTag ("Stratify");
          eq -> Tag ("Unstratify");
          stratifiedByMagnitude = false;
        }
    }

  void Travail ()
    { //  Update distFromCamera for shaders
      distFromCamera = Loc () . DistFrom (Feld () -> Camera () -> ViewLoc ());

      //  If an owner has had posession of the system for over 5 seconds
      //  kick them off.  Occasionally, noise in the system, like a coffee
      //  mug on your desk, can be interpreted as a fist and take ownership
      //  of the system
      if (! IsHeedless () && CurTime () > 5.0)
        { StopHeeding ();
          WARN ("I just ousted your lazy owner");
        }
    }

  // GESTURE INTERACTIONS WITH THE DATASET CLASS
  // Translation
  void FistAppear (PointingEvent *e)
    { //  INFORM ("seismo FistAppear " + e -> Provenance ());

      //  Set ownership (heeding) of the system so that not more than one
      //  user can translate the system at a time.
      if (IsHeedless ())
        { Heed (e);    //  Set the current pointer's Provenance as the owner
          ZeroTime (); //  Zero out CurTime ()
                       //  (tested for in Travail)
        }
    }

  void FistVanish (PointingEvent *e)
    { // INFORM ("seismo FistVanish " + e -> Provenance ());
      StopHeeding (e); //  Release the current pointer's ownership
    }

  void FistMove (PointingEvent *e)
    { if (IsHeeding (e)) //  if the current pointer is the owner
        { ZeroTime ();   //  Zero out CurTime ()

          //  Translate the DataSystem
          IncTranslation (6.0 * FistMotion (e, PhysLoc ()));
        }
    }

  // Rotation
  void FingerMove (PointingEvent *e)
    { if (viewDataAsGlobe)
          { //  Rotate around the UnWrangleRay (Feld () -> Up ()) axis.
            //  UnWrangleRay () determines the current Up () axis for the system
            //  ( taking into account currently applied rotations and translations )
            //  so that we can rotate the globe horizontally no matter what the orientation
            IncRotation (UnWrangleRay (Feld () -> Up ()),
                         (e -> PhysOrigin () . x - e -> PrevOrigin () . x) / 250);

            //  Rotate vertically around the UnWrangleRay (Feld () -> Up ()) axis
            IncRotation (UnWrangleRay (Feld () -> Over ()),
                         - (e -> PhysOrigin () . y - e -> PrevOrigin () . y) / 250);
          }
    }

  // iOS INTERACTIONS WITH THE DATASET CLASS
  void SwipeUp (BlurtEvent *e)
    { IncRotation (UnWrangleRay (Feld () -> Over ()), -PI / 2); }
  void SwipeDown (BlurtEvent *e)
    { IncRotation (UnWrangleRay (Feld () -> Over ()), PI / 2); }
  void SwipeRight (BlurtEvent *e)
    { IncRotation (UnWrangleRay (Feld () -> Up ()), PI / 2); }
  void SwipeLeft (BlurtEvent *e)
    { IncRotation (UnWrangleRay (Feld () -> Up ()), -PI / 2); }

  // KEYBOARD + MOUSE INTERACTIONS WITH THE DATASET CLASS
  // Mouse
  void PointingHarden (PointingEvent *e)
    { //  Set ownership (heeding) of the system so that not more than one
      //  user can translate the system at a time.
      if (IsHeedless ())
        { Heed (e);    //  Set the current pointer's Provenance as the owner
          ZeroTime (); //  Zero out CurTime ()
        }
    }
  void PointingSoften (PointingEvent *e)
    { StopHeeding (e); }

  void PointingMove (PointingEvent *e)
    { if (IsHeeding (e)) //  if the current pointer is the owner
        { ZeroTime ();   //  Zero out CurTime ()
          if(viewDataAsGlobe) //  rotate the DataSystem
            { //  Rotate around the UnWrangleRay (Feld () -> Up ()) axis.
              //  UnWrangleRay () determines the current Up () axis for the system
              //  ( taking into account currently applied rotations and translations )
              //  so that we can rotate the globe horizontally no matter what the orientation
              IncRotation (UnWrangleRay (Feld () -> Up ()),
                           IntersectionDiff (e, PhysLoc ()) . x / 100);

              //  Rotate vertically around the UnWrangleRay (Feld () -> Up ()) axis
              IncRotation (UnWrangleRay (Feld () -> Over ()),
                           - IntersectionDiff (e, PhysLoc ()) . y / 100);
            }
          else //  if !viewDataAsGlobe, translate the DataSystem
            { IncTranslation (5 * IntersectionDiff (e, PhysLoc ())); }
        }
    }

  //  Keyboard
  void Blurt (BlurtEvent *e)
    { //  Mimic fist rotation for debugging without a 3D sensor
      if (Utters (e, "w")) //  Rotate negatively on the Over () axis
        { IncRotation (UnWrangleRay (Feld () -> Over ()), -PI / 18); }
      else if (Utters (e, "s")) //  Rotate positively on the Over () axis
        { IncRotation (UnWrangleRay (Feld () -> Over ()), PI / 18); }
      else if (Utters (e, "a")) //  Rotate negatively on the Up () axis
        { IncRotation (UnWrangleRay (Feld () -> Up ()), -PI / 18); }
      else if (Utters (e, "d")) //  Rotate positively on the Up () axis
        { IncRotation (UnWrangleRay (Feld () -> Up ()), PI / 18); }

      //  Mimic fist translation for debugging without a 3D sensor
      int64 translation_amount = 40;
      if (Utters (e, "i")) //  Transalte positively on the Norm () axis
        { IncTranslation (translation_amount * 2 * Feld () -> Norm ()); }
      else if (Utters (e, "k")) //  Transalte negatively on the Norm () axis
        { IncTranslation (-translation_amount * 2 * Feld () -> Norm ()); }
      else if (Utters (e, "j")) //  Transalte positively on the Norm () axis
        { IncTranslation (-translation_amount * Feld () -> Over ()); }
      else if (Utters (e, "l")) //  Transalte negatively on the Over () axis
        { IncTranslation (translation_amount * Feld () -> Over ()); }
      else if (Utters (e, "y")) //  Transalte positively on the Over () axis
        { IncTranslation (translation_amount * 1 * Feld () -> Up ()); }
      else if (Utters (e, "h")) //  Transalte negatively on the Up () axis
        { IncTranslation (-translation_amount * 1 * Feld () -> Up ()); }

      //  Turn on/off an automatic rotation of the DataSystem
      else if (Utters (e, ".")) //  On
        { RotationAnimateSine (UnWrangleRay (Feld () -> Up ()), Rad (30), 12.5); }
      else if (Utters (e, ",")) //  Off
        { RotationAnimateChase (0.75);
          SetRotation (UnWrangleRay (Feld () -> Up ()), 0);
        }

      //  System reset to the Original Settings
      else if (Utters (e, "!"))
        { OriginalSettings (); }

      //  Toggle viewDataAsGlobe (between globe and equirectangular modes)
      else if (Utters (e, "@"))
        { RotationAnimateChase (0.75);
          SetRotation (Vect (0, 1, 0), 0);
          SetRotation (Vect (1, 0, 0), 0);

          if(viewDataAsGlobe)
            { viewDataAsGlobe = false; }
          else
            { viewDataAsGlobe = true; }

          CountryBorders *cb = Find ("country_borders_dataset") -> As <CountryBorders> ();
          cb -> UpdatePointLocations();
          cb -> UpdatePointColors();

          Earthquakes *eq2 = Find ("earthquake_dataset") -> As <Earthquakes> ();
          eq2 -> UpdatePointLocations ();
        }

      else if (Utters (e, "next"))
        { Str genus = e -> Genus ();
          bool view = viewDataAsGlobe;

          if (genus . Match ("left"))
            { if (viewDataAsGlobe)
                viewDataAsGlobe = false;
              else
                viewDataAsGlobe = true;
            }

          if ( view == viewDataAsGlobe)
            return;

          RotationAnimateChase (0.75);
          SetRotation (Vect (0, 1, 0), 0);
          SetRotation (Vect (1, 0, 0), 0);

          CountryBorders *cb = Find ("country_borders_dataset") -> As <CountryBorders> ();
          cb -> UpdatePointLocations();
          cb -> UpdatePointColors();

          Earthquakes *eq2 = Find ("earthquake_dataset") -> As <Earthquakes> ();
          eq2 -> UpdatePointLocations ();
        }
    }

};


void Setup ()
{ // Black background color
  SetFeldsColor (0.0, 0.0, 0.0);

  //  Turn off Greenhouse cursors
  HideNeedlePoints ();

  //  Register for any non-default gesters
  RegisterOneHandBlurt ("next", LShapePose);

  //  Set the intial state of the global variables
  viewDataAsGlobe = false;
  stratifiedByMagnitude = false;
  explodedByDepth = false;

  //  Create the DataSystem (with CountryBorders and Earthquakes children)
  DataSystem *g = new DataSystem ();

  //  Center the DataSystem on the screen and orient them like the Feld
  g -> SlapOnFeld ();
}

