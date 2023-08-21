# vgedit+
[![GPLv3 License](https://img.shields.io/badge/license-GPLv3-blue.svg?style=flat-square)](https://opensource.org/licenses/GPL-3.0)
[![gh actions](https://img.shields.io/github/actions/workflow/status/vgmoose/vgedit/main.yml?style=flat-square)](https://github.com/vgmoose/vgedit/actions/workflows/main.yml)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome!-tomato.svg?style=flat-square)](http://makeapullrequest.com)

A basic SDL2 text editor designed for easy use on a video game console using a controller or touch screen. The primary goal of this project is to allow you to not have to turn to another device to edit text files.

<img src="https://user-images.githubusercontent.com/2467473/83720891-fb573600-a607-11ea-9c18-42e5e75994fe.png" width="400" alt="File browser" /> <img src="https://user-images.githubusercontent.com/2467473/83720896-fdb99000-a607-11ea-9f8c-439da907aa76.png" width="400" alt="Editor View" />

### Passing files to vgedit
vgedit can be passed a file name to open by specifying it as the first argument on the command line. On PC this looks like `vgedit.exe test.txt`, and on Switch/Wii U, if supported, the calling homebrew can pass the argument before launching vgedit.

Another way to pass a target file is to write an `args.json` file in the same directory as vgedit. For example, to open `test.txt`:
```json
{
    "filename": "sd:/path/to/test.txt",
    "callback": "sd:/switch/the_caller.nro"
}
```

An optional callback can be specified, which will be launched when vgedit exits. This is useful for homebrew that want to open a file, edit it, and then return back.

After reading the path info from the `args.json` file, vgedit will delete it, so it can be used as a one-time file opener. In this mode, vgedit will not show the file browser and close after saving or discarding changes.

## Compilation instructions
This program is written using [Chesto](https://github.com/fortheusers/chesto) and has a dependency on SDL2. It targets Switch, Wii U, and PC.

You can get pre-compiled binaries for each console platform under [GH Actions](https://github.com/vgmoose/vgedit/actions/workflows/main.yml) for a given commit. [Click here](https://nightly.link/vgmoose/vgedit/workflows/main/main) to download the latest nightly builds without signing in.

The latest PC binaries are also available to download under these [GH Actions](https://github.com/vgmoose/vgedit/actions/workflows/pc-builds.yml), or [here](https://nightly.link/vgmoose/vgedit/workflows/pc-builds/main) without signing in.

### Building with Docker
The [fortheusers/sealeo image](https://github.com/4TU/sealeo) has all required dependencies to build on any of the target platforms:
```
git clone --recursive https://github.com/vgmoose/vgedit.git
cd vgedit
export PLATFORM=wiiu    # or switch, pc
docker run -v $(pwd):/code -it ghcr.io/fortheusers/sealeo /bin/bash -c "cd /code && make $PLATFORM"
```

The result should be a `vgedit.rpx` (or `.wuhb`, `.nro`, `.bin`) file in the current directory.

### Building Manually
Depending on your target platform and the current state of its toolchain, you should be able to build it by installing the deps and running: `make <platform>`

For a list of platforms and their dependencies as of this time of writing, see [Sealeo's configuration script](https://github.com/fortheusers/sealeo/blob/main/dependency_helper.sh).

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
