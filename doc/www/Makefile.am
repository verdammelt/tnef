EXTRA_DIST	=
SUBDIRS		=	cgi-bin htdocs
WEB_LOC		=	verdammelt@tnef.sf.net:/home/groups/t/tn/tnef/
WEB_TEST_LOC	=	~damned/public_html/tnef-www/

web-build web-test web-push:
	list='$(SUBDIRS)'; for subdir in $$list; do			\
		test "$$subdir" = . 					\
			|| (cd $$subdir 				\
				&& $(MAKE) $(AM_MAKEFLAGS)		\
					WEB_LOC=$(WEB_LOC)$$subdir/	\
					WEB_TEST_LOC=$(WEB_TEST_LOC)$$subdir/ \
						$@);  			\
	done
