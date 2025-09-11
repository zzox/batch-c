# use this to compile and run on mac
`gcc -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL include/libraylib.a main.c -o build/my_app && build/my_app`

# after downloading project
`mkdir results`
`mkdir build`
`mkdir include`

move all resources into `build/resources`

optional: set working directory in `main.c` -> `ChangeDirectory`

## use this to make libraylib.a
[https://github.com/raysan5/raylib/wiki/Working-on-macOS](https://github.com/raysan5/raylib/wiki/Working-on-macOS#without-xcode---building-statically)
