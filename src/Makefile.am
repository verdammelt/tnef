EXTRA_DIST	=	alloc.h			\
			basename.h		\
			getopt_long.h		\
			ldiv.h			\
			mapi-types.data		\
			mapi-names.data		\
			mkdata.awk		\
			path.h			\
			sizes.h			\
			strdup.h		\
			tnef-names.data		\
			tnef-types.data 
bin_PROGRAMS	=	tnef
tnef_SOURCES	=	alloc.c main.c path.c tnef.c tnef.h
BUILT_SOURCES	=	tnef_names.c tnef_names.h	\
			tnef_types.c tnef_types.h	\
			mapi_types.c mapi_types.h	\
			mapi_names.c mapi_names.h
BUILT_OBJS	=	tnef_names.o tnef_types.o 	\
			mapi_types.o mapi_names.o
tnef_LDADD	=	@LIBOBJS@ $(BUILT_OBJS)

MOSTLYCLEANFILES=	tnef_names.c tnef_names.h	\
			tnef_types.c tnef_types.h	\
			mapi_types.c mapi_types.h	\
			mapi_names.c mapi_names.h

tnef.o:	tnef.h
tnef.h: sizes.h
$(tnef_OBJECTS): $(BUILT_OBJS)
tnef_names.c tnef_names.h: tnef-names.data mkdata.awk
	$(SHELL) -c 'TAG=tnef_name $(AWK) -f $(srcdir)/mkdata.awk \
			$(srcdir)/tnef-names.data'
tnef_types.c tnef_types.h: tnef-types.data mkdata.awk
	$(SHELL) -c 'TAG=tnef_type $(AWK) -f $(srcdir)/mkdata.awk \
			$(srcdir)/tnef-types.data'
mapi_types.c mapi_types.h: mapi-types.data mkdata.awk
	$(SHELL) -c 'TAG=mapi_type $(AWK) -f $(srcdir)/mkdata.awk \
			$(srcdir)/mapi-types.data'
mapi_names.c mapi_names.h: mapi-names.data mkdata.awk
	$(SHELL) -c 'TAG=mapi_name $(AWK) -f $(srcdir)/mkdata.awk \
			$(srcdir)/mapi-names.data'
