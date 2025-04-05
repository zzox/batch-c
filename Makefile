# not working, use this to compile on mac
clang -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL include/libraylib.a main.c -o build/my_app
open build/my_app