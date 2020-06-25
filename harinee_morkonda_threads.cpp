//Harinee Morkonda 
//OS HW 3 

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

//shared variables
static int maxCars = 0;
static int maxNBcars = 0;
static int maxSBcars = 0;
static int cuNorthCars = 0;
static int cuSouthCars = 0;
static int cuTotalCars = 0;
static int carsWaiting = 0;
//shared variables

static pthread_mutex_t mutex; //mutex
static pthread_cond_t condi = PTHREAD_COND_INITIALIZER; //condition variable

struct car {
	unsigned int delay;
	unsigned int traveltime;
	string direction;
};

struct car arr[145];
int arraysize = 0;

int northCarNum = 0;
int southCarNum = 0;



void readingInput(istream &cin) {
	int val;
	cin >> val;
	maxCars = val;
	cin >> val;
	maxNBcars = val;
	cin >> val;
	maxSBcars = val;

	cout << "Maximum number of cars in the tunnel: " << maxCars << endl;
	cout << "Maximum number of northbound cars: " << maxNBcars << endl;
	cout << "Maximum number of southbound cars:" << maxSBcars << endl;

	unsigned int num1;
	int num2;
	string word;
	while (cin >> num1 >> word >> num2) {
		arr[arraysize].delay = num1;
		arr[arraysize].direction = word;
		arr[arraysize].traveltime = num2;

		//cout << arr[arraysize].delay << " "
		//	<< arr[arraysize].direction << " "
		//	<< arr[arraysize].traveltime << endl;

		arraysize++;
	}

}//reading cin

void *northchild(void *arg) {
	int index = (int)arg;
	int travel = arr[index].traveltime;
	northCarNum++;

	pthread_mutex_lock(&mutex); //get mutex lock
	bool Northwaiting = false;

	int north = northCarNum;

	cout << "Northbound car # " << north << " arrives at the tunnel" << endl;

	while (cuNorthCars == maxNBcars || cuTotalCars == maxCars) {
		Northwaiting = true;
		pthread_cond_wait(&condi, &mutex);
	} //waiting is over after the while loop is over

	cout << "Northbound car # " << north << " enters the tunnel" << endl;
	cuNorthCars++; //after waiting is over
	cuTotalCars++;

	pthread_mutex_unlock(&mutex); //release lock
	sleep(travel); //sleep for travel time
	pthread_mutex_lock(&mutex);
	cout << "Northbound car # " << north << " exits the tunnel" << endl;

	cuNorthCars--;
	cuTotalCars--;


	pthread_cond_broadcast(&condi);
	if (Northwaiting == true) {
		carsWaiting++;
	}

	//pthread_cond_signal(&condi);
	pthread_mutex_unlock(&mutex);
}

void *southchild(void *arg) {
	int index = (int)arg;
	int travel = arr[index].traveltime;
	southCarNum++;

	pthread_mutex_lock(&mutex); //get mutex lock
	bool Southwaiting = false;

	int south = southCarNum;

	cout << "Southbound car # " << south << " arrives at the tunnel." << endl;

	while (cuSouthCars == maxSBcars || cuTotalCars == maxCars) {
		Southwaiting = true;
		pthread_cond_wait(&condi, &mutex);
	} //waiting is over after the while loop is over
	cout << "Southbound car # " << south << " enters the tunnel." << endl;

	cuSouthCars++; //after waiting is over
	cuTotalCars++;

	pthread_mutex_unlock(&mutex); //release lock
	sleep(travel); //sleep for travel time
	pthread_mutex_lock(&mutex);
	cout << "Southbound car # " << south << " exits the tunnel." << endl;

	cuSouthCars--;
	cuTotalCars--;
	pthread_cond_broadcast(&condi);

	if (Southwaiting == true) {
		carsWaiting++;
	}
	//pthread_cond_signal(&condi);
	pthread_mutex_unlock(&mutex);

}
int main() {
	readingInput(cin); //read input from the file and store in the struct
	pthread_t threadID[145];

	pthread_mutex_init(&mutex, NULL);

	int numThreads = 0;
	int i;
	for (i = 0; i < arraysize; i++) {
		numThreads++;	
		sleep(arr[i].delay);

		if (arr[i].direction == "N") {
			pthread_create(&threadID[i], NULL, northchild, (void *)i);
		}
		else if (arr[i].direction == "S") {
			pthread_create(&threadID[i], NULL, southchild, (void *)i);
		}
	}
	for (int j = 0; j < arraysize; j++) {
		pthread_join(threadID[j], NULL);
	}

	cout << northCarNum << " northbound car(s) crossed the tunnel." << endl;
	cout << southCarNum << " southbound car(s) crossed the tunnel." << endl;
	cout << carsWaiting << " car(s) had to wait." << endl;
	return 0;

}