
#include <Greenhouse.h>
#include "Table.h"

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

 $ export OB_SHARE_PATH=/path/to/point-cloud/share:$OB_SHARE_PATH
 */


class Brain  :  public Points
{
public:
  Table table;
  Trove <Vect> vertices;
  Trove <Vect> normals;

  Brain (Str const& filename)
  { table . Load (filename);
    vertices = table . VectColumn (0, 1, 2);
    normals  = table . VectColumn (3, 4, 5);
    SetVertexCount (table . RowCount ());
    SetVertexLocs (vertices);
    SetNorms (normals);
    SetVerticesReady ();
  }

  Vect Center () const
  { if (0 == vertices . Count ()) return ZeroVect;
    
    Vect total = ZeroVect;    
    for (int64 i = 0 ; i < vertices . Count () ; i++)
      total += vertices[i];

    return total / float64 (vertices . Count ());
  }
};

void Setup ()
{ Brain *brain = new Brain ("data/brain.txt");
  brain -> LoadShaders ("shaders/brain.vert", "shaders/brain.frag");
  Vect center = brain -> Center ();
  brain -> SetTranslation (Feld () -> Loc () - 
                           Vect(center.x, center.y, 1000.0));
  brain -> RotationAnimateSine (center + Vect (0, 1, 0), 180.0, 10.0);
}
