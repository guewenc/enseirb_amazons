# Game of the Amazons

## Description

This project is an implementation of the game of the Amazons in C language. It allows two players to compete by playing on a square board of 10x10 squares.

## Prerequisites

This project requires the GSL library to be installed. You can install it on Ubuntu with the following command:

```bash
sudo apt-get install libgsl-dev
```

## Installation

To install the game, simply run the following command:

```bash
make clean install
```

## Usage

The game consists of two parts: a server and two clients. To start the game, you first need to launch the server, and then the two clients. To do this, run the following command:

```bash
./install/server client1.so client2.so
```

The clients are identified by their name, which is passed as an argument when launching the game. The players can then take turns playing using the available commands.

## Run tests

The tests related to the project are present in the `tst` folder.

```bash
make test
```

## Documentation

A Doxygen configuration file is present at the root of the project. Link to the Doxygen project: <https://github.com/doxygen/doxygen>.
Prerequisite `bison` package `>= 2.7`. Change the `DOT_PATH` variable in the `dgenerate` file to the location of your `dot` package. You can find it with the `which dot` command.

To compile the documentation, follow these instructions:

```bash
make clean doc
```

All generated documentation is present in the `doc` folder at the root of the project.

## Contact

If you have any questions or comments about this project, you can contact the authors:

* Guewen COUSSEAU
* Benjamin DAYRES
* Corentin OZANE
* Maxime GAJIC
* Badr EL-HABTI
