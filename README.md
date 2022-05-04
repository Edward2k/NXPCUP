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

## FAQ

### Where do I write my code?
In `source -> main.c` We are currently making a nice "write and play" environment. We will update the README accordingly.

### How do I build the project?
On the right-hand-side of MbedStudio, you'll find a hammer Icon. Hit that and let it build. This is will likely take a long time the first time as it is compiling an entire OS for the target.
Subsequent builds will be much faster.

### How do I flash the program?
... -> We are making some scripts to automate this. 

