
/* (c)  oblong industries */

/*
  imago-worker

  This is part of the 'hydra' example. Hydra is a graphical
  front-end for two long-running 'worker' programs: imago-worker
  and wit-worker.

  imago-worker listens to the 'imago' pool for any proteins
  having a "image-request" descrip. It packs an image into a
  protein as one of the ingests, and deposits the protein
  back into the 'imago' pool.

  Instructions:

  1. run imago-worker and leave it running.

  2. Optionally, open a terminal and 'peek imago' to
     observe the pool traffic on the 'imago' pool.
     This program will create the pool if it doesn't exist.

  2. Run hydra (see examples/hydra/)

  3. When you click the image object in hydra, it puts a
     request protein in the 'imago' pool.  imago-worker
     watches for these proteins and reponds with result
     proteins.


  Notes:

  - imago-worker accepts an optional "-p poolname" argument, naming 
    a pool to use other than 'echo'.  To use a pool named 'foo':

        ./imago-worker foo

  - To listen to a pool on another machine:

        ./imago-worker tcp://hostname/poolname

  - To create a new pool from the command line:

      p-create witter

 */

// Define WORKER at the top of the file, before including
// "Greenhouse.h", to run a Greenhouse program that is 'headless':
// no window, no GUI.  Could be used as a background worker, daemon,
// or script.
#define WORKER

#include "Greenhouse.h"


struct Listener  :  public Thing
{ public:

  int64 page;
  const Str poolname;

  Listener (Str pool)  :  Thing (), poolname (pool)
    { page = 0;

      ParticipateInPool (poolname);
      INFORM ("Participating in pool '" + poolname + "'");

      //  Here we listen only for the exact descrip that we want
      ListenForDescrip ("image-request");
      INFORM ("Listening for any proteins with descrip 'image-request'");
    }


  void Metabolize (const Protein &p)
    { INFORM (p);
      INFORM ("image-request protein received, responding...");
      ImageData *tex = FetchImageData ("images/"
                                       + INT ((page++) % 6 + 1) + ".jpg");
      if (tex)
        { Protein response = ProteinWithDescrip ("image-result");
          AppendIngest  (response, "image-texture", tex);
          Deposit (response, poolname);
        }
      else
        WARN ("Couldn't send response because we failed to load an image.");
    }
};


void Setup ()
{ int64 p_pos = args . Find ("-p");
  if (p_pos > -1 && args . Count () > p_pos + 1)
    new Listener (args[p_pos + 1]);
  else
    new Listener ("imago");
}



