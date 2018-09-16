PROJECT?=controller
ENFORCER=av

DARKNET=../darknet
VPATH=$(DARKNET):$(DARKNET)/include/*

SRC = 	$(wildcard $(DARKNET)/src/*.c)
OBJ =	$(SRC:.c=.o)
DEPS=	$(wildcard $(DARKNET)/obj/*.o)

# Force 32-bit build on Athlon64/Opteron systems
ifeq ($(shell uname -p), x86_64)
  GCC32=-m32 
endif

export ESTEREL=/opt/esterelv6_01

CC       = gcc
CFLAGS   = $(GCC32) -Wall -L$(ESTEREL)/lib -I$(ESTEREL)/include
LDFLAGS  = $(GCC32) -L$(ESTEREL)/lib -I$(ESTEREL)/include -lrt -lpthread

#CFG      = /home/esterel/etc/v5_92-env
#ESTEREL := $(shell sed -n -e "s/ESTEREL=\(.*\)/\1/p" $(CFG))

ESTEREL2C= $(ESTEREL)/bin/esterel
EFLAGS   =
EFLAGS4X = -simul -I

XES      = $(ESTEREL)/bin/xes
XESFLAGS = -cc "$(CC)" $(foreach t,$(GCC32),-Xcomp $(t))

XEVE     = $(ESTEREL)/bin/xeve

# Fix for XES on a Linux system with glibc-2.3.3:
#    ": undefined reference to `__ctype_b'"
# see: http://newweb.ices.utexas.edu/misc/ctype.c
#
ifeq ($(shell uname -s), Linux)
  XESCTYPEFIX=ctype.o
endif

main: $(DEPS) main.c F_$(ENFORCER).c ga.c ensemble_data.c ensemble.c controller.c controller_data.c plant.c tmr.c ann.c ann_fixed.c libfixmath/fix16.c libfixmath/fix16_exp.c
	$(CC) -o $@ $^ -I $(DARKNET)/include/include.h -lm -ggdb3 -pthread -g

all: esterel enforcer main

run: main
	./main

esterel: controller.c ensemble.c

controller.c: controller.strl controller.h
	@echo " *** Esterel COMPILE $<  --->   $@"
	$(ESTEREL2C) $(EFLAGS) $<
ensemble.c: ensemble.strl ensemble.h
	@echo " *** Esterel COMPILE $<  --->   $@"
	$(ESTEREL2C) $(EFLAGS) $<

enforcer: F_$(ENFORCER).c

F_$(ENFORCER).c: $(ENFORCER).erte
	easy-rte-parser -i ../$(ENFORCER) -o ../$(ENFORCER)
	easy-rte-c -i ../$(ENFORCER) -o ../$(ENFORCER)

darknet: 
	$(MAKE) objs -C $(DARKNET)

clean:
	rm -f *.o *.exe *.xes *.xev *.blif *.pml *.elf *.wca /tmp.* $(PROJECT).c
	rm -f $(patsubst %.strl,%.c,$(wildcard *.strl))
	rm -f $(patsubst %.strl,%,$(wildcard *.strl))
	rm -f *.xml
	rm -f F_*