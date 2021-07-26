# Demand_Paging_Concept_Simulation

page replacement algorithms decide which memory pages to page out, sometimes called swap out, or write to disk, when a page of memory needs to be allocated.Page replacement happens when a requested page is not in memory (page fault interrupt occurs) and a free page cannot be used to satisfy the allocation, either because there are none, or because the number of free pages is lower than some threshold.

LRU(least recently used replacement policy)
It is based on the fact that the pages that are used recently have very high probability to be used in the near future.LRU swaps out the page which is least  recently used  and thus allocating the page requested by processor to corresponding frame.

IMPLEMENTATION:
1Countre_based_implementation
2Stack_based_implementation
![image](https://user-images.githubusercontent.com/54262847/127028423-6f2a72dc-7131-4a8e-bd9a-51c8b8e590f5.png)

Countre_based_implementation![image](https://user-images.githubusercontent.com/54262847/127028486-d1b93897-38f9-460a-b608-105b055708cc.png)

To keep track which page is least recently used ,we maintain a counter(sort of a logical clock) field in Page_Table[] additionally.

whenever  free frame is not available ,we scan through the page table[] and swap out the process which has least counter filed.
![image](https://user-images.githubusercontent.com/54262847/127028573-a2ca8e6c-ece3-47a8-9d33-6ac91a67ac3a.png)

SOURCE CODE EXPALANATION

Whenever user enters a record_number,there can be following  2 possibilities:

1the page corresponding to record is already in the memory(i.e;frames[]).

    So,at first,we check if it is already present or not by checking (PageTable[pno].valid_bit==1).
    If it is already present,we update the global_counter and assign it to PageTable[pno].counter and return
    corresponding frm_no ,i.e; PageTable[pno].frm_no.

2if the corresponding page is not present,Then

   a)
        check if free frame is available or not.
       If it is available,then read the corresponding page into this frame and update counterfield, set valid_bit
       to 1(since it is now present in memory),modify_bit to 0,frm_no to this frame.

      Then simply return that free frame_no.
![image](https://user-images.githubusercontent.com/54262847/127028658-b0682ff5-b66a-4331-9e88-ee7d2b565d91.png)
