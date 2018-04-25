#!/bin/bash

# Fai partire i tool del mestiere.
# Prepararsi a una lunga nottata.


emacs --execute "(set 'compile-command \"cd $PWD/src/ && ./build.sh debug\")" "./src/sat/main.c" &
#./debugger.sh &
exit
