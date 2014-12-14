#
# Sample Makefile

# Variables
ARCHIVE=ar
CC=gcc 
LINK=gcc
CFLAGS=-c -Wall -I. -fpic -g
LINKFLAGS=-L. -g
LIBFLAGS=-shared -Wall
LINKLIBS=-lcmpsc311 -lsmsa -lgcrypt

# Files to build
LIBS=		libsmsa.so \
		libcmpsc311.so \
		libsmsa_st.a \
		libcmpsc311_st.a 
			

SASIM_OBJFILES=		smsa_sim.o \
			smsa_driver.o \
			smsa_cache.o
SMWL_OBJFILES=		smsa_workload.o
SMSALIB_OBJFILES=	smsa.o \
			smsa_unittest.o
LOGLIB_OBJFILES=	cmpsc311_log.o \
			cmpsc311_util.o
TARGETS=		smsasim \
			smsa_wlgen \
			verify

					
# Suffix rules
.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS)  -o $@ $<

# Productions

dummy : $(TARGETS) 

smsasim : $(SASIM_OBJFILES) $(LIBS)
	$(LINK) $(LINKFLAGS) -o $@ $(SASIM_OBJFILES) $(LINKLIBS) 
	
smsa_wlgen : $(SMWL_OBJFILES) $(LIBS)
	$(LINK) $(LINKFLAGS) -o $@ $(SMWL_OBJFILES) $(LINKLIBS) 

verify : verify.o
	$(LINK) $(LINKFLAGS) -o $@ verify.o

# 64-bit libraries

libsmsa.so : $(SMSALIB_OBJFILES)
	$(LINK) $(LIBFLAGS) -o $@ $^

libsmsa_st.a : $(SMSALIB_OBJFILES)
	$(ARCHIVE) rcs -o $@ $^
	
libcmpsc311.so : $(LOGLIB_OBJFILES)
	$(LINK) $(LIBFLAGS) -o $@ $^

libcmpsc311_st.a : $(LOGLIB_OBJFILES)
	$(ARCHIVE) rcs -o $@ $^

# Cleanup 
clean:
	rm -f $(TARGETS) $(SMWL_OBJFILES) $(LIBS) $(SASIM_OBJFILES) \
		$(SMSALIB_OBJFILES) $(LOGLIB_OBJFILES) verify.o
  
# Dependancies
cmpsc311_log.o: cmpsc311_log.c cmpsc311_log.h
cmpsc311_util.o: cmpsc311_util.c cmpsc311_util.h cmpsc311_log.h
smsa.o: smsa.c smsa.h smsa_internal.h cmpsc311_log.h cmpsc311_util.h
smsa_sim.o: smsa_sim.c smsa.h smsa_unittest.h cmpsc311_log.h
smsa_unittest.o: smsa_unittest.c smsa.h smsa_internal.h cmpsc311_log.h
smsa_workload.o: smsa_workload.c smsa.h cmpsc311_log.h cmpsc311_util.h
