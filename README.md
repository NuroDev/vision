<div align='center'>

  <a href='https://github.com/nurodev/vision'>
    <img alt='Canvas' src='./.github/assets/banner.png?raw=true' />
  </a>

  [![License](https://img.shields.io/badge/%20%F0%9F%93%84%20MIT-3287ef.svg?longCache=true&style=for-the-badge)](https://opensource.org/licenses/MIT) 
  [![Gitmoji](https://img.shields.io/badge/-%20%F0%9F%98%9C-FFDD67.svg?longCache=true&style=for-the-badge)](https://gitmoji.carloscuesta.me/) 

  <br />
</div>

## ✨ Features

<details>
  <summary>Normal Mapping</summary>
  
  #### Screenshot
  <img alt="Normal Mapping" src="./.github/assets/normal_mapping.gif?raw=true" />

  #### Video
  <a href='https://youtu.be/3da86eRz4Tc'>
    <img alt='Video' src='https://img.youtube.com/vi/3da86eRz4Tc/0.jpg' />
  </a>
</details>

<details>
  <summary>Parallax Mapping</summary>
  
  #### Screenshot
  <img alt="Parallax Mapping" src="./.github/assets/parallax_mapping.gif?raw=true" />

  #### Video
  <a href='https://youtu.be/AXFGglWZZ74'>
    <img alt='Video' src='https://img.youtube.com/vi/AXFGglWZZ74/0.jpg' />
  </a>
</details>

<details>
  <summary>Post-processing</summary>
  
  #### Blur
  <img alt="Post-processing - Blur" src="./.github/assets/pp_blur.png?raw=true" />

  #### Chromatic Aberration
  <img alt="Post-processing - Chromatic Aberration" src="./.github/assets/pp_chromatic_aberration.gif?raw=true" />

  #### Gaussian Blur
  <img alt="Post-processing - Gaussian Blur" src="./.github/assets/pp_gaussian_blur.png?raw=true" />

  #### Color Space
  <img alt="Post-processing - Color Space" src="./.github/assets/pp_color_space.png?raw=true" />

  #### Color Tint
  <img alt="Post-processing - Color Tint - Gameboy" src="./.github/assets/pp_color_tint_gameboy.png?raw=true" />
  <img alt="Post-processing - Color Tint - Mono" src="./.github/assets/pp_color_tint_mono.png?raw=true" />
  <img alt="Post-processing - Color Tint - Inverted" src="./.github/assets/pp_color_tint_inverted.png?raw=true" />
  <img alt="Post-processing - Color Tint - Sepia" src="./.github/assets/pp_color_tint_sepia.png?raw=true" />

  #### Pixelation
  <img alt="Post-processing - Pixelation" src="./.github/assets/pp_pixelation.png?raw=true" />

  #### Vignette
  <img alt="Post-processing - Vignette" src="./.github/assets/pp_vignette.png?raw=true" />

  #### Video
  <a href='https://youtu.be/Bp0RTS6dGhA'>
    <img alt='Video' src='https://img.youtube.com/vi/Bp0RTS6dGhA/0.jpg' />
  </a>
</details>

<details>
  <summary>Terrain Generation</summary>
  
  #### Video
  <a href='https://youtu.be/UfZ9p7pJoGE'>
    <img alt='Video' src='https://img.youtube.com/vi/UfZ9p7pJoGE/0.jpg' />
  </a>
</details>

<details>
  <summary>Skeletal Animation / Kinematics (WIP)</summary>
  
  #### Model Loading
  <img alt="Model Loading" src="./.github/assets/kinematics_model_loading.png?raw=true" />
</details>

## ❓ About

<details>
  <summary>What is this?</summary>
  
  Vision is an experimental graphics research framework that was primarily began development during my 4th and final year of university to learn some more advanced graphical rendering techniques (In the case of Vision, primarily using DirectX11).

  In an effort to continue learning some more advanced graphical rendering techniques, I have made some minor modifications to the original project and open sourced it so others are able to both contribute towards the project and also learn from it.
  
</details>

<details>
  <summary>Why?</summary>
  
  When the project first began development towards the end of 2019, I began to quickly discover that the number of resources available to learn some more advanced graphics rendering techniques were few and far between.

  As such I wanted to help try and give back and help others who may find themselves in a position that was similar to mine. With further development of the project anyone should be able to use this project to understand multiple graphical rendering techniques without having to reference a 10+ year old book.

  When development of the project has progressed to a relatively stable point, work will begin on creating written documention for both using the project, along with in-depth analysis of all of the projects features and how each of them work. For example, a write up on normal mapping, how it works and why it is used.
  
</details>

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

## 📄 License

MIT © [Ben Dixon](https://github.com/nurodev/vision/blob/master/LICENSE)
