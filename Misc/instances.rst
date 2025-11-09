.. role:: fsmlang(code)
	:language: fsmlang

=========
Instances
=========

Generally, only one instance of an FSM is needed, so, by default, FSMLang creates only one.  However, it is possible to create none or
multiple instances using :code:`-i[u]` where u is a non-negative integer.

When Hierarchical State Machine support was introduced, this facility was abandoned; the HSM design did not provide for multiple instances
of sub-machines.  This problem was able to be left unsolved only due to the prevelance of single instance invocations.  Implementations
of data translation functions simply used the single instance of the sub-machine generated under these circumstances. The resolution is
described in `A Single Instance`_, below.

The creation of three instances both using :code:`-i3` and :code:`-i0` has been tested.

-----------------
A Single Instance
-----------------

As mentioned above, prior to FSMLang v2.0, the single instance for hierarchical machines had the limitation that data translation
functions needed to be aware of the single instance created for each sub-machine. This was facilitated by publishing that instance in
the private header of each sub-machine.  Starting with v2.0, however, the signature of the data translation functions contains a pointer
to the sub-machine, even in the single-instance case.

The machine pointer is declared as a constant pointer to void, in order to have a common function typedef encompassing all sub-machines.
A new convenience macro, :code:`DECLARE_INSTANCE` is provided to facilitate the declaration of a local pointer of the proper type.

The following example, from full_test110/sub-actions.c in the FSMLang repo illustrates this usage.

.. code-block:: c

	void test_fsm_sub_grab_parent_e1_data(pTEST_FSM_DATA pdata, const void *pfsm)
	{
	   DBG_PRINTF("%s", __func__);
	
	   DECLARE_INSTANCE(psub);
	
	   psub->data.sub_e1_count += pdata->e1_count;
	
	   DBG_PRINTF("sub e1 accumulator: %u", psub->data.sub_e1_count);
	}

	
:code:`DECLARE_INSTANCE` is defined as

.. code-block:: c

	#define DECLARE_INSTANCE(A) FSM_TYPE_PTR A = (FSM_TYPE_PTR) pfsm


------------------
Multiple Instances
------------------

The multiple instance usage is quite similar to the single instance case, with only the minimal required differences.

full_test123 provides a good example.  fsm is called with :code:`-i3`.

.. code-block:: fsmlang
	
   native
   {
   #include "test.h"
   }
   
   machine test_fsm
      actions return events;
      native impl
      {
      #define INIT_FSM_DATA0 { .e1_count = 0 }
      #define INIT_FSM_DATA1 { .e1_count = 100 }
      #define INIT_FSM_DATA2 { .e1_count = 1000 }
      }
   {
      data
      {
         int e1_count;
      }
   
      event e1 data translator grab_e1_data {int i;};
   
      state s1;
   
      machine sub
         actions return events;
         native impl
         {
         #define INIT_FSM_DATA0 { .sub_e1_count = 0 }
         #define INIT_FSM_DATA1 { .sub_e1_count = 100 }
         #define INIT_FSM_DATA2 { .sub_e1_count = 1000 }
         }
      {
         data
         {
            int sub_e1_count;
         }
   
         event parent::e1 data translator grab_parent_e1_data;
         state s1;
   
         machine subSub
            actions return events;
            native impl
            {
            #define INIT_FSM_DATA0 { .subSub_e1_count = 0 }
            #define INIT_FSM_DATA1 { .subSub_e1_count = 100 }
            #define INIT_FSM_DATA2 { .subSub_e1_count = 1000 }
            }
         {
            data
            {
               int subSub_e1_count;
            }
      
            event parent::e1 data translator grab_parent_e1_data;
            state s1;
      
            action do_nothing[e1, s1];
         }
   
         action do_nothing[e1, s1];
      }
   
      machine sub2
         actions return events;
         native impl
         {
         #define INIT_FSM_DATA0 { .sub2_e1_count = 0 }
         #define INIT_FSM_DATA1 { .sub2_e1_count = 100 }
         #define INIT_FSM_DATA2 { .sub2_e1_count = 1000 }
         }
      {
         data
         {
            int sub2_e1_count;
         }
   
         event parent::e1 data translator grab_parent_e1_data;
         state s1;
   
         machine subSub
            actions return events;
            native impl
            {
            #define INIT_FSM_DATA0 { .subSub_e1_count = 0 }
            #define INIT_FSM_DATA1 { .subSub_e1_count = 100 }
            #define INIT_FSM_DATA2 { .subSub_e1_count = 1000 }
            }
         {
            data
            {
               int subSub_e1_count;
            }
      
            event parent::e1 data translator grab_parent_e1_data;
            state s1;
      
            action do_nothing[e1, s1];
         }
   
         action do_nothing[e1, s1];
      }
   
      action handle_e1[e1, s1];
   
   }

In anticipation of three instances being created, three data initialization macros are defined.  If desired, the definition of these macros
can be moved to the compiler command line, leaving the .fsm file to be identical, no matter the number of instances created.

The top-level public header file created contains the declaration of three run functions:

.. code-block:: c

	void run_test_fsm_instance0(pTEST_FSM_EVENT);
	
	void run_test_fsm_instance1(pTEST_FSM_EVENT);
	
	void run_test_fsm_instance2(pTEST_FSM_EVENT);
	

The generated top-level source file contains the desired number of machine instances:

.. code-block:: c

	static TEST_FSM test_fsm_INSTANCE0 = {
	   .state = test_fsm_s1
	   , .event = THIS(e1)
	   , .actionArray = &test_fsm_action_array
	   , .subMachineArray = &test_fsm_sub_fsm_if_array
	   , .data = INIT_FSM_DATA0
	   , .instance = 0
	   , .fsm = test_fsmFSM
	   };
	static TEST_FSM test_fsm_INSTANCE1 = {
	   .state = test_fsm_s1
	   , .event = THIS(e1)
	   , .actionArray = &test_fsm_action_array
	   , .subMachineArray = &test_fsm_sub_fsm_if_array
	   , .data = INIT_FSM_DATA1
	   , .instance = 1
	   , .fsm = test_fsmFSM
	   };
	static TEST_FSM test_fsm_INSTANCE2 = {
	   .state = test_fsm_s1
	   , .event = THIS(e1)
	   , .actionArray = &test_fsm_action_array
	   , .subMachineArray = &test_fsm_sub_fsm_if_array
	   , .data = INIT_FSM_DATA2
	   , .instance = 2
	   , .fsm = test_fsmFSM
	   };
	void run_test_fsm_instance0(pTEST_FSM_EVENT e)
	{
	   pTEST_FSM ptest_fsm = &test_fsm_INSTANCE0;
	      ptest_fsm->fsm(ptest_fsm,e);
	}
	
	void run_test_fsm_instance1(pTEST_FSM_EVENT e)
	{
	   pTEST_FSM ptest_fsm = &test_fsm_INSTANCE1;
	      ptest_fsm->fsm(ptest_fsm,e);
	}
	
	void run_test_fsm_instance2(pTEST_FSM_EVENT e)
	{
	   pTEST_FSM ptest_fsm = &test_fsm_INSTANCE2;
	      ptest_fsm->fsm(ptest_fsm,e);
	}

Generated sub-machine sources create an array of pointers to their instances, to facilitate the selection of the correct sub-machine
instance by the parent machine.

.. code-block:: c

	static SUB sub_INSTANCE0 = {
	   .state = sub_s1
	   , .event = THIS(e1)
	   , .actionArray = &sub_action_array
	   , .subMachineArray = &sub_sub_fsm_if_array
	   , .data = INIT_FSM_DATA0
	   , .instance = 0
	   , .fsm = subFSM
	   };
	static SUB sub_INSTANCE1 = {
	   .state = sub_s1
	   , .event = THIS(e1)
	   , .actionArray = &sub_action_array
	   , .subMachineArray = &sub_sub_fsm_if_array
	   , .data = INIT_FSM_DATA1
	   , .instance = 1
	   , .fsm = subFSM
	   };
	static SUB sub_INSTANCE2 = {
	   .state = sub_s1
	   , .event = THIS(e1)
	   , .actionArray = &sub_action_array
	   , .subMachineArray = &sub_sub_fsm_if_array
	   , .data = INIT_FSM_DATA2
	   , .instance = 2
	   , .fsm = subFSM
	   };
	static void * const sub_INSTANCES[test_fsm_numInstances] = {
	     &sub_INSTANCE0
	   , &sub_INSTANCE1
	   , &sub_INSTANCE2
	};

A pointer to this sub-machine instance array is included in the sub-machine interface structure:

.. code-block:: c
	:emphasize-lines: 8

	typedef TEST_FSM_EVENT_ENUM (*TEST_FSM_SUB_MACHINE_FN)(const void*,TEST_FSM_EVENT_ENUM);
	typedef struct _test_fsm_sub_fsm_if_ TEST_FSM_SUB_FSM_IF, *pTEST_FSM_SUB_FSM_IF;
	struct _test_fsm_sub_fsm_if_
	{
	   TEST_FSM_EVENT_ENUM      first_event;
	   TEST_FSM_EVENT_ENUM      last_event;
	   TEST_FSM_SUB_MACHINE_FN   const subFSM;
	   void *   const             (*instanceArray)[test_fsm_numInstances];
	};

A parent machine uses this array to refer to the desired sub-machine instance, using the parent's own instance member:

.. code-block:: c
	:emphasize-lines: 13, 14

	static TEST_FSM_EVENT_ENUM findAndRunSubMachine(pTEST_FSM pfsm, TEST_FSM_EVENT_ENUM e)
	{
	   for (TEST_FSM_SUB_MACHINES machineIterator = THIS(firstSubMachine);
	        machineIterator < THIS(numSubMachines);
	        machineIterator++
	       )
	   {
	         if (
	            ((*pfsm->subMachineArray)[machineIterator]->first_event <= e)
	            && ((*pfsm->subMachineArray)[machineIterator]->last_event > e)
	             )
	         {
	            const void * pinstance =  (*((*pfsm->subMachineArray)[machineIterator])->instanceArray)[pfsm->instance];
	            return ((*(*pfsm->subMachineArray)[machineIterator]->subFSM)(pinstance, e));
	         }
	   }
	
	   return THIS(noEvent);
	
	}

As in the single-instance case, when sharing events, any data translation functions are given a pointer to the sub-machine instance.

.. code-block:: c
	:emphasize-lines: 9, 11, 12

	TEST_FSM_EVENT_ENUM test_fsm_pass_shared_event(pTEST_FSM pfsm, pTEST_FSM_SHARED_EVENT_STR sharer_list[])
	{
	   TEST_FSM_EVENT_ENUM return_event = THIS(noEvent);
	
	   for (pTEST_FSM_SHARED_EVENT_STR *pcurrent_sharer = sharer_list;
	        *pcurrent_sharer && return_event == THIS(noEvent);
	        pcurrent_sharer++)
	   {
	      const void * pinstance =  (*(*pcurrent_sharer)->psub_fsm_if->instanceArray)[pfsm->instance];
	      if ((*pcurrent_sharer)->data_translation_fn)
	         (*(*pcurrent_sharer)->data_translation_fn)(&pfsm->data, pinstance);
	      return_event = (*(*pcurrent_sharer)->psub_fsm_if->subFSM)(pinstance, (*pcurrent_sharer)->event);
	   }
	
	   return return_event;
	}


------------
No Instances
------------

One use case for the "no instance" facility would be a need to place instances into separate memory regions.  Macros are provided
to declare instances.  Because of this, many structures and functions which would otherwise be hidden are exposed.  New headers,
<file_name>_instance_macros.h, are created to contain most of the macro definitions.  The top-level header contains the top-level
macro.

The code examples are taken from full_test126.

.. code-block:: fsmlang
	:caption: test_fsm.fsm

	native
	{
	#include "test.h"
	}
	
	machine test_fsm
	{
	   data
	   {
	      int e1_count;
	      char *name;
	   }
	
	   event e1 data translator grab_e1_data {int i;};
	
	   state s1;
	
	   machine sub
	   {
	      data
	      {
	         int sub_e1_count;
	      }
	
	      event parent::e1 data translator grab_parent_e1_data;
	      state s1;
	
	      machine subSub
	      {
	         data
	         {
	            int subSub_e1_count;
	         }
	   
	         event parent::e1 data translator grab_parent_e1_data;
	         state s1;
	   
	         action do_nothing[e1, s1];
	      }
	
	      action do_nothing[e1, s1];
	   }
	
	   machine sub2
	   {
	      data
	      {
	         int sub2_e1_count;
	      }
	
	      event parent::e1 data translator grab_parent_e1_data;
	      state s1;
	
	      machine subSub
	      {
	         data
	         {
	            int subSub_e1_count;
	         }
	   
	         event parent::e1 data translator grab_parent_e1_data;
	         state s1;
	   
	         action do_nothing[e1, s1];
	      }
	
	      action do_nothing[e1, s1];
	   }
	
	   action handle_e1[e1, s1];
	
	}

When :code:`-i0` is used, the top-level public header contains the macro needed by the client code to create instances of the machine:

.. code-block:: c
	:caption: From test_fsm.h

	#include "test_fsm_instance_macros.h"
	#define TEST_FSM_INSTANCE(A, B, D_test_fsm_sub, D_test_fsm_sub_subSub, D_test_fsm_sub2, D_test_fsm_sub2_subSub)\
	TEST_FSM_SUB_INSTANCE(A, D_test_fsm_sub, D_test_fsm_sub_subSub)\
	TEST_FSM_SUB2_INSTANCE(A, D_test_fsm_sub2, D_test_fsm_sub2_subSub)\
	void * const A##_test_fsm_sma[test_fsm_numSubMachines] = {\
	     p##A##_sub   , p##A##_sub2};\
	static TEST_FSM A = {\
	   .state = test_fsm_s1\
	   , .event = test_fsm_e1\
	   , .statesArray = &test_fsm_state_fn_array\
	   , .fsm = test_fsmFSM\
	   , .subMachineArray = &test_fsm_sub_fsm_if_array\
	   , .subMachines = &A##_test_fsm_sma\
	   , .data = B\
	};\
	\
	static pTEST_FSM p##A = &(A)

This macro is invoked from the client code:

.. code-block:: c
	:caption: From test.c

	#define INIT_FSM_DATA { .e1_count = 0, .name="test_fsm" }
	#define INIT_FSM_DATA_SUB { .sub_e1_count = 0 }
	#define INIT_FSM_DATA_SUB2 { .sub2_e1_count = 0 }
	#define INIT_FSM_DATA_SUB_SUB { .subSub_e1_count = 0 }
	TEST_FSM_INSTANCE(test_fsm
	                  , INIT_FSM_DATA
	                  , INIT_FSM_DATA_SUB
	                  , INIT_FSM_DATA_SUB_SUB
	                  , INIT_FSM_DATA_SUB2
	                  , INIT_FSM_DATA_SUB_SUB
	                  );
	
	#define INIT1_FSM_DATA { .e1_count = 100, .name="test_fsm1" }
	#define INIT1_FSM_DATA_SUB { .sub_e1_count = 100 }
	#define INIT1_FSM_DATA_SUB2 { .sub2_e1_count = 100 }
	#define INIT1_FSM_DATA_SUB_SUB { .subSub_e1_count = 100 }
	TEST_FSM_INSTANCE(test_fsm1
	                  , INIT1_FSM_DATA
	                  , INIT1_FSM_DATA_SUB
	                  , INIT1_FSM_DATA_SUB_SUB
	                  , INIT1_FSM_DATA_SUB2
	                  , INIT1_FSM_DATA_SUB_SUB
	                  );
	
	#define INIT2_FSM_DATA { .e1_count = 1000, .name="test_fsm2" }
	#define INIT2_FSM_DATA_SUB { .sub_e1_count = 1000 }
	#define INIT2_FSM_DATA_SUB2 { .sub2_e1_count = 1000 }
	#define INIT2_FSM_DATA_SUB_SUB { .subSub_e1_count = 1000 }
	TEST_FSM_INSTANCE(test_fsm2
	                  , INIT2_FSM_DATA
	                  , INIT2_FSM_DATA_SUB
	                  , INIT2_FSM_DATA_SUB_SUB
	                  , INIT2_FSM_DATA_SUB2
	                  , INIT2_FSM_DATA_SUB_SUB
	                  );
	

The three instances of the machine created here are placed in the same memory region; moving them is an exercise left to the reader.  Note that
the naming scheme for the data initialization macros is no longer mandatory, since they are now arguments to a macro invocation.

The client code must also create any desired run functions.  This code models functions after the FSMLang generated *run* functions:

.. code-block:: c
	:caption: From test.c

	static void run_test_fsm(pTEST_FSM_EVENT pevent)
	{
	   ptest_fsm->fsm(ptest_fsm, pevent);
	}
	
	static void run_test_fsm1(pTEST_FSM_EVENT pevent)
	{
	   ptest_fsm1->fsm(ptest_fsm1, pevent);
	}
	
	static void run_test_fsm2(pTEST_FSM_EVENT pevent)
	{
	   ptest_fsm2->fsm(ptest_fsm2, pevent);
	}

