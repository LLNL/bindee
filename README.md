# bindee

bindee is a best-effort, binding tool for generating C++\-Python bindings.

### Requirements

bindee depends on two open source projects:

* [Clang LibTooling]: library for operating on the Clang AST
* [pybind11]: library for generating C++\-Python bindings

### Building

bindee uses `llvm-config`, which should have been installed with Clang, to generate correct compilation flags.
After the correct versions of `clang++` and `llvm-config` have been loaded:

```
$ make [EXTRA_FLAGS=compiler_flags] && make test
```

The `EXTRA_FLAGS` variable allow users to avoid specifying common `compiler_flags` for every `bindee` invocation (see below).

### Running

To bind all possible names:
```
$ ./bindee file.hh -- [compiler_flags]
```

`compiler_flags` is the set of flags you would use to normally compile `file.hh`, e.g. macro definitions or include paths.

To bind specific names:
```
$ ./bindee -d file.hh -- [compiler_flags]
$ # remove unnecessary lines in file.in
$ ./bindee -i file.in file.hh -- [compiler_flags]
```

After generating a bind file, replace any `@TEXT@` strings until the code successfully compiles.

### Contributing

Contributions may be made by creating a pull request into the `master` branch.
Bugfixes and features must be accompanied by a test.

### License

bindee is distributed under the terms of the MIT license.
All new contributions must be made under this license.

See LICENSE and NOTICE for details.

SPDX-License-Identifier: MIT

LLNL-CODE-803397

[//]: #
   [Clang LibTooling]: <https://clang.llvm.org/docs/LibTooling.html>
   [pybind11]: <https://github.com/pybind/pybind11>
