TCRESTHOMEDIR?=$(HOME)/t-crest
TCRESTINCLUDEDIR?=$(TCRESTHOMEDIR)/patmos/c
TCRESTLINKDIR?=$(TCRESTHOMEDIR)/patmos/tmp
TCRESTNOCINIT?=$(TCRESTHOMEDIR)/patmos/c/cmp/nocinit.c

PROJECT?=playgame
FUNCTION?=playgame

ENFORCER?=Wolf_Rabbit_Game

BUILDDIR?=../tmp
LIBNOC=$(BUILDDIR)/libnoc.a
LIBMP=$(BUILDDIR)/libmp.a
LIBCORETHREAD=$(BUILDDIR)/libcorethread.a
LIBETH=$(BUILDDIR)/libeth.a
LIBELF=$(BUILDDIR)/libelf.a

#Delay to global memory per request in cycles
T_DELAY_CYCLES?=21
#Global memory transfer time per burst in cycles
G_DELAY_CYCLES?=0
#Transfer size (burst size) of the global memory in bytes
B_SIZE?=16

$(TCRESTHOMEDIR)/patmos/tmp/$(PROJECT).elf: ./*.c
	patmos-clang \
	-target patmos-unknown-unknown-elf \
	-O2 \
	-I $(TCRESTINCLUDEDIR) \
	-I $(TCRESTINCLUDEDIR)/libelf/ \
	-mpatmos-disable-vliw \
	-mpatmos-method-cache-size=0x1000 \
	-mpatmos-stack-base=0x200000 \
	-mpatmos-shadow-stack-base=0x1f8000 \
	-Xgold --defsym -Xgold __heap_end=0x1f0000 \
	-Xgold -T \
	-Xgold $(TCRESTHOMEDIR)/patmos/hardware/spm_ram.t \
	-o $(TCRESTHOMEDIR)/patmos/tmp/$(PROJECT).elf \
	./*.c \
	$(TCRESTNOCINIT) \
	-L $(TCRESTLINKDIR) \
	-lmp \
	-lnoc \
	-lcorethread \
	-leth \
	-lelf \
	-lm \
	-D PROGS_PER_CORE=$(PROGS_PER_CORE) \
	-mserialize=$(TCRESTHOMEDIR)/patmos/tmp/$(PROJECT).pml

build: $(TCRESTHOMEDIR)/patmos/tmp/$(PROJECT).elf
	
wcet-config: 
	platin pml-config \
	--target patmos-unknown-unknown-elf \
	-o $(TCRESTHOMEDIR)/patmos/tmp/$(PROJECT)config.pml \
	--tdelay $(T_DELAY_CYCLES) \
	--gtime $(G_DELAY_CYCLES) \
	--bsize $(B_SIZE)

wcet: build wcet-config
	platin wcet \
	-i $(TCRESTHOMEDIR)/patmos/tmp/$(PROJECT).pml \
	--enable-wca \
	--disable-ait \
	--stats \
	--binary $(TCRESTHOMEDIR)/patmos/tmp/$(PROJECT).elf \
	-e $(FUNCTION) \
	-o $(PROJECT).wca \
	--report $(PROJECT)_wcet_report.txt \
	-i $(TCRESTHOMEDIR)/patmos/tmp/$(PROJECT)config.pml



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

all: esterel enforcer main

esterel: $(PROJECT).c

main: main.c F_move.c F_position.c F_score.c playgame_data.c playgame.c game.c ann.c ann_fixed.c libfixmath/fix16.c libfixmath/fix16_exp.c libfixmath/fix16_sqrt.c libfixmath/fix16_trig.c
	$(CC) -o $@ $^ -lm -ggdb3

run: main
	./main

mainonly: main.c ann.c ann_fixed.c libfixmath/fix16.c libfixmath/fix16_exp.c libfixmath/fix16_sqrt.c libfixmath/fix16_trig.c
	$(CC) -o main ann.c ann_fixed.c libfixmath/fix16.c libfixmath/fix16_exp.c libfixmath/fix16_sqrt.c libfixmath/fix16_trig.c main.c -lm -ggdb3

$(PROJECT).c: $(PROJECT).strl $(PROJECT).h
	@echo " *** Esterel COMPILE $<  --->   $@"
	$(ESTEREL2C) $(EFLAGS) $<

enforcer: F_move.c F_position.c F_score.c

#generate the C sources from the erte files
F_move.c: move.erte
	easy-rte-parser -i ../$(ENFORCER) -o ../$(ENFORCER)
	easy-rte-c -i ../$(ENFORCER) -o ../$(ENFORCER)
F_position.c: position.erte
	easy-rte-parser -i ../$(ENFORCER) -o ../$(ENFORCER)
	easy-rte-c -i ../$(ENFORCER) -o ../$(ENFORCER)
F_score.c: score.erte
	easy-rte-parser -i ../$(ENFORCER) -o ../$(ENFORCER)
	easy-rte-c -i ../$(ENFORCER) -o ../$(ENFORCER)

clean:
	rm -f *.o *.exe *.xes *.xev *.blif *.pml *.elf *.wca /tmp.*
	rm -f $(patsubst %.strl,%.c,$(wildcard *.strl))
	rm -f $(patsubst %.strl,%,$(wildcard *.strl))