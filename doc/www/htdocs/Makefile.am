EXTRA_DIST	=
WEB_DIST	= index.html style.css

index.html: index.html.in
	now=`date +"%Y-%m-%d %T %Z"`; sed -e "s/BUILDDATE/$$now/" $< > $@

web-build: $(WEB_DIST)

web-test: web-build
	for f in $(WEB_DIST); do		\
		scp $$f $(WEB_TEST_LOC);	\
	done

web-push: web-build
	for f in $(WEB_DIST); do		\
		scp $$f $(WEB_LOC);		\
	done
