#!/bin/bash

trap ' ' INT

rm massif_dpll_print.log
valgrind --tool=massif --massif-out-file=massif_dpll.log ../bin/pci --solver=dpll
ms_print massif_dpll.log > massif_dpll_log_print.log
rm massif_dpll.log
less massif_dpll_log_print.log
