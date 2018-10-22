#include "./network/udp.h"
#include "./time/time.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


float PID(float reference, float y, float Kp){//, float Ki float dt){
  float error = reference - y;
  //*integral += error * dt;

  //float derivative  = (error - prev_error) / dt;
  //float prev_error  = error;    

  return (Kp * error);// + Ki * integral); //+ Kd * derivative);
}

int main(){
    UDPConn* conn = udpconn_new("192.168.0.1", 9999);
	
    char sendBuf[64];
    char recvBuf[64];    
    memset(recvBuf, 0, sizeof(recvBuf));
    
    sprintf(sendBuf, "START"); 
    udpconn_send(conn, sendBuf);
    
    int tempCounter = 0;
    //CONTROLLER FUNCTIONALITY
    //sprintf(sendBuf, "SET:1");  
    //udpconn_send(conn, sendBuf);
    
    float u = 0;
    float Kp = 10;
    float Ki = 1;
    float y = 0; 
    float reference = 1;
   
    
    while(/*time > 1sec*/ tempCounter < 1000){
      tempCounter++;
      printf("%d\n", tempCounter);
      sprintf(sendBuf, "GET");  
      udpconn_send(conn, sendBuf);
      
      udpconn_receive(conn, recvBuf, sizeof(recvBuf));
      
      if(strncmp(recvBuf, "GET_ACK", 7) == 0){
	  char* command = strtok(recvBuf, ":");
	  y = atof(strtok(NULL, ":"));
	  printf("y: %f\n", y);
	  //y = atof(strtok(recvBuf, ":"));
	
      }
      //printf("y: %d", y);
      u = PID(reference, y, Kp);
      printf("u: %f\n", u);
      
      sprintf(sendBuf, "SET:%f", u);
      udpconn_send(conn, sendBuf);
    }
    
    
    sprintf(sendBuf, "GET");  
    udpconn_send(conn, sendBuf);
    
    udpconn_receive(conn, recvBuf, sizeof(recvBuf));
    
    
    // END functions
    sprintf(sendBuf, "STOP");
    
    udpconn_send(conn, sendBuf);
    
    udpconn_delete(conn);
    return 0;
}



