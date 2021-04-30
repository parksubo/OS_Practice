

// num of producer : 5, num of consumer : 2

#include <stdio.h>	
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define Q_SIZE 10	// size for circular queue
#define P_CNT 6		// num of producing (6*5 = 30)
#define C_CNT 15    // num of consuming (15*2 = 30)

/* variables for mutex */
pthread_cond_t buffers = PTHREAD_COND_INITIALIZER;	// condition variable for producing buffer
pthread_cond_t items = PTHREAD_COND_INITIALIZER; 	// condition variable for consuming items
pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER;	// mutex for producing

/* variables for circular queue */
int CQ[Q_SIZE];	// circular queue
int front = 0;
int rear = 0;
int CQ_cnt = 0;	// the size of CQ

int pc = 1;		// producer id
int cc = 1;		// consumer id

/* return 1 item for buffer */
int getQ() {
	int output = 99;
	if(CQ_cnt == 0) {
		printf("Queue is empty. \n");
		return -1;
	}
	else {
		output = CQ[rear];
		rear = (rear + 1) % 10;
		CQ_cnt--;
		return output;
	}
}

/* add input to buffer */
int addQ(int input) {
	if(CQ_cnt == 10) {
		printf("Queue is full .\n");
		return -1;
	}
	else {
		CQ[front] = input;
		front = (front + 1) % 10;
		CQ_cnt++;
		return 1;
	}
}

/* thread for producer */
void *producer(void *arg) {
	int i, id, input;

	id = pc++;
	for(i=0; i < P_CNT; i++) {
		input = random() % 100;
		usleep(input);
		printf("producer %d add Q %d\n", id, input);
		
		pthread_mutex_lock(&buffer_lock);
		while(CQ_cnt == 10) pthread_cond_wait(&buffers, &buffer_lock);
		addQ(input);
		pthread_cond_signal(&items);
		pthread_mutex_unlock(&buffer_lock);
		
		
	}
}

/* thread for consumer */ 
void *consumer (void *arg) {
	int i, id, output;
	id = cc++;;
	
	for(i=0; i < C_CNT; i++) {
		usleep(random() % 100);
		
		pthread_mutex_lock(&buffer_lock);
		while(CQ_cnt == 0) pthread_cond_wait(&items, &buffer_lock);
		output = getQ();
		pthread_cond_signal(&buffers);
		pthread_mutex_unlock(&buffer_lock);
	
		if(output != -1) {
			printf("consumer %d getQ %d\n", id , output);
		}
		
	}
}

int main() {
	pthread_t producer_thread[5];
	int producer_id[5];
	pthread_t consumer_thread[2];
	int consumer_id[2];

	int status, i;

	srand(time(NULL));

	// create producer threads
	for(i=0; i < 5; i++) {
		if((producer_id[i] = pthread_create(&producer_thread[i], NULL, producer, NULL)) < 0) {
			exit(0);
		}
	}
	
	// create consumer threads
	for(i=0; i < 2; i++) {
		if((consumer_id[i] = pthread_create(&consumer_thread[i], NULL, consumer, NULL)) < 0) {
			exit(0);
		}
	}

	// start threads
	pthread_join(producer_thread[0], (void **)&status);
	pthread_join(producer_thread[1], (void **)&status);
    pthread_join(producer_thread[2], (void **)&status);  
	pthread_join(producer_thread[3], (void **)&status);
	pthread_join(producer_thread[4], (void **)&status);
	pthread_join(consumer_thread[0], (void **)&status);
	pthread_join(consumer_thread[1], (void **)&status);


	return 0;



}




























