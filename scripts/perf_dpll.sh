#!/bin/bash

rm callgrind_dpll.log
valgrind --tool=callgrind --callgrind-out-file=callgrind_dpll.log ../bin/pci --solver=dpll
kcachegrind ./callgrind_dpll.log &
