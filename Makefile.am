AUTOMAKE_OPTIONS=	gnu

EXTRA_DIST	=	TODO configure BUGS.in tnef.spec.in
SUBDIRS		=	doc src tests

web-build web-test web-push:
	(cd doc && $(MAKE) $(AM_MAKEFLAGS) $@)
