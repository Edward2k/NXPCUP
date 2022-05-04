# NXPCUP FMUK66 framework

Due to the instability of the development environment NXP provided, one was created in house. Special thanks to @nelis for setting this up.

## Setup

Follow the below steps chronologically.

### Tools on host system 

1) MbedStudio (Account required) -> For development
2) Our Git Repo 
3) Jlink{EXE/RTT} -> For debugging

### Preparing the project

1) Open MbedStudio and perform setup (login and follow the tutorial if you like)
2) Open a `New Workspace`, and open the repository of the the git repo just cloned
3) At the bottom of MbedStudio, you'll see a list of libraries. You'll see the `fix` icon warning you out-of-sync libraries. Press Fix and let them install. 
4) Apply the patches. To do so, copy and paste the following commands from the top-directory of the repo.
```bash
cd FastPWM
git apply ../patches/mbed-os_fmuk66-FastPWM.patch
cd ../mbed-os
git apply ../patches/mbed-os_fmuk66-i2c0_pinmap.patch
git apply ../patches/mbed-os_fmuk66-PinNames.patch 
cd ..
```
5) Hit the hammer icon to build the project. You'll know it worked if the output pane reads 
```none
Total Static RAM memory (data + bss): 12942(+4) bytes
Total Flash memory (text + data): 72159(+510) bytes
Image: BUILD/K66F/ARMC6/NXP.hex
```

## FAQ

### Where do I write my code?
In `source -> main.c` We are currently making a nice "write and play" environment. We will update the README accordingly.

### How do I build the project?
On the right-hand-side of MbedStudio, you'll find a hammer Icon. Hit that and let it build. This is will likely take a long time the first time as it is compiling an entire OS for the target.
Subsequent builds will be much faster.

### How do I flash the program?
... -> We are making some scripts to automate this. 

