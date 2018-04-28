#!/bin/bash

rm callgrind_dpll
valgrind --tool=callgrind --callgrind-out-file=callgrind_dpll ../bin/pci --solver=dpll
kcachegrind ./callgrind_dpll &
