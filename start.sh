#!/bin/bash

# Fai partire i tool del mestiere.
# Prepararsi a una lunga nottata.


emacs --execute "(set 'compile-command \"cd $PWD/src/ && ./build.sh debug\")" "./src/sat/main.c" &
urxvt -e ddd -f ./bin/pci &
exit
