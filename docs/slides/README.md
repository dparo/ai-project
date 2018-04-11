# Prerequisites to modify and build the slides
- Install `ruby-gem` and `rubygem-bundler` with your distro package manager first
`gem install bundler`
- If you're using `rvm` run:
`rvm use default`


# Creating a slide for the first time ( not the case here )
- `mkdir my-awesome-presentation`
- `cd my-awesome-presentation`
- `bundle config --local github.https true`
- `bundle --path=.bundle/gems --binstubs=.bundle/.bin`

## Optional copy or clone reveal.js locally. Allows to modify temes
   or view slides offline
- `git clone -b 3.6.0 --depth 1 https://github.com/hakimel/reveal.js.git`
- If you're already using git for version control and want it to use it as a submodule run:
- `git submodule add --depth 1 https://github.com/hakimel/reveal.js.git`

# Building/Rendering the slides
- `bundle exec asciidoctor-revealjs -a revealjsdir=https://cdnjs.cloudflare.com/ajax/libs/reveal.js/3.6.0 slides.adoc`
- If you have local reveal-js clone (this is the case here):
- `bundle exec asciidoctor-revealjs slides.adoc`