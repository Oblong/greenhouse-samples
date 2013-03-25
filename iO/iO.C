
/* (c)  oblong industries */

#include "Greenhouse.h"

/*
  iO

  To interact with this application, first download the 'g-speak mobile' app
  from the iTunes App Store.  (It's free.)
 
  https://itunes.apple.com/us/app/g-speak-mobile/id460504931?mt=8

  1.  Build and run the iO application

  2.  Run the g-speak mobile app on your iOS device

  3.  The Greenhouse package-installer has already installed and started 
      two background services, pool_tcp_server & pool-server-zeroconf-adapter, 
      which allow your application to be discoverable.  So hopefully your
      machine name will automagically appear in the list of available systems.

      If it doesn't, try the "Connect manually..." option.  For a machine 
      named foo, try entering foo or foo.local

  4.  Once the app is connected:

        - touch, hold, and wave the device around to move a pointer
        - swipe up, down, left, and right
        - double-tap to 'harden' (click)

      Some lag over Wifi is to be expected.  We're working on it.

  5.  In the iOS Settings app, you can change the g-speak mobile app to
      "touchpad" mode, where it will act more like a laptop touchpad.

        - double-tap to 'harden' (click)
        - swipe requires two fingers

*/
class PointerDetector : public Thing
{ public:

  // Maps pointer-event sources to text labels that belong to them
  Dictionary <Str, Text*> labels;
  
  // Text at the center of the feld
  Text *center_text;

  // Background image (if one is uploaded by user)
  Image *bg_img;

  PointerDetector () : Thing ()
    { // reset the pointer text to the pointer name
      // every 2 seconds (after an iOS swipe)
      SetFireTimer (2.0);
      
      // set up the background image
      bg_img = new Image ();
      bg_img -> SlapOnFeld ();
      bg_img -> SetAdjColor (Color (1.0, 0.0));   //  transparent

      // set up the text field
      center_text = new Text ("Connect using the g-speak mobile app\n"
                            "from the iTunes store.\n"
                            "Then try point, tap, double-tap, & swipe.");
      center_text -> SetFontSize (Feld () -> Height () / 25.0);
      center_text -> SlapOnFeld ();

      RegisterForMobileNotifications ();
    }

  void PointingMove (PointingEvent *e)
    { // the Intersection of a ray from the pointer to the Feld
      Vect intersect = Intersection (e, Feld () -> Loc ());

      Text *t = labels . Get (e -> Provenance ());
      
      // If we detected a new pointer source,
      // create a new Text instance and put it in the list
      if (! t)
        { t = new Text ("pointer " + INT (labels . Count () + 1)
                              + " = " + e -> Provenance ());
          t -> SetObliviousness (true);
          t -> Heed (e);
          t -> SetAlignmentLeft ();
          t -> SetFontSize (Feld () -> Width () / 20.0);
          labels . Put (e -> Provenance (), t);
        }

      t -> SetTranslation (intersect + Vect (10, 10, 0));
    }

  void Fired () 
    { for (int64 i = 0  ;  i < labels . Count ()  ;  i++)
        { Text *t = labels . NthVal (i);
          t -> SetString ("pointer " + INT (i + 1) + " = "
                          + labels . NthKey (i));
        }
    }

  void PointingHarden (PointingEvent *e)
    { if (Text *t = labels . Get (e -> Provenance ()))
        t -> SetString ("Pointing Harden");
    }

  void SwipeUp (BlurtEvent *e)
    { if (Text *t = labels . Get (e -> Provenance ()))
        t -> SetString ("Swipe Up");

      // Request an image upload
      Protein p = Remote::ImageRequestProtein (e -> Provenance (), 
                                    "Upload an image to set the background");
      Remote::DepositImageRequest (p);
    }

  void SwipeDown (BlurtEvent *e)
    { if (Text *t = labels . Get (e -> Provenance ()))
        t -> SetString ("Swipe Down");

      //  Request a text entry
      Protein p = Remote::TextfieldRequestProtein (e -> Provenance (),
                                         "Enter some text to display");
      Remote::DepositTextfieldRequest (p);
    }

  void SwipeRight (BlurtEvent *e)
    { if (Text *t = labels . Get (e -> Provenance ()))
        t -> SetString ("Swipe Right");
    }

  void SwipeLeft (BlurtEvent *e)
    { if (Text *t = labels . Get (e -> Provenance ()))
        t -> SetString ("Swipe Left");
    }

  void ImageUploadComplete (const Str &provenance, ImageData *img_data)
    { if (Text *t = labels . Get (provenance))
        t -> SetString ("Image Upload");

      // Set the image and fit it to the feld
      bg_img -> SetImage (img_data);
      if (ImageDataAspect (img_data)  >  Aspect (Feld ()))
        { bg_img -> SetWidth  (Feld () -> Width ());
          bg_img -> SetHeight (Feld () -> Width () * ImageDataAspect (img_data));
        }
      else
        { bg_img -> SetHeight (Feld () -> Height ());
          bg_img -> SetWidth  (Feld () -> Height () *  ImageDataAspect (img_data));
        }

      //  Now that we have an actual image, go opaque
      bg_img -> SetAdjColor (Color (1.0, 1.0));
    }

  void TextUploadUpdate (const Str &provenance, const Str &text)
    { if (Text *t = labels . Get (provenance))
        t -> SetString ("TextUpload");

      center_text -> SetString (text);
    }

};

void Setup ()
{ new PointerDetector (); }
