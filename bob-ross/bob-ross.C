
#include "Greenhouse.h"

class DotsSquare  :  public Sketch
{ float64 width;
public:
  DotsSquare (float64 w)
  { width = w;
    SetStroked (false);
    SetStrokeColor (Color (0.12, 0.12, 0.91));
    SetFillColor (Color (0.8, 0.4, 0, 0.7));
    for (int i = 0  ;  i <= 10  ;  ++i)
      { for (int j = 0; j <= 10; ++j)
          { SetFilled (true);
            SetStroked (false);
            if (Random (0, 5) < 1)
              SetFillColor (Color (0, 0, 0.91, 0.5));
            else if (FillColor() != Color (0.8, 0.4, 0, 0.6))
              SetFillColor (Color (0.8, 0.4, 0, 0.6));
            DrawEllipse (Vect (i*width/10, j*-width/10, 1), ((i+j+2)*width/500), ((i+j+2)*width/500));
            SetStroked (true);
            DrawPoint (Vect (i*width/10, j*-width/10, 2));
          }
      }
  }
};

class LinesSquare : public Sketch
{ Trove <float64> widths;
  Trove <float64> positions;
  Trove <float64> speeds;
  int64 num;
  float64 width;

public:

  LinesSquare (float64 w)
  { width = w;
    num = 40;
    for (int i = 0  ;  i < num  ;  ++i)
      { widths . Append (Random(5, 15));
        positions . Append (Random(0, width));
        speeds . Append (Random(1, 2));
      }
  }

  void Travail()
  { Clear();
    SetStrokeColor (Color (0.91, 0.91, 0.91, 0.75));
    for(int i = 0  ;  i < num  ;  i++)
      { positions . Nth(i) += speeds . Nth(i);
        if(positions . Nth(i) > width)
          positions . Nth(i) = 0;
        SetStrokeWidth (widths . Nth (i));
        DrawLine (Vect (positions . Nth(i), 0, 0), Vect (positions . Nth (i), width, 0));
      }
  }
};

class CurveSquare  :  public Sketch
{ float64 width;
public:
  CurveSquare (float64 w)
  { width = w;
    SetFillColor (Color (0, 0, 1, 0.5));
    DrawRect (Vect (0, 0, 0), width, width);
    SetStrokeColor (Color (0.8, 0.4, 0,0.7));
    DrawBezier (Vect (width/2, width/2, 0), Vect (-width/2, -width/2, 0), Vect (-width/2, width/2, 0), Vect (width/2, -width/2, 0));
  }
};

class ArcSquare  :  public Sketch
{ float64 width;
public:
  ArcSquare (float64 w)
  { width = w;
    INFORM ("Default drawing settings are:\nStroked: " + ToStr (QueryStroked ()) +
           "\nStrokeColor: " + ToStr (StrokeColor ()) +
           "\nStrokeWidth: " + ToStr (StrokeWidth ()) +
           "\nFilled: " + ToStr (QueryFilled ()) +
           "\nFillColor: " + ToStr (FillColor ()));
    for (int i = 0  ;  i <= 20  ;  i+=2)
      { DrawArc (Vect (0, 0, 1), i*width/20, i*width/20, PI, PI*3/2);
      }
  }
};

void Setup() {
  float64 square_size;
  if (Feld() -> Width() < Feld() -> Height())
    square_size = Feld() -> Width() * 0.3;
  else
    square_size = Feld() -> Height() * 0.3;

  LinesSquare *ls = new LinesSquare(square_size);
  ls -> SlapOnFeld ();
  ArcSquare *ts = new ArcSquare(square_size);
  ts -> SlapOnFeld ();
  CurveSquare *cs = new CurveSquare(square_size);
  cs -> SlapOnFeld ();
  cs -> IncTranslation (Vect (-square_size/2, square_size/2,0));

  Sketch *t = new Sketch ();
  t -> SlapOnFeld ();
  t -> IncTranslation (Vect (0, 0, -1));
  t -> SetStroked (false);
  t -> SetFillColor (Color (0.9, 0.3, 0, 0.25));
  t -> DrawShape( Vect (-square_size, square_size, 0), Vect (-square_size, -square_size, 0), Vect (square_size, -square_size, 0));

  DotsSquare *ds = new DotsSquare(square_size);
  ds -> SlapOnFeld ();
}