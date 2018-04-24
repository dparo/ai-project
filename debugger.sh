#!/bin/bash

# Usage run this script

# After open a new terminal
# Run on the new terminal the command: `tty`
# On the terminal running gdb type: `tty <TTY>`
# Now rerun the application and you wil have the output
# and input of the application in the new terminal

gnome-terminal -- gdb -ex "tui enable" bin/pci
