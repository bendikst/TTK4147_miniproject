#include "./network/udp.h"
#include "./time/time.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

sem_t period_sem, controller_sem;
UDPConn* conn;

float PID(float reference, float y, float Kp, float Ki, float dt, float* integral){
  float error = reference - y;
  *integral += error * dt;
  return (Kp * error + Ki * (*integral)); //+ Kd * derivative);
}


float parse_y(char buf[]){
    char* command = strtok(buf, ":");
    float y = atof(strtok(NULL, ":"));
    return y;
}


void send_get(){
    char sendBuf[64];
    sprintf(sendBuf, "GET");  
    udpconn_send(conn, sendBuf);
}


void send_u(float u){
    char sendBuf[64];
    sprintf(sendBuf, "SET:%f", u);
    udpconn_send(conn, sendBuf);
}


void* periodic_timer(void* args){
    struct timespec waketime;
    clock_gettime(CLOCK_REALTIME, &waketime);
    struct timespec period = {.tv_sec = 0, .tv_nsec = 1500*1000};
    
    while(1){
      sem_wait(&controller_sem);
      waketime = timespec_add(waketime, period);
      //printf("time: %d:%d\n", waketime.tv_sec, waketime.tv_nsec);
      //clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &waketime, NULL);
      nanosleep_modified(&waketime);
      sem_post(&period_sem);
    }
}


void* controller_func(void* arg){
    char recvBuf[64];
    memset(recvBuf, 0, sizeof(recvBuf));
    float u = 0;
    float Kp = 10;
    float Ki = 800;
    float y = 0; 
    float reference = 1;
    float integral = 0;
    
    struct timespec start_time;
    struct timespec now_time;

    clock_gettime(CLOCK_REALTIME, &start_time);
    struct timespec reference_change_time = {.tv_sec = 1, .tv_nsec = 0};
    struct timespec end_time = {.tv_sec = 2, .tv_nsec = 0};
    reference_change_time = timespec_add(start_time, reference_change_time);
    end_time = timespec_add(start_time, end_time);
    
    while(timespec_cmp(now_time, end_time) <= 1){
      sem_wait(&period_sem);
      clock_gettime(CLOCK_REALTIME, &now_time);
      if(timespec_cmp(now_time, reference_change_time) >= 1){
	reference = 0;
      }
      send_get();
      udpconn_receive(conn, recvBuf, sizeof(recvBuf));
      if(strncmp(recvBuf, "GET_ACK", 7) == 0){
	  y = parse_y(recvBuf);
      }
      
      u = PID(reference, y, Kp, Ki, 0.0015, &integral);
      send_u(u);
      sem_post(&controller_sem);
      
    } 
}


int main(){
    //Set up code
    sem_init(&period_sem, 0, 0);
    sem_init(&controller_sem, 0, 1);
    conn = udpconn_new("192.168.0.1", 9999);
	
    char sendBuf[64];
    char recvBuf[64];    
    memset(recvBuf, 0, sizeof(recvBuf));
    
    sprintf(sendBuf, "START"); 
    udpconn_send(conn, sendBuf);
    
    
    pthread_t periodic_thread;
    pthread_t control_thread;
    pthread_create(&periodic_thread, NULL, periodic_timer, NULL);
    pthread_create(&control_thread, NULL, controller_func, NULL);
  
    // END functions
    pthread_join(control_thread, NULL);
    sprintf(sendBuf, "STOP");
    udpconn_send(conn, sendBuf);   
    udpconn_delete(conn);
    pthread_join(periodic_thread, NULL);
    return 0;
}



