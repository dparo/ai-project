#!/bin/bash

EDITOR=emacsclient
export EDITOR
konsole -e gdb -ex "tui enable" -ex "tty $(tty)" bin/pci
