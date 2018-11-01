/* schedule.c
 * This file contains the primary logic for the
 * scheduler.
 */
#include "schedule.h"
#include "macros.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "list.h"
#include "my_sjf_lib.h"

#define NEWTASKSLICE (NS_TO_JIFFIES(100000000))

#define A 0.5	//factor used in expected burst formula

/* Local Globals
 * rq - This is a pointer to the runqueue that the scheduler uses.
 * current - A pointer to the current running task.
 */
struct runqueue *rq;
struct task_struct *current;

/* External Globals
 * jiffies - A discrete unit of time used for scheduling.
 *			 There are HZ jiffies in a second, (HZ is
 *			 declared in macros.h), and is usually
 *			 1 or 10 milliseconds.
 */
extern long long jiffies;
extern struct task_struct *idle;

/*-----------------Initilization/Shutdown Code-------------------*/
/* This code is not used by the scheduler, but by the virtual machine
 * to setup and destroy the scheduler cleanly.
 */

 /* initscheduler
  * Sets up and allocates memory for the scheduler, as well
  * as sets initial values. This function should also
  * set the initial effective priority for the "seed" task
  * and enqueu it in the scheduler.
  * INPUT:
  * newrq - A pointer to an allocated rq to assign to your
  *			local rq.
  * seedTask - A pointer to a task to seed the scheduler and start
  * the simulation.
  */
void initschedule(struct runqueue *newrq, struct task_struct *seedTask)
{
	seedTask->next = seedTask->prev = seedTask;
	newrq->head = seedTask;
	newrq->nr_running++;
}

/* killschedule
 * This function should free any memory that
 * was allocated when setting up the runqueu.
 * It SHOULD NOT free the runqueue itself.
 */
void killschedule()
{
	return;
}


void print_rq () {
	struct task_struct *curr;

	printf("Rq: ");
	curr = rq->head;
	if (curr)
		printf("%p", curr);
	while(curr->next != rq->head) {
		curr = curr->next;
		printf(", %p", curr);
	};
	printf("\n");
}

/*-------------Scheduler Code Goes Below------------*/
/* This is the beginning of the actual scheduling logic */

/* schedule
 * Gets the next task in the queue
 */
void schedule()
{
	struct task_struct *curr;	//used in context switch
	struct task_struct *old_current;	//the value of the process that was running before the context switch (used for logistics)
	struct task_struct *iter;	//struct task_struct * variable used for run queue iteration purposes
	long double min_Exp_Burst = LDBL_MAX;	//smallest expected burst value of all the processes
	long double max_WaitingInRQ = 0;	//indicates the maximum time that a process waits in the runqueue
	long double temp_last_burst = 0, temp_next_expected_burst = 0;		//temporary values for the last burst and next expected burst of the current process
																		//supposing that the process will lose the cpu after the next context switch


	if (rq->nr_running == 1) {
		current->need_reschedule = 0;	/* Always make sure to reset that, in case *
										* we entered the scheduler because current*
										* had requested so by setting this flag   */
		context_switch(rq->head);	//context switch to init because there are no other processes in the runqueue
		return;
	}
	else{
		// keep the temporary values of last burst and next expected burst of the current process
		//supposing that the process will lose the cpu after the next context switch
		//if the current process is still active in the runqueue and has not been deactivated
		if( ((current->prev!=NULL && current->next!=NULL) && (current != rq -> head) && (current->need_reschedule))  ||  ((current->need_reschedule == 0)) ){
			// temporary last burst for the running process supposing that it will lose the processor
			temp_last_burst = sched_clock()-(current->start_time);
			// temporary next expected burst for the running process supposing that it will lose the processor
			temp_next_expected_burst = (temp_last_burst+(A * current->next_expected_burst))/(1+A);
			// temporarily set the min_Exp_Burst variable to the expected burst of the current process
			//in case we don't find another process with a better expected burst
			min_Exp_Burst = temp_next_expected_burst;
		}
		current->need_reschedule = 0;	/* Always make sure to reset that, in case *
										* we entered the scheduler because current*
										* had requested so by setting this flag   */

		update_waiting_in_rq_values();	//update the waiting in runqueue value for every process

		curr = current;	// keep the node address in case we don't find another process with better next_expected_burst value

		// traverse the list to find the process with the minimum Exp_Burst and the maximum WaitingInRQ
		iter = rq->head;
		while(iter->next != rq->head) {
			iter = iter->next;
			if(iter == current){
				//skip the current process because we want to use the temporary values and not the ones that are saved in the task_struct
				continue;
			}
			//find the process with the minimum next_expected_burst
			if(iter->next_expected_burst < min_Exp_Burst){
				min_Exp_Burst = iter->next_expected_burst;
				curr = iter;	// keep the node address in case we want to schedule using the expected burst
			}
			//find the process with the maximum waiting_in_rq
			if(iter->waiting_in_rq > max_WaitingInRQ){
				max_WaitingInRQ = iter->waiting_in_rq;
			}
		}

		//update the goodness for the current process using the temporary value for next_expected_burst
		current->goodness = ((1 + temp_next_expected_burst)/(1 + min_Exp_Burst)) * ((1 + max_WaitingInRQ)/(1 + current->waiting_in_rq));
		//update goodness value for every process
		update_goodness(min_Exp_Burst, max_WaitingInRQ);

		// comment the next line to use SJF with Exp_Burst for scheduling
		curr = find_min_goodness();		//find the process with the minimum goodness

		old_current = current;	// keep the value of the process that is running before the context switch in order to calculate the necessary logistics

		context_switch(curr);	//context switch to the next process that has been chosen

		if(old_current != current){
			if(sched_clock() > old_current->last_time_runnable){
				// save the last time that the process ran
				old_current->last_time_runnable = sched_clock();
			}
			// calculate the value of the last burst of the process that was running before the context switch
			old_current->last_burst = sched_clock()-(old_current->start_time);
			// calculate the value of the next expected burst of the process that was running before the context switch
			old_current->next_expected_burst = ((old_current->last_burst)+(A * old_current->next_expected_burst))/(1+A);
			// save the start time of the process that will run after the context switch
			curr->start_time = sched_clock();
		}
		return;
	}
}


/* sched_fork
 * Sets up schedule info for a newly forked task
 */
void sched_fork(struct task_struct *p)
{
	//initialization of all the extra values added to the task_struct
	p->start_time = 0;
 	p->last_burst = 0;
	p->next_expected_burst = 0;
	p->last_time_runnable = 0;
	p->waiting_in_rq = 0;
	p->goodness = 0;

	p->time_slice = 100;
}

/* scheduler_tick
 * Updates information and priority
 * for the task that is currently running.
 */
void scheduler_tick(struct task_struct *p)
{
	schedule();
}

/* wake_up_new_task
 * Prepares information for a task
 * that is waking up for the first time
 * (being created). */
void wake_up_new_task(struct task_struct *p)
{
	p->next = rq->head->next;
	p->prev = rq->head;
	p->next->prev = p;
	p->prev->next = p;

	rq->nr_running++;

	if(sched_clock() > p->last_time_runnable){
		// save the last time that the process entered the runqueue
		p->last_time_runnable = sched_clock();
	}
}

/* activate_task
 * Activates a task that is being woken-up
 * from sleeping.
 */
void activate_task(struct task_struct *p)
{
	p->next = rq->head->next;
	p->prev = rq->head;
	p->next->prev = p;
	p->prev->next = p;

	rq->nr_running++;
	if(sched_clock() > p->last_time_runnable){
		// save the last time that the process entered the runqueue
		p->last_time_runnable = sched_clock();
	}
}

/* deactivate_task
 * Removes a running task from the scheduler to
 * put it to sleep.
 */
void deactivate_task(struct task_struct *p)
{
	p->prev->next = p->next;
	p->next->prev = p->prev;
	p->next = p->prev = NULL; /* Make sure to set them to NULL *
							   * next is checked in cpu.c      */

	rq->nr_running--;
}
