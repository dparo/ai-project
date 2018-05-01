#!/bin/bash

set -x

pandoc -o book.pdf slides.md
pandoc -t slidy -V slidy-url=./Slidy2 -s slides.md -o slides.html
# pandoc -t beamer -s slides.md -o beamer.pdf
