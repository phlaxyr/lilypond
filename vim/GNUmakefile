depth = ..

INSTALLATION_OUT_DIR=$(vimdir)/syntax
INSTALLATION_OUT_FILES=$(LILYPOND_WORDS)
# $(outdir)/lilypond-words $(outdir)/lilypond-words.el

EXTRA_DIST_FILES=$(call src-wildcard,*.vim) vimrc

LILYPOND_WORDS = $(outdir)/lilypond-words $(outdir)/lilypond-words.vim
LILYPOND_WORDS_DEPENDS =\
  $(top-src-dir)/lily/lily-lexer.cc \
  $(buildscript-dir)/lilypond-words \
  $(top-src-dir)/scm/markup.scm \
  $(top-src-dir)/ly/engraver-init.ly

STEPMAKE_TEMPLATES=install-out

# vimdir defined in config.make
include $(depth)/make/stepmake.make

local-install:
	for a in compiler ftdetect ftplugin indent syntax; do \
		$(INSTALL) -d $(DESTDIR)$(vimdir)/$$a \
		&& $(INSTALL) -m 644 $(src-dir)/lilypond-$$a.vim $(DESTDIR)$(vimdir)/$$a/lilypond.vim ; \
	done

local-uninstall:
	for a in compiler ftdetect ftplugin indent syntax; do \
		rm $(DESTDIR)$(vimdir)/$$a/lilypond.vim ; \
		rmdir $(DESTDIR)$(vimdir)/$$a; \
	done
	-rmdir -p $(DESTDIR)$(vimdir)

$(buildscript-dir)/lilypond-words:
	$(MAKE) -C $(depth)/scripts/build

$(LILYPOND_WORDS):
	$(call ly_progress,Making,$@,)
	cd $(top-src-dir) && $(buildscript-dir)/lilypond-words --words --vim --dir=$(top-build-dir)/vim/$(outconfbase)

all: $(LILYPOND_WORDS)
