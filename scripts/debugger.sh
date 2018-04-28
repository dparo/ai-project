#!/bin/bash



# STDOUT=$(mktemp)
# GDBLOG=$(mktemp)

# konsole -e tail -F "$STDOUT" &
# konsole -e tail -F "$GDBLOG" &

export EDITOR="emacsclient"


# tty
# do: while true; do sleep 1000; done

while true; do
    gdb \
        -tty="/dev/null" \
        ../bin/pci \
        -ex "set args --solver=tp"\
        -ex "start"
done


#-ex "set logging file $GDBLOG" \
    #-ex "set logging on" \

    

