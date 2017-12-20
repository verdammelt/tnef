SUBDIRS		=	www
man_MANS	=	tnef.1
EXTRA_DIST	=	file-format.txt tnef.1.in FAQ

web-build web-test web-push:
	(cd www && $(MAKE) $(AM_MAKEFLAGS) $@)
