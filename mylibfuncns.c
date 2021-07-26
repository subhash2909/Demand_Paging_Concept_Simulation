#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<errno.h>
#include "myheader.h"

typedef struct {
  int frm_no;
  int valid_bit;
  int modify_bit;
  int reference_bit;
  int counter;// counter field
} pageItem;

int pagesize = rec_per_page * RECORDSIZE;
char * Frames[no_of_frames];
int fd;
int FrameTable[no_of_frames];
int page_fault_count = 0;
pageItem PageTable[PTABLESIZE];


int logical_clock = 0;//counter set to 0 initially


/*  **********************    FUNCTIONS   ************************************ */


int getFrameNo(int pno) {

  int fno;


//If the page is already present in memory-->update counter field of corresponding page and return corresponding frame_no
  if (PageTable[pno].valid_bit != 0) {
    printf("Page already present in memory\n");
    PageTable[pno].counter = ++logical_clock;
    //print_frame_table();print_page_table();
    return PageTable[pno].frm_no;
  }

//If page is not found,control comes here


//Check if a free frame is available
//if it is available-->bring corresponding page into that frame and update counter,and return that free frame_no

  for (int i = 0; i < no_of_frames; i++)
    if (FrameTable[i] == -1) {
      printf("Free frame available\nreading the page into a free frame\n");
      readPage(pno, i);
      update_page_table(pno, i, 1, 0);
      PageTable[pno].counter = ++ logical_clock;
      //print_frame_table();print_page_table();
      return i;
    }



//control comes here if no free frame is available
  printf("No free frame available\nLRU algorithm invoked\n");


//find victim_pno that should be swapped out
  int victim_pno = LRU_countre_based_replacement_algo();


//print victim page_no
  printf("Victim_pno--->%d\n", victim_pno);


//get frm_no of corresponding victim_page_no
  fno = PageTable[victim_pno].frm_no;



//if modify_bit of victim_page_no is set(i.e record updated),write back modified data(record) of victim_page_no into student_data before overwriting that frame
  if (PageTable[victim_pno].modify_bit == 1)writeFrame(fno, victim_pno);


//update page entry of victim_page.(since victim_page is going to be overwritten)
//victim->frm=-1,victim->validbit=0,victim->modify_bit=0
  update_page_table(victim_pno, -1, 0, 0);



//Read the requested page into this freed frame,update the counter in corresponding page_entry
  readPage(pno, fno);
  update_page_table(pno, fno, 1, 0); //valid_bit of pno->1,modified_bit->0
  PageTable[pno].counter = ++logical_clock;
  //print_frame_table();print_page_table();

//return fno
  return fno;
}



//finds the victim_page_no(page with least counter value,i.e;least recently used)
int LRU_countre_based_replacement_algo() {

  int victim_pno;
  int min_counter = 100000000;//minimum counter set to infinity

  for (int i = 0; i < PTABLESIZE; i++) {
    if (PageTable[i].valid_bit == 1 && PageTable[i].counter < min_counter) { //if page is present in memory and counter is less than minimum counter
      min_counter = PageTable[i].counter;
      victim_pno = i;
    }
  }

  return victim_pno;
}




void set_modify_bit(int pno) {//invoked whenever record is updated
  PageTable[pno].modify_bit = 1;
}



void print_page_fault_count() {//prints page_fault_count
  printf("page fault count is---->  %d\n", page_fault_count);
}



void update_page_table(int pno, int frm_no, int valid_bit, int modify_bit) { //updates corresponding page_entry details
  PageTable[pno].frm_no = frm_no;
  PageTable[pno].valid_bit = valid_bit;
  PageTable[pno].modify_bit = modify_bit;
}




//prints frame_table( to check frame_table contents at any point of time)

void print_frame_table() {
  printf("frame table\n ");
  for (int i = 0; i < no_of_frames; i++)
    printf("%d---->%d\n", i, FrameTable[i]);
}




//prints page_table( to check page_table  contents at any point of time)

void print_page_table() { //prints page_table()
  printf("page table\n");
  for (int i = 0; i < PTABLESIZE; i++)
    printf("%d %d %d %d\n", PageTable[i].frm_no, PageTable[i].valid_bit, PageTable[i].modify_bit, PageTable[i].counter);
}



//prints number of frames
void print_no_of_frames() {
  printf("Number of frames: %d\n\n", no_of_frames);
}





/******************************************************************************************************************/
/******************************************************************************************************************/


/* Initialization Function: Initializes all aspects */
void Initialize() {
  int i;
  /*  Allocating Memory to Frames, and initializing the FrateTable */
  for (i = 0; i < no_of_frames; i++) {
    Frames[i] = (char *) calloc(pagesize, sizeof(char));
    FrameTable[i] = -1; // -1 indicates frame is empty
  }
  /* Initializing the PageTable */
  for (i = 0; i < PTABLESIZE; i++) {
    PageTable[i].frm_no = -1;
    PageTable[i].valid_bit = 0;
    PageTable[i].modify_bit = 0;
    PageTable[i].reference_bit = 0;
    PageTable[i].counter = 0; //change
  }
  /*  Opening the Data File  */
  fd = open("./student-data.csv", O_RDWR );
  if (fd < 0) {
    printf("Error Number % d\n", errno);
    perror("Program");
    exit(1);
  }
}

/* Finalization Function: Finalizes all aspects */
void Finalize() {
  int i;
  /* Write back all non-empty frames to file, and freeing the memory allocated to all frames */
  for (i = 0; i < no_of_frames; i++) {
    if (FrameTable[i] != -1) {
      writeFrame(i, FrameTable[i]);
    }
    free(Frames[i]);
  }
  /* Closing the Data File */
  close(fd);
  if (fd < 0)printf("Error! Could not close the data file...\n");
}

/* Reads specified page from disk-file to the specified Frame in memory */
void readPage(int pno, int frm_no) {
  lseek(fd, (pno * pagesize), SEEK_SET);
  read(fd, Frames[frm_no], pagesize);
  FrameTable[frm_no] = pno;
  page_fault_count++;
}

/* Writes the contents of the specified frame in memory to the specified page of the disk-file */
void writeFrame(int frm_no, int pno) {
  int res;
  lseek(fd, (pno * pagesize), SEEK_SET);
  res = write(fd, Frames[frm_no], pagesize);
  if (res < 0) {
    perror("Write Error.");
    exit(1);
  }
}

/* Displays the contents of the specified memory-frame */
void printFrame(int frm_no) {
  int i;
  for (i = 0; i < pagesize; i++) {
    printf("%c", Frames[frm_no][i]);
  }
  printf("\n");
}

/* Translates the specified slno (record-number) to its corresponding page no. and page offset values */
void getPageDetails(int slno, int *pno, int *offset) {
  int q, r;
  q = (slno - 1) / rec_per_page;
  r = (slno - 1) - (rec_per_page * q);
  *pno = q;
  *offset = r;
}

/* Displays the RECORD contained at the specified offset of the specified memory-frame */
void printRecord(int frm_no, int offset) {
  int i, startindx, endindx;
  startindx = offset * RECORDSIZE;
  endindx = startindx + RECORDSIZE;
  for (i = startindx; i < endindx; i++) {
    printf("%c", Frames[frm_no][i]);
  }
  printf("\n");
}


/* Updates the Name Field of the RECORD contained at the specified offset of
the specified memory-frame, with the given string */
void updateRecord(int frm_no, int offset, char *string) {
  int i, startindx, endindx;
  if (strlen(string) != 4) {
    printf("Length of the input string must be equal to 4.\n");
    exit(0);
  }
  else {
    startindx = (offset * RECORDSIZE) + 14;
    for (i = 0; i < 4; i++) Frames[frm_no][startindx++] = string[i];
  }
}
