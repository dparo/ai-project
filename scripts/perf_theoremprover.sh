#!/bin/bash

rm callgrind_theoremprover.log
valgrind --tool=callgrind --callgrind-out-file=callgrind_theoremprover.log ../bin/pci --solver=tp
kcachegrind ./callgrind_theoremprover.log &
