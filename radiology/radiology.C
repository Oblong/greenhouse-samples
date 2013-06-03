/* (c)  oblong industries */

#include "Greenhouse.h"

/**
  To ensure the image resources are found when running this sample, you will
  need to do the following:

    If running in Xcode:
      - set your working directory to this sample's share directory:

        - Go to Product -> Scheme -> Edit Scheme
        - Under “Options”, select “Use custom working directory” and point it
          to the share folder located in the same location as this .C file

    If running via terminal:
      - add the local share/ folder to your OB_SHARE_PATH by running the
        following command in your terminal (with your correct path subbed in):

        $ export OB_SHARE_PATH=/path/to/radiology/share:$OB_SHARE_PATH
*/


//  For working with directories in the file system
#include <dirent.h>
#include <ImageMagick/Magick++.h>

#include <libMedia/PngImageClot.h>

using namespace oblong::media;

//  Comparison function for use in Trove sort
static int cmp_str (const Str &a, const Str &b)
  { return a . Compare (b); }


class Skelly : public Thing
{ public:

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
        sl -> SetHeightUniformly (0.7 * Feld () -> Height ());
        sl -> SetName (image_paths [i]);
        layers . Append (sl);
        AppendKid (sl);
      }

    SetLayerByIndex (0);
  }

  void UpdateStartIndex ()
  { start_index = top_index; }

  void SetLayerByPercentage (float64 perc)
  { SetLayerByIndex (start_index + floor (perc * layers . Count ())); }

  void SetLayerByIndex (int64 ind)
  { int64 top = (ind < layers . Count ()) ? ind : (layers . Count () - 1);
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
    in_dir = opendir (ToStr (full_path));
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

Skelly *skelly1;
Skelly *skelly2;
Image *floater;

class InfoGraphic : public Thing
{
public:
  Image *guy;
  Vect line_b, line_t;
  Vect line_b_left, line_t_left;
  float64 start_cent, cur_cent;
  Vect start_loc;
  float64 max_over;

  InfoGraphic () : Thing ()
  { guy = new Image ("images/radiology/male_side.png");
    guy -> SetHeightUniformly (0.3 * Feld () -> Height ());
    guy -> SetTranslation (Inside (Feld (), Vect (0.38, -0.05, 0.0)));
    guy -> IncTranslation (100 * Feld () -> Norm ());
    guy -> SetAdjColor (Color (1.0, 0.5));

    start_cent = cur_cent = 0.0;
    max_over = 0;

    SetAdjColor (Color ("#ff9900"));
    SetOnWindshield (true);
    SetFireTimer (0.5, 1.0);
  }

  void Fired ()
  { line_b = line_t_left = guy -> Loc () + 0.25 * guy -> Width () * Over () -
                           0.55 * guy -> Height () * guy -> Up ();
    line_t = line_b_left = guy -> Loc () + 0.25 * guy -> Width () * Over () +
                           0.55 * guy -> Height () * guy -> Up ();
    max_over = 0.7 * guy -> Width ();
  }

  void DrawSelf ()
  { glBegin (GL_QUADS);
    glVertex (line_b);
    glVertex (line_b + 0.5 * guy -> Over ());
    glVertex (line_t + 0.5 * guy -> Over ());
    glVertex (line_t);
    glEnd ();
  }

  void UpdateStart ()
  { skelly1 -> UpdateStartIndex ();
    skelly2 -> UpdateStartIndex ();
    start_cent = cur_cent;
  }

  void SetSelection (float64 perc)
  { skelly1 -> SetLayerByPercentage (perc);
    skelly2 -> SetLayerByPercentage (perc);
    SetLineByPercentage (perc);
  }

  void SetLineByCenter (float64 cent)
  { cent = cent > max_over ? max_over : cent;
    cent = cent < 0.0 ? 0.0 : cent;
    line_t = line_t_left - cent * guy -> Over ();
    line_b = line_b_left - cent * guy -> Over ();
    cur_cent = cent;
  }

  void SetLineByPercentage (float64 perc)
  { float64 cent = start_cent + perc * max_over;
    SetLineByCenter (cent);
  }

  void Travail ()
  { if (! IsHeedless () && CurTime () > 2.0)
      StopHeeding ();
  }

  void DisplacementAppear (DisplacementEvent *e)
  { if (IsHeedless ())
      { Heed (e);
        UpdateStart ();
      }
  }

  void DisplacementMove (DisplacementEvent *e)
  { if (IsHeedless ())
      { Heed (e);
        UpdateStart ();
      }
    if (IsHeeding (e))
      { float64 d_y = e -> CumuLinearOffset () . Dot (Norm ());
        // calculate the percent of movement over a foot
        // figure a person only wants to move their fist about a foot
        d_y = -d_y / 300.0;

        // determine the index based on where we started
        SetSelection (d_y);
        ZeroTime ();
      }
  }

  void DisplacementVanish (DisplacementEvent *e)
  { StopHeeding (e); }

  /// The following methods handle the mouse and iOS...
  void PointingHarden (PointingEvent *e)
  { if (IsHeedless ())
      { Heed (e);
        start_loc = Intersection (e);
        UpdateStart ();
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
    SetSelection (d_y);

    ZeroTime ();
  }

};

InfoGraphic *legend;

class GalleryImg : public Image {
private:
  bool hidden;
  v2int64 skellyLayers;
  float64 line_loc;

public:
  GalleryImg (int64 skelly1Layer, int64 skelly2Layer, float64 legend_line)
    : Image ()
  { skellyLayers . x = skelly1Layer;
    skellyLayers . y = skelly2Layer;
    line_loc = legend_line;
    hidden = false;
    ColorAnimateQuadratic (0.25);
  }

  void Hide ()
  { SetAdjColor (Color (1.0, 0.0));
    hidden = true;
  }

  void Show ()
  { SetAdjColor (Color (1.0, 1.0));
    hidden = false;
  }

  void Fired ()
  { floater -> SetAdjColorHard (Color (1.0, 0.0));
    skelly1 -> SetLayerByIndex (skellyLayers . x);
    skelly2 -> SetLayerByIndex (skellyLayers . y);
    legend  -> SetLineByCenter (line_loc);
    legend  -> UpdateStart ();
  }

  void PointingInsideHarden (PointingEvent *e)
  { if (hidden)
      return;

    floater -> SetImage (Data ());
    floater -> SetAdjColor (Color (1.0, 1.0));
    floater -> SetTranslationHard (Loc ());
    floater -> SetTranslation (Inside (Feld (), Vect (-0.025, 0.1, 0.0)));
    floater -> SetScaleHard (0.3);
    floater -> SetScale (1.7);
    SetFireTimer (0.6, 1);

    Str prov = e -> Provenance ();
    if (prov . Matches ("iOS") || prov . Matches ("android"))
      { SendImageToMobileDevice (prov, "radiology", Data ());
        DisplayImageOnMobileDevice (prov, "radiology");
      }
  }
};

class Gallery : public Image {
private:
  float gallery_height;
  float64 over_offset;
  Vect rightmost_trans;
  int64 rightmost_entry, leftmost_entry;
  Image *l_arrow, *r_arrow;
  Thing *scroll_region;

  Color default_bg;
  Color disabled;
  Color enabled;
  Color hovered;

public:
  Gallery () : Image ()
  { over_offset = Feld () -> Width () / 6.0;
    gallery_height = 0.15 * Feld () -> Height ();
    rightmost_trans = -2.5 * over_offset * Over ();
    rightmost_entry = 0;
    leftmost_entry = 1;
    default_bg = Color (0.1, 1.0);//14, 0.14, 0.19, 1.0);
    enabled = Color (0.4, 1.0);
    disabled = Color (0.0, 1.0);//25, 1.0);
    hovered = Color ("#ff9900");//0.23, 0.23, 0.28, 1.0);

    SetImage (NULL);
    SetHeight (gallery_height);
    SetWidth (Feld () -> Width () - 2 * over_offset);
    //SetBackingColor (default_bg);
    SlapOnFeld ();

    l_arrow = new Image ("images/radiology/l-arrow.png");
    l_arrow -> SetName ("scroll-right");
    l_arrow -> SetTranslation (rightmost_trans);
    l_arrow -> SetHeightUniformly (0.8 * gallery_height);
    l_arrow -> SetAdjColor (disabled);
    AppendKid (l_arrow);

    r_arrow = new Image ("images/radiology/r-arrow.png");
    r_arrow -> SetName ("scroll-left");
    r_arrow -> SetTranslation (2.5 * over_offset * Over ());
    r_arrow -> SetHeightUniformly (0.8 * gallery_height);
    r_arrow -> SetAdjColor (disabled);
    AppendKid (r_arrow);

    scroll_region = new Thing ();
    scroll_region -> TranslationAnimateQuadratic (0.2);
    AppendKid (scroll_region);
  }

  void Blurt (BlurtEvent *e)
  { if (Utters (e, "snapshot"))
      TakeSnapshot ();
  }

  void PointingHarden (PointingEvent *e)
  { if (l_arrow -> IsHeeding (e))
      ScrollRight ();
    else if (r_arrow -> IsHeeding (e))
      ScrollLeft ();
  }

  void PointingMove (PointingEvent *e)
  { if (l_arrow -> HitCheck (e))
      { if (CanScrollRight ())
        { l_arrow -> Heed (e);
          l_arrow -> SetAdjColor (hovered);
        }
      }
    else if (l_arrow -> IsHeeding (e))
      { l_arrow -> StopHeeding (e);
        l_arrow -> SetAdjColor (enabled);
      }

    if (r_arrow -> HitCheck (e))
        { if (CanScrollLeft ())
          { r_arrow -> Heed (e);
            r_arrow -> SetAdjColor (hovered);
          }
        }
    else if (r_arrow -> IsHeeding (e))
      { r_arrow -> StopHeeding (e);
        r_arrow -> SetAdjColor (enabled);
      }
  }

  void SwipeLeft (BlurtEvent *e)
  { if (CanScrollLeft ())
      ScrollLeft ();
  }

  void SwipeRight (BlurtEvent *e)
  { if (CanScrollRight ())
      ScrollRight ();
  }

  void SwipeUp (BlurtEvent *e)
  { TakeSnapshot (); }

  void TakeSnapshot ()
  { INFORM ("Snapshotting!");

    if (CurTime () < 1.0)
      return;

    std::list <Magick::Image> images;
    Str src = skelly1 -> current_layer -> Data () -> SourceName ();
    Magick::Image left (ToChar (src));
    images . push_back (left);
    src = skelly2 -> current_layer -> Data () -> SourceName ();
    Magick::Image right (ToChar (src));
    images . push_back (right);

    std::list <Magick::Image> montage;
    Magick::MontageFramed montageOpts;
    montageOpts . borderWidth (0);
    montageOpts . geometry ("512x512+0+0>");
    montageOpts . tile ("2x1");
    Magick::montageImages (&montage, images . begin (),
                           images . end (), montageOpts);

    Magick::Image& final = montage . front ();
    final . resize ("33%");
    final . write ("tmp_montage.png");

    GalleryImg *snap = new GalleryImg (skelly1 -> top_index,
                                       skelly2 -> top_index,
                                       legend -> cur_cent);
    snap -> SetImage ("tmp_montage.png", true);
    AppendToGallery (snap);
    ZeroTime ();
  }

  void AppendToGallery (GalleryImg *img)
  { rightmost_trans += over_offset * Over ();
    img -> SetTranslation (rightmost_trans);
    img -> SetHeightUniformly (0.8 * gallery_height);
    img -> SetBorderColor (Color (0.25, 1.0));
    img -> SetBorderWidth (1.0);
    scroll_region -> AppendKid (img);
    if (scroll_region -> KidCount () < 5)
      rightmost_entry++;
    if (CanScrollLeft ())
      { r_arrow -> SetAdjColor (enabled);
        img -> SetAdjColorHard (Color (1.0, 0.0));
      }
    if (CanScrollRight ())
      l_arrow -> SetAdjColor (enabled);
  }

  bool CanScrollLeft ()
  { if (rightmost_entry < scroll_region -> KidCount () &&
        scroll_region -> KidCount () > 4)
      return true;
    return false;
  }

  bool CanScrollRight ()
  { if (leftmost_entry > 1)
      return true;
    return false;
  }

  void ScrollLeft ()
  { INFORM ("scroll left");
    scroll_region -> IncTranslation (-over_offset * Over ());
    leftmost_entry++;
    rightmost_entry++;
    scroll_region -> NthKid (rightmost_entry - 1) -> AsLocus () -> SetAdjColor (Color (1.0, 1.0));
    scroll_region -> NthKid (leftmost_entry - 2) -> AsLocus () -> SetAdjColor (Color (1.0, 0.0));
    l_arrow -> SetAdjColor (enabled);
    if (! CanScrollLeft ())
      { r_arrow -> SetAdjColor (disabled);
        r_arrow -> StopHeeding ();
      }
  }

  void ScrollRight ()
  { INFORM ("scroll right");
    scroll_region -> IncTranslation (over_offset * Over ());
    leftmost_entry--;
    rightmost_entry--;
    scroll_region -> NthKid (rightmost_entry) -> AsLocus () -> SetAdjColor (Color (1.0, 0.0));
    scroll_region -> NthKid (leftmost_entry - 1) -> AsLocus () -> SetAdjColor (Color (1.0, 1.0));
    r_arrow -> SetAdjColor (enabled);
    if (! CanScrollRight ())
      { l_arrow -> SetAdjColor (disabled);
        l_arrow -> StopHeeding ();
      }
  }
};

void Setup ()
{ Magick::InitializeMagick (NULL);

  SetFeldsColor (Color (0.0, 1.0));
  //  Use logical ordering, where the last-created object
  //  is drawn on top
  SetDepthSorting (false);

  Image *bg = new Image ();
  bg -> SetImage (NULL);
  bg -> SetBackingColor (Color (0.0, 1.0));
  bg -> SetTranslation (Inside (Feld (), Vect (0.0, 0.1, 0.0)));
  bg -> SetHeight (0.8 * Feld () -> Height ());
  bg -> SetWidth (Feld () -> Width ());

  // create a new skelly object and move it left
  skelly1 = new Skelly ("images/radiology/ct");
  skelly1 -> SetTranslation (Inside (Feld (), Vect (-0.248, 0.1, 0.0)));

  // create a new skelly object and move it right
  skelly2 = new Skelly ("images/radiology/pet");
  skelly2 -> SetTranslation (Inside (Feld (), Vect (0.17, 0.1, 0.0)));

  Gallery *g = new Gallery ();
  g -> IncTranslationHard (-0.4 * Feld () -> Height () * Feld () -> Up ());

  floater = new Image ();
  floater -> TranslationAnimateQuadratic (0.6);
  floater -> ScaleAnimateQuadratic (0.6);
  floater -> SetAdjColor (Color (1.0, 0.0));
  floater -> SetOnWindshield (true);

  DoNotRegisterForDefaultEvents ();
  RegisterDisplacement ("pushback", OpenPalmFingersTogetherPose);
  RegisterTwoHandBlurt ("snapshot", LShapePose, LShapePose);

  legend = new InfoGraphic ();
}
