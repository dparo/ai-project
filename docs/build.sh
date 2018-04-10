#!/bin/bash
mkdir -p build
asciidoctor -a linkcss relazione.adoc --destination-dir="./build" && firefox build/relazione.html
