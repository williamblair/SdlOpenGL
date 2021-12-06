set INCDIRS=/I D:\vs_libs\include
set LIBDIRS=/link /LIBPATH:D:\vs_libs\lib
set TARGET=/OUT:main.exe
set LIBS=OpenGL32.lib SDL2.lib SDL2main.lib glew32.lib
set FLAGS=/DWIN32=1
set SOURCES=*.cpp


cl %SOURCES% %FLAGS% %INCDIRS% %LIBDIRS% %LIBS% %TARGET%

