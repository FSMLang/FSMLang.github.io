======================
States and Transitions
======================

States can have entry and exit functions. They can inhibit the running of sub-machines.  Additionally, a machine can have a
transition function which will be called on every state transitions.  Finally, state transitions can be specifed by explicit
declaration, or they can be done through a function.  To explore these topics, we'll continue to build on the hsmCommunicator,
starting where we left it at the end of the :doc:`../Data/eventdata` discussion.

.. toctree::
	:maxdepth: 2

	stateentryexit
	transitionfns

	Source/sourceindex
