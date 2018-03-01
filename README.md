# ZebrafishTracker

Authors:

    Peter Polidoro <polidorop@janelia.hhmi.org>

License:

    BSD

## Example Usage

```shell
cd ~/git/ZebrafishTracker
./bin/ZebrafishTracker
```

```shell
./bin/ZebrafishTracker -h
Usage: ZebrafishTracker [params]

	-?, -h, --help, --usage (value:true)
		Print usage and exit.
	-b, --blind
		Do not communicate with camera.
	-c, --configuration (value:../ZebrafishTrackerConfiguration)
		Configuration repository path.
	-d, --debug
		Print debug info.
	-m, --mouse
		Track mouse click location instead of blob.
	-p, --paralyze
		Do not communicate with stage so it does not move.
	-r, --recalibrate
		Recalibrate with chessboard before running.

```

## Installation

### Setup Linear Motors

<https://github.com/janelia-experimental-technology/linear_motor_setup>

### Setup Jetson

<https://github.com/janelia-experimental-technology/nvidia_jetson_setup>

### Install Dependencies

```shell
sudo apt-get install cmake
sudo apt-get install libboost-all-dev
```

# Checkout Git Repositories

```shell
mkdir ~/git
cd ~/git
git clone https://github.com/janelia-experimental-technology/ZebrafishTracker.git
git clone https://github.com/janelia-experimental-technology/ZebrafishTrackerConfiguration.git
```

### Compile

```shell
cd ~/git/ZebrafishTracker
mkdir build
cd build
cmake ..
make
```
