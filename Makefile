FEATURETOOL = opp_featuretool
FEATURES_H = src/s2f/features.h

.PHONY: all clean cleanall makefiles makefiles-so makefiles-lib makefiles-exe checkenvir checkmakefiles doxy doc submodule-init

all: checkmakefiles $(FEATURES_H)
	@cd src && $(MAKE)

clean: checkmakefiles
	@cd src && $(MAKE) clean

cleanall: checkmakefiles
	@cd src && $(MAKE) MODE=release clean
	@cd src && $(MAKE) MODE=debug clean
	@rm -f src/Makefile $(FEATURES_H)

S2F_PROJ = $(SIMCAN_HOME)

MAKEMAKE_OPTIONS := -f --deep -KINET4_5_PROJ=$(INET_ROOT) -DINET_IMPORT -I$$\(INET4_5_PROJ\)/src -L$$\(INET4_5_PROJ\)/src -o Simcan2Fog -O out -pSimcan2Fog -I. -lINET$$\(D\) 

makefiles: makefiles-so

makefiles-so: checkenvir $(FEATURES_H)
	@FEATURE_OPTIONS=$$($(FEATURETOOL) options -f -l) && cd src && opp_makemake --make-so $(MAKEMAKE_OPTIONS) $$FEATURE_OPTIONS

makefiles-lib: checkenvir $(FEATURES_H)
	@FEATURE_OPTIONS=$$($(FEATURETOOL) options -f -l) && cd src && opp_makemake --make-lib $(MAKEMAKE_OPTIONS) $$FEATURE_OPTIONS

makefiles-exe: checkenvir $(FEATURES_H)
	@FEATURE_OPTIONS=$$($(FEATURETOOL) options -f -l) && cd src && opp_makemake $(MAKEMAKE_OPTIONS) $$FEATURE_OPTIONS

checkenvir:
	@if [ "$(S2F_PROJ)" = "" ]; then \
	echo; \
	echo '==========================================================================='; \
	echo '<simcan_home>/setenv is not sourced. Please change to the Simcan2Fog root directory'; \
	echo 'and type "source setenv" to initialize the environment!'; \
	echo '==========================================================================='; \
	echo; \
	exit 1; \
	fi

checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '========================================================================'; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '========================================================================'; \
	echo; \
	exit 1; \
	fi

# generate an include file that contains all the WITH_FEATURE macros according to the current enablement of features
$(FEATURES_H): $(wildcard .oppfeaturestate) .oppfeatures
	@$(FEATURETOOL) defines >$(FEATURES_H)

doc:
	@cd doc/src && $(MAKE)
	@doxygen doxy.cfg

ddoc:
	@cd doc/src && ./docker-make html && echo "===> file:$$(pwd)/_build/html/index.html"

