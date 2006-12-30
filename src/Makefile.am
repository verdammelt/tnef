SUBDIRS = lib app

# lib_LTLIBRARIES		=	libtnef.la
# 
# libtnef_la_SOURCES	= 	alloc.c attr.c attr_names.c	\
# 				attr_types.c io.c mapi.c 	\
# 				mapi_names.c mapi_types.c 	\
# 				tnef.c
# 
# libtnef_la_LIBADD	=	$(LIBOBBJS) $(ALLOCA)
# 
# MOSTLYCLEANFILES=	attr_names.c attr_names.h	\
# 			attr_types.c attr_types.h	\
# 			mapi_types.c mapi_types.h	\
# 			mapi_names.c mapi_names.h
# 
# $(libtnef_a_OBJECTS): $(BUILT_OBJS)
# 
# attr_names.c attr_names.h: tnef-names.data mkdata.awk
# 	$(SHELL) -c 'TAG=attr_name $(AWK) -f $(srcdir)/mkdata.awk \
# 			$(srcdir)/tnef-names.data'
# attr_types.c attr_types.h: tnef-types.data mkdata.awk
# 	$(SHELL) -c 'TAG=attr_type $(AWK) -f $(srcdir)/mkdata.awk \
# 			$(srcdir)/tnef-types.data'
# mapi_types.c mapi_types.h: mapi-types.data mkdata.awk
# 	$(SHELL) -c 'TAG=mapi_type $(AWK) -f $(srcdir)/mkdata.awk \
# 			$(srcdir)/mapi-types.data'
# mapi_names.c mapi_names.h: mapi-names.data mkdata.awk
# 	$(SHELL) -c 'TAG=mapi_name $(AWK) -f $(srcdir)/mkdata.awk \
# 			$(srcdir)/mapi-names.data'
# 
# bin_PROGRAMS	=	test
# test_LDADD	=	$(lib_LTLIBRARIES)
# TESTS		=	test
