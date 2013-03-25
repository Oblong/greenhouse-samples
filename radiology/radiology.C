/* (c)  oblong industries */

#include "Greenhouse.h"

//  For working with directories in the file system
#include <dirent.h>

//  Comparison function for use in Trove sort
static int cmp_str (const Str &a, const Str &b)
  { return a . Compare (b); }


class Skelly : public Thing
{ public:
  
  Vect start_loc;
  int64 top_index;
  int64 start_index;
  Trove <Image *> layers;
  Image *current_layer;

  Skelly (const Str &dir) : Thing (),
      top_index (-1),
      start_index (0),
      current_layer (NULL)
    { Trove <Str> image_paths = ReadImageDir (dir);
      int64 num_imgs = image_paths . Count ();
      image_paths . Quicksort (cmp_str);

      for (int64 i = 0; i < num_imgs; i++)
        { Image *sl = new Image (image_paths [i]);

          // make it invisible and the height of the screen
          sl -> SetAlpha (0.0);
          sl -> SetHeightUniformly (Feld () -> Height ());
	        sl -> SetName (image_paths [i]);
          layers . Append (sl);
          AppendKid (sl);
        }

      SetTopmostLayer (0);
    }

  void SetTopmostLayer (int64 ind)
    { int64 top = (ind < layers . Count ())  ?  ind  :  
                                              (layers . Count () - 1);
      top = (top > 0)  ?  top  :  0;
      if (top == top_index)
        return;
      
      if (current_layer)
        current_layer -> SetAlpha (0.0);
      current_layer = layers[top];
      if (current_layer)
        current_layer -> SetAlpha (1.0);
      top_index = top;
    }

  /// The following methods handle the fist gesture

  void FistVanish (PointingEvent *e)
    { // release the fist from ownership
      StopHeeding (e);
    }

  void FistMove (PointingEvent *e)
    { if (IsHeedless ())
      	{ Heed (e);
      	  start_loc = e -> PhysOrigin ();
      	  start_index = top_index;
      	}

      if (! IsHeeding (e))
	      return;

      // how much has the user fisted in the direction of the screen?
      float64 d_y = (e -> PhysOrigin () - start_loc) . Dot (Norm ());

      // calculate the percent of movement over a foot
      // figure a person only wants to move their fist about a foot
      d_y = d_y / 300.0;

      // determine the index based on where we started
      int64 ind = start_index + floor (d_y * layers . Count ());

      // show the new top layer
      SetTopmostLayer (ind);
    }

  /// The following methods handle the mouse...

  void PointingHarden (PointingEvent *e)
    { if (IsHeedless ())
        { Heed (e);
          start_loc = Intersection (e);
          start_index = top_index;
        }
    }

  void PointingSoften (PointingEvent *e)
    { if (IsHeeding (e))
        StopHeeding (e);
    }

  void PointingMove (PointingEvent *e)
    { if (! IsHeeding (e))
        return;

      float64 d_y = (Intersection (e) - start_loc) . Dot (Up ());

      // calculate the percent of movement over half the height of the screen
      // figure a person only wants to drag the mouse half way up the screen
      d_y = 2 * d_y / Feld () -> Height ();

      // determine the index based on where we started
      int64 ind = start_index + floor (d_y * layers . Count ());

      // show the new top layer
      SetTopmostLayer (ind);
    }

  Trove <Str> ReadImageDir (const Str &dir)
    { Trove <Str> imgs;
      Str full_path = FindDirPath (dir);
      if (full_path . IsEmpty ())
        { WARN ("Could not open directory: " + dir);
          return imgs;
        }
      
      DIR *in_dir;
      struct dirent *entry;
      INFORM ("opening directory: " + full_path);
      in_dir = opendir (CHAR (full_path));
      if (in_dir)
        { entry = readdir (in_dir);
	        while(entry)
            { Str name = Str (entry->d_name);
              if (name . Match (".png"))
                imgs . Append (full_path + "/" + name);
	            entry = readdir (in_dir);
            }
        }
      return imgs;
    }

};

void Setup ()
{ SetFeldsColor (0.0, 0.0, 0.0);
  
  //  Use logical ordering, where the last-created object 
  //  is drawn on top
  SetDepthSorting (false);

  INFORM ("setting up the first skelly");

  // create a new skelly object and move it left
  Skelly *skelly1 = new Skelly ("images/pet");
  skelly1 -> SetTranslation (Inside (Feld (), Vect (-0.25, 0.0, 0.0)));

  INFORM ("setting up the first skelly");

  // create a new skelly object and move it right
  Skelly *skelly2 = new Skelly ("images/ct");
  skelly2 -> SetTranslation (Inside (Feld (), Vect (0.25, 0.0, 0.0)));
}
