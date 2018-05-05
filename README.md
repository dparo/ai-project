# Cloning
`git clone --recursive https://github.com/dparo/ai-project`

# Building
Please refer to the `README.md` located in `src/` directory.

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
      
      
      
  CURRENTLY RESERVED AND NOT IMPLEMENTED OPERATORS
  SUPPORT MAY BE ADDED LATER
  ================================================
  
  =              ASSIGN OPERATOR
  +              CONCAT OPERATOR
  +=             CONCAT AND ASSIGN OPERATOR
  *              DEREF OPERATOR
  f(x, y, ...)   FNCALL OPERATOR
  a[i, ...]      INDEX OPERATOR
  a{i, ...}      COMPOUND OPERATOR
  
  #              ENUMERATE OPERATOR
  $              EXIST OPERATOR
  @              AT OPERATOR
  in             IN KEYWORD OPERATOR
  ?:             TERNARY OPERATOR
  ```
* Currently **NO** implementation, but the following idioms can be
  parsed correctly
  ```
  (a, b, c, ...)     PAREN DELIMITED LIST
  [a, b, c, ...]     BRACKET DELIMITED LIST
  {a, b, c, ...}     BRACE DELIMITED LIST
  ```
* An identifier is any token starting either with a letter or underscore.
  Examples:
  ```
  a    _a_   xyz  a1
  ```
* Support for parsing C-style strings, currently the interpreter **rejects** strings:
  ```
  "Hello world"
  "Hello world with escaped \"quotes\""
  ```
  Same thing for C-style character constants (The REPL rejects them)
  ```
  'A'  'B'   '\n'
  ```
* Support for C-Style comments `//` `/*   */`. The REPL simply ignores comments.
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
    
# Having fun with the REPL
* Let's prove if De-Morgan was right altogether when he revised his own laws
  
  First we will prove it using bruteforcing method. Run:
  
  ```
  ./bin/pci --solver=bf
  ```
  
  In the REPL type the De-Morgan Law: `~(a | b) == (~a & ~b)`
  
  In the Result column of the generated table you should see
  only `1's`.
  
  **NOTE**: The columns are only `tab` separated and they are not
  aligned. While this makes reading the result more difficult, it 
  allows for simpler spreadsheet import and generation of `.csv` files,
  which is required anyway when the formula starts to have more than
  2 or 3 literals.

## There's gotta be a better way !
* Yes, the interpreter has a special solver for proving `laws` and `theorems`.
  Run the interpreter with:
  ```
  ./bin/pci --solver=th
  ```
  
  Now in the REPL retype the same formula `~(a | b) == (~a & ~b)`.
  
  The interpreter will output a nice colorfull green message
  saying that the theorem was proven successfully. It also
  prints some usefull informations like the amount of time
  it took for doing the required computations.

# What now?
* I don't know, keep having fun with it. Implement new features
  for the interpreter like lists or support for propositional calculus.
  Or write a circuit synthesizer that uses this program
  as a backend to prove simple circuits validity. 
  Or try to rewrite it in order to overcome the limitations referred
  down below.
  
  **Sky is the limit my friend...**

# LIMITATIONS (IMPORTANT)
* This project is supposed to be used merely as studying material
  and just to have some fun learning and interacting with the `SAT World`.
  In the current implementation of the `SAT` for every recursion call
  of `DPLL` we duplicate the `AST`. This leads to easier implementation
  of the algorithm and technically faster (if not instant)
  `backtracking-time` performance, since,
  when `DPLL` arrives to an unstatisfiable solution it just needs to `free`
  the `AST` and return to the caller which in turn stores it's own copy.
  
  > This leads to **HUGE** **EXPONENTIAL** memory consumption.
  
  For example those random formulas leads to a **peak** memory
  consumption of:
  
  ```c
  /* F1 */ a == b ^ c -> d | e <-> f ^ g & h | j | k & l > n | m & o ^ q
  /* F2 */ a == b ^ c -> d | e <-> f ^ g & h | j | k & l > n | m & o ^ q ~& r
  /* F3 */ a == b ^ c -> d | e <-> f ^ g & h | j | k & l > n | m & o ^ q ~& r ^ s == t
  ```
  
  | Formula | Memory Consumption |
  |---------|--------------------|
  |  F1     |  4.7 MegaBytes     |
  |  F2     |  7.68 MegaBytes    |
  |  F3     |  96.20 MegaBytes   |
  
  It is obviously dependent on the branching factor. `AND` and `OR` operators
  tend to be cheap, while operators like `^` `==` tend to branch more which
  causes more visible exponential memory consumption
  
* This interpreter is not obviously an `industrial strength proven` interpreter
  and may only be used with caution and just to have some fun.
  Don't rely on it to try to synthesize logic circuits with **millions**
  of `logic gates`.
  
