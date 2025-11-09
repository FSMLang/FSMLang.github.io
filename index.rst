.. FSMLang documentation master file, created by
   sphinx-quickstart on Tue Apr 29 20:06:52 2025.

.. role:: fsmlang(code)
	:language: fsmlang

====================================
FSMLang: Better State Machine Design
====================================

.. admonition:: Compatibility Warning

	Version 2.0 was released in November of 2025.  The bump in the major version reflects an incompatibility in the generated
	API for data translation functions.  See :doc:`Misc/instances` for a complete description of the change and its motivation.

FSMLang is was created to allow design work in the problem space of finite state machines without the encumbrances of
any particular implementation language. Thus, FSMLang is implemented as a "pre-processor," generating code in any
desired general programming language to implement the described finite state machine. FSMLang allows effort to be
focused on the definition of events (including the sequences in which the events are expected), states, and transitions.
Indeed, though the action to be taken in any
particular event/state intersection is declarable, the actual definition of that action is treated as a
detail which falls outside the scope of FSMLang. Moreover, the mechanisms for collecting or determining events are
also outside the language scope. FSMLang creates an object or objects in a target programming language which, when
inserted into the larger program structure will invoke the correct actions and make the correct transitions for the
events handed it.

.. admonition:: "Any general programming language"
	:class: sidebar

	Presently, FSMLang only supports the C programming language.  It is left as an exercise to the reader to
	develop support for other languages.  The FSMLang maintainers will supply what help is needed.

The created state machine contains a single state variable, which should not be manipulated by any user-written
function. This variable is maintained on the heap, not on the machine's function call stack. This means that
the machine must not be called recursively; neither from within any action function, nor from separate threads of
execution. The keyword :fsmlang:`reentrant` can be used to designate machines which are called
from different execution threads. Macros will be inserted at the beginning and end of the state machine function
which are to be defined by the user to properly protect the machine from re-entrance.

Supporting the desire to keep FSMLang focused on machine design, provision is made to insert :fsmlang:`native` blocks
into the machine specification files.  These blocks are not interpreted by FSMLang, but are copied into the generated
source at different points.  The blocks also serve to reduce or eliminate the need to post-process the generated code.

Data can be specified for both machines and events.  States may have entry and exit functions.  Transitions may be done
to a specified state, or through the auspices of a transition function.  A machine may have a transition function called
when any state transition is made.  And, machines may have sub-machines (to any (keep it reasonable!) depth).

FSMLang supports designing with action functions that return events (the default), states, or nothing.
Returning events from action functions is a powerful technique for the machine to immediately feed events to itself,
simplifying, for example, the management of internal errors.  Imagine an action function which must allocate some memory to
complete its task; should that memory not be available, the function can simply exit, returning a "memory not available"
event; the state machine processing for this situation can be placed in another action function, and the machine can be
transitioned to an error state, should the situation be unrecoverable. The core FSM function loops, calling appropriate actions,
then making designated transitions, until an action function returns :fsmlang:`noEvent`.

Having said all of that, useful state machines can be designed with actions that return states or nothing.

The language syntax is intended to make the action the state machine will take in response to any input immediately
clear.  Nevertheless, a UML state chart or a simple HTML event-state action matrix are useful tools for state machine
visualization.  FSMLang can output both HTML and PlantUML to assist in visualizing and documenting the state machine.
For somewhat more formal documentation, ReStructuredText output is available, as well.

Both the language and this documentation are intended to assist in the design of useful state machines.  Feedback is
welcomed, both for language and for documentation improvements.  Use the *issue* mechanisms in either GitHub repo.

The repos are at `Language <https://github.com/FSMLang/FSMLang>`_
and `Documents <https://github.com/FSMLang/FSMLang.github.io>`_.

`Revision history <_static/release_notes.html>`_ is maintained for the curious, and for those who support medical device development tool validation.

This documentation presents language concepts primarily through two examples, :doc:`Simple/simpleindex`,
and :doc:`HSM/hsmindex`.  :doc:`Data/dataindex` shows how FSMLang handles data for both machines and events, and
:doc:`States/stateindex` rounds out the discussion of states.  The important topic of event sequences is taken up
in :doc:`Sequences/index`.

.. only:: html

	:download:`Download </_build/pdf/FSMLang.pdf>` this document as a pdf.

.. toctree::
	:hidden:
	:maxdepth: 2

	self
	Simple/simpleindex
	HSM/hsmindex
	Data/dataindex
	States/stateindex
	Sequences/index
	Misc/instances
	Misc/cloptions

.. only:: pdf

	.. footer::

		###Page### 


