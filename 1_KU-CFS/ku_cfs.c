#include "ku_cfs.h"

int main(int argc, char* argv[]) {
	Node* List    = NULL;
	Node* Current = NULL; //for printing linkedlist
	Node* Selected = NULL; //for getting head of the linkedlist
	Node* NewNode = NULL;

	char alphabet[2];
	alphabet[0] = 65;
	alphabet[1] = '\0';

	int i, process_num, process_index;
	int process_total = 0;

	int timeslice = 1;
	int running_timeslice = atoi(argv[6])*timeslice;

	pid_t kill_pid;
	int kill_count = 0;

	if(argc != 7){
		printf("ku_cfs: Wrong number of arguments\n");
		exit(1);
	}
	
	for(i=1; i<6; i++) {
		process_num = atoi(argv[i]);
		process_total += process_num;
		//printf("Process Total Number : %d\n", process_total);
		process_index = 0;
		pid_t pids[process_num], pid;
		while(process_index < process_num) {
			pids[process_index] = fork();
			if(pids[process_index] < 0) {
				fprintf(stderr, "fork failed\n");
				exit(1);
			} else if(pids[process_index] == 0) { //child
				//printf("child %d\n", getpid());
				execl("./ku_app", "ku_app" , alphabet, (char*) NULL);
			} else { //parent
				//printf("parent %d, child %d\n", getpid(), pids[process_index]);
				switch(i) { //Initialize vruntime, DeltaExec, weight, count, pid
					case 1:
						NewNode = create_node(0, 0, 0.64, 0, pids[process_index]);
						break;
					case 2:
						NewNode = create_node(0, 0, 0.8, 0, pids[process_index]);
						break;
					case 3:
						NewNode = create_node(0, 0, 1, 0, pids[process_index]);
						break;
					case 4:
						NewNode = create_node(0, 0, 1.25, 0, pids[process_index]);
						break;
					case 5:
						NewNode = create_node(0, 0, 1.5625, 0, pids[process_index]);
						break;
				}
				append_node(&List, NewNode);
			}
			alphabet[0] += 1;
			process_index++;
		} //while loop end
	} //for loop end

	sleep(5);

	void time_handler(int signum) {
		sort_linkedlist(List);

		/* print_linkedlist
		int Count = get_node_count(List);
		for (int j = 0; j<Count; j++) {
		    Current = get_node_at(List, j);
		    printf("List[%d] : %lf %lf %lf %d %d\n", j, Current->vruntime, Current->DeltaExec, Current->weight, Current->count, Current->pid);
		}*/
		
		Selected = get_node_at(List, 0);	
		kill(Selected->pid, SIGCONT);

		Selected->DeltaExec = timeslice;
		Selected->vruntime = Selected->vruntime + Selected->DeltaExec * Selected->weight;
		Selected->DeltaExec = 0; // Initialize DeltaExec
		Selected->count += 1;

		sleep(1);
		if(Selected->count == running_timeslice) { //problem: if CTRL+Z, there's zombie process. At the end, exit(0) kills all though.
			kill_pid = Selected->pid;
			remove_node(&List, Selected);
			destroy_node(Selected);
			kill(kill_pid, SIGKILL);
			kill_count += 1;
			printf("kill_pid : %d\n", kill_pid);
			if(kill_count == process_total)
				exit(0);
		}
		else
			kill(Selected->pid, SIGSTOP);
	}

	memset(&act, 0, sizeof(act));
	act.sa_handler = &time_handler;
	sigaction(SIGALRM, &act, NULL);

	timer.it_value.tv_sec = timeslice;
	timer.it_value.tv_usec = 0;
	timer.it_interval = timer.it_value;

	if(setitimer(ITIMER_REAL, &timer, NULL) == -1) { //setitimer executes time_handler per timer
		perror("ERROR calling setitimer()");
	    	exit(1);
	}

	while(1) {
		pause();
	}

	return 0;
}

Node* create_node(double vruntime, double DeltaExec, double weight, int count, pid_t pid) {
	Node* NewNode = (Node*)malloc(sizeof(Node));

	NewNode->vruntime = vruntime;
	NewNode->DeltaExec = DeltaExec;
	NewNode->weight = weight;
	NewNode->count = count;
	NewNode->pid = pid;
	NewNode->NextNode = NULL;

	return NewNode;
}

void destroy_node(Node* Node) {
	free(Node);
}

void append_node(Node** Head, Node* NewNode) {
	if ((*Head) == NULL) {        
		*Head = NewNode;
	} else {
		Node* Tail = (*Head);
		while (Tail->NextNode != NULL) {
			Tail = Tail->NextNode;
		}
		Tail->NextNode = NewNode;
	}
}

void remove_node(Node** Head, Node* Remove) {
	if (*Head == Remove) {
	*Head = Remove->NextNode;
	} else {
		Node* Current = *Head;
		while (Current != NULL && Current->NextNode != Remove) {
			Current = Current->NextNode;
		}
		if (Current != NULL)
			Current->NextNode = Remove->NextNode;
	}
}

Node* get_node_at(Node* Head, int Location) {
	Node* Current = Head;
	while (Current != NULL && (--Location) >= 0) {
		Current = Current->NextNode;
	}
	return Current;
}

int get_node_count(Node* Head) {
	int Count = 0;
	Node* Current = Head;

	while (Current != NULL) {
		Current = Current->NextNode;
		Count++;
	}
	return Count;
}

void sort_linkedlist(Node* Head) {
	Node *i, *j;
	double tmp_vruntime;
	double tmp_weight;
	int tmp_count;
	pid_t tmp_pid;

	for(i=Head; i->NextNode != NULL; i=i->NextNode) {
		for(j=i->NextNode; j!= NULL; j=j->NextNode) {
			if(i->vruntime > j->vruntime) {
				tmp_vruntime = i->vruntime;
				i->vruntime = j->vruntime;
				j->vruntime = tmp_vruntime;

				tmp_weight = i->weight;
				i->weight = j->weight;
				j->weight = tmp_weight;

				tmp_count = i->count;
				i->count = j->count;
				j->count = tmp_count;

				tmp_pid = i->pid;
				i->pid = j->pid;
				j->pid = tmp_pid;
			}
		}
	}
}
