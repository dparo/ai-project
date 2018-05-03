# How it works.
* The `code-gen` reads templates file from the `templates` directory.
  It applies substituion rules to generate common source-code.
  It allows to "achieve" ( in quote marks! ) metaprogramming in C,
  and avoid maintenance issue of maintaining multiple files which are
  basically identical with a couple of differences.
  
  Currently the `code-gen` reads `templates/stack.template.c` to
  generate all the STACKS datastructures required by the SAT solver.
  
# IMPORTANT
* Every time the templates or the code-gen source code are modified,
  you'll need to re-run the process of generating source code.
  Go to the top-level `../src` directory and run 
  ```
  ./build.sh code-gen
  ```
  And then rebuild the SAT solver to use the new source code
  ```
  ./build.sh debug
         OR
  ./build.sh release
  ```
  
