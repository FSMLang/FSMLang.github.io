.. role:: fsmlang(code)
	:language: fsmlang

========
Gnu Make
========

------------------
Supporting FSMLang
------------------

The FSMLang development is done in C.  And, since a large part of the development tree is devoted to 
testing, the maintainers have put some effort into having FSMLang support GNU Make in a natural way.

First, the file, fsmrules.mk, found in the root of the development tree, provides all of the target
rules needed for creating the output files supported by FSMLang.  The file also contains rules
specifically to support the testing tree.  These will not be discussed at present.

Starting at the top, we have the additional suffixes supported and a definition for our FSM macro,
should one not have been provided.

.. code-block:: make

	####################################################
	#
	#  .fsm rules
	#
	
	.SUFFIXES: .fsm .html .plantuml 

	FSM ?= fsm


Cleanliness is important; this next material identifies all of the output generated by FSMLang so
that the cleanfsm target can remove it.

.. code-block:: make

	GENERATED_SRC = $(shell $(FSM) -M $(FSM_FLAGS) $(FSM_SRC))
	GENERATED_HDR = $(shell $(FSM) -Mh $(FSM_FLAGS) $(FSM_SRC))
	
	cleanfsm:
		@-rm -f $(GENERATED_SRC) 2> /dev/null
		@-rm -f $(GENERATED_HDR) 2> /dev/null
		@-rm -f *.fsmd           2> /dev/null

Next, Make was created for project managment, which has, as a key point, dependency tracking.  So,
fsmrules.mk provides a way to make dependency rules for files created by FSMLang.

.. code-block:: make

	%.fsmd: %.fsm
		@set -e; $(FSM) -Md $(FSM_FLAGS) $< > $@

	ifneq ($(MAKECMDGOALS),clean)
	-include $(FSM_SRC:.fsm=.fsmd)
	endif

The dependencies (.fsmd) created here, and the generated file lists above, are C-output specific,
but a similar rule and variables could be set up for html and plantuml output.

The suffix rules are pretty standard; though perhaps they should be adapted to more modern usage.

.. code-block:: make

	.fsm.o:
		@$(FSM) $(FSM_FLAGS) $< > fsmout 
		@$(CC) -c $(CFLAGS) $*.c
		@rm -f $*.c
	
	.fsm.c:
		@$(FSM) $(FSM_FLAGS) $< > fsmout 
	
	.fsm.h:
		@$(FSM) $(FSM_FLAGS) $< > fsmout 
	
	.fsm.html:
		@$(FSM) $(FSM_HTML_FLAGS) -th $< > fsmout 
	
	.fsm.plantuml:
		@$(FSM) $(FSM_PLANTUML_FLAGS) -tp $< > fsmout 

	.fsm.rst:
		@$(FSM) $(FSM_RST_FLAGS) -tr $< > fsmout

These rules assume the including Makefile has defined appropriate make variables giving any
desired command line options.

Separate flags are expected for each type of FSMLang output:

* FSM_FLAGS for C output (FLAGS *must* contain -tc, -ts, -te or no -t option)
* FSM_HTML_FLAGS for HTML output (the FLAGS do *not* need to contain -th)
* FSM_PLANTUML_FLAGS for PlantUML output (the FLAGS do *not* need to contain -tp)
* FSM_RST_FLAGS for ReStructuredText output (the FLAGS do *not* need to contain -tr)

Of course, supporting files also need to be included in the build.
By adopting a naming convention for action files (in the FSMLang development tree, all such file 
names contain '-actions'), the including Makefile can easily provide the full list of object
files needed.

-------------------------------
The simpleCommunicator Makefile
-------------------------------

To put this all together, we'll build and test the simple communicator.

The Makefile begins by supplying the pieces needed by the material above:

.. code-block:: make

	##########################################################
	#
	#  Makefile for the simpleCommunicator FSMLang example
	#
	
	SRC = $(wildcard *actions.c)
	FSM_SRC = $(wildcard *.fsm)
	
	FSM_FLAGS=-tc --generate-weak-fns=false
	
	CFLAGS=-DSIMPLE_COMMUNICATOR_DEBUG
	
Then, after including the C-language relevant portions of the above material, we have the
stuff specific to our test project:

.. code-block:: make

	OBJS=$(SRC:.c=.o) $(GENERATED_SRC:.c=.o)
	
	all: simpleCommunicator
	
	test.out: simpleCommunicator
		./$< >$@
	
	simpleCommunicator: $(OBJS) $(FSM) Makefile
		$(CC) -o $@ $(LDFLAGS) $(OBJS)
	
	clean: cleanfsm
		@-rm -f $(OBJS)            2> /dev/null
		@-rm -f simpleCommunicator 2> /dev/null
		@-rm -f test.out           2> /dev/null
	
	$(SRC): simpleCommunicator_priv.h

The action functions we will use are "strong" versions of the weak functions which FSMLang would
have created:

.. code-block:: c

	SIMPLE_COMMUNICATOR_EVENT UFMN(sendMessage)(FSM_TYPE_PTR pfsm)
	{
	   DBG_PRINTF("%s", __func__);
	   (void) pfsm;
	   return THIS(noEvent);
	}
	
	SIMPLE_COMMUNICATOR_EVENT UFMN(queueMessage)(FSM_TYPE_PTR pfsm)
	{
	   DBG_PRINTF("%s", __func__);
	   (void) pfsm;
	   return THIS(noEvent);
	}
	
	SIMPLE_COMMUNICATOR_EVENT UFMN(checkQueue)(FSM_TYPE_PTR pfsm)
	{
	   DBG_PRINTF("%s", __func__);
	   (void) pfsm;
	   return THIS(noEvent);
	}
	
	SIMPLE_COMMUNICATOR_EVENT UFMN(noAction)(FSM_TYPE_PTR pfsm)
	{
	   DBG_PRINTF("%s", __func__);
	   (void) pfsm;
	   return THIS(noEvent);
	}

And, our "test" routine simply calls our machine with its two events:

.. code-block:: c

	int main(void)
	{
	
	   run_simpleCommunicator(THIS(SEND_MESSAGE));
	   run_simpleCommunicator(THIS(ACK));
	
	   return 0;
	}

Executing

``make test.out``

creates:

.. code-block:: text

	event: simpleCommunicator_SEND_MESSAGE; state: simpleCommunicator_IDLE
	simpleCommunicator_sendMessage
	event: simpleCommunicator_ACK; state: simpleCommunicator_AWAITING_ACK
	simpleCommunicator_checkQueue

The first and third lines of the output are generated from the built-in debug messaging; the
second and fourth are the output of the action functions. Inspecting this output, we find that
it is what is expected from the event sequence executed.

