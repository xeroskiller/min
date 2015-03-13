Welcome to the Min() Game Engine! This is the Alpha Build Version 0.1.

This package is the Windows source code distribution of the Min() Game Engine. The examples are linked to a master Visual Studio 2013 Solution file in Min()\minGL\min.sln.

You can build all needed projects from this solution file. There are four projects, Datafile, roadgame, anim_md5, and minGL. The datafile project is for saving models in the Min() file format. MinGL is the game engine library. Roadgame and anim_md5 are example projects using the engine.

These packages have been tested for compilation under Visual Studio 2013 - XP platform toolset. You can turn off the XP build sequence by commenting out __BUILD__XP in the targetver.h and stdafx.h header files and switching the Visual Studio platform toolset to the non XP version.

The package also comes with the GLEW and Newton Game Dynamics library and header files and should work seamlessly with the engine. You must make sure to add glew32.lib and newton.lib to the exe directory of any new projects.