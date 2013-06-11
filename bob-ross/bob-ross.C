
#include "Greenhouse.h"

class DotsSquare : public Sketch
{
public:
  DotsSquare()
  { SetStroked(false);
    SetStrokeColor(Color(0.12,0.12,0.91));
    SetFillColor(Color(0.8,0.4,0,.7));
    for (int i = 0; i <= 10; ++i)
      { for (int j = 0; j <= 10; ++j)
          { SetFilled(true);
            SetStroked(false);
            if (Random(0,5) < 1)
              SetFillColor(Color(0,0,0.91,0.5));
            else if (FillColor() != Color(0.8,0.4,0,.6))
              SetFillColor(Color(0.8,0.4,0,.6));
            DrawEllipse(Vect(i*50,j*-50,1),(i+j+2), (i+j+2));
            SetStroked(true);
            DrawPoint(Vect(i*50,j*-50,2));
          }
      }
  }
};

class LinesSquare : public Sketch
{ Trove <float64> widths;
  Trove <float64> positions;
  Trove <float64> speeds;
  int64 num;

public:

  LinesSquare()
  { num = 40;
    for (int i = 0; i < num; ++i)
      { widths . Append (Random(5,15));
        positions . Append (Random(0,500));
        speeds . Append (Random(1,2));
      }
  }

  void Travail()
  { Clear();
    SetStrokeColor(Color(0.91,0.91,0.91,.75));
    for(int i = 0; i < num; i++)
      { positions . Nth(i) += speeds . Nth(i);
        if(positions . Nth(i) > 500)
          positions . Nth(i) = 0;
        SetStrokeWidth(widths.Nth(i));
        DrawLine(Vect(positions.Nth(i),0,0),Vect(positions.Nth(i),500,0));
      }
  }
};

class CurveSquare : public Sketch
{
public:
  CurveSquare()
  { SetFillColor (Color(0,0,1,.5));
    DrawRect(Vect(0,0,0), 500, 500);
    SetStrokeColor(Color(0.8,0.4,0,.7));
    DrawBezier(Vect(250,250,0),Vect(-250,-250,0),Vect(-250,250,0),Vect(250,-250,0));
  }
};

class ArcSquare : public Sketch
{
public:
  ArcSquare ()
  { INFORM("Default drawing settings are:\nStroked: " + ToStr(QueryStroked()) +
           "\nStrokeColor: " + ToStr(StrokeColor()) +
           "\nStrokeWidth: " + ToStr(StrokeWidth()) +
           "\nFilled: " + ToStr(QueryFilled()) +
           "\nFillColor: " + ToStr(FillColor()));
    for (int i = 0; i <= 20; i+=2)
      { DrawArc(Vect(0,0,1),i*25,i*25,PI,PI*3/2);
      }
  }
};

void Setup() {
  LinesSquare *ls = new LinesSquare();
  ls -> SlapOnFeld();
  ArcSquare *ts = new ArcSquare();
  ts -> SlapOnFeld();
  CurveSquare *cs = new CurveSquare();
  cs -> SlapOnFeld();
  cs -> IncTranslation(Vect(-250,250,0));

  Sketch *t = new Sketch();
  t -> SlapOnFeld();
  t -> IncTranslation(Vect(0,0,-1));
  t -> SetStroked(false);
  t -> SetFillColor(Color(0.9,0.3,0,0.25));
  t -> DrawShape(Vect(-500,500,0), Vect(-500,-500,0), Vect(500,-500,0));

  DotsSquare *ds = new DotsSquare();
  ds -> SlapOnFeld();
}