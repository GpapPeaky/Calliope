#define N 100 // Element count
typedef int sem;

sem mutex = 1; // Mutual exlcusion
sem empty = N; // Empty, no items
sem full = 0;  // Empty, no items

void producer() {
    int item;
    while (1) {
        item = produce_item();
        down(&empty);
        down(&mutex); // Enter region, block any other procs when inserting items
        
        insert(item);
        
        // Update
        up(&full);
        up(&mutex); // Leave region
    }
}

void consumer() {
    int item;
    while (1) {
        down(&full);   // Wait if empty
        down(&mutex);  // Enter region
        
        item = remove(); // Remove
        
        up(&mutex);    // leave       
        up(&empty);    // notify empty 
        
        consume(item);
    }
}