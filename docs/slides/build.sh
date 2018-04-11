#!/bin/bash

bundle exec asciidoctor-revealjs -a revealjsdir=https://cdnjs.cloudflare.com/ajax/libs/reveal.js/3.6.0 slides.adoc
bundle exec asciidoctor-revealjs slides.adoc
