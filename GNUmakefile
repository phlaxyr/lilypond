# -*-makefile-*-

# Do not publish non-polished or non-finished or outdated translations.
LANGUAGES = de es fr nl

.PHONY: add all clean default dist menuify out scripts site TAGS tree $(LANGUAGES)

PYTHON = python
SCRIPTS = $(wildcard scripts/*.py scripts/*.scm scripts/*.sh)
SCRIPTDIR = scripts

VERSION = $(shell expr "$$(grep 'Revision: [0-9]\+' site/index.html)" : '.*Revision: \([0-9]\+\.[0-9]\+\)')
DISTDIR = lily-web-$(VERSION)

READMES = ChangeLog README TRANSLATION
SITE_HTML = $(shell find site -name '*.html')
LOCAL_HTML = $(shell find fr nl -name '*.html')
NO_TRANSLATION = '/announce-|/devel/|/donate|/old-|/older-|/search'
FILES = GNUmakefile newweb.css \
 $(SITE_HTML) $(IHTML) $(LOCAL_HTML) $(NON_HTML) $(READMES) $(SCRIPTS)
MAKE_LANGUAGE=	$(MAKE) LANG=$@ png menuify DOWNLOAD_URL="$(DOWNLOAD_URL)"

outball = site.tar.gz

LANG =
SITE = site

DOWNLOAD_URL = 'http://lilypond.org/download/'
INKSCAPE = inkscape

default: all

ifneq ($(LANG),)
SITE = $(LANG)
po: po/out/newweb.po $(mo)

po-update: po/out/newweb.po
	rm -f po/out/$(LANG).po
	msgmerge po/$(LANG).po po/out/newweb.po -o po/out/$(LANG).po

po-replace: po/out/newweb.po
	mv po/out/newweb.po po/newweb.pot
	mv po/out/$(LANG).po po/$(LANG).po

po/$(LANG).po: po/newweb.pot
	msgmerge --update $@ $^

mo = out/locale/$(LANG)/LC_MESSAGES/newweb.mo
$(mo): po/$(LANG).po
	mkdir -p $(dir $@)
	msgfmt --output=$@ $<

# Only regenerate for LANGs
$(LANG)/%.svg: site/%.svg $(mo) scripts/translate.py GNUmakefile
	mkdir -p $(dir $@)
	LANG=$(LANG) $(PYTHON) $(SCRIPTDIR)/translate.py --remove-quotes --outdir=$(dir $@) $(@:$(LANG)/%=site/%)

# no inkscape on lilypond.org
.PRECIOUS: %.png %.svg


%.png: %.svg GNUmakefile
	-$(INKSCAPE) --export-area-drawing --export-png=$@ --export-background-opacity=0 $<

out/site/%.$(LANG).png: $(LANG)/%.png
	-cp $< $@
else
LANG_LOOP = $(foreach lang,$(LANGUAGES),$(MAKE) LANG=$(lang) $@ &&) true

po:
	$(LANG_LOOP)
po-update: po
	$(LANG_LOOP)
po-replace: po-update
	$(LANG_LOOP)
endif

EXT = .jpeg .ly .pdf .png
HTML = $(shell find $(SITE) -name '*.html')
IHTML = $(shell find site -name '*.ihtml')
NON_HTML = $(shell find site -false $(EXT:%=-or -name '*%'))
TREE = $(shell cd site && find . -type d -not -name CVS)
PY = $(shell find scripts site -name '*.py')
SVG = $(shell find site -name '*.svg')

all: scripts linktree update-versions menuify $(LANGUAGES) apache-1.3.x-fixup

update-versions:
	$(PYTHON) $(SCRIPTDIR)/versiondb.py --download --dbfile lilypond.versions --url $(DOWNLOAD_URL)

# In Apache 1.3.x, .nl always has a higher LanguagePriority than the
# empty content-language, everyone will see the Dutch pages.
# Adding .en pages links fixes that.
apache-1.3.x-fixup:
	rm -f $$(find out/site -name '*.en.html')
	for i in $$(find out/site -name '*.html' | grep -v '\...\.html'); do\
		b=$$(basename $$i .html);\
		ln -sf $$b.html $$(dirname $$i)/$$b.en.html;\
	done;
	rm -f $$(find out/site -name '*.en.png')
	for i in $$(find out/site -name '*.png' | grep -v '\...\.png'); do\
		b=$$(basename $$i .png);\
		ln -sf $$b.png $$(dirname $$i)/$$b.en.png;\
	done;

scripts:
	python $(SCRIPTDIR)/big-page.py site/about/automated-engraving
	python $(SCRIPTDIR)/big-page.py fr/about/automated-engraving

# no silly buttons
site: all

TAGS:
	etags $$(find scripts site -name '*.html' -o -name '.py')

po/out/newweb.po: $(PY) $(SVG) $(IHTML)
	mkdir -p po/out
	rm -f po/out/newweb.po
	touch po/out/newweb.po
	xgettext --default-domain=newweb \
		--from-code=utf-8 --join --add-comments \
		--language=python --keyword=_ \
		 --output-dir=$(dir $@) $(PY) $(SVG)
	xgettext --default-domain=newweb \
		--join --add-comments\
		--language=c --keyword=_ --keyword=_@ \
		--output-dir=$(dir $@) $(IHTML)

define LANGUAGE_template
$(1):
	$(MAKE) LANG=$(1) png menuify DOWNLOAD_URL="$(DOWNLOAD_URL)"
endef

$(foreach lang,$(LANGUAGES),$(eval $(call LANGUAGE_template,$(lang))))

check-translation:
	python $(SCRIPTDIR)/check-translation.py $(HTML)

png: $(SVG:site/graphics/%.svg=out/site/graphics/%.$(LANG).png)

show-committish:
	@$(foreach a, $(wordlist 2,1000,$(MAKECMDGOALS)), \
		echo -n '    Translation of GIT committish: ';\
		git-rev-list --max-count=1 HEAD $(a) &&) true
	$(eval MAKECMDGOALS=)

new:
	mkdir -p $(LANG)
	cd $(LANG) && mkdir -p $(TREE)
	HEAD=`git-rev-list --max-count=1 HEAD` && \
	$(foreach i, $(SITE_HTML), \
		(test -e $(LANG)/$(i:site/%=%) \
			|| sed 's/<FILL[^>]*>/'$$HEAD'/g' < $(i) \
				> $(LANG)/$(i:site/%=%)) && ) true
	-find ./$(LANG) | grep -E $(NO_TRANSLATION) | xargs rm -rf

tree:
	rm -rf out/site
	mkdir -p out/site
	cd out/site && mkdir -p $(TREE)

menuify: $(mo)
	LANG=$(LANG) $(PYTHON) $(SCRIPTDIR)/format-page.py --version-db lilypond.versions --verbose $(FMP_OPTIONS) --outdir=out $(HTML)

linktree: tree
	$(foreach i, $(NON_HTML), ln -f $i out/$i &&) true
	cd out && touch .xvpics && rm -rf $$(find -name .xvpics)
	ln -f newweb.css out/site

out/$(outball): site
	cd out && tar czvf $(outball) site 


WEBSERVER=lilypond.org:
upload: site
	cd out/site \
	   && chgrp -R lilypond . \
	   && chmod -R g+w * \
	   && chmod 2775 . $$(find . -type d)
	cd out/site \
	   && rsync -go --stats --progress -rltvu . $(WEBSERVER)/var/www/lilypond/web/
# do not --delete, lilypond.org has no inkscape, transated pngs will be removed
#	   && rsync --delete -go --stats --progress -rltvu . $(WEBSERVER)/var/www/lilypond/web/

dist:
	mkdir $(DISTDIR)
	cp --parents -av $(FILES) $(DISTDIR)
	tar cfz $(DISTDIR).tar.gz $(DISTDIR)
	rm -rf $(DISTDIR)

clean:
	rm -rf out po/out

