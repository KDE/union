# Union

Union is a style engine designed to provide a unified style description to a set
of separate output styles.

## Getting Started

To use Union in your application, ensure it is installed and then tell your
application to use Union as a style. For detailed instructions on how to to
this, see the [Using Union in applications][applications] page in the
documentation.

[applications]: https://files.quantumproductions.info/union/applications.html

If you wish to use the library or develop Union itself, you should start by
reading the [Overview][overview] page. This gives a general overview of
how the code is structured, important technical concepts and relevant types to
use.

[overview]: https://files.quantumproductions.info/union/overview.html

Note that currently there is no documentation about developing styles for Union.
This is due to there being only one input format plugin that uses KDE Plasma's
SVG styling.

## Building

To build Union, you will need a recent version of CMake and Qt version 6.8 or
higher. To build the Plasma SVG input plugin, you will additionally need
[RapidYAML][rapidyaml] version 0.6 or higher.

[rapidyaml]: https://github.com/biojppm/rapidyaml
