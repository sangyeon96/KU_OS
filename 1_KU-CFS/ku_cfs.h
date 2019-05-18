#include <stdio.h>
#include <stdlib.h> /* for atoi */
#include <signal.h> /* for signal */
#include <string.h> /* for memset */
#include <sys/time.h> /* for setitimer */
#include <unistd.h> /* for pause */

typedef struct tagNode {
	double vruntime;
	double DeltaExec;
	double weight;
	int count;
	pid_t pid;
	struct tagNode* NextNode;
}Node;

struct sigaction act;
struct itimerval timer;

Node* create_node(double vruntime, double DeltaExec, double weight, int count, pid_t pid);
void destroy_node(Node* Node);
void append_node(Node** Head, Node* NewNode);
void remove_node(Node** Head, Node* Remove);
Node* get_node_at(Node* Head, int Location);
int get_node_count(Node* Head);
void sort_linkedlist(Node* Head);
void time_handler(int signum);