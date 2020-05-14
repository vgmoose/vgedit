# vgedit
[![GPLv3 License](https://img.shields.io/badge/license-GPLv3-blue.svg?style=flat-square)](https://opensource.org/licenses/GPL-3.0)
[![gitlab ci](https://gitlab.com/vgmoose/vgedit/badges/master/pipeline.svg?style=flat-square)](https://gitlab.com/vgmoose/vgedit/pipelines)
[![travis](https://img.shields.io/travis/vgmoose/vgedit.svg?label=travis&style=flat-square)](https://travis-ci.org/vgmoose/vgedit)
[![twitter](https://img.shields.io/twitter/follow/vgmoose.svg?label=twitter&style=flat-square)](https://twitter.com/vgmoose)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome!-tomato.svg?style=flat-square)](http://makeapullrequest.com)

A basic SDL2 text editor designed for easy use on a video game console using a controller or touch screen. The primary goal of this project is to allow you to not have to turn to another device to edit text files.

## Compilation instructions
This program is written using [Chesto](https://gitlab.com/4TU/chesto) and has a dependency on SDL2. It targets Switch, Wii U, and PC.

You can get pre-compiled binaries for each platform under [Pipelines](https://gitlab.com/vgmoose/vgedit/pipelines) for a given commit. The download artifacts dropdown is to the right of the build passing status.

### Building for Switch (with libnx)
1. Install [dkp-pacman](https://devkitpro.org/viewtopic.php?f=13&t=8702)
2. Install devkitA64 and needed Switch dependencies via dkp-pacman:
```
sudo dkp-pacman -S devkitA64 libnx switch-tools switch-bzip2 switch-freetype switch-libjpeg-turbo switch-sdl2 switch-sdl2_gfx switch-sdl2_image switch-sdl2_ttf switch-libpng switch-mesa
```
3. Once it's all setup, recursively clone the repo and run make:
```
git clone --recursive https://github.com/vgmoose/vgedit.git
cd vgedit
make -f Makefile.switch
```

If all goes well, `vgedit.nro` should be sitting in the current directory.

### Building for Wii U (with WUT)
The below instructions are currently for Linux and macOS
1. Install [dkp-pacman](https://devkitpro.org/viewtopic.php?f=13&t=8702)
2. Setup [wiiu-fling](https://gitlab.com/QuarkTheAwesome/wiiu-fling#wiiu-fling) according to the instructions
3. Install sdl2, wut, devkitPPC and other dependencies (on macOS, use `wut-osx` instead of `wut-linux`)
```
sudo dkp-pacman -S wut-linux wiiu-sdl2 devkitPPC wiiu-libromfs wiiu-sdl2_gfx wiiu-sdl2_image wiiu-sdl2_ttf ppc-bzip2 ppc-freetype ppc-libpng
```
4. Once the environment is setup:
```
git clone --recursive https://github.com/vgmoose/vgedit.git
cd vgedit
make -f Makefile.wiiu
```

If all goes well, `vgedit.rpx` should be sitting in the current directory.

### Building for PC
There's a separate makefile for building the SDL2 app for PC. Below instructions are for Ubuntu, but should be similar on other platforms:
```
sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-gfx-dev gcc g++ git
git clone --recursive https://github.com/vgmoose/vgedit.git
cd vgedit
make -f Makefile.pc
```

You can also build for PC using the [bake](https://github.com/SanderMertens/bake) build system.

## License
This software is licensed under the GPLv3.

> Free software is software that gives you the user the freedom to share, study and modify it. We call this free software because the user is free. - [Free Software Foundation](https://www.fsf.org/about/what-is-free-software)

Folder and file icons by [Alfredo Hernandez](https://www.flaticon.com/authors/alfredo-hernandez) under the Flaticon Basic License

#### Contributors
- [vgmoose](https://github.com/vgmoose)
- [satelliteseeker](https://github.com/satelliteseeker)

### Contributing
It's not required, but running a clang-format before making a PR helps to clean up styling issues:
```
find . \( -name "*.cpp" -or -name "*.hpp" \) -not -path "./libs/*" -exec clang-format -i {} \;
```

Any and all PRs are welcome and appreciated! In particular, there is still some work to be done to properly align the cursor on text files with long lines, or when switching between touch screen and controller. You can also find me to talk directly about these issues on discord as vgmoose#3545 .
