==============================
Usage and Command Line Options
==============================



.. option:: Usage : /mnt/c/GitHub/FSMLang2/linux/fsm [-tc|s|e|h|p|r] [-o outfile] <filename>.fsm

.. option:: 'c'

	gets you c code output based on an event/state table,



.. option:: 's'

	gets you c code output with individual state functions using switch constructions,



.. option:: 'e'

	gets you c code output with a table of functions for each event using switch constructions,



.. option:: 'h'

	gets you html output



.. option:: 'p'

	gets you PlantUML output



.. option:: 'r'

	gets you reStructuredText output



.. option:: -i0

	inhibits the creation of a machine instance	any other argument to 'i' allows the creation of an instance;	this is the default



.. option:: -c

	will create a more compact event/state table when -tc is used	with machines having actions which return states



.. option:: -s

	prints some useful statistics and exits

.. option:: -o <outfile>

	will use <outfile> as the filename for the top-level machine output.	Any sub-machines will be put into files based on the sub-machine names.



.. option:: --generate-weak-fns=false

	suppresses the generation of weak function stubs.

.. option:: --short-user-fn-names=true

	causes user functions (such as action functions to use only the	machine name when the sub-machine depth is 1).

.. option:: --force-generation-of-event-passing-actions

	forces the generation of actions which pass events	when weak function generation is disabled.	The generated functions are not weak.



.. option:: --core-logging-only=true

	suppresses the generation of debug log messages in all but the core FSM function.

.. option:: --generate-run-function<=true|false>

	this option is deprecated.  The run function is always generated;	no RUN_STATE_MACHINE macro is provided.



.. option:: --include-svg-img<=*true|false>

	adds <img/> tag referencing <filename>.svg to include an image at	the top of the web page.



.. option:: --css-content-internal=true

	puts the CSS directly into the html.

.. option:: --css-content-filename=<filename>

	uses the named file for the css citation, or	for the content copy.



.. option:: --add-plantuml-title=<*true|false>

	adds the machine name as a title to the plantuml.

.. option:: --add-plantuml-legend=<*center|left|right|top|*bottm>

	adds a legend to the plantuml.	Center, bottom are the defaults.  Horizontal and vertial parameters can be added in a quoted string.	Center is a horizontal parameter.	By default, event, state, and action lists are	included in the legend, and event descriptions are removed	from the body of the diagram.



.. option:: --exclude-states-from-plantuml-legend=<*true|false>

	excludes state information from the plantuml legend.	When excluded from legend, state comments are included in the diagram body.



.. option:: --exclude-events-from-plantuml-legend=<*true|false>

	excludes event information from the plantuml legend.

.. option:: --exclude-actions-from-plantuml-legend=<*true|false>

	excludes action information from the plantuml legend.

.. option:: --convenience-macros-in-public-header[=<*true|false>]

	includes convenience macros	(THIS, UFMN, e.g.) in the public header of the top-level machine;	otherwise, they are placed in the private header.



.. option:: --add-machine-name

	adds the machine name when using the --short-debug-names option

.. option:: --add-event-cross-reference<=true|*false>

	adds a cross-reference list as a comment block	in front of the machine event enumeration. Omitting the optional argument is equivalent	to specifying "true"



.. option:: --event-cross-ref-only<=*true|false>

	creates a cross-reference list as a separate file.	When the format is not specified by --event-cross-ref-format, json is provided.	The file created is <filename>.[json|csv|tab|xml]



.. option:: --event-cross-ref-format=[json|csv|tab|xml]

	specifies the output format for --event-cross-ref-only.	Specifying this option obviates --event-cross-ref-only.



.. option:: --include-state-cross-refs=<*true|false>

	Includes cross reference for states.	Top-level states are placed in the same file as the events; sub-machines each get their own files.



.. option:: --add-plantuml-prefix-string=<text>

	will add the specified text to the plantuml output before	any generated output.  This option can be specified multiple times; all text will be	added in the order given	for the content copy.



.. option:: --add-plantuml-prefix-file=<text>

	will add the text in the specified file	to the plantuml output before any generated output.	This option can be specified multiple times; all text will be	added in the order given	for the content copy.



.. option:: -M

	prints the file name(s) of the source files that would have been created to stdout.	This is useful in Makefiles for getting the list of files	that will be generated 	(e.g. GENERATED_SRC=$(shell $(FSM) -M -tc $(FSM_SRC))).	This option must preceed the -t option.



.. option:: -Mh

	prints the file name(s) of the headers that would have been created to stdout.	This is useful in Makefiles for getting the list of files	that will be generated 	(e.g. GENERATED_HDRS=$(shell $(FSM) -M -tc $(FSM_SRC))).	This option must preceed the -t option.  And, only tc or ts are applicable.



.. option:: -Md

	print a line suitable for inclusion in a Makefile giving the recipe for	creating dependent files.	This option must preceed the -t option.



.. option:: --add-profiling-macros<=true|*false>

	adds profiling macros at the beginning	and end of the FSM function, and before and after invocation of action functions.



.. option:: --profile-sub-fsms<=true|*false>

	adds profiling macros at the beginning	and end of the FSM function in sub-machines.  Profiling macros	must also be enabled.



.. option:: --empty-cell-fn=<name>

	designates a function to be called when	an event/state cell is empty.



.. option:: --inhibiting-states-share-events<=*true|false>

	When true, events are shared to	sub-machines even in states which inhibit them.  The default is to not share.	This option allows sharing behavior of version 1.45.1 and before to be preserved.



.. option:: -v

	prints the version and exits

