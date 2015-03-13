//
//  main.c
//
//  TKU CSIE 401411508 , 資訊三C
//  Created by 郭彥君 on 15/3/2.
//  Copyright (c) 2015年 郭彥君. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef int bool;
#define true 1
#define false 0
#define MEMORYSIZE 50000
char buffer [MEMORYSIZE];

typedef struct node{
    int processID;
    int startTime;
    int processTime;
    int alloSpace;
    int endTime;
    struct node *next;
    struct node *front;
}Node;
typedef struct schedule_node{
    int processID;
    int time;
    int alloSpace;
    enum states{allocate,release}States;
    struct schedule_node *next;
    struct schedule_node *front;
    //for allocate node
    struct schedule_node *releaseNode;
    
    //for release node
    int startAd;
    int endAd;
}Schedule_node;

typedef struct queue{
    struct node *head;
    struct node *tail;
}Queue;

typedef struct schedule_queue{
    struct schedule_node *head;
    struct schedule_node *tail;
}Schedule_queue;

char tmp[50];
FILE *fp;
FILE *wfile;
void enTxt(char *name,Schedule_node * ptrSchedule);
void printBufferStatus (void);
void initBufferStatus(void);
void firstFitPutIntoMemory(Schedule_queue *Scheq,Schedule_node *ptrSchedule);
void releaseMemory(Schedule_queue *Scheq,Schedule_node *ptrSchedule);
void enSchedule_queue(Queue *queue,Schedule_queue *Scheq);
void process_schedule_queue(void (*fitFunctionType)(),Schedule_queue *Scheq,Schedule_node *ptrSchedule);
int main(int argc, const char * argv[])
{
    char tmp[50];
    char *tok=" ";
    char *parseTmpString;
    
    Queue *q;
    q=(Queue*)malloc(sizeof(Queue));
    q->head=NULL;
    q->tail=NULL;
    Schedule_queue *Scheq;
    Scheq=(Schedule_queue*)malloc(sizeof(Schedule_queue));
    Scheq->head=NULL;
    Scheq->tail=NULL;
    
    
    Node *ptr;
    FILE *fp;
    
    int i,t,globalEndTime=0;
    
    //Read file
    
    fp = fopen("input.txt", "r");
    wfile= fopen("first.txt", "w+");
    if (!fp) {
        printf("File open failed\n");
        exit(1);
    }
    if (!wfile) {
        printf("File open failed\n");
        exit(1);
    }
    //End Read
    
    //init buffer
    initBufferStatus();
    //
    //Read file to Queue
    Node *newnode;
    while (fgets(tmp, 50, fp) != NULL) {
        // If read a new line , than allocation a new node
        
        newnode=(Node*)malloc(sizeof(Node));
        //
        
        
        parseTmpString=strtok(tmp," ");
        newnode->processID=atoi(parseTmpString);
        
        parseTmpString=strtok(NULL, " ");
        newnode->startTime=atoi(parseTmpString);
        
        
        parseTmpString=strtok(NULL, " ");
        newnode->processTime=atoi(parseTmpString);
        
        parseTmpString=strtok(NULL, " ");
        newnode->alloSpace=atoi(parseTmpString);
        
        newnode->endTime=(newnode->processTime)+(newnode->startTime);
        
        //Calculate when system stop
        if (newnode->endTime > globalEndTime) {
            globalEndTime=newnode->endTime;
        }
        //End calculate
        //   printf("%d %d %d %d",newnode->processID,newnode->startAd,newnode->processTime,newnode->alloSpace);
        
        //為第一個節點
        if (q->head == NULL) {
            
            q->head=newnode;
            q->tail=newnode;
            
            q->head->next=q->tail;
            q->tail->front=q->head;
        }else{//為最後一個節點
            
            q->tail->next=newnode;
            q->tail=newnode;
            q->tail->next=NULL;
            
            newnode->front=q->tail;
        }
        
    
        
    }
    
    fclose(fp);
    
    //End read
    
    //enqueue
    
    enSchedule_queue(q, Scheq);
    
    //End enqueue
    Schedule_node *ptrSchedule;
    ptrSchedule=Scheq->head;
    int count=0;
    while (ptrSchedule!=NULL) {
        count++;
        if (ptrSchedule->States==allocate) {
            //printf("Enter process ID: %d , Enter time: %d\n",ptrSchedule->processID,ptrSchedule->time);
        }else if (ptrSchedule->States==release){
            //printf("Release process ID: %d ,time: %d\n",ptrSchedule->processID,ptrSchedule->time);
        }
        ptrSchedule=ptrSchedule->next;
    }
    printf("count=:%d",count);
    //First-Allocation Algo
    
    
    ptrSchedule=Scheq->head;
    
    while (ptrSchedule!=NULL) {
        if (ptrSchedule->States==allocate) {
            firstFitPutIntoMemory(Scheq, ptrSchedule);

        }else if(ptrSchedule->States==release){
            releaseMemory(Scheq, ptrSchedule);
        }
        ptrSchedule=ptrSchedule->next;
    }

    //End First
    
    
    
    //Best-Allocation Algo
    
    
    ptrSchedule=Scheq->head;
    while (ptrSchedule!=NULL) {
        
        ptrSchedule=ptrSchedule->next;
    }
    
    //End Best
    
    //Worst-Allocation Algo
    
    //End Worst
    
    
    fclose(fp);
    return 0;
}
void enSchedule_queue(Queue *queue,Schedule_queue *Scheq){
    Node *ptr;
    Schedule_node *ptrSchedule;
    
    ptr=queue->head;
    while (ptr!=NULL) {
        Schedule_node *new_sche_node_allo;
        Schedule_node *new_sche_node_relea;
        //Test for queue
//        printf("%d %d %d %d %d",ptr->processID,ptr->startTime,ptr->processTime,ptr->alloSpace,ptr->endTime);
//        printf("\n");
//        //End test
        
        
        //allocate event
        new_sche_node_allo=(Schedule_node*)malloc(sizeof(Schedule_node));
        new_sche_node_allo->processID=ptr->processID;
        new_sche_node_allo->time=ptr->startTime;
        new_sche_node_allo->alloSpace=ptr->alloSpace;
        new_sche_node_allo->States=allocate;
        
        //release event
        
        new_sche_node_relea=(Schedule_node*)malloc(sizeof(Schedule_node));
        new_sche_node_relea->processID=ptr->processID;
        new_sche_node_relea->time=ptr->endTime;
        new_sche_node_relea->alloSpace=ptr->alloSpace;
        new_sche_node_relea->States=release;
        
        
        //binding allocate node with release node
        new_sche_node_allo->releaseNode=new_sche_node_relea;
        //visit the schedule queue
        
        //if first event
        if (Scheq->head==NULL && Scheq->tail==NULL) {
            Scheq->head=new_sche_node_allo;
            Scheq->tail=new_sche_node_relea;
            new_sche_node_allo->next=new_sche_node_relea;
            new_sche_node_relea->next=NULL;
        }else{
            
            
            
            //allocate
           // ptrSchedule=Scheq->head;
           
//            //for release
//            ptrSchedule=Scheq->head;
//            while (ptrSchedule!=NULL) {
//                
//                //insert tail
//                if (new_sche_node_relea->time > Scheq->tail->time) {
//                    new_sche_node_relea->front=Scheq->tail;
//                    
//                    Scheq->tail->next=new_sche_node_relea;
//                    Scheq->tail=new_sche_node_relea;
//                //insert head
//                }else if (new_sche_node_relea->time < Scheq->head->time){
//                    Scheq->head->front=new_sche_node_relea;
//                    
//                    new_sche_node_relea->next=Scheq->head;
//                    Scheq->head=new_sche_node_relea;
//                
//                    
//                    
//                }else if (new_sche_node_relea->time > ptrSchedule->time &&
//                          new_sche_node_relea->time < ptrSchedule->next->time){
//                    new_sche_node_relea->front=ptrSchedule;
//                    ptrSchedule->next->front=new_sche_node_relea;
//                    
//                    new_sche_node_relea->next=ptrSchedule->next;
//                    ptrSchedule->next=new_sche_node_relea;
//                }else if (new_sche_node_allo->time==ptrSchedule->time && ptrSchedule!=Scheq->tail &&ptrSchedule!=Scheq->head){
//               
//                }else if(new_sche_node_relea->time == ptrSchedule->time &&ptrSchedule==Scheq->tail){
//                    printf("may insert last node\n");
//                    //new_sche_node_relea->next=ptrSchedule->next;
//                    //ptrSchedule->next=new_sche_node_relea;
//                }else  if(new_sche_node_relea->time == ptrSchedule->time &&ptrSchedule==Scheq->head){
//                    printf("may insert first node\n");
//                }else{
//                    printf("other error\n");
//                }
//                ptrSchedule=ptrSchedule->next;
//            }
//            
            //for allocate
//            ptrSchedule=Scheq->head;
//            
//            while (ptrSchedule!=NULL) {
//                //insert tail
//                if (new_sche_node_allo->time > Scheq->tail->time) {
//                    new_sche_node_allo->front=Scheq->tail;
//                    
//                    Scheq->tail->next=new_sche_node_allo;
//                    Scheq->tail=new_sche_node_allo;
//                
//                //insert head
//                }else if (new_sche_node_allo->time < Scheq->head->time){
//                    Scheq->head->front=new_sche_node_allo;
//                    
//                    new_sche_node_allo->next=Scheq->head;
//                    Scheq->head=new_sche_node_allo;
//                    
//                //insert
//                }else if (new_sche_node_allo->time > ptrSchedule->time &&
//                          new_sche_node_allo->time < ptrSchedule->next->time){
//                    new_sche_node_allo->front=ptrSchedule;
//                    ptrSchedule->next->front=new_sche_node_allo;
//                    
//                    new_sche_node_allo->next=ptrSchedule->next;
//                    ptrSchedule->next=new_sche_node_allo;
//                }else if(new_sche_node_allo->time == ptrSchedule->time){
//                    //TODO:select which insert first
//                    
//                }
//            
//                ptrSchedule=ptrSchedule->next;
//            }
//            
//            
        }
        ptr=ptr->next;
    }
}


void firstFitPutIntoMemory(Schedule_queue *Scheq,Schedule_node *ptrSchedule){
    int count=0;
    int i=0;
    int startAd,endAd;
    int flag=false;
    int space=ptrSchedule->alloSpace;
    
    //For file Declaration
    
    
    
    //End Declaration
    for (i=0; i<MEMORYSIZE; i++) {
        if (buffer[i]=='0') {
            count++;
            if (count==space) {
                //It is possible to allocate to memory
                //1.return true,set flag=1;
                flag=true;
                //2.sent out startAd and endAd
                ptrSchedule->releaseNode->endAd=i;
                ptrSchedule->releaseNode->startAd=i-space+1;
                
                endAd=i;
                startAd=i-space+1;
                
                //3.Print out the log
                fprintf(wfile,"%d %d %d\n",ptrSchedule->processID,startAd,ptrSchedule->time);
                //printf("Succesed enter process id:%d ,startAd:%d,endAd:%d,time:%d\n",ptrSchedule->processID,startAd,endAd,ptrSchedule->time);
                
                //printf("My release id is:%d,startAd:%d,endAd:%d\n",ptrSchedule->processID,startAd,endAd);
                //4.Break the loop
                break;
            }
        }else if(buffer[i]=='1'){
            count=0;
        }
    }
    if (flag==true) {
        while (startAd<=endAd) {
            buffer[startAd]='1';
            startAd++;
        }
        
        enTxt("first.txt", ptrSchedule);
        
    }else if(flag==false){
        
        
//        printf("Fail to allocate process:%d\n",ptrSchedule->processID);
//        //fail to allocate
//        printf("false-id:%d\n",ptrSchedule->processID);
//        printf("front id:%d,next id:%d",ptrSchedule->front->processID,ptrSchedule->next->processID);
//        if (ptrSchedule->releaseNode->next==NULL) {
//            printf("next error\n ");
//        }else if (ptrSchedule->releaseNode->front==NULL){
//            printf("front error\n");
//        }
//        //        //sent signal to release node, tell him kill himself,means remove from queue
//        printf("i wanna remove %d\n",ptrSchedule->releaseNode->processID);
//        
//        if (ptrSchedule->releaseNode==NULL) {
//            
//        }
//              //  ptrSchedule->releaseNode->front->next=ptrSchedule->releaseNode->next;
//              //  ptrSchedule->releaseNode->next->front=ptrSchedule->releaseNode->front;
//        
//        printf("Succese remove process:%d\n",ptrSchedule->processID);
    }
    
}
void releaseMemory(Schedule_queue *Scheq,Schedule_node *ptrSchedule){
//    printf("Release Node, Process ID:%d,startAd:%d,EndAd:%d\n",ptrSchedule->processID,ptrSchedule->startAd,ptrSchedule->endAd);
    if (ptrSchedule->startAd==0 && ptrSchedule->endAd ==0) {
        //Not enter Memory at all , skip
    }
    else{
        //Free Memory
        //printf("Remove from process:%d , start:%d ,end:%d,time:%d\n ",ptrSchedule->processID,ptrSchedule->startAd,ptrSchedule->endAd,ptrSchedule->time);
        int i;
        for (i=ptrSchedule->startAd; i<=ptrSchedule->endAd; i++) {
            buffer[i]='0';
        }
    }
}
void printBufferStatus(void){
    int i;
    for (i = 0; i< MEMORYSIZE; i++)
    {
        printf("%c ",buffer[i]);
    }
    
}
void initBufferStatus(void){
    int i;
    for (i=0; i<MEMORYSIZE; i++) {
        buffer[i]='0';
    }
}
void enTxt(char *name,Schedule_node * ptrSchedule){
   //fprintf(wfile, "%d %d",ptrSchedule->processID,ptrSchedule->startAd);
}
