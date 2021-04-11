# Colorize

## Contents

* [Introduction](#introduction)
* [Installation](#installation)
* [Usage](#usage)

## Introduction

This is a program for dealing with palette-mapped images.
The base image is specified as a PGM specifying palette indices.
A second image contains the palatte information:
each individual palette occupies its own row,
and there is no limit to how many exist.
Finally a third, optional, image specifies per-region palette information.
This map need not be the same size as the base image;
smaller maps automatically result in larger regions using the same palette.

## Installation

Requires a C compiler, the [Netpbm][netpbm] library,
BSD make and `pkg-config`.
Installation is as simple as:

    bmake colorize
    PREFIX=/usr/local
    install -dm0755 "${PREFIX}/bin"
    install -dm0755 "${PREFIX}/share/man/man1"
    install -m0755 colorize "${PREFIX}/bin"
    install -m0644 colorize.1 "${PREFIX}/share/man/man1"

`PREFIX` should be set to the desired installation prefix.
The `install` commands may need to be run as root,
depending on permissions in the target directories.

[netpbm]: http://netpbm.sourceforge.net

## Usage

The only required option is the collection of palettes.

    colorize -p palettes.ppm -m map.pgm base.pgm

Both the `-m map.pgm` option and the `base.pgm` image are optional.
If the map file is not specified,
then there is a single region set to the first palette only.
If the base image is not specified,
then it is read from the standard input.
Output images are written to the standard output.

In terms of memory requirments,
the entire collection of palettes is loaded into memory at once,
but only one row each of the map and base image are processed at a time.
This allows larger images to be processed.
