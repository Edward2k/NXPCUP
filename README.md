# NXPCUP FMUK66 framework

Due to the instability of the development environment NXP provided, one was created in house. Special thanks to @nelis for setting this up.

## Setup

Follow the below steps chronologically.

### Tools on host system 

1) MbedStudio (Account required) -> For development
2) Our Git Repo 
3) Jlink{EXE/RTT} -> For debugging

### Installing MbedStudio on Linux

1) Download MbedStudio here: https://os.mbed.com/studio/
2) Give the file "MbedStudio-1.4.3.sh" execution rights: `chmod +x MbedStudio-1.4.3.sh`
3) Execute the file to install MbedStudio: `./MbedStudio-1.4.3.sh`
4) Follow the installation instructions

### Setting up JLink

1) Download the Linux 64-bit DEB Installer here: https://www.segger.com/downloads/jlink
2) Execute the file to install JLink: `sudo apt install ./JLink_Linux_V764d_x86_64.deb`
3) If you get the warning `Download is performed unsandboxed as root as file...`, that is fine
4) Set up the JLink cable as described here: https://nxp.gitbook.io/nxp-cup/mr-buggy3-developer-guide/mr-buggy3-software-setup#connect-our-fmu-and-j-link-debugging-interface-to-our-computer

### Setting up the Pixy2

1) Install PixyMon as described here: https://docs.pixycam.com/wiki/doku.php?id=wiki:v2:installing_pixymon_on_linux
2) If you get the error: `Package 'qt5-default' has no installation candidate`, pls try `sudo apt install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools`
3) If PixyMon does not recognize the Pixy2, you might have to restart your pc for the Pixy2 permissions to take effect
4) Select Program > line_tracking
5) Select Configure Parameters to tweak the settings for the Pixy2. You want your Pixy2 to recognize the black line as stable as possible. Also make sure to set the Interface > Data out port to I2C.

### Preparing the project

1) Do not plug in the car via USB yet
2) Open MbedStudio and perform setup (login and follow the tutorial if you like)
3) Click on `Open Workspace` and open the repository of the the git repo just cloned
4) At the bottom of MbedStudio, you'll see a list of libraries. You'll see the `fix` icon warning you out-of-sync libraries. Press Fix and let them install. 
5) Apply the patches. To do so, copy and paste the following commands from the top-directory of the repo.
```bash
cd FastPWM
git apply ../patches/mbed-os_fmuk66-FastPWM.patch
cd ../mbed-os
git apply ../patches/mbed-os_fmuk66-i2c0_pinmap.patch
git apply ../patches/mbed-os_fmuk66-PinNames.patch 
cd ..
```
6) Plug in the car via the JLink USB cable. The car should be automatically recognized by MbedStudio.
7) Hit the hammer icon on the left side to build the project. This is will likely take a long time the first time as it is compiling an entire OS for the target. Subsequent builds will be much faster. You'll know it worked if the output pane reads 
```none
Total Static RAM memory (data + bss): 12942(+12942) bytes
Total Flash memory (text + data): 72159(+72159) bytes
Image: BUILD/FMUK66/ARMC6/NXP.hex
```

### Flashing the program

1) Top left, click Terminal > New Terminal
2) Execute the command `JLinkExe jlink/jflash.jlink`
3) Your program should immediately start running on the car

### Viewing the prints

If you want to debug your program and find out where your code goes wrong or you simply want to see what values your car prints then follow these instructions.

1) The following steps can either be done in your default terminal or in an MbedStudio terminal by selecting Terminal > New Terminal.
2) Open a new Terminal and run `JLinkRTTViewerExe`
3) Open a new Terminal, run `JLinkExe` and connect to the car: `connect`, `MK66FN2M0XXX18`, `S`, `4000`
4) As soon as you flash a new program on the car, it should automatically appear in JLinkRTTViewerExe

## FAQ

### Where do I write my code?
In `source -> main.c` We are currently making a nice "write and play" environment. We will update the README accordingly.
