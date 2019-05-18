#include "ku_malloc.h"
#define KU_MAGIC 0x19311946U

void *malloc(size_t size) {

	hheader_t *hptr = NULL;
	void *ptr = NULL;
	hnode_t tmp; //to get tmp value
	hnode_t *prevNode = NULL; //to change prevNode's next value
	hnode_t *curr = NULL; //to traverse through the free-list
	bool nextfit = false;
	size_t alloc_size = size + sizeof(hheader_t); //used when sbrk

	if(size == 0) {
		fprintf(stderr, "[Alloc] Addr: %p Length: 0\n", ptr);
		return NULL;
	}

	/*check free_list
	int count = get_node_count(free_list);
	hnode_t *current = free_list;
	for (int i = 0; i < count; i++ ) {
		current = get_node_at(free_list, i);
		fprintf(stderr, "free_list[%d]'s size : %lu, next pointer : %p\n", i, current->size, current->next);
    	}*/

	if(free_list == NULL) {
		hptr = sbrk(alloc_size);
		if(hptr != (void *)-1) { //sbrk possible
			ptr = (void *)hptr + sizeof(hheader_t);
			hptr->size = size;
			hptr->magic = (unsigned long)ptr ^ KU_MAGIC;
			//fprintf(stderr, "hptr Addr : %p\n", hptr);
		}
	} else { //next-fit
		curr = free_list; //free_list head
		if(curr->next == NULL && size <= curr->size) {
			//if free_list has only one node, allocate possible
			if(curr->size - size <= sizeof(hnode_t)) {
				size = curr->size; //allocate that free_node all.
				hptr = (hheader_t *)curr;
				ptr = (void *)hptr + sizeof(hheader_t);
				hptr->size = size;
				hptr->magic = (unsigned long)ptr ^ KU_MAGIC;
				//no splitting needed but free_list should be changed
				free_list = NULL;
			} else {
				tmp.size = curr->size; //original data saved
				hptr = (hheader_t *)curr;
				ptr = (void *)hptr + sizeof(hheader_t);
				hptr->size = size;
				hptr->magic = (unsigned long)ptr ^ KU_MAGIC;
				//splitting
				free_list = (void *)ptr + size;
				free_list->size = tmp.size - hptr->size - sizeof(hnode_t);
			}
			alloc_last_node = curr;
			nextfit = true;
		} else { //if free_list has more than one node
			while(curr->next != NULL) {
				if(alloc_last_node == NULL && size <= curr->size) { //first allocation
					if(curr->size - size <= sizeof(hnode_t)) {
						tmp.next = curr->next;
						size = curr->size;
						hptr = (hheader_t *)curr;
						ptr = (void *)hptr + sizeof(hheader_t);
						hptr->size = size;
						hptr->magic = (unsigned long)ptr ^ KU_MAGIC;
						remove_node(&free_list, curr);
						if(prevNode != NULL)
							prevNode->next = tmp.next;
						else
							free_list = (void *)curr;
					} else {
						tmp.size = curr->size; //original data saved
						tmp.next = curr->next;
						hptr = (hheader_t *)curr;
						ptr = (void *)hptr + sizeof(hheader_t);
						hptr->size = size;
						hptr->magic = (unsigned long)ptr ^ KU_MAGIC;
						//splitting
						curr = (void *)ptr + size;
						curr->size = tmp.size - hptr->size - sizeof(hnode_t);
						curr->next = tmp.next;
						*(hnode_t *)curr = *curr;
						if(prevNode != NULL)
							prevNode->next = (void *)curr;
						else
							free_list = (void *)curr;
					}
					alloc_last_node = curr;
					nextfit = true;
					break;
				} else if(alloc_last_node != curr && size <= curr->size) { //satisfy next-fit
					if(curr->size - size <= sizeof(hnode_t)) {
						tmp.next = curr->next;
						size = curr->size;
						hptr = (hheader_t *)curr;
						ptr = (void *)hptr + sizeof(hheader_t);
						hptr->size = size;
						hptr->magic = (unsigned long)ptr ^ KU_MAGIC;
						remove_node(&free_list, curr);
						if(prevNode != NULL)
							prevNode->next = tmp.next;
						else
							free_list = (void *)curr;
					} else {
						tmp.size = curr->size; //original data saved
						tmp.next = curr->next;
						hptr = (hheader_t *)curr;
						ptr = (void *)hptr + sizeof(hheader_t);
						hptr->size = size;
						hptr->magic = (unsigned long)ptr ^ KU_MAGIC;
						//splitting
						curr = (void *)ptr + size;
						curr->size = tmp.size - hptr->size - sizeof(hnode_t);
						curr->next = tmp.next;
						*(hnode_t *)curr = *curr;
						if(prevNode != NULL)
							prevNode->next = (void *)curr;
						else
							free_list = (void *)curr;
					}
					alloc_last_node = curr;
					nextfit = true;
					break;
				} else {
					//nextfit = false;
				}
				prevNode = curr;
				curr = curr->next;
			}
			if(!nextfit) { //can't next-fit -> there are two cases(next-fit starts from the beginning, sbrk)
				curr = free_list;
				while(curr->next != NULL) {
					if(size <= curr->size) {
						if(curr->size - size <= sizeof(hnode_t)) {
							tmp.next = curr->next;
							size = curr->size;
							hptr = (hheader_t *)curr;
							ptr = (void *)hptr + sizeof(hheader_t);
							hptr->size = size;
							hptr->magic = (unsigned long)ptr ^ KU_MAGIC;
							remove_node(&free_list, curr);
							if(prevNode != NULL)
								prevNode->next = tmp.next;
							else
								free_list = (void *)curr;
						} else {
							tmp.size = curr->size; //original data saved
							tmp.next = curr->next;
							hptr = (hheader_t *)curr;
							ptr = (void *)hptr + sizeof(hheader_t);
							hptr->size = size;
							hptr->magic = (unsigned long)ptr ^ KU_MAGIC;
							//splitting
							curr = (void *)ptr + size;
							curr->size = tmp.size - hptr->size - sizeof(hnode_t);
							curr->next = tmp.next;
							*(hnode_t *)curr = *curr;
							if(prevNode != NULL)
								prevNode->next = (void *)curr;
							else
								free_list = (void *)curr;
						}
						alloc_last_node = curr;
						nextfit = true;
						break;
					}
					prevNode = curr;
					curr = curr->next;			
				}
				if(!nextfit) { //if still next-fit impossible, sbrk
					hptr = sbrk(alloc_size);
					if(hptr != (void *)-1) { //sbrk possible
						ptr = (void *)hptr + sizeof(hheader_t);
						hptr->size = size;
						hptr->magic = (unsigned long)ptr ^ KU_MAGIC;
					}
				}
			}
		}		
	}

	if(hptr > 0) {
		fprintf(stderr, "[Alloc] Addr: %p Length: %lu\n", ptr, size);
		return ptr;
	} else { //ex. sbrk() returns (void *)-1
		fprintf(stderr, "[Alloc] Addr: NULL Length: %lu\n", size);
		return NULL;
	}
}

void free(void *ptr) {
	hheader_t *hptr = (void *)ptr - sizeof(hheader_t); //header address
	assert(hptr->magic == (unsigned long)ptr ^ KU_MAGIC); //abort the program if assertion is false
	
	hnode_t newNode;
	newNode.size = hptr->size;
	newNode.next = free_list;

	*(hnode_t *)hptr = newNode;
	free_list = (hnode_t *)hptr;
	fprintf(stderr, "[Free]  Addr: %p Length: %lu\n", ptr, newNode.size);

	//after free, coalescing
}