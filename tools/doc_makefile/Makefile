#========================================================================#
# Makefile for generating project documentation                          #
# ---------------------------------------------------------------------- #
#                                                                        #
# -- update source code fragments from the documentation                 #
#                                                                        #
# Given a MASTER_TEX_NAME LaTeX file name containing lgrind              #
# environments that reference files like sourcefile.cpp.tex or           #
# headerfile.h.tex, generate those files form the actual                 #
# source files (sourcefile.cpp and headerfile.h) using lgrind.           # 
#                                                                        #
# This makefile transforms any source file from your source or include   #
# directories into tex files, that can be used in the final              #
# documentation.                                                         #
# The documentation target builds the pdf from the master LaTex file.    #
#                                                                        #
#          -- use as a template for more important stuff --              #
#
# *** Requirements: *****************************************************#
#
# pdflatex, lgrind
#
# ***  Make Targets (depends on): ***************************************#
#
# documentation: Depends on the source code transformations in tex.
#                This target builds the pdf from [MASTER_TEX_NAME].tex 
# clean: Cleans the generated files (e.g. .cpp.tex, .h.tex, .pdf)
# [source file]-> tex: Uses lgrind (inclusion output) to transform source
#                      code into tex files.
# [include file]->tex: Same as above for header files
#
#
# *** Variables Used (default): *****************************************#
#
# srcdir (src) - the directory where your sources reside (*.c, *.cpp)
# includedir (include) - your include directory (*.h, *.hpp)
# docdir (doc) - directory for pdf documentation output
# outtex (doc/tex) - directory with the MASTER_TEX_NAME file (see below)
#                    and where the transformed code files will be saved
#                    (*.cpp.tex, *.h.tex)
# SOURCE_SUFFIX (cpp) - source file extensions to be transformed with
#                       lgrind
# INCLUDE_SUFFIX (h) - same as above, for include files
# MASTER_TEX_NAME (doc) - name of the existing LaTex documentation file,
#                         that uses lgrind environments for source code
#                         inclusion. Do not include the .tex extension.

#========================================================================#
# Lucian Carata, calucian[]yahoo.co.uk, 2008                             #
# B.Sc. Utility / Technical University "Gh. Asachi" Iasi                 #
#========================================================================#

# source files directory
srcdir = src
includedir = include
# pdf output dir
docdir = doc
# tex output dir
outtex = doc/tex
# build directory
builddir = .

# sources suffix
SOURCE_SUFFIX = cpp
INCLUDE_SUFFIX = h
MASTER_TEX_NAME = doc

#----------------- NO NEED TO MODIFY BELOW THIS LINE ----------------#

# project files for transformation
sources :=$(wildcard $(srcdir)/*.$(SOURCE_SUFFIX))
includes :=$(wildcard $(includedir)/*.$(INCLUDE_SUFFIX))

# tex files
texs := $(notdir $(sources))
texs := $(addprefix $(builddir)/, $(texs))
texs := $(texs:.$(SOURCE_SUFFIX)=.$(SOURCE_SUFFIX).tex)
hs := $(notdir $(includes))
hs := $(addprefix $(builddir)/, $(hs))
hs := $(hs:.$(INCLUDE_SUFFIX)=.$(INCLUDE_SUFFIX).tex)

.PHONY: documentation clean

$(builddir)/%.$(SOURCE_SUFFIX).tex: $(srcdir)/%.$(SOURCE_SUFFIX)
	@echo Generating tex file from source $?
	lgrind -i -lcc $? > $(outtex)/$@

$(builddir)/%.$(INCLUDE_SUFFIX).tex: $(includedir)/%.$(INCLUDE_SUFFIX)
	@echo Generating tex file from source $?
	lgrind -i -lcc $? > $(outtex)/$@

documentation: $(texs) $(hs)
	@echo Building pdf documentation
	pdflatex -output-directory $(outtex) $(MASTER_TEX_NAME).tex
	-rm $(outtex)/*.aux
	mv $(outtex)/$(MASTER_TEX_NAME).pdf $(docdir)/$(MASTER_TEX_NAME).pdf 

clean:
	-rm $(outtex)/*.log
	-rm $(outtex)/*.*.tex
	-rm $(docdir)/*.pdf