# vgedit+
[![GPLv3 License](https://img.shields.io/badge/license-GPLv3-blue.svg?style=flat-square)](https://opensource.org/licenses/GPL-3.0)
[![gitlab ci](https://gitlab.com/vgmoose/vgedit/badges/master/pipeline.svg?style=flat-square)](https://gitlab.com/vgmoose/vgedit/pipelines)
[![travis](https://img.shields.io/travis/vgmoose/vgedit.svg?label=travis&style=flat-square)](https://travis-ci.org/vgmoose/vgedit)
[![twitter](https://img.shields.io/twitter/follow/vgmoose.svg?label=twitter&style=flat-square)](https://twitter.com/vgmoose)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome!-tomato.svg?style=flat-square)](http://makeapullrequest.com)

A basic SDL2 text editor designed for easy use on a video game console using a controller or touch screen. The primary goal of this project is to allow you to not have to turn to another device to edit text files.

## Compilation instructions
This program is written using [Chesto](https://gitlab.com/4TU/chesto) and has a dependency on SDL2. It targets Switch, Wii U, and PC.

You can get pre-compiled binaries for each platform under [Pipelines](https://gitlab.com/vgmoose/vgedit/pipelines) for a given commit. The download artifacts dropdown is to the right of the build passing status.

### Building with Docker
The [4tu/spheal image](https://gitlab.com/4TU/spheal) has all required dependencies to build on any of the target platforms:
```
git clone --recursive https://github.com/vgmoose/vgedit.git
export PLATFORM=wiiu    # or switch, 3ds, wii, pc, pc-sdl1
docker run -v $(pwd):/code -it registry.gitlab.com/4tu/spheal /bin/bash -c "cd /code && make $PLATFORM"
```

The result should be a `vgedit.rpx` (or `.nro`, `.3dsx`, `.dol`, `.bin`, `.bin-sdl1`) file in the current directory.

### Building Manually
Depending on your target platform and the current state of its toolchain, you should be able to build it by installing the deps and running: `make <platform>`

For a list of platforms and their dependencies as of this time of writing, see [Spheal's configuration script](https://gitlab.com/4TU/spheal/-/blob/master/dependency_helper.sh#L25-45).

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

Any and all PRs are welcome and appreciated!
