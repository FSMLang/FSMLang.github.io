====================
The HSM Communicator
====================

The HSM Communicator illustrates the use of hierarchical state machines (HSMs).

Hierarchical State Machines are indicated whenever the process to be managed involves loosely connected, highly coherent, sub-processes.  Our
communications example here is one such situation.  Another might be the management of an external FLASH part through a QSPI interface.

.. admonition:: Real-life Example: Design Re-use
	:class: 'sidebar'

	A driver for an internal FLASH controller was developed using FSMLang.  The driver allowed writes to the flash from different parts of the
	code to be queued for sequential operations.  When it was desired to write to external FLASH as well, a sub-machine to manage the QSPI
	interaction was added to the existing state machine, with minimal adaption of the existing logic.

To be most useful, sub-machines should be thought of as sub-routines.  Just as sub-routines might be created for "readability," a sub-machine might
clarify an FSM design by encapsulating events which always go together.  And, just as with sub-routines, a sub-machine might encapsulate a process
that must be repeated in response to an event, regardless of what else might have been going on at the time.

Here, one thinks again, of a communications example: establishing a Bluetooth Low Energy (BLE) connection in a noisy environment, when that
connection involves some "higer-level" sequence of steps.  A sub-machine that knows how to recover from a signal loss at any time can help the
higher-level machine continue through the "higher-level" sequence essentially without regard to the signal loss.

.. toctree::
	:maxdepth: 2

	hsmexplained1
	hsmexplained2
	hsmexplained3
	hsmexplained4

	visualizing

	RST/hsmCommunicator

	Source/sourceindex

