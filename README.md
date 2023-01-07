# A Simple library for generating images.


## Features

Pograph is a c++ library for generating .ppm files.
Generating and reading both plain and raw files is supported.
Currently there is support for creating rectangles, circles and custom sprites.
##### Important
Files with comments inside cannot be used to create a sprite.

## Build

### Linux

```
$ git clone github.com/KxUgh/pograph.git
$ cd pograph
$ g++ -fPIC -shared -o libpograph.so pograph.cpp
$ sudo cp pograph.h /usr/local/include/
$ sudo cp pograph.cpp /usr/local/lib/
```
To use the library include `pograph.h` in your application and compile it with `-lpograph` flag.

## Galery

![img](gallery/square_showcase.png)

![img](gallery/circle_showcase.png)

![img](gallery/sprite_showcase.png)