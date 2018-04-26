#!/bin/bash



# STDOUT=$(mktemp)
# GDBLOG=$(mktemp)

# konsole -e tail -F "$STDOUT" &
# konsole -e tail -F "$GDBLOG" &

export EDITOR="emacsclient"


# tty
# do: while true; do sleep 1000; done

gdb \
    -tty="/dev/null" \
    ../bin/pci \
    -ex "start"


#-ex "set logging file $GDBLOG" \
    #-ex "set logging on" \

    

