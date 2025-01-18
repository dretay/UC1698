- [Overview](#overview)
- [Compiling](#compiling)
- [Configuring the pins](#configuring-the-pins)
- [Hardware](#hardware)

# Overview
This project contains the low level GDISP driver for [UGFX](https://ugfx.io/) and reference PCB design needed to drive a 160x160 Black COG Graphic LCD Display Module w/UC1698 Controller [like this one](https://www.buydisplay.com/160x160-black-cog-graphic-lcd-display-module-uc1698-controller).
![](https://raw.githubusercontent.com/dretay/UC1698/master/pics/IMG_5088.jpg)

# Compiling
This project was tested with a project based in STM32CubeIDE. To integrate this project, you need to add the UGFX base library to your project, then include this project to expose the relevant drivers.
![](https://raw.githubusercontent.com/dretay/UC1698/master/pics/cubeide_sources.png)
![](https://raw.githubusercontent.com/dretay/UC1698/master/pics/cubeide_includes.png)

Once the project has been added, it will be built due to references made in the `src/gfx_mk.c` to the larger UGFX project.

# Configuring the pins
The project works by leveraging two 8-bit shift registers to send the appropriate signaling and data information to the screen. You can choose to send this data either through a dedicated hardware SPI interface or through a software-implemented bit-banged SPI if hardware is unavailable (see `src/ShiftRegister.c` for more info). 
In the future DMA may be added to this driver, but at the moment its refresh speed is sufficient for projects I need it for.

# Hardware
You can download the board design here: https://drive.proton.me/urls/KZKH2CP9B0#GZa0iEeRZ5Vj. 
