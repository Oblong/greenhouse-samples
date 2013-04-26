Greenhouse Samples
==================
This repo contains sample applications written using Oblong's Greenhouse SDK. 

Greenhouse is a toolkit for creative coders; it is free for non-commercial and limited-scale use.

Greenhouse is a minimalist version of Oblong's g-speak SDK. (g-speak is what we at Oblong use to build all our spatially-aware, multi-user, multi-platform, multi-device software.) We've taken the core parts of g-speak and distilled them into a C++ SDK whose goal is to be approachable, powerful, and concise.

Greenhouse includes our own custom Kinect-processing pipeline, Leap Motion support, and hooks for using mobile devices as input.

Download the Greenhouse SDK here:  [greenhouse.oblong.com](http://greenhouse.oblong.com/)

Pre-requisites
==============
- These samples assume the Greenhouse SDK is already installed.
- The Greenhouse SDK for Mac requires
    - Mac OS X 10.6 or greater
    - **Xcode**  
      Available from: the Mac App Store
    - **Xcode Command Line Tools**  
      Available from: within Xcode.  
      Select Preferences -> Downloads -> Command Line Tools


About the samples
=================
Each sample includes an Xcode project file, which Mac users with Xcode installed can just double-click. Separately, each sample has a vanilla Makefile as well, for command line usage.

For runnable samples that focus on using specific features of the Greenhouse API, see the [Greenhouse API reference](http://greenhouse.oblong.com/reference.html).

**helloworld** 
  
  - Just about the simplest thing that can possibly work.
  - To build at the command line:
   
        cd helloworld
        make
        ./helloworld

  - To build with Xcode:
       - open helloworld.xcodeproj
       - press Run button


**empty-proj**

  - The empty sample provides a handy place to paste in code from samples on the [Greenhouse API reference](http://greenhouse.oblong.com/reference.html)

**hantenna**

  - handling Kinect input
  - learning the set of gestures that the hantenna pipeline supports

**leap**

  - handling Leap Motion input

**heartbeats3D**
  
  - using a Thing as a container for other objects
  - working with timers
  - working with protein messages
  - handling user pointing events
  - creating simple text labels
  - doing work every frame with the Travail() method
  - tagging/untagging objects
  - animating color
  - parent/kid relationships
  
**flocking-hearts**
  
  - using a Thing as a container for other objects
  - handling user pointing events
  - doing work every frame with the Travail() method
  - parent/kid relationships and using Parent()
  - arranging objects on a flat plane using vector math
  - behavioral simulation

**radiology**
  
  - sorting a Trove
  - working directly with the file system
  - handling user pointing events and Kinect events
  - using logical ordering instead of depth-sorting the scene
  
**box-nav**
  
  - using a Thing as a container for other objects
  - handling user pointing events and Kinect events
  - doing work every frame with the Travail() method
  - arranging objects in true 3D space
  - working with timers
  - working with protein messages
  - using a custom class inherited from Thing (Box.h)
  - tagging/untagging objects
  - animating color, rotation, translation
  - tickle/thrill/chill system
  - OpenGL drawing (immediate mode)
  
**echo**
  
  - making a Greenhouse 'worker' application
  - listening for incoming protein messages
  - sending out protein messages
  - using command-line protein utilities (peek, poke, etc)
  
**hydra** / **imago-worker**
  
  - making a Greenhouse 'worker' application
  - listening for incoming protein messages
  - sending out protein messages
  - using command-line protein utilities (peek, poke, etc)
  - packing and unpacking data from protein messages
  - coordinating messages among multiple Greenhouse programs
  - coordinating with a Ruby program
  - using Plasma in Ruby
  - connecting to a web service from Ruby
  
**iO**
  
  - taking input from the g-speak mobile app ([for iOS](https://itunes.apple.com/us/app/g-speak-mobile/id460504931?mt=8) or [Android](https://play.google.com/store/apps/details?id=com.oblong.android.gspeakpointer))
  - using a Thing as a container for other objects
  - working with raw image data
  - working with protein messages
  - handling user pointing events
  - creating simple text labels
  - setting timers
  
**cities**
  
  - working with latitude/longitude data sets
  - setting up VBOs (vertex buffers) in the DataSet class
  - using OpenGL shaders
  - handling user pointing events, Kinect events, and keyboard input

**seismo**

  - similar to cities but a richer application
  - working with latitude/longitude data sets
  - setting up VBOs (vertex buffers) in the DataSet class
  - using OpenGL shaders
  - handling user pointing events, Kinect events, and keyboard input

Contributing
============
Found a bug in a sample?  
Built something cool?  
Pull requests are welcome!

Issues?
=======
If you have issues with the samples, or with the SDK itself, use the Issue Tracker for this repo, or join the [Greenhouse SDK Google Group](https://groups.google.com/forum/#!forum/greenhouse-sdk) -- or both. 

Or send email to [greenhouse@oblong.com](mailto:greenhouse@oblong.com).
