#define N 100

// Via messages

void producer() {
    int item;
    message m;
    while(1) {
        item = produce_item();        
        receive(consumer, &m); // Wait for is_empty message
        build(&m, item);
        send(consumer, &m);
     }
}

void consumer() {
    int item;
    message m;
    
    for(i=0;i<N;i++) send(producer, &m); // Initial is_empty messages
    while(1) {
        receive(producer, &m);
        item = extract(&m); // Extract item from message
        send(producer, &m); // Send empty message            
        consume_item(item);
    }
}