#include "my_sjf_lib.h"

extern struct runqueue *rq;
extern struct task_struct *current;

//update the waiting_in_rq values of every process using the current time
void update_waiting_in_rq_values(){
	struct task_struct *iter;

	iter = rq->head;
	while(iter->next != rq->head) {
		iter = iter->next;
		if(iter == current){
			// set 0 for the current process as it ran until now
			iter->waiting_in_rq = 0;
			continue;
		}
		iter->waiting_in_rq = sched_clock() - iter->last_time_runnable;
	}
}

//update goodness value for every process
void update_goodness(long double min_Exp_Burst,long double  max_WaitingInRQ){
	struct task_struct *iter;

	iter = rq->head;
	while(iter->next != rq->head) {
		iter = iter->next;
		if(iter == current){
			//we have already found the goodness value for the current process using the temporary value for next_expected_burst
			continue;
		}
		iter->goodness = ((1 + iter->next_expected_burst)/(1 + min_Exp_Burst)) * ((1 + max_WaitingInRQ)/(1 + iter->waiting_in_rq));
	}
}

// find minimum goodness value
struct task_struct *find_min_goodness(){
	struct task_struct *iter;
	struct task_struct *curr;
	long double min_goodness;

	// set the first process-node after init(which is head of the list) as the one having minimum goodness value
	iter = rq->head;
	curr = iter->next;
	min_goodness = iter->next->goodness;
	// traverse the runqueue and compare every process' goodness value with the one being currently the minimum one
	while(iter->next != rq->head) {
		iter = iter->next;
		if(iter->goodness < min_goodness){
			min_goodness = iter->goodness;
			curr = iter;
		}
	}

	return curr;
}
