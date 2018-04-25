#!/bin/bash



# Use Konsole.
# Both `urxvt` and `gnome-terminal` seems
# to crash randomly when rerunning the executable with
# the `run` command


# For coloring the GDB prompt output if wanted
# set prompt \001\033[01;34m\002(GDB) \001\033[0m\002


EDITOR=emacsclient

export EDITOR

# STDOUT=$(mktemp)
# GDBLOG=$(mktemp)

# -ex "tty $(tty)"


# konsole -e tail -F "$STDOUT" &
# konsole -e tail -F "$GDBLOG" &


gdb \
    -ex "tui enable"  \
    -ex "source .gdbsource" \
    -ex "tty /dev/null" \
    bin/pci \
    -ex "start"



    #-ex "set logging file $GDBLOG" \
    #-ex "set logging on" \

    

