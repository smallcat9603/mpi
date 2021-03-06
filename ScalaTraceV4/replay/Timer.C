#include "Timer.h"

extern int doTestFor;

Timer::Timer():
leftover(0),
timestamp(0),
cur_event(NULL),
prev_event(NULL),
accumulated(0),
compute(NULL)
{}

Timer::Timer(compute_t* _compute):
leftover(0),
timestamp(0),
cur_event(NULL),
prev_event(NULL),
accumulated(0),
compute(_compute)
{}


Timer::~Timer(){}

long long int Timer::getCurrentTime(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

int Timer::busyWait(int sleep_duration){
	struct timeval param, begin, now, rem;

	param.tv_sec = sleep_duration/1000000;
	param.tv_usec = sleep_duration%1000000;
	gettimeofday (&begin, NULL);
	timeradd (&begin, &param, &param);
	rem.tv_sec = 0;
	rem.tv_usec = 0;
	while (rem.tv_sec >= 0) {
		if(compute)
			( *(compute->func) )( compute->param );
		gettimeofday (&now, NULL);
		timersub (&param, &now, &rem);
	}
	
	return rem.tv_sec * 1000000 + rem.tv_usec;
}

void Timer::setCurrentEvent(Event *event){
	prev_event = cur_event;
	cur_event = event;
}

void Timer::setTime(){
	timestamp = getCurrentTime();
}

void Timer::resetTime(){
	timestamp = 0;
}

void Timer::maxTime() {
	timestamp = LLONG_MAX;
}

long long int Timer::getTime(){
	long long int t = getCurrentTime();
	return t - timestamp;
}

int Timer::getCompTime(){
	StackSig* prev_sig = NULL;
	if(prev_event)
		prev_sig = prev_event->getSignature();
	return (int) cur_event->getStatValue(PHASE_COMP, STAT_TIME, prev_sig);
}

int Timer::getCommTime(){
	StackSig* prev_sig = NULL;
	if(prev_event)
		prev_sig = prev_event->getSignature();
	return (int) cur_event->getStatValue(PHASE_COMM, STAT_TIME, prev_sig);
}


extern int my_rank;
int comp_time;

void Timer::simulateComputeTime(bool doComm, compute_t *_exec) {
	int sleep_duration = 0;
	comp_time = sleep_duration = getCompTime();
	if(doComm){
		sleep_duration += getCommTime();
	}
	sleep_duration -= getTime ();
	sleep_duration += leftover;
	/*if(my_rank == 0)
		cout<<"compu: "<<comp_time<<" commt: "<<getCommTime()<<" getti: "<<getTime()<<" left: "<< leftover<<"\n";*/

	/*if(my_rank == doTestFor){
		cout << comp_time << "   " << sleep_duration << endl;
	}*/

	if(sleep_duration > 0 && _exec){
		( *(_exec->func) )( _exec->param );
		sleep_duration = getCompTime();
		if(doComm){
			sleep_duration += getCommTime();
		}
		sleep_duration -= getTime();
		sleep_duration += leftover;
	}
	if(my_rank == doTestFor){
	}
	leftover = 0;

	if (sleep_duration > 0){
		leftover = busyWait(sleep_duration);
	} else {
	    leftover = sleep_duration;
	}
}

