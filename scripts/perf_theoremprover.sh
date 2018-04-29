#!/bin/bash

rm callgrind_theoremprover
valgrind --tool=callgrind --callgrind-out-file=callgrind_theoremprover ../bin/pci --solver=tp
kcachegrind ./callgrind_theoremprover &
