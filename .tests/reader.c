typedef int sem;
sem mutex = 1;
sem db = 1; // Check databes accessible
int rc;     // Active reader count

void reader() {
    while(1) {
        down(&mutex); // LOCK       
        rc = rc + 1;
        if (rc == 1) down(&db); // One reader, can access the database
        up(&mutex);   // UNLOCK
        read();
        down(&mutex); // LOCK
        rc = rc - 1;
        if (rc == 0) up(&db); // No readers last reader, locks it
        up(&mutex); // UNLOCK
        use();
     }
}

void writer() {
    while(1) {
        dat();
        down(&db); // Access
        put(); 
        up(&db);   // Leave
    }
}
