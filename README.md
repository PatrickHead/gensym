# gensym - A utility for generating Ringdove EDA schematic symbols

This is a C language utility that performs the same function as the official Ringdove boxsym command.

<a id="TOC"></a>
## Table of contents

* [Introduction](#introduction)
* [Installation](#installation)
* [Quick start](#quick-start)
* [Usage](#usage)
* [Known issues and limitations](#known-issues-and-limitations)
* [Getting help](#getting-help)
* [License](#license)

<a id="introduction"></a>
## Introduction

<b>gensym<b> is a CLI command the generates Ringdove EDA symbol files in standard lihata format.  <b>gensym</b> uses the same input file format as the official Ringdove boxsym command.  The <b>gensym</b> command has been created with a couple of objectives in mind.  The first to correct a few auto placement and sizing issues with boxsym.  The second is to provide a foundation for expanding the symbol generation beyond rectangular box symbols in the future.  gensym also lends itself to high speed bulk symbol generation.

[Back to Table of Contents](#TOC)

<a id="installation"></a>
## Installation

A reasonably modern version of GNU Autotools should be installed before installing <b>gensym</b> from the source files.  At the time of writing this document, Autotools version 2.71 were used.

To compile the source code, the standard build tools are required, such as GCC, binutils, make, etc.

If you want to create the Doxygen API documentation, then of course Doxygen and friends need to installed.

If you have obtained the <b>gensym</b> source code from a GIT repository, then execute the `autoreconf` program while in the directory that the <b>gensym</b> GIT clone is located.

For both GIT clones and GNU Package tarballs, run the usual `./configure` followed by `make`.

To install the built `gensym` utility, run `sudo make install`.

[Back to Table of Contents](#TOC)

<a id="quick-start"></a>
## Quick start

  * to see the `gensym` help screen: run `gensym -h`

[Back to Table of Contents](#TOC)

<a id="usage"></a>
## Usage

gensym v1.0.0

  usage:

  gensym -h

  gensym [-s] [-o <output name>] <file name> ...

  where:

    -s generate file(s) for each symbol slot
    -o use <output name> for output file(s)

    <file name> name of boxsym(.bs) definition file

  -h -- this help screen

[Back to Table of Contents](#TOC)

<a id="known-issues-and-limitations"></a>
## Known issues and limitations

At the time of writing of this document, there are no known issues with the <b>gensym</b> software.

[Back to Table of Contents](#TOC)

<a id="getting-help"></a>
## Getting help

If you encounter any bugs or require additional help, contact [Patrick Head](mailto:patrickhead@gmail.com)

[Back to Table of Contents](#TOC)

<a id="license"></a>
## License

All code and configuration data for the **gensym** software is licensed under the [GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html) or later.   See **COPYING** for details.

_This_ README file is itself distributed under the terms of the [Creative Commons 1.0 Universal license (CC0)](https://creativecommons.org/publicdomain/zero/1.0/).

[Back to Table of Contents](#TOC)

