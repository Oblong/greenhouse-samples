
/* (c) oblong industries */

/*
  echo

  This is a basic plasma echo daemon.

  echo listens on a pool named 'echo' for proteins having the
  descrips "input".  echo then makes a new protein where the
  "input" descrip is replaced with with "output", and deposits
  that protein back onto the 'echo' pool.  Or, if
  the protein had a "pool" ingest, onto the named pool.

  Instructions:

  1. Build and run echo

  2. Open a terminal window where we can observe what's happening
     in the 'echo' pool:

          peek echo

  4. Open another terminal window, where we can put a protein
     into the 'echo' pool, with 'input' as a descrip and hello:world
     as an ingest (ingests are simply key/value pairs):

          poke -d input -i hello:world echo

     (Pool name comes last in the poke command.)

  5.  Echo's output should say something like:

        Participating in pool 'echo'
        Listening for any proteins with descrip 'input'
        {[input], [(hello, world)]}
        Got protein; echo to pool: 'echo'

      Peek's output should show the content of two proteins:
      the one you sent, and then the one that echo deposited.


  Notes

  - echo accepts an optional "-p poolname" argument, naming 
    a pool to use other than 'echo'.  To use a pool named 'foo':

      ./echo -p foo

  - To listen to a pool on another machine:

      ./echo -p tcp://hostname/poolname

  - To create a new pool from the command line:

      p-create echo

 */

// Simply define WORKER at the top of the file, before including
// "Greenhouse.h", to run a Greenhouse program that is 'headless' --
// no window, no GUI.  Could be used as a background worker, daemon,
// or script.
#define WORKER

#include "Greenhouse.h"


struct Echoer  :  public Thing
{
  const Str out_pool;

  Echoer (Str pool)  :
      Thing (),
      out_pool (pool)
    { ParticipateInPool (pool);
      INFORM ("Participating in pool '" + pool + "'");
      ListenForDescrip ("input");
      INFORM ("Listening for any proteins with descrip 'input'");
    }

  const Protein CopyAndTweakProtein (const Protein &p)  const
    { const Slaw &descrips = p . Descrips ();
      Slaw out_descrips = Slaw::List ("output");
      for (int64 i = 1  ;  i < descrips . Count ()  ;  ++i)
        out_descrips = out_descrips . ListAppend (descrips[i]);
      return Protein (out_descrips, p . Ingests ());
    }


  void Metabolize (const Protein &p)
    { INFORM (p);

      Str outbound_pool (out_pool);
      //  If there's no pool ingest key the out_pool,
      //  variable remains unchanged
      p . Ingests () . Find ("pool") . Into (outbound_pool);

      INFORM ("Got protein; echoing to pool '" + outbound_pool + "'");

      Deposit (CopyAndTweakProtein (p), outbound_pool);
    }
};


void Setup ()
{ int64 p_pos = args . Find ("-p");
  if (p_pos > -1 && args . Count () > p_pos + 1)
    new Echoer (args[p_pos + 1]);
  else
    new Echoer ("echo");
}
