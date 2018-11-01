#ifndef MY_SJF_LIB_H
#define MY_SJF_LIB_H

#include "schedule.h"

void update_waiting_in_rq_values();
void update_goodness(long double min_Exp_Burst,long double  max_WaitingInRQ);
struct task_struct *find_min_goodness();

#endif
