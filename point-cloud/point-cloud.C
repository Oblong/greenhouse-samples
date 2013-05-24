
#include <Greenhouse.h>

class Brain  :  public DataTable,
                public PointCloud
{
public:
  DataColumn<Vect> verts, norms;

  Brain (Str const& filename)
    : verts (*this, 0),
      norms (*this, 3)
  { Load (filename);
    SetPointLocations (verts);
    SetPointNorms (norms);
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
