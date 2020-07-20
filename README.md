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
To configure the project there are 2 key steps that need to be done:

<details>
  <summary>Configure assimp</summary>
  
  To setup assimp, simply run the [`configure.cmd`](./configure.cmd) script located in the root of the repository.
  With the script run you shold now be able to open the project in Visual Studio fine.

</details>

<details>
  <summary>Project references</summary>
  
  The final step required is to update the project references to use the newly generated assimp project configuration.
  
  Every time the previous "**Configure Assimp**" step is run, Visual Studio will generate a new project ID for `assimp`. To fix this, every time the script is run, open the `vision` project's "References" drop down from the solution explorer. Right-click and remove the current `assimp` reference, followed by right clicking "References" add re-adding the `assimp` project.

  This will add the `assimp` project as a build dependency.

</details>

### 🚑 Support
If you are having any issues with the project, either getting it up and running or using the project itself, feel free to open an [issue](https://github.com/nurodev/vision/issues).

<details>
  <summary>Manually configuring assimp</summary>
  
  A common issue when using vision is setting up `assimp`, a core project dependency. If the script to configure `assimp` fails, I recommend this great
  tutorial by [Tomasz Gałaj](https://github.com/Shot511) on how to setup `assimp` for an OpenGL project: [shot511.github.io/how-to-setup-opengl-project-with-cmake](https://shot511.github.io/2018-05-29-how-to-setup-opengl-project-with-cmake).

</details>

<details>
  <summary>Manually compiling assimp</summary>
  
  Another common issue, especially when first compiling the project, can be that an error will be thrown due to either `assimp.lib` or `assimp.dll` cannot be found.

  This error is most commonly thrown when the `assimp` project has not been built and as such the compiled artifacts could not be copied to the build directory for `vision`.

  To fix this, you can manually build `assimp`. To do this right click the `assimp` project and click "Build".
  
</details>

### ❤️ Credits

All humanoid models (`.fbx`) models were provided by [Mixamo](https://www.mixamo.com).

All other assets (Texture, models, etc) were provided by my academic institution. 

# 📄 License

MIT © [Ben Dixon](https://github.com/nurodev/vision/blob/master/LICENSE)
