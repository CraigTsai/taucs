#########################################################
# Linux                                                 #
#########################################################
OBJEXT=.o
LIBEXT=.a
EXEEXT= 
F2CEXT=.f
PATHSEP=/
DEFFLG=-D

FC        = g77
FC        = gfortran
FFLAGS    = -O3 -g -fno-second-underscore -Wall 
FOUTFLG   =-o 

COUTFLG   = -o
CFLAGS    = -O3 -g -D_POSIX_C_SOURCE=199506L -Wall -pedantic -ansi -fPIC -fexceptions -D_GNU_SOURCE 
CFLAGS    = -g -O3 -Wall -Werror -pedantic -ansi 
# for some reason, -std=c99 -pedantic crashes 
# with the error message "imaginary constants are a GCC extension"
# (seems to be a gcc bug, gcc 3.3.1)
CFLAGS    = -O3 -Wall -Werror -std=c89 -pedantic
CFLAGS    = -O3 -Wall -Werror -std=c99 
CFLAGS    = -O3 -Wall -std=c99 

LD        = $(CC) 
LDFLAGS   = 
LOUTFLG   = $(COUTFLG)

AR        = ar cr
AOUTFLG   =

RANLIB    = ranlib
RM        = rm -rf

HOME_STOLEDO = /specific/a/home/cc/cs/stoledo/Public

# These are for a Pentium4 version of ATLAS (not bundled with taucs)
#LIBBLAS   = -L $(HOME_STOLEDO)/Linux_P4SSE2/lib -lf77blas -lcblas -latlas \
#            -L /usr/lib/gcc-lib/i386-redhat-linux/2.96 -lg2c
##LIBLAPACK = -L $(HOME_STOLEDO)/Linux_P4SSE2/lib -llapack
#LIBLAPACK = -L $(HOME_STOLEDO)/Linux_P4SSE2/lib -llapack -L . -llapackfull

#LIBBLAS   = -L external/lib/linux -lf77blas -lcblas -latlas
#LIBLAPACK = -L external/lib/linux -llapack
#LIBLAPACK = -L external/lib/linux -llapack -llapack_sytrf 

#LIBMETIS  = -L external/lib/linux -lmetis 

#LIBF77 = -lg2c  
#LIBC   = -lm 

LIBBLAS   = -L external/lib/linux -lgoto -lpthread
LIBLAPACK = external/lib/linux/lapack_LINUX.a

LIBMETIS  = -L external/lib/linux -lmetis

LIBF77 = -lgfortran
LIBC   = -lm

#########################################################







