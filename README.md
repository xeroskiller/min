# minGE
Repository for the Min() Game Engine

Welcome to the Min() Game Engine! This is the Alpha Build Version 0.1.

This package is the Windows source code distribution of the Min() Game Engine. The examples are linked to a master Visual Studio 2013 Solution file in Min()\minGL\min.sln.

You can build all needed projects from this solution file. There are four projects, Datafile, roadgame, anim_md5, and minGL. The datafile project is for saving models in the Min() file format. MinGL is the game engine library. Roadgame and anim_md5 are example projects using the engine.

These packages have been tested for compilation under Visual Studio 2013 - XP platform toolset. You can turn off the XP build sequence by commenting out __BUILD__XP in the targetver.h and stdafx.h header files and switching the Visual Studio platform toolset to the non XP version.

The package also comes with the GLEW and Newton Game Dynamics library and header files and should work seamlessly with the engine. You must make sure to add glew32.lib and newton.lib to the .exe directory of any new projects.

You can download GLEW at http://glew.sourceforge.net/

You can download Newton Game Dynamics at http://www.newtondynamics.com/

MD5 Model and MD5 Animation Support was pulled and modified from http://3dgep.com/loading-and-animating-md5-models-with-opengl/ by Jeremiah van Oosten which was derived from http://tfc.duke.free.fr/coding/md5-specs-en.html by David Henry. 