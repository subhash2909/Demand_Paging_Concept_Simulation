#include<stdio.h>
#include<stdlib.h>
#include "myheader.h"

int main() {
  int i, pno, fno, offset, recno;
  char ch;
  char str[5];


  print_no_of_frames();

  Initialize();
  while (1) {
    printf("Enter Record No: ");
    scanf("%d", &recno);
    if ((recno < 0) || (recno > 94)) {
      printf("Invalid Record Number !!! \n");
      break;
    }
    getPageDetails(recno, &pno, &offset);
    printf("Page No = %d, Page Offset = %d\n", pno, offset);
    fno = getFrameNo(pno);
    //printf("Printing Frame %d Before the update\n", fno);
    //printFrame(fno);
    printf("Details of the Requseted Record:-\n");
    printRecord(fno, offset);
    printf("Do you want to update the record (Y/N): ");
    scanf(" %c", &ch);
    if ((ch == 'Y') || (ch == 'y')) {
      printf("Enter the new name (4 characters):");
      scanf(" %[^\n]%*c", str);
      updateRecord(fno, offset, str);
      set_modify_bit(pno);                //sets modify_bit to 1 if corresponding  page is updated

      //printf("Printing Frame %d After the update\n", fno);
      //printFrame(fno);
      printf("Details of the Record after Updaed :-\n");
      printRecord(fno, offset);
    }

  }
  Finalize();
  print_page_fault_count();  //prints page_fault_count at last...

  return 0;
}
