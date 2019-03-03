# CharAnime

A char anime player for cmd/terminal/termux etc.

## CONTENTS

+ [BUILD](#BUILD)
+ [ABOUT](#ABOUT)

## BUILD

### DEVELOPMENT ENVIRONMENT

#### WINDOWS

My compiler: **MinGW-W64 GCC-8.1.0 x86_64-posix-seh**

[Download ffmpeg4.1 builds](https://ffmpeg.zeranoe.com/builds/)

Copy ffmpeg-dev include & lib to project-dir/dev,
Copy ffmpeg-shared dll files to project-dir/bin

#### LINUX

Use apt:

    sudo apt install libavutil libavformat libavcodec libswscale

Or compile source file:

[Download ffmpeg4.1 source](https://www.ffmpeg.org/download.html)

In ffmpeg4.1-source dir:

    ./configure --enable-shared --prefix=../ffmpeg-dev
    make
    make install

The files are in ../ffmpeg-dev,
then copy "lib" files to /lib,
"include" directory to project-dir/dev.

### BUILD THE PROJECT

In the project root directory:

    mkdir build
    cd build
    cmake -G"Unix Makefiles" ../
    make

## ABOUT

### [charanime_config](bin/charanime_config.txt)

Font size and line spacing is hard to unify.

So I use this file to set the screen param.

It's hard to use now :(.

the format is:

    width height
    from to 'char'
    from to 'char'
    ...

The pixel grey value will map to char.
