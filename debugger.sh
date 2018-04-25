#!/bin/bash



# STDOUT=$(mktemp)
# GDBLOG=$(mktemp)

# konsole -e tail -F "$STDOUT" &
# konsole -e tail -F "$GDBLOG" &


gdb \
    -tty="/dev/null" \
    bin/pci \
    -ex "start"



#-ex "set logging file $GDBLOG" \
    #-ex "set logging on" \

    

