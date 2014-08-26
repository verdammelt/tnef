AUTOMAKE_OPTIONS=	foreign

EXTRA_DIST	=	TODO configure BUGS.in tnef.spec.in README.md
SUBDIRS		=	doc src tests

web-build web-test web-push:
	(cd doc && $(MAKE) $(AM_MAKEFLAGS) $@)
