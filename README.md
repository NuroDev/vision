<div align='center'>

  <a href='https://github.com/nurodev/vision'>
    <img alt='Canvas' src='./.github/assets/banner.png?raw=true' />
  </a>

  [![License](https://img.shields.io/badge/%20%F0%9F%93%84%20MIT-3287ef.svg?longCache=true&style=for-the-badge)](https://opensource.org/licenses/MIT) 
  [![Gitmoji](https://img.shields.io/badge/-%20%F0%9F%98%9C-FFDD67.svg?longCache=true&style=for-the-badge)](https://gitmoji.carloscuesta.me/) 

  <br />
</div>

## 🔨 Development

The current state of the engine means the only available build platform currently is Window (Win32).

###  📋 Requirements

To setup and use the project you will need to have the following tools installed:
 - [Git](https://git-scm.com)
 - [CMake](https://cmake.org) 
 - [Visual Studio](https://visualstudio.microsoft.com) 

### ⬇️ Installation

Clone the repository:
```shell
git clone --recursive https://github.com/nurodev/vision.git
```

### 🔧 Configuring
To configure the project, there is only one key step that needs to be performed before you can open the project in Visual Studio and get up and running.
This step is to run the [`configure.cmd`](./configure.cmd) script located in the root of the repository.
Once the script has been run then there should be no further work required to get the project running.

### 🚑 Support
If you are having any issues with the project, either getting it up and running or using the project itself, feel free to open an [issue](https://github.com/nurodev/vision/issues).

A common issue when using vision is setting up `assimp`, a core project dependency. If the script to configure `assimp` fails, I recommend this great
tutorial by [Tomasz Gałaj](https://github.com/Shot511) on how to setup `assimp` for an OpenGL project: [shot511.github.io/how-to-setup-opengl-project-with-cmake](https://shot511.github.io/2018-05-29-how-to-setup-opengl-project-with-cmake).

### ❤️ Credits

All humanoid models (`.fbx`) models were provided by [Mixamo](https://www.mixamo.com).

All other assets (Texture, models, etc) were provided by my academic institution. 

# 📄 License

MIT © [Ben Dixon](https://github.com/nurodev/vision/blob/master/LICENSE)
