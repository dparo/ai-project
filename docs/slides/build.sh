#!/bin/bash

set -x

dot -Gdpi=92 -Tpng dot/formula_op_conv.dot -o imgs/formula_op_conv.png
dot -Gdpi=92 -Tpng dot/formula_demorgan.dot -o imgs/formula_demorgan.png
dot -Gdpi=92 -Tpng dot/formula_neg_elim.dot -o imgs/formula_neg_elim.png
dot -Gdpi=92 -Tpng dot/formula_or_distrib.dot -o imgs/formula_or_distrib.png

pandoc -o book.pdf slides.md
pandoc -t slidy -V slidy-url=./Slidy2 -s slides.md -o slides.html
# pandoc -t beamer -s slides.md -o beamer.pdf


