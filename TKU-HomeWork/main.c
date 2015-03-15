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
int gabege=0;
int gabege2=0;
typedef int bool;
#define true 1
#define false 0
#define MEMORYSIZE 10000
#define MAX(x,y) ( ((x)>(y) ) ? (x):(y) )
char buffer [MEMORYSIZE];
typedef struct memorynode{
    int startAd;
    int endAd;
    int space;
    struct memorynode *next;
}memoryNode;
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
typedef struct memoryqueue{
    struct memorynode *head;
    struct memorynode *tail;
}memoryQueue;
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
int firstFitPutIntoMemory(Schedule_node *);
int BestPutIntoMemory(Schedule_node *);
int WorstPutIntoMemory(Schedule_node *ptrSchedule);
void releaseMemory(Schedule_node *);
void enSchedule_queue(Queue *queue,Schedule_queue *,Schedule_queue *);
void process_schedule_queue(void (*fitFunctionType)(),Schedule_queue *Scheq,Schedule_node *ptrSchedule);

Schedule_node *sort( Schedule_node *start );
Schedule_node *list_switch( Schedule_node *l1, Schedule_node *l2 );
//Declaration: For File processing
FILE *fp;
FILE *wfile;
FILE *xfile;
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
    wfile=fopen("log_first.txt","w+");
    if (!wfile) {
        printf("fail to load wfile");
    }
    xfile=fopen("log_best.txt", "w+");
    if (!xfile) {
        printf("fail to load xfile");
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
    
    
    //Start Enqueue and sort
    
    enSchedule_queue(q,EnterQ,ReleaQ);

    ReleaQ->head=sort(ReleaQ->head);
    SchePtr=ReleaQ->head;
    
    //End enqueue
    
    
    
    //Test

    //First-Allocation Algo
    
    
    EnterPtr=EnterQ->head;
    ReleaPtr=ReleaQ->head;
    int cache;
    while (EnterPtr!=NULL || ReleaPtr!=NULL) {
        if (EnterPtr==NULL) {

            releaseMemory(ReleaPtr);
            
            ReleaPtr=ReleaPtr->next;
            
        }else if (ReleaPtr==NULL){

            firstFitPutIntoMemory(EnterPtr);
            EnterPtr=EnterPtr->next;
        }
        
        else if (EnterPtr->time > ReleaPtr->time) {
            
            releaseMemory(ReleaPtr);
            
            ReleaPtr=ReleaPtr->next;
        }else if (EnterPtr->time == ReleaPtr->time){
            
            releaseMemory(ReleaPtr);
        
            ReleaPtr=ReleaPtr->next;
        }else if(EnterPtr->time < ReleaPtr->time){
            
            
            cache=firstFitPutIntoMemory( EnterPtr);
            if (cache==1) {
                fprintf(wfile,"%d %d\n",EnterPtr->processID,EnterPtr->startAd);
            }else if(cache==0){
                fprintf(wfile,"%d %d\n",EnterPtr->processID,MEMORYSIZE);
            }
        
            EnterPtr=EnterPtr->next;
        }else{
            printf("error");
        }
    }
    fclose(wfile);
    
    printf("First fit complete\n");
    //End First
    
    
    printf("Best fit\n");
//Best-Allocation Algo
    
    EnterPtr=EnterQ->head;
    ReleaPtr=ReleaQ->head;
    
    while (EnterPtr!=NULL || ReleaPtr!=NULL) {
        if (EnterPtr==NULL) {
            
            releaseMemory(ReleaPtr);
            ReleaPtr=ReleaPtr->next;
            
        }else if (ReleaPtr==NULL){
            cache=BestPutIntoMemory(EnterPtr);
            if (cache==0) {
                
                //printf("%d %d\n",EnterPtr->processID,MEMORYSIZE);
                
            }else if (cache==1){
                gabege2++;
                //printf("%d %d\n",EnterPtr->processID,EnterPtr->startAd);
                
            }
            EnterPtr=EnterPtr->next;
        }
        
        else if (EnterPtr->time > ReleaPtr->time) {
            
            releaseMemory(ReleaPtr);
            ReleaPtr=ReleaPtr->next;
        }else if (EnterPtr->time == ReleaPtr->time){
            
            releaseMemory(ReleaPtr);
            ReleaPtr=ReleaPtr->next;
        }else if(EnterPtr->time < ReleaPtr->time){
            cache=BestPutIntoMemory(EnterPtr);
            if (cache==0) {
                gabege++;
            }else if (cache==1){
                gabege2++;
                printf("%d %d\n",EnterPtr->processID,EnterPtr->startAd);

            }
            
            EnterPtr=EnterPtr->next;
        }else{
            //printf("error");
        }
    }
    
    //End Best
    
    //Worst-Allocation Algo
    
    
    
    
    //End Worst
    
    printf("\ngabege:%d\ngabege2:%d\n",gabege,gabege2);
    //fclose(fp);
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
int firstFitPutIntoMemory(Schedule_node *ptrSchedule){
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
                ptrSchedule->startAd=startAd;
                //3.Print out the log
                
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
        return 1;
    }else if(flag==false){

        ptrSchedule->releaseNode->endAd=-1;
        ptrSchedule->releaseNode->startAd=-1;
        return 0;
    }
    return -1;
}
int BestPutIntoMemory(Schedule_node *ptrSchedule){
    int i=0,count=0;
    int minimalLocation=0;
    int startAd,endAd;
    int memoryStartAd=0,memoryEndAd=0,memory=0;
    int flag=false;
    int space=ptrSchedule->alloSpace;
    int smallestMemory=MEMORYSIZE+1;
    int smallStartAd=0,smallEndAd=0,smallflag=0;
    for (i=0; i<MEMORYSIZE-1; i++) {
        if (buffer[i]=='0') {
            
            if (buffer[i+1]=='1') {
                //really String end ,record the memorysize
                //and compare to database's best momery
                //First, database's momery is very large
                //The first String must be large than spaze
                //Than got right to compare
                memoryEndAd=i;
                //Calcute how memory large
                memory=(memoryEndAd-memoryStartAd+1);
                if (memory>=space&&memory<smallestMemory) {
                    //got better memory
                    //Store information to database
                    smallStartAd=memoryStartAd;
                    smallEndAd=memoryEndAd;
                    smallestMemory=memory;
                    smallflag=1;
                }
            }else if (buffer[i+1]=='0'){
                //tmp end
                memoryEndAd=i+1;
            }
        }else if (buffer[i]=='1'){
            if (buffer[i+1]=='1') {
                
            }else if(buffer[i+1]=='0'){
                memoryStartAd=i+1;
            }
        }
    }
    
    if (memoryEndAd==0&&memoryStartAd==0) {
        
        ptrSchedule->releaseNode->startAd=-1;
        ptrSchedule->releaseNode->endAd=-1;
        
        return 0;
    }
    
    //Calcute tmp memory size
    memory=(memoryEndAd-memoryStartAd+1);
    if (smallflag==1) {
        //compare to tmp memory
        //cause the end might end by 0
        //not store in database
        
        //comfime tmp large than space,than got right to compare
        if (memory<smallestMemory) {
            //tmp memory is better
            smallStartAd=memoryStartAd;
            smallEndAd=memoryEndAd;
            smallestMemory=memory;
        }
        else{
            //database's data is better
            
            //just keep empty
        }
        
        //use smallestmemory to allocate
        startAd=smallStartAd;
        endAd=smallStartAd+space-1;
        
        ptrSchedule->releaseNode->startAd=startAd;
        ptrSchedule->releaseNode->endAd=endAd;
        ptrSchedule->startAd=smallStartAd;
        
        while (smallStartAd<=endAd) {
            buffer[smallStartAd]='1';
            smallStartAd++;
        }
        
        return  1;
    }else{
        //just use tmp memory data
        //must confirm large than space;
        if (memory>=space) {
            startAd=memoryStartAd;
            endAd=memoryStartAd+space-1;
            ptrSchedule->releaseNode->startAd=memoryStartAd;
            ptrSchedule->releaseNode->endAd=endAd;
            ptrSchedule->startAd=memoryStartAd;
            while (memoryStartAd <= endAd) {
                buffer[memoryStartAd]='1';
                memoryStartAd++;
            }
            return  1;
        }else{
            //not sufficient to provide space to allocate
            //than mean fail allocate
            
            ptrSchedule->releaseNode->startAd=-1;
            ptrSchedule->releaseNode->endAd=-1;
            
            return 0;
        }
    }
    return -1;
}
int WorstPutIntoMemory(Schedule_node *ptrSchedule){
    int i=0,count=0;
    int minimalLocation=0;
    int startAd,endAd;
    int memoryStartAd=0,memoryEndAd=0;
    int flag=false;
    int space=ptrSchedule->alloSpace;
    int Largestflag=MEMORYSIZE+1;
    int LargeStartAd=0,LargeEndAd=0;
    for (i=0; i<MEMORYSIZE-1; i++) {
        if (buffer[i]=='0') {
            
            if (buffer[i+1]=='1') {
                //end
                memoryEndAd=i;
                if ((memoryEndAd-memoryStartAd)+1 >= space) {
                    
                    if ((memoryEndAd-memoryStartAd)+1 > Largestflag ) {
                        Largestflag=(memoryEndAd-memoryStartAd)+1;
                        LargeStartAd=memoryStartAd;
                        LargeEndAd=memoryEndAd;
                    }
                }
                
            }else if (buffer[i+1=='0']){
                memoryEndAd=i+1;
            }
        }else if (buffer[i]=='1'){
            if (buffer[i+1]=='1') {
                
            }else if(buffer[i+1]=='0'){
                memoryStartAd=i+1;
            }
        }
    }
    
    if (Largestflag!=MEMORYSIZE+1) {
        //compare two
        if ((LargeEndAd-LargeStartAd)+1 > (memoryEndAd-memoryStartAd+1)) {
            startAd=LargeStartAd;
            endAd=endAd;
            
            ptrSchedule->releaseNode->endAd=endAd;
            ptrSchedule->releaseNode->startAd=startAd;
            ptrSchedule->startAd=startAd;
            
            while (startAd<=endAd) {
                buffer[startAd]='1';
                startAd++;
            }
        }
    }else{
        if ((memoryEndAd-memoryStartAd)+1 >= space) {
            
            //use memoryAd to allocate
            startAd=memoryStartAd;
            endAd=memoryEndAd;
            
            ptrSchedule->releaseNode->endAd=endAd;
            ptrSchedule->releaseNode->startAd=startAd;
            ptrSchedule->startAd=startAd;
            
            while (startAd<=endAd) {
                buffer[startAd]='1';
                startAd++;
            }
            
        }else{
            //cant putinto
        }
        
    }
    
    return -1;
}

void releaseMemory(Schedule_node *ptrSchedule){
    int flag=-1;
    if (ptrSchedule->startAd==-1 && ptrSchedule->endAd ==-1) {
        //Not enter Memory at all , skip
        flag=0;
    }
    else{
        //Free Memory
        while (ptrSchedule->startAd<=ptrSchedule->endAd) {
            buffer[ptrSchedule->startAd]='0';
            ptrSchedule->startAd++;
        }
        flag=1;
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