#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std;
#define THREAD_NUM 4

pthread_mutex_t counter_mutex[4], left_mutex[4], road_mutex[4];
int left_car[4], counter[4];

int get_counter(int car_id) {
    int x;
    pthread_mutex_lock(&counter_mutex[car_id]);
    x = counter[car_id];
    pthread_mutex_unlock(&counter_mutex[car_id]);
    return x;
}

void increase_counter(int car_id) {
    pthread_mutex_lock(&counter_mutex[car_id]);
    counter[car_id]++;
    pthread_mutex_unlock(&counter_mutex[car_id]);
}

int get_left(int car_id) {
    int x;
    pthread_mutex_lock(&counter_mutex[car_id]);
    x = left_car[car_id];
    pthread_mutex_unlock(&counter_mutex[car_id]);
    return x;
}

int decrease_left(int car_id) {
    pthread_mutex_lock(&left_mutex[car_id]);
    left_car[car_id]--;
    pthread_mutex_unlock(&left_mutex[car_id]);
}

bool can_cross(int car_id) {
    for (int i = 0; i < 4; ++i) {
        if (get_left(i) && counter[car_id] > counter[i]) {
            return false;
        }
    }
    return true;
}

class cross_data {
public:
    cross_data() {
        mutex_1 = road_mutex[0];
        mutex_2 = road_mutex[1];
        direction = 'x';
        car_id = 0;
        m1 = "x";
        m2 = "x";
    }

    cross_data(pthread_mutex_t a, pthread_mutex_t b, char c) {
        mutex_1 = a;
        mutex_2 = b;
        direction = c;
        car_id = 0;
    }

    void update(int a, int b, char c, string e, string f) {
        mutex_1 = road_mutex[a];
        mutex_2 = road_mutex[b];
        direction = c;
        car_id = 0;
        m1 = e;
        m2 = f;
    }

    pthread_mutex_t mutex_1;
    pthread_mutex_t mutex_2;
    char direction;
    int car_id;
    string m1;
    string m2;
};
cross_data cross_d[4];

void cross(cross_data x) {
    while (1) {
        while (!can_cross(x.car_id));
        if (pthread_mutex_trylock(&x.mutex_1) == 0) {
            cout << x.direction << " " << x.car_id << " acquire " << x.m1 << endl;
            break;
        }
        increase_counter(x.car_id);
    }

    while (1) {
        while (!can_cross(x.car_id));
        if (pthread_mutex_trylock(&x.mutex_2) == 0) {
            cout << x.direction << " " << x.car_id << " acquire " << x.m2 << endl;
            break;
        }
        increase_counter(x.car_id);
    }
    pthread_mutex_unlock(&x.mutex_1);
    pthread_mutex_unlock(&x.mutex_2);
    cout << x.direction << " " << x.car_id << " release locks " << endl;
    cout << x.direction << " " << x.car_id << " leaves at X " << endl;
    decrease_left(x.car_id);
}

void *ThreadRunner(void *arg) {
    int i = *((int *) arg);
    cout << i << endl;
    while(get_left(i)){
        cross(cross_d[i]);
    }
    return NULL;
}

int main() {
    pthread_t tid[THREAD_NUM];
    for (int j = 0; j < 4; j++) {
        pthread_mutex_init (&counter_mutex[j], NULL);
        pthread_mutex_init (&left_mutex[j], NULL);
        pthread_mutex_init (&road_mutex[j], NULL);
    }
    int i;

    cross_d[3].update(2, 3, 'W', "lock_2", "lock_3");
    cross_d[0].update(0, 2, 'N', "lock_0", "lock_2");
    cross_d[1].update(1, 0, 'E', "lock_1", "lock_0");
    cross_d[2].update(3, 1, 'S', "lock_3", "lock_1");
    left_car[0] = 1;
    left_car[1] = 1;
    left_car[2] = 0;
    left_car[3] = 0;
    int t[THREAD_NUM];
    int id[4];
    for (i = 0; i < THREAD_NUM; i++) {
        id[i] = i;
        pthread_create(&tid[i], NULL, ThreadRunner, (void *) &id[i]);
    }

    for (i = 0; i < THREAD_NUM; i++) {
        pthread_join(tid[i], NULL);
    }
}