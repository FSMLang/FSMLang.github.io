##########################################################
#
#  Makefile for the hsmCommunicator FSMLang example
#

SRC = $(wildcard *actions.c)
FSM_SRC = $(wildcard *.fsm)

FSM_FLAGS=-ts --generate-weak-fns=false --force-generation-of-event-passing-actions

CFLAGS=-DHSM_COMMUNICATOR_DEBUG                   \
       -DHSM_COMMUNICATOR_ESTABLISH_SESSION_DEBUG \
       -DHSM_COMMUNICATOR_SEND_MESSAGE_DEBUG      \
       -g -ggdb

## FSMLang start

.SUFFIXES: .fsm .html .plantuml 

FSM ?= fsm

GENERATED_SRC = $(shell $(FSM) -M $(FSM_FLAGS) $(FSM_SRC))
GENERATED_HDR = $(shell $(FSM) -Mh $(FSM_FLAGS) $(FSM_SRC))

cleanfsm:
	@-rm -f $(GENERATED_SRC) 2> /dev/null
	@-rm -f $(GENERATED_HDR) 2> /dev/null
	@-rm -f *.fsmd           2> /dev/null

%.fsmd: %.fsm
	@set -e; $(FSM) -Md $(FSM_FLAGS) $< > $@

.fsm.o:
	@$(FSM) $(FSM_FLAGS) $< > fsmout 
	@$(CC) -c $(CFLAGS) $*.c
	@rm -f $*.c

.fsm.c:
	@$(FSM) $(FSM_FLAGS) $< > fsmout 

.fsm.h:
	@$(FSM) $(FSM_FLAGS) $< > fsmout 

ifneq ($(MAKECMDGOALS),clean)
-include $(FSM_SRC:.fsm=.fsmd)
endif

## FSMLang end

OBJS=$(SRC:.c=.o) $(GENERATED_SRC:.c=.o)

all: hsmCommunicator

test.out: hsmCommunicator
	./$< >$@

hsmCommunicator: $(OBJS) $(FSM) Makefile
	$(CC) -o $@ $(LDFLAGS) $(OBJS)
	@-astyle $(GENERATED_SRC) $(GENERATED_HDR)

clean: cleanfsm
	@-rm -f $(OBJS)            2> /dev/null
	@-rm -f hsmCommunicator    2> /dev/null
	@-rm -f test.out           2> /dev/null
	@-rm -f *.orig             2> /dev/null

$(SRC): hsmCommunicator_priv.h

