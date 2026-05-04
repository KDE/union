# Union

Union is a style engine designed to provide a unified style description to a set
of separate output styles.

## Getting Started

To use Union, you will need to have Union installed, either by installing it using
your favorite package manager, or by building it from source.

To use Union in a QtQuick or Kirigami application, run the application with the
environment variable `QT_QUICK_CONTROLS_STYLE` set to `org.kde.union`.

To learn more about how to use or develop Union, see the [Union Documentation][].

[Union Documentation]: https://api.kde.org/union-index.html

## Building

To build Union, you will need a recent version of CMake and Qt version 6.9 or
higher. You will also need to build [cxx-rust-cssparser] which is required for
the default CSS input format.

[cxx-rust-cssparser]: https://invent.kde.org/libraries/cxx-rust-cssparser

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
