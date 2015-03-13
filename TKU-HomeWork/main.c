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
#define MEMORYSIZE 1000000

char buffer [MEMORYSIZE];

typedef struct node{
    int processID;
    int startTime;
    int processTime;
    int alloSpace;
    int endTime;
    struct node *next;
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
void enTxt(char *name,Schedule_node * ptrSchedule);
void printBufferStatus (void);
void initBufferStatus(void);
void firstFitPutIntoMemory(Schedule_node *);
void releaseMemory(Schedule_node *);
void enSchedule_queue(Queue *queue,Schedule_queue *,Schedule_queue *);
void process_schedule_queue(void (*fitFunctionType)(),Schedule_queue *Scheq,Schedule_node *ptrSchedule);

Schedule_node *sort( Schedule_node *start );
Schedule_node *list_switch( Schedule_node *l1, Schedule_node *l2 );
//Declaration: For File processing
FILE *fp;
//End Declaration
int main(int argc, const char * argv[])
{
    initBufferStatus();
    //Declaration: For processing file to queue
    char tmp[50];
    char *tok=" ";
    char *parseTmpString;
    //End Declaration
    
    //Declaration: For queue and node
    Queue *q;
    q=(Queue*)malloc(sizeof(Queue));
    q->head=NULL;
    q->tail=NULL;
    Schedule_queue *EnterQ,*ReleaQ;
    EnterQ=(Schedule_queue*)malloc(sizeof(Schedule_queue));
    ReleaQ=(Schedule_queue*)malloc(sizeof(Schedule_queue));
    EnterQ->head=NULL;
    EnterQ->tail=NULL;
    ReleaQ->head=NULL;
    ReleaQ->tail=NULL;
    
    Node *ptr;
    Node *newnode;
    Schedule_node *SchePtr;
    Schedule_node *EnterPtr;
    Schedule_node *ReleaPtr;
    //End Declaration
    
    
    
    //Read file
    
    fp = fopen("input.txt", "r");
    if (!fp) {
        printf("File open failed\n");
        exit(1);
    }
    
    //fprintf(wfile, "test\n");
    //End Read
    
    //Init buffer
    //initBufferStatus();
    //Enit Init
    
    
    //Read file to Queue
    
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

        //為第一個節點
        if (q->head == NULL && q->tail==NULL) {
            
            q->head=newnode;
            q->tail=newnode;
            
            q->tail->next=NULL;
        }else{//為最後一個節點
            
            q->tail->next=newnode;
            q->tail=newnode;
            q->tail->next=NULL;
            
        }
        
    }//End while
    
    fclose(fp);
    
    //End read
    
    
    //Start Enqueue
    
    enSchedule_queue(q,EnterQ,ReleaQ);

    ReleaQ->head=sort(ReleaQ->head);
    SchePtr=ReleaQ->head;
    //lSchedule_node_bubble_sort();
//    while (SchePtr!=NULL) {
//       printf("%d %d\n",SchePtr->processID,SchePtr->time);
//        SchePtr=SchePtr->next;
//    }
    //End enqueue
    
    
    
    //Test
//    Schedule_node *ptrSchedule;
//    ptrSchedule=Scheq->head;
//    int count=0;
//    while (ptrSchedule!=NULL) {
//        count++;
//        if (ptrSchedule->States==allocate) {
//            //printf("Enter process ID: %d , Enter time: %d\n",ptrSchedule->processID,ptrSchedule->time);
//        }else if (ptrSchedule->States==release){
//            //printf("Release process ID: %d ,time: %d\n",ptrSchedule->processID,ptrSchedule->time);
//        }
//        ptrSchedule=ptrSchedule->next;
//    }
    //First-Allocation Algo
    
    
    EnterPtr=EnterQ->head;
    ReleaPtr=ReleaQ->head;
    
    while (EnterPtr!=NULL || ReleaPtr!=NULL) {
        if (EnterPtr==NULL) {

            printf("Enter Queue is empty!\n");
            releaseMemory(ReleaPtr);
            ReleaPtr=ReleaPtr->next;
            
        }else if (ReleaPtr==NULL){

            //printf("Release Queue is empty!\n");
            firstFitPutIntoMemory(EnterPtr);
            EnterPtr=EnterPtr->next;
        }
        
        else if (EnterPtr->time > ReleaPtr->time) {
            
            //printf("processing Release %d, Time:%d\n",ReleaPtr->processID,ReleaPtr->time);
            releaseMemory(ReleaPtr);
            ReleaPtr=ReleaPtr->next;
        }else if (EnterPtr->time == ReleaPtr->time){
            
            //printf("processing Release %d, Time:%d\n",ReleaPtr->processID,ReleaPtr->time);
            releaseMemory(ReleaPtr);
            ReleaPtr=ReleaPtr->next;
        }else if(EnterPtr->time < ReleaPtr->time){
            
            //printf("processing Enter %d, Start:%d\n",EnterPtr->processID,EnterPtr->time);
            
            firstFitPutIntoMemory( EnterPtr);
            
            EnterPtr=EnterPtr->next;
        }else{
            printf("error");
        }
    }
    

    //End First
    
    
    
    //Best-Allocation Algo
    
    
//    ptrSchedule=Scheq->head;
//    while (ptrSchedule!=NULL) {
//        
//        ptrSchedule=ptrSchedule->next;
//    }
//    
    //End Best
    
    //Worst-Allocation Algo
    
    //End Worst
    
    
    fclose(fp);
    return 0;
}
void enSchedule_queue(Queue *queue,Schedule_queue *EnterQ,Schedule_queue *ReleaQ){
    Node *ptr;
    Schedule_node *ptrSchedule;
    
    ptr=queue->head;
    while (ptr!=NULL) {
        Schedule_node *new_sche_node_allo;
        Schedule_node *new_sche_node_relea;
        
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
        
        
        //Record where is the release node
        new_sche_node_allo->releaseNode=new_sche_node_relea;
        //End Record
        
        //Enter Q processing
        //If First Node
        if (EnterQ->head==NULL && EnterQ->tail==NULL) {
            EnterQ->head=new_sche_node_allo;
            EnterQ->tail=new_sche_node_allo;
            new_sche_node_allo->next=NULL;
        }else{
        //If next node
            EnterQ->tail->next=new_sche_node_allo;
            EnterQ->tail=new_sche_node_allo;
            EnterQ->tail->next=NULL;
        }
        //End Enter Q processing
        
        //Relea Q processing
        //If First Node
        if (ReleaQ->head==NULL && ReleaQ->tail==NULL) {
            ReleaQ->head=new_sche_node_relea;
            ReleaQ->tail=new_sche_node_relea;
            new_sche_node_relea->next=NULL;
        }else{
            //If next node
            ReleaQ->tail->next=new_sche_node_relea;
            ReleaQ->tail=new_sche_node_relea;
            ReleaQ->tail->next=NULL;
        }
        //End Enter Q processing
        
        
        ptr=ptr->next;
    }
}

void firstFitPutIntoMemory(Schedule_node *ptrSchedule){
    int count=0;
    int i=0;
    int startAd,endAd;
    int flag=false;
    int space=ptrSchedule->alloSpace;
    
    //wfile=fopen("first.txt", "w");
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
                ptrSchedule->startAd=startAd;
                //3.Print out the log
                //fprintf(wfile,"%d %d\n",ptrSchedule->processID,startAd);
                printf("Succesed enter process id:%d ,startAd:%d\n",ptrSchedule->processID,startAd);
                
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
        
        //enTxt("first.txt", ptrSchedule);
        
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
void releaseMemory(Schedule_node *ptrSchedule){
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

   //fprintf(wfile,"%d\n",ptrSchedule->processID);
}

Schedule_node *append( Schedule_node *start, int newdata )
{
    Schedule_node *new, *end, *ret;
    
    if( (new = (Schedule_node *)malloc(sizeof(Schedule_node))) == NULL) {
        fprintf( stderr, "Memory Allocation error.\n" );
        // In Windows, replace following with a return statement.
        exit(1);
    }
    if( start == NULL )
        ret = new;
    else {
        ret = start;
        end = start;
        while( end->next != NULL ) end = end->next;
        end->next = new;
    }
    new->time = newdata;
    new->next = NULL;
    return ret ;
}

Schedule_node *sort( Schedule_node *start )
{
    Schedule_node *p, *q, *top;
    int changed = 1;
    
    /*
     * We need an extra item at the top of the Schedule_node just to help
     * with assigning switched data to the 'next' of a previous item.
     * It (top) gets deleted after the data is sorted.
     */
    
    if( (top = (Schedule_node *)malloc(sizeof(Schedule_node))) == NULL) {
        fprintf( stderr, "Memory Allocation error.\n" );
        // In Windows, replace following with a return statement.
        exit(1);
    }
    
    top->next = start;
    if( start != NULL && start->next != NULL ) {
        /*
         * This is a survival technique with the variable changed.
         *
         * Variable q is always one item behind p. We need q, so
         * that we can make the assignment q->next = Schedule_node_switch( ... ).
         */
        
        while( changed ) {
            changed = 0;
            q = top;
            p = top->next;
            while( p->next != NULL ) {
                /* push bigger items down */
                if( p->time > p->next->time ) {
                    q->next = list_switch( p, p->next );
                    changed = 1;
                }
                q = p;
                if( p->next != NULL )
                    p = p->next;
            }
        }
    }
    p = top->next;
    free( top );
    return p;
}


Schedule_node *list_switch( Schedule_node *l1, Schedule_node *l2 )
{
    l1->next = l2->next;
    l2->next = l1;
    return l2;
}