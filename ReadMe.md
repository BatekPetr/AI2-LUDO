# AI2-LUDO Player

## Project description
## History

## How to run:

This guide will walk you through the steps to clone, build, and run the AI2-LUDO project on Ubuntu 22.04.

### Prerequisites

Ensure that you have the following installed:
- Git
- C++ compiler (GCC)
- Qt5 development libraries

### Install Dependencies

Open a terminal and run the following commands to install the required packages:

```bash
sudo apt update
sudo apt install build-essential qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools g++
```

### Clone Repo, Build and Run

```bash
git clone https://github.com/BatekPetr/AI2-LUDO.git
cd AI2-LUDO
mkdir build
cd build
qmake ../ludo.pro
make
./ludo
```

