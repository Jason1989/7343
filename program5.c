#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define CLOCK_SIZE 4

struct Page {
      int frameId;
      int pageId;
      int use;
      int modify;
};

struct Page * Page(int frameId, int pageId, int use, int modify) {
     struct Page * p = malloc(sizeof(struct Page));
     if (p) {
          p->frameId = frameId;
          p->pageId = pageId;
          p->use = use;
          p->modify = modify;
     }
     return p;
}

/*
1.   Read a record.

2.   Check to see if the page in the record is already in the clock and if it is update to appropriate flag(s).  (This will not reposition the next frame pointer)

3.   If step 2 fails, beginning at the current position of the next frame pointer scan the clock.   During this scan make no changes to the use bit.  The first frame encountered with (u = 0; m = 0) is selected for replacement.

4.   If step 3 fails, scan the clock again.  The first frame encountered with (u = 0; m = 1) is selected for replacement.  During this scan, set the use bit to 0 for each frame that is bypassed.

5.   If step 4 fails, the frame pointer will have once again returned to its original position and all the frames will have a use bit of 0.  Repeat step 3 and, if necessary, step 4.  By time a frame will be found for replacement.

6.   Write the updated clock to file results.txt. 

7.   Repeat 1-6 until end of testdata.txt reached
*/

void writeModification(FILE* filePtr, int next, struct Page* pages[4]){
      for(int i = 0; i < CLOCK_SIZE; i++){
            struct Page *page = pages[i];
            if(i == next){
                  fprintf(filePtr, "%5d%10d%10d%10d%15s\n", page->frameId, page->pageId, page->use, page->modify, "<- next frame");
            }else{
                  fprintf(filePtr, "%5d%10d%10d%10d\n", page->frameId, page->pageId, page->use, page->modify);
            }
      }
}

void step2(int *res, FILE* filePtr, int pp, char operation, int next, struct Page* pages[4]){
      for (int i = next; i < CLOCK_SIZE; i++) {
            struct Page *page = pages[i];
            if(page->pageId == pp){
                  page->use = 1;
                  if(operation == 'w'){
                        page->modify = 1;
                  }
                  writeModification(filePtr, next, pages);
                  res[0] = 1;
                  return;
            }
      }
      for(int i = 0; i < next; i++){
            struct Page *page = pages[i];
            if(page->pageId == pp){
                  page->use = 1;
                  if(operation == 'w'){
                        page->modify = 1;
                  }
                  writeModification(filePtr, next, pages);
                  res[0] = 1;
                  return;
            }
      }
      res[0] = 0;
}

void step3(int *res, FILE* filePtr, int pp, char operation, int next, struct Page* pages[4]){
      for (int i = next; i < CLOCK_SIZE; i++) {
            struct Page *page = pages[i];
            if(page->use == 0 && page->modify == 0){
                  page->pageId = pp;
                  page->use = 1;
                  if(operation == 'w'){
                        page->modify = 1;
                  }else{
                        page->modify = 0;
                  }
                  writeModification(filePtr, (i + 1) % CLOCK_SIZE, pages);
                  res[0] = 1;
                  res[1] = (i + 1) % CLOCK_SIZE;
                  return;
            }
      }
      for(int i = 0; i < next; i++){
            struct Page *page = pages[i];
            if(page->use == 0 && page->modify == 0){
                  page->pageId = pp;
                  page->use = 1;
                  if(operation == 'w'){
                        page->modify = 1;
                  }else{
                        page->modify = 0;
                  }
                  writeModification(filePtr, (i + 1) % CLOCK_SIZE, pages);
                  res[0] = 1;
                  res[1] = (i + 1) % CLOCK_SIZE;
                  return;
            }
      }
      res[0] = 0;
}

void step4(int *res, FILE* filePtr, int pp, char operation, int next, struct Page* pages[4]){
      for (int i = next; i < CLOCK_SIZE; i++) {
            struct Page *page = pages[i];
            if(page->use == 0 && page->modify == 1){
                  page->pageId = pp;
                  page->use = 1;
                  if(operation == 'w'){
                        page->modify = 1;
                  }else{
                        page->modify = 0;
                  }
                  writeModification(filePtr, (i + 1) % CLOCK_SIZE, pages);
                  res[0] = 1;
                  res[1] = (i + 1) % CLOCK_SIZE;
                  return;
            }else{
                 page->use = 0; 
            }
      }
      for(int i = 0; i < next; i++){
            struct Page *page = pages[i];
            if(page->use == 0 && page->modify == 1){
                  page->pageId = pp;
                  page->use = 1;
                  if(operation == 'w'){
                        page->modify = 1;
                  }else{
                        page->modify = 0;
                  }
                  writeModification(filePtr, (i + 1) % CLOCK_SIZE, pages);
                  res[0] = 1;
                  res[1] = (i + 1) % CLOCK_SIZE;
                  return;
            }else{
                 page->use = 0; 
            }
      }
      res[0] = 0;
}

int writeClockToFile(FILE* filePtr, int pp, char operation, int next, struct Page* pages[4]) {
      fprintf(filePtr, "%5s%10s%10s%10s\n","FRAME","PAGE","USE","MODIFY");
      int res[2] = {0, next};
      //step 2
      step2(res,filePtr, pp, operation, next, pages);
      if(res[0] == 1) return res[1];
      //step 3
      step3(res, filePtr, pp, operation, next, pages);
      if(res[0] == 1) return res[1];
      //step 4
      step4(res, filePtr, pp, operation, next, pages);
      if(res[0] == 1) return res[1];
      //step 5
      for (int i = next; i < CLOCK_SIZE; i++) {
            struct Page *page = pages[i];
            page->use = 0;
      }
      for(int i = 0; i < next; i++){
            struct Page *page = pages[i];
            page->use = 0;
      }
      //repeat step 3 and step 4
      step3(res, filePtr, pp, operation, next, pages);
      if(res[0] == 1) return res[1];
      step4(res, filePtr, pp, operation, next, pages);
      if(res[0] == 1) return res[1];
      return next;
}

struct Page *pages[CLOCK_SIZE];


int main()  {
      char inFileName[ ] = "testdata.txt";
      FILE *inFilePtr = fopen(inFileName, "r");
      if(inFilePtr == NULL) {
            printf("File %s could not be opened.\n", inFileName);
            exit(1);
      }
      char outFileName[ ] = "results.txt";
      FILE *outFilePtr = fopen(outFileName, "w");
      int pp;
      char operation;
      if(outFilePtr == NULL) {
            printf("File %s could not be opened.\n", outFileName);
            exit(1);
      }
      for (int i = 0; i < CLOCK_SIZE; i++) {
            struct Page *page = Page(i, -1, 0, 0);
            pages[i] = page;
      }
      fscanf(inFilePtr, "%d%c", &pp, &operation);
      int next = 0;
      while(!feof(inFilePtr)) {
            fprintf(outFilePtr, "Page referenced: %d %c\n", pp, operation);
            next = writeClockToFile(outFilePtr, pp, operation, next, pages);
            fscanf(inFilePtr, "%d%c", &pp, &operation);
      }//end while
      fclose(inFilePtr);
      fclose(outFilePtr);
      return 0;
}