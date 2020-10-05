IDIR     =../Inc
SRCDIR   = ../Src
UTILSDIR = ./utils
CC       = gcc
DEFINE   = ANC_MATH_TESTING_IMPLEMENTATION
CFLAGS   = -I $(IDIR) -I $(UTILSDIR) -D $(DEFINE) -Wall -pedantic
LIBS     = -lm

ODIR     =_build

# Building tests

all: test_iir3_circular test_fir_circular_decimate test_fir_circular_interp test_fir_circular test_lnlms_circular

test_vector_handler.o:
	$(CC) -c $(UTILSDIR)/test_vector_handler.c -o $(ODIR)/test_vector_handler.o $(CFLAGS)

anc_parameters.o:
	$(CC) -c $(SRCDIR)/anc_parameters.c -o $(ODIR)/anc_parameters.o $(CFLAGS)

iir3_circular.o:
	$(CC) -c $(SRCDIR)/iir3_circular.c -o $(ODIR)/iir3_circular.o $(CFLAGS)

fir_circular_decimate.o:
	$(CC) -c $(SRCDIR)/fir_circular_decimate.c -o $(ODIR)/fir_circular_decimate.o $(CFLAGS)

fir_circular_interp.o:
	$(CC) -c $(SRCDIR)/fir_circular_interp.c -o $(ODIR)/fir_circular_interp.o $(CFLAGS)

fir_circular.o:
	$(CC) -c $(SRCDIR)/fir_circular.c -o $(ODIR)/fir_circular.o $(CFLAGS)

lnlms_circular.o:
	$(CC) -c $(SRCDIR)/lnlms_circular.c -o $(ODIR)/lnlms_circular.o $(CFLAGS)

test_iir3_circular.o:
	$(CC) -c test_iir3_circular/test_iir3_circular.c -o $(ODIR)/test_iir3_circular.o $(CFLAGS)

test_fir_circular_decimate.o:
	$(CC) -c test_fir_circular_decimate/test_fir_circular_decimate.c -o $(ODIR)/test_fir_circular_decimate.o $(CFLAGS)

test_fir_circular_interp.o:
	$(CC) -c test_fir_circular_interp/test_fir_circular_interp.c -o $(ODIR)/test_fir_circular_interp.o $(CFLAGS)

test_fir_circular.o:
	$(CC) -c test_fir_circular/test_fir_circular.c -o $(ODIR)/test_fir_circular.o $(CFLAGS)

test_lnlms_circular.o:
	$(CC) -c test_lnlms_circular/test_lnlms_circular.c -o $(ODIR)/test_lnlms_circular.o $(CFLAGS)

test_iir3_circular: test_vector_handler.o iir3_circular.o anc_parameters.o test_iir3_circular.o
	$(CC) $(ODIR)/test_vector_handler.o $(ODIR)/iir3_circular.o $(ODIR)/anc_parameters.o $(ODIR)/test_iir3_circular.o -o $(ODIR)/test_iir3_circular $(CFLAGS) $(LIBS)

test_fir_circular_decimate: test_vector_handler.o fir_circular_decimate.o anc_parameters.o test_fir_circular_decimate.o
	$(CC) $(ODIR)/test_vector_handler.o $(ODIR)/fir_circular_decimate.o $(ODIR)/anc_parameters.o $(ODIR)/test_fir_circular_decimate.o -o $(ODIR)/test_fir_circular_decimate $(CFLAGS) $(LIBS)

test_fir_circular_interp: test_vector_handler.o fir_circular_interp.o anc_parameters.o test_fir_circular_interp.o
	$(CC) $(ODIR)/test_vector_handler.o $(ODIR)/fir_circular_interp.o $(ODIR)/anc_parameters.o $(ODIR)/test_fir_circular_interp.o -o $(ODIR)/test_fir_circular_interp $(CFLAGS) $(LIBS)

test_fir_circular: test_vector_handler.o fir_circular.o anc_parameters.o test_fir_circular.o
	$(CC) $(ODIR)/test_vector_handler.o $(ODIR)/fir_circular.o $(ODIR)/anc_parameters.o $(ODIR)/test_fir_circular.o -o $(ODIR)/test_fir_circular $(CFLAGS) $(LIBS)

test_lnlms_circular: test_vector_handler.o fir_circular.o lnlms_circular.o anc_parameters.o test_lnlms_circular.o
	$(CC) $(ODIR)/test_vector_handler.o $(ODIR)/fir_circular.o $(ODIR)/lnlms_circular.o $(ODIR)/anc_parameters.o $(ODIR)/test_lnlms_circular.o -o $(ODIR)/test_lnlms_circular $(CFLAGS) $(LIBS)


# Running tests

run_all: all run_test_iir3_circular run_test_fir_circular_decimate run_test_fir_circular_interp run_test_fir_circular run_test_lnlms_circular

run_test_iir3_circular:
	@./$(ODIR)/test_iir3_circular

run_test_fir_circular_decimate:
	@./$(ODIR)/test_fir_circular_decimate

run_test_fir_circular_interp:
	@./$(ODIR)/test_fir_circular_interp

run_test_fir_circular:
	@./$(ODIR)/test_fir_circular

run_test_lnlms_circular:
	@./$(ODIR)/test_lnlms_circular

# Cleaning build

.PHONY: clean

clean:
	rm -rf $(ODIR)/*

