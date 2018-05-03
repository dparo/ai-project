
# Introduzione

# Cloning
`git clone --recursive https://github.com/dparo/ai-project`

# Running the interpreter
* `./bin/pci --solver=SOLVER`. If no solver is specified it defaults to `dpll`.
  Solver can be one of:
  ```
  bf                           = Use bruteforce solver
  dpll                         = Use DPLL solver
  th | tp | dpll-th | dpll-tp  = Prove a Theorem with DPLL backend
  ```

# Interacting with the REPL
* A token is seperated by whitespaces and punctuators
* Punctuators are basic operators, the currently available operators
  are:
  ```
  ~   !    NOT OPERATOR
  |   ||   OR OPERATOR
  &   &&   AND OPERATOR
  ~|       NOR OPERATOR
  ~&       NAND OPERATOR
  ^        XOR OPERATOR
  ==       IS EQUAL OPERATOR
  >        GREATER OP
  >=       GREATER EQUAL OP
  <        LESS OP
  <=       LESS EQUAL OP
  ->       IMPLY OPERATOR
  <->      DOUBLE IMPLY OPERATOR
      
  CURRENTLY UNDERSTOOD BUT NOT IMPLEMENTED OPERATORS
  
  =              ASSIGN OPERATOR
  +              CONCAT OPERATOR
  +=             CONCAT AND ASSIGN OPERATOR
  *              DEREF OPERATOR
  f(x, y, ...)   FNCALL OPERATOR
  a[i, ...]      INDEX OPERATOR
  a{i, ...}      COMPOUND OPERATOR
  
  #              ENUMERATE OPERATOR
  $              EXIST OPERATOR
  @              ON OPERATOR
  in             KEYWORD OPERATOR
  ?:             TERNARY OPERATOR
  ```
* An identifier is any token starting either with a letter or underscore
* Support for C-Style comments `//` `/*   */`
* **True** and **False** constants can be used respectively with `1` or `0` digits.
* Any other digit will be rejected by the interpreter: `1000` is not a valid constant

## Writing the first simple formula
* Start typing simple formulas with the available operators:
  ```
  A & B
  ```
  `A` and `B` will be considered literals and unassigned values.
  The interpreter, based on the specified solver will either:
  * Generate a truth table for every possible assignment of the literals
    (Bruteforce Solver)
  * Find a satisfiable solution (at least one assignment that will make
    the formula true). (DPLL Solver)
  * Verify if the formula is always `true` indipendently on the interpretation
    of the input values. (Theorem Prover Solver)
    
  
