EXTRA_DIST	=
WEB_DIST	=

web-build:

web-test: web-build
	for f in $(WEB_DIST); do		\
		scp $$f $(WEB_TEST_LOC);	\
	done

web-push:
	for f in $(WEB_DIST); do		\
		scp $$f $(WEB_LOC);		\
	done