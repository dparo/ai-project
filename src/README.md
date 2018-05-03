# Directory Structure
* `sat` contains the source code for the SAT solver.
* `ext-deps` contains git-submodule and external library dependencies
  from other sources
* `code-gen` contains the source code of a small program used to generate
  C-source code used later in the SAT solver compilation. See `code-gen/README.md`
  for more infos.
* `common` contains shared source code used by both: the SAT src, and the code-gen src.
  It is not used by `ext-deps`
* `__generated__` is the directory where `code-gen` places the generated files.
  **You should not add any other file in there cause they will be deleted**

# Building the SAT solver
## Installing the required dependencies
* In order to build the SAT solver you will need a C11 compatible compiler.
  Both `gcc` and `clang` should work fine, but currently the only
  tested compiler is gcc.
  On Debian-based distro's install gcc:
  ```
  sudo apt install gcc
  ```
  The SAT solver depends even on the external library `libreadline`.
  On Debian-based distro's you can install it with:
  ```
  sudo apt install libreadline-dev
  ```
  
## (Optional) Suggested dependencies for development
* For Debugging:
  ```
  sudo apt install gdb
  ```
* For measuring performance:
  ```
  sudo apt install valgrind
  ```

## After installing the required dependencies
* From this directory run either
  ```
  ./build.sh debug    # Builds the Debug version (Extra assert and checks will be compiled)
  ./build.sh release  # Builds the Release version
  ```
* (Optional) If you change the source code for `code-gen` and the templates it uses, you
  will need to re-generate the produced source. Run:
  ```
  ./build.sh code-gen
  ```
  It is not necessary to run it the first time, because the generated C-source code files
  are already distributed with this GIT repository.

  
