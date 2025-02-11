# HSRT

I. If you use Linux, install SFML's dependencies using your system package manager. On Ubuntu and other Debian-based distributions you can use the following commands:
   ```
   sudo apt update
   sudo apt install \
       libxrandr-dev \
       libxcursor-dev \
       libxi-dev \
       libudev-dev \
       libfreetype-dev \
       libflac-dev \
       libvorbis-dev \
       libgl1-mesa-dev \
       libegl1-mesa-dev \
       libfreetype-dev
   ```
II.Using CMake from the command line is straightforward as well.
   Be sure to run these commands in the root directory of the project you just created.

   ```
   cmake -B build
   cmake --build build
   ```

III. Enjoy!

The source code is dual licensed under Public Domain and MIT -- choose whichever you prefer.

<sub> Used sfml-template </sub>

# Projekt

W planach jest raytracing w hiperbolistycznej przestrzeni Poincarégo ale nie wiem jeszcze nawet czy jest to możliwe i jakie da efekty, możliwe że to plan który nie działą od samego początku
