#!/bin/bash



# Use Konsole.
# Both `urxvt` and `gnome-terminal` seems
# to crash randomly when rerunning the executable with
# the `run` command


# For coloring the GDB prompt output if wanted
# set prompt \001\033[01;34m\002(GDB) \001\033[0m\002


EDITOR=emacsclient

export EDITOR


rm .gdblog

konsole -e  gdb \
        -ex "tui enable" -ex "tty $(tty)" \
        -ex "source .gdbsource" \
        bin/pci &

sleep 1

konsole -e tail -F ".gdblog" &
