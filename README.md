# DubiousEngine
Welcome to the Dubious Engine. This is my attempt to create an *understandable* Physics Engine. 
It's a Discrete Engine that uses GJK and EPA to perform collision detection and response.

My initial goal was to bring this Engine to the point where it could stably stack a large number
of cubes as fast as possible. I think I have acheived this as it will currently stack 120 cubes
at 40 fps on my mid level desktop PC.

In an attempt to provide meaningful documentation about it, I am creating posts here:
http://dubiousoft.com/2017/01/23/dubious-engine-table-of-contents/

Questions, comments, suggestions, etc are always welcome. The blog listed above would be the
best place to start the discussion.

Doxygen docs available at http://saintdubious.github.io/DubiousEngine/

# Build Instructions
There are two external libraries that you'll need to have configured before you can build this 
project. One of them is easy:

## SDL
SDL comes in a nice, pre-packaged download that should just work as is. You can download it
from http://libsdl.org/, specifically check out http://libsdl.org/download-2.0.php and grab the
"Windows Development Libraries" zip file. Unzip it wherever you want and then create a new
environment variable called SDL_ROOT. This solution will expect to have an include path
like $(SDL_ROOT)\include and library path like $(SDL_ROOT)\lib\x86

## OpenCL
Unfortunately the OpenCL story is not as clean. I had to grab my copy from NVidia somewhere.
I don't recall where I got it, but it came buried inside something else that may have been
called "NVIDIA GPU Programming SDK" (at least that's the name of the folder on my drive). You'll
need to create another environment variable called OPENCL_ROOT. This solution will expect to 
have an include path like $(OPENCL_ROOT)\inc and library path like $(OPENCL_ROOT)\lib\Win32

Sorry, I have no idea where to get OpenCL for other graphics cards. The good news is that I don't
actually use that much OpenCL, so you could always just delete it.