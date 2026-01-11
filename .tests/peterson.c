#define FALSE 0
#define TRUE 1
#define N 2 // Procs

int turn;
int interested[N]; // Procs that want to read, init to 0

void enter(int proc) {
    int other = 1 - proc;
     
    interested[proc] = TRUE; // Wants to read
    turn = proc;
    
    while (turn == proc && interested[other] == TRUE); // Busy wait, check again and again if both are interested to entered
}

void leave(int proc) {
    interested[proc] = FALSE; // Proc is not interested, will leave the region
}
