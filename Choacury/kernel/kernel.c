#include "kernel.h"


// Main operating system

pcb_t pcb[ 16 ], *current = NULL;
int runningPrograms[16];
int ipcFlag =0;
uint32_t sizeOfContext;
void forkProgram(ctx_t* ctx);
void printNum(int num);
void incAges();
int highestPriority();
dp_data initData();
int pickFork();
void putDownForks();
void exitProgram(ctx_t *ctx);
void resetAge(int pid);
void putDownForks();
dp_data *data;


// Process scheduler
void scheduler( ctx_t* ctx ) {
    //Go to next pcb
    if (ipcFlag == 0) {
        int currentProgram = current->pid;
        incAges();
        int next = highestPriority();
        if (next != currentProgram){
            memcpy( &pcb[ currentProgram ].ctx, ctx, sizeof( ctx_t ) );
            memcpy( ctx, &pcb[ next ].ctx, sizeof( ctx_t ) );


            current = &pcb[ next ];
            //printNum(next);

        }
        resetAge(next);
    }
}

// Process increment process age
void incAges (){
    for (int i = 0; i <16; i ++){
        if (runningPrograms[i] != -1){
            runningPrograms[i]+= pcb[i].priority;
        }
    }
}

// Reset process age
void resetAge(int pid){
    runningPrograms[pid] = pcb[pid].priority + (runningPrograms[pid] - pcb[pid].priority )/2;
}

// Find highest priority process
int highestPriority () {
    int highestPriority = -1;
    int highestPID = -1;
    for (int i = 0; i < 16; i++) {
        if (runningPrograms[i] > highestPriority) {
            highestPID = pcb[i].pid;
            highestPriority = runningPrograms[i];
        }
    }
    return highestPID;
}

//Program to find the next program available
int nextProgram (){
    for (int i = 0; i <16; i ++){
        if (runningPrograms[i] == -1){
            return(i);
        }
    }
    writeStr("Max Programs reached, overwriting program 0");
    return 0;
}

// Reset the kernel
void kernel_handler_rst(ctx_t* ctx) {
    numPrograms = 0;
    sizeOfContext = sizeof(ctx_t);

    for (int i = 0; i < 16; i++) {
        memset( &pcb[ i ], 0, sizeof( pcb_t ) );
        runningPrograms[i] = -1;

    }

    pcb[ 0 ].pid      = 0;
    pcb[ 0 ].priority = 1;
    pcb[ 0 ].ctx.cpsr = 0x50;
    pcb[ 0 ].ctx.pc   = ( uint32_t )( entry_P0 );
    pcb[ 0 ].ctx.sp   = ( uint32_t )(  &tos_Programs );

    runningPrograms[numPrograms] = 1;


    numPrograms ++;


    pcb[ 1 ].pid      = 1;
    pcb[ 1 ].ctx.cpsr = 0x50;
    pcb[ 1 ].priority = 1;
    pcb[ 1 ].ctx.pc   = ( uint32_t )( entry_P0 );
    pcb[ 1 ].ctx.sp   = ( uint32_t )(  &tos_Programs - ( 0x00001000 ));
    runningPrograms[numPrograms] = 1;
    numPrograms ++;


    pcb[ 2 ].pid      = 2;
    pcb[ 2 ].ctx.cpsr = 0x50;
    pcb[ 2 ].priority = 1;
    pcb[ 2 ].ctx.pc   = ( uint32_t )( entry_P0 );
    pcb[ 2 ].ctx.sp   = ( uint32_t )(  &tos_Programs - ( 0x00002000 ));
    runningPrograms[numPrograms] = 1;
    numPrograms ++;

    pcb[ 3 ].pid      = 3;
    pcb[ 3 ].ctx.cpsr = 0x50;
    pcb[ 3 ].ctx.pc   = ( uint32_t )( entry_P0 );
    pcb[ 3 ].ctx.sp   = ( uint32_t )(  &tos_Programs - ( 0x00003000 ));


    current = &pcb[ 0 ];

    memcpy( ctx, &current->ctx, sizeof( ctx_t ) );
    writeStr("reset\n");
    //*data = initData();

////////////////////////////////////////////////////////////
dp_data initData;
initData.max =2;
initData.noWithForks = 0;
initData.forks[0] = 18;
initData.forks[1] = 18;
initData.forks[2] = 18;
initData.forks[3] = 17;
initData.forks[4] = 17;
initData.forks[5] = 17;
initData.forks[6] = 17;
initData.forks[7] = 17;
initData.forks[8] = 17;
initData.forks[9] = 17;
initData.forks[10] = 17;
initData.forks[11] = 17;
initData.forks[12] = 17;
initData.forks[13] = 17;
initData.forks[14] = 17;
initData.forks[15] = 17;
data = &initData;
////////////////////////////////////////////////////////////

  /* Configure the mechanism for interrupt handling by
   *
   * - configuring timer st. it raises a (periodic) interrupt for each
   *   timer tick,
   * - configuring GIC st. the selected interrupts are forwarded to the
   *   processor via the IRQ interrupt signal, then
   * - enabling IRQ interrupts.
   */

   UART0->IMSC           |= 0x00000010; // enable UART    (Rx) interrupt
   UART0->CR              = 0x00000301; // enable UART (Tx+Rx)
   GICD0->ISENABLER[ 1 ] |= 0x00001010; // enable timer  and GIC        interrupt

   TIMER0->Timer1Load     = 0x00100000; // select period = 2^20 ticks ~= 1 sec
   TIMER0->Timer1Ctrl     = 0x00000002; // select 32-bit   timer
   TIMER0->Timer1Ctrl    |= 0x00000040; // select periodic timer
   TIMER0->Timer1Ctrl    |= 0x00000020; // enable          timer interrupt
   TIMER0->Timer1Ctrl    |= 0x00000080; // enable          timer

   GICC0->PMR             = 0x000000F0; // unmask all            interrupts
   GICC0->CTLR            = 0x00000001; // enable GIC interface
   GICD0->CTLR            = 0x00000001; // enable GIC distributor


   irq_enable();

   return;
}


void kernel_handler_svc( ctx_t* ctx, uint32_t id ) {


    switch( id ) {
        case 0x00 : { // yield()
            scheduler( ctx );
            break;
        }
        case 0x01 : { // write( fd, x, n )
            int   fd = ( int   )( ctx->gpr[ 0 ] );
            char*  x = ( char* )( ctx->gpr[ 1 ] );
            int    n = ( int   )( ctx->gpr[ 2 ] );

            for( int i = 0; i < n; i++ ) {
                PL011_putc( UART0, *x++ );
            }

            ctx->gpr[ 0 ] = n;
            break;
        }
        case 0x02 : {
            writeStr("SVC FORK\n");
            //scheduler(ctx);
            forkProgram(ctx);
            //scheduler(ctx);
            break;
        }
        case 0x03 : {
            writeStr("SVC EXIT\n");
            exitProgram(ctx);
            break;
        }
        case 0x04 : {

            ctx->gpr[ 0 ] = pickFork();
            break;
        }
        case 0x05 : {

            putDownForks();
            break;
        }
        default   : {
            break;
        }
    }
}

// Handle interupt request
void kernel_handler_irq(ctx_t* ctx) {
    //  read  the interrupt identifier so we know the source.

    uint32_t id = GICC0->IAR;

    if( id == GIC_SOURCE_TIMER0 ) {
        TIMER0->Timer1IntClr = 0x01;
        scheduler(ctx);
    }

    else if( id == GIC_SOURCE_UART0 ) {
      uint8_t x = PL011_getc( UART0 );
      PL011_putc( UART0, 'f' );


      if (x ==  'f') {
          fork();
      } else if (x ==  'e'){
          exitP();
      } else if (x ==  's'){
          scheduler(ctx);
      }


      UART0->ICR = 0x10;
    }


    GICC0->EOIR = id;
    return;
}

// Handle process fork
void forkProgram(ctx_t* ctx){
    if (numPrograms < 16) {

        //Get unique ID
        int newID = nextProgram();


        // Calculate top of stack of parent
        uint32_t tos_Parent = (uint32_t) &tos_Programs - ( current->pid * ( 0x00001000 ));
        //( uint32_t )(  &tos_Programs - ( 0x00003000 ));

        //Calculate the top of the stack of child
        uint32_t tos_Child = ( uint32_t ) ((int) &tos_Programs) - ( (newID) * ( 0x00001000 ));

        //Calculate the position of the stack pointer of child process

        uint32_t sp_Child = (uint32_t) tos_Child + (tos_Parent - ctx->sp);


        //Increment number of programs
        numPrograms++;


        //Set new programs properties
        pcb[ newID ].pid      = newID;
        pcb[ newID ].ctx.pc   = ( uint32_t )( entry_P2 );
        pcb[ newID ].ctx.cpsr = 0x50;

        pcb[ newID ].ctx.sp   = tos_Child;
        //pcb[ newID ].ctx.lr   = ctx->lr;
        pcb[ newID ].priority = current->priority;

        //Copy context



        //Copy stack - 0x0..0 because it copys from the bottem of the stack,
        //memcpy(tos_Child - 0x00001000, tos_Parent - 0x00001000, 0x00001000 );
        memcpy( ( void* ) (tos_Child - 0x00001000), ( void* ) (tos_Parent - 0x00001000), 0x00001000);


        //Set the new stack pointer


        //Add program to running programs list
        runningPrograms[newID] = pcb[newID].priority;

        writeStr("Fork\n");
        return;

    } else {
        //writeStr("Memory Full\n");
        return;
    }
}

// Exit a processes
void exitProgram(ctx_t* ctx){

    //int next = nextProgram();
    //memcpy( ctx, &pcb[ next ].ctx, sizeof( ctx_t ) );
    runningPrograms[current->pid] = -1;
    scheduler(ctx);

    //memset( &current, 0, sizeof( pcb_t ) );

    //current = &pcb[next];

    return;
}

void printNum(int num) {
    char buffer[6];
    itoa(num,buffer);
    for (int j=0; j<2;j++){
        PL011_putc( UART0, buffer[j]);
    }
    return;
}


// Initialise data
dp_data initData(){
    dp_data initData;
	initData.max = 0;
    data->forks[0] = 0;
    data->forks[1] = 1;
    data->forks[2] = 2;
    data->forks[3] = -1;
    data->forks[4] = -1;
    data->forks[5] = -1;
    data->forks[6] = -1;
    data->forks[7] = -1;
    data->forks[8] = -1;
    data->forks[9] = -1;
    data->forks[10] = -1;
    data->forks[11] = -1;
    data->forks[12] = -1;
    data->forks[13] = -1;
    data->forks[14] = -1;
    data->forks[15] = -1;

	 initData.noWithForks = 0;
    return initData;
}

// dining philosophers handler
int pickFork(){
    //If noeating >= max return 0
    //if rightfork available return 1
    //if leftfork available return 0
    dp_data newData;
    newData = *data;
    int pid = current->pid;

    if (newData.noWithForks > newData.max){
        data = &newData;
        printNum(pid);
        writeStr(" Attempted eating but could not\n");
        return 0;
        //if the fork is available
    } else if ((newData.forks[pid] == 18) /*&& (data->forks[pid] != pid)*/){
        newData.forks[pid] = pid;
        if (newData.forks[(pid+numPrograms-1)%numPrograms] == pid){
            data = &newData;
            printNum(pid);
            writeStr(" Picked up right fork and started eating\n");
            return 2;
        } else {
        newData.noWithForks ++;
        data = &newData;
        printNum(pid);
        writeStr(" Picked up right fork\n");
        return 1; }
    } else if ((newData.forks[(pid+numPrograms-1)%numPrograms] == 18)/*&& (data->forks[((pid+15)%16)] != pid)*/){
        newData.forks[(pid+numPrograms-1)%numPrograms] = pid;
        //newData.noWithForks ++;
        if (newData.forks[pid] == pid){
            data = &newData;
            printNum(pid);
            writeStr(" Picked up left fork and started eating\n");
            return 2;
        } else {
        newData.noWithForks ++;
        data = &newData;
        printNum(pid);
        writeStr(" Picked up left fork\n");
        return 1;
    }
    }
    return 0;
}

// Dining philosophers handler
void putDownForks(){
    //reduce holding forks by one

    dp_data newData;
    newData = *data;

    newData.noWithForks --;
    int pid = current->pid;
    newData.forks[pid] = 18;
    newData.forks[(pid+numPrograms-1)%numPrograms] = 18;
    data = &newData;
    printNum(current->pid);
    writeStr(" Finished Eating and now thinking\n");
    return;
}

int getIpcFlag(){
    return ipcFlag;
}

void setIpcFlag(int i){
    ipcFlag = i;
    return;
}
