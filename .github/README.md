# Core Library

This library provides foundational utilities (particularly those requiring platform-dependent implementations), including cross-platform numeric types, logging and other tools for debug builds, basic testing utils, hashing infrastructure, a distinct type for representing UTF-8 code units and exception-related utils.

Interface documentation can be directly found in the library header file, [libraries/core.hpp](../libraries/core.hpp), in Javadoc-esque documentation comments.

## Licence

The content of the Core repository is free software; you can redistribute it and/or modify it under the terms of the [GNU General Public License](http://www.gnu.org/licenses/gpl-2.0.txt) as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

The content is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

## Quick Start

*   Building requires [SCons 2](http://scons.org/) and [buildtools](https://github.com/gcrossland/buildtools). Ensure that the contents of buildtools is available on PYTHONPATH e.g. `export PYTHONPATH=/path/to/buildtools` (or is in the SCons site_scons dir).
*   From the working directory (or archive) root, run SCons to make a release build, specifying the compiler to use and where to find it e.g. `scons CONFIG=release TOOL_GCC=/usr/bin`.
    *   The library files are deployed to the library cache dir, which is (by default) under buildtools.
*   Run the tests by running the executable produced by the build and checking that it completes successfully e.g. `./core || echo "tests failed"`.
    *   If a test fails, the test executable will abort. To get more information (particularly a stack trace), run a debug build with a debugger.
