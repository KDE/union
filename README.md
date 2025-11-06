# Union

Union is a style engine designed to provide a unified style description to a set
of separate output styles.

## Getting Started

To use Union in your Kirigami application, ensure it is installed and then use
`QT_QUICK_CONTROLS_STYLE=org.kde.union` or equivalent in code.

To learn more about how to use or develop Union, see the [Union Documentation][].

[Union Documentation]: https://api.kde.org/union-index.html

Note that currently there is no documentation about developing styles for Union.
This is due to there being only one input format plugin that uses KDE Plasma's
SVG styling.

## Building

To build Union, you will need a recent version of CMake and Qt version 6.8 or
higher. To build the Plasma SVG input plugin, you will additionally need
[RapidYAML][rapidyaml] version 0.6 or higher.

[rapidyaml]: https://github.com/biojppm/rapidyaml

You can build and install Union [manually with CMake](cmake) like this:

[cmake]: https://develop.kde.org/docs/getting-started/building/cmake-build/

```bash
cmake -B build/ --install-prefix /usr
cmake --build build/ --parallel
sudo cmake --install build/
```

Or with [kde-builder][]:

[kde-builder]: https://develop.kde.org/docs/getting-started/building/kde-builder-setup/

```bash
kde-builder union
```

To allow building the code examples, use:

```bash
cmake -B build --install-prefix /usr -DBUILD_EXAMPLES=ON
```

Or:

```bash
kde-builder union
kde-builder union --no-include-dependencies --cmake-options=-DBUILD_EXAMPLES=ON
```

## Get Involved

You may contact the Union developers in the [Union room](https://go.kde.org/matrix/#/#union:kde.org) on [Matrix](https://community.kde.org/Matrix).
