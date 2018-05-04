# Viewing the slides
* The slides can just be opened with a web-browser.
  Just open `slides.html` with your favourite web-browser.
  
  
# Modifying and building the slides
* The slides are simply constructed with simple markdown file: `slides.md`.
  You will need pandoc to convert the markdown file `.md` to
  a `.html` file used for viewing the slides.
  (Yes exactly, no PowerPoint bullshit. Praise the Shell)
* Download pandoc from [Pandoc Website](https://pandoc.org/index.html)
  or if you're on any Debian based distro `sudo apt install pandoc`
* run `build.sh` to compile the slides
* **NOTE**: Graphviz `dot` program is required to compile the graphs
  representation used for the slides. You don't need to install it,
  it is optional.
  On Debian-based distros: `sudo apt install graphviz`

# PDF Output (Optional)
* In order to compile the book pdf version of the slide you will need
  latex installed, the latex fonts, and some basic other
  dependencies, which i don't remember at the top of my head.
  It should be straight-forward to figure it out based
  on the error messages.
  On Debian-based distros install latex with
  `sudo apt install texlive texlive-fonts-recommended`
  
