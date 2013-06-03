
#include <Greenhouse.h>

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
 
 $ export OB_SHARE_PATH=/path/to/point-cloud/:$OB_SHARE_PATH
 */


class Brain  :  public DataTable,
                public PointCloud
{
public:
  DataColumn<Vect> verts, norms;

  Brain (Str const& filename)
    : verts (*this, 0),
      norms (*this, 3)
  { Load (filename);
    SetLocations (verts);
    SetNorms (norms);
    SetUseNorms (true);
    SetReady (true);
  }

  Vect Center () const
  { Vect tots = Vect (0, 0, 0);
    if (0 == verts . size ()) return tots;
    BOOST_FOREACH (Vect const& v, verts)
      tots += v;
    return tots / float64 (verts . size ());
  }
};

void Setup ()
{ Brain *brain = new Brain ("brain.txt");
  brain -> LoadShaders ("brain.vert", "brain.frag");
  Vect center = brain -> Center ();
  brain -> SetTranslation (Feld () -> Loc () - Vect(center.x, center.y, 1000.0));
  brain -> RotationAnimateSine (center + Vect (0, 1, 0), 180.0, 10.0);
}
