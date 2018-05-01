#!/bin/bash

set -x

pushd "./dot"
dot -Gdpi=92 -Tpng formula_op_conv.dot -o formula_op_conv.png
dot -Gdpi=92 -Tpng formula_demorgan.dot -o formula_demorgan.png
dot -Gdpi=92 -Tpng formula_neg_elim.dot -o formula_neg_elim.png
dot -Gdpi=92 -Tpng formula_or_distrib.dot -o formula_or_distrib.png
popd

pandoc -o book.pdf slides.md
pandoc -t slidy -V slidy-url=./Slidy2 -s slides.md -o slides.html
# pandoc -t beamer -s slides.md -o beamer.pdf


