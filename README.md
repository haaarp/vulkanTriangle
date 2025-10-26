# Vulkan Tutorial 

My walkthrough of the vulkan tutorial from https://vulkan-tutorial.com/Introduction

> [!WARNING]  
> Wayland is not yet implemented. Use X11 instead for now.

## Development Setup

<details>
  <summary><h3>OpenSUSE Tumbleweed</h3></summary>
  
  <pre><code>
$ sudo zypper in glm-devel vulkan-devel shaderc libXi-devel libglfw-devel libXxf86vm-devel
  </code></pre>

</details>

<details>
  <summary><h3>Arch Linux</h3></summary>
  
  <pre><code>
$ sudo pacman -Syu vulkan-devel glfw glm libxi libxxf86vm
  </code></pre>

</details>

After you have installed the necessary packages, run `vkcube` to see if Vulkan
works.

