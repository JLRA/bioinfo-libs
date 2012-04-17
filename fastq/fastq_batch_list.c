
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fastq_batch_list.h"


//=====================================================
// functions to manage fastq batch list item
//=====================================================

//-----------------------------------------------------
// fastq_batch_list_item_free
//
// free memory for this structure,
// the input parameter 'all' indicates free memory
// associated to the pointers in his structure
//-----------------------------------------------------

void fastq_batch_list_item_free(fastq_batch_list_item_t* item_p, int all) {
  if (all) {
    char log_message[50];
    sprintf(log_message, "fastq_batch_list_item_free, free batch_p !!!");
    LOG_DEBUG(log_message);
    
    free(item_p->batch_p);
  }
  free(item_p);
}


//=====================================================
// functions to manage fastq batch list
//=====================================================

//-----------------------------------------------------
// fastq_batch_list_init
//
// init to zero the object and initialize the lock
//-----------------------------------------------------

void fastq_batch_list_init(fastq_batch_list_t* list_p, int producers) {
   memset(list_p, 0, sizeof(fastq_batch_list_t));
   //list_p->lock = PTHREAD_MUTEX_INITIALIZER;
   pthread_mutex_init(&(list_p->lock), NULL);
   list_p->producers = producers;
}

//-----------------------------------------------------
// fastq_batch_list_insert
//
// insert a batch object in the end of the list,
// according to fifo order,
//-----------------------------------------------------

void fastq_batch_list_insert(fastq_batch_list_item_t* item_p, fastq_batch_list_t* list_p) {

  if (list_p==NULL) return;

  pthread_mutex_lock(&list_p->lock);

  if (list_p->first_p==NULL) {
    //item_p->id = 0;
    item_p->prev_p = NULL;
    item_p->next_p = NULL;
    list_p->first_p = item_p;
    list_p->last_p = item_p;
  } else {
    list_p->last_p->next_p = item_p;
    //item_p->id = list_p->last_p->id + 1;
    item_p->prev_p = list_p->last_p;
    item_p->next_p = NULL;
    list_p->last_p = item_p;
  }
  list_p->length++;

  pthread_mutex_unlock(&list_p->lock);
}

//-----------------------------------------------------
// fastq_batch_list_remove
//
// remove the first batch object in the begining
// of the list,
// according to fifo order,
//-----------------------------------------------------

fastq_batch_list_item_t* fastq_batch_list_remove(fastq_batch_list_t* list_p) {

  if (list_p==NULL) return NULL;

  pthread_mutex_lock(&list_p->lock);

  // just get the first element, and if is not null update the first-element pointer
  fastq_batch_list_item_t* item_p = list_p->first_p;
  if (item_p!=NULL) {
    list_p->first_p = item_p->next_p;
    list_p->length--;
  }

  pthread_mutex_unlock(&list_p->lock);

  return item_p;
}

//-----------------------------------------------------
// fastq_batch_list_length
//
// returns list length
//-----------------------------------------------------

int fastq_batch_list_length(fastq_batch_list_t* list_p) {

  int length = 0;

  if (list_p==NULL) return length;

  pthread_mutex_lock(&list_p->lock);
  length = list_p->length;
  pthread_mutex_unlock(&list_p->lock);

  return length;
}

//-----------------------------------------------------
// fastq_batch_list_length_by_source_id
//
// returns list length for a given source id
//-----------------------------------------------------

int fastq_batch_list_length_by_source_id(fastq_batch_list_t* list_p, int source_id) {

  int length = 0;

  if (list_p==NULL) return length;

  pthread_mutex_lock(&list_p->lock);
  length = list_p->length_by_source_id[source_id];
  pthread_mutex_unlock(&list_p->lock);

  return length;
}

//-----------------------------------------------------
// fastq_batch_list_get_producers
//
// insert a batch object in the end of the list,
// according to fifo order,
//-----------------------------------------------------

int fastq_batch_list_get_producers(fastq_batch_list_t* list_p) {

  int producers = 0;

  if (list_p==NULL) return producers;

  pthread_mutex_lock(&list_p->lock);
  producers = list_p->producers;
  pthread_mutex_unlock(&list_p->lock);

  return producers;
}

//-----------------------------------------------------
// fastq_batch_list_incr_producers
//
// insert a batch object in the end of the list,
// according to fifo order,
//-----------------------------------------------------

int fastq_batch_list_incr_producers(fastq_batch_list_t* fastq_batch_list_p) {

  if (fastq_batch_list_p==NULL) return 0;

  pthread_mutex_lock(&fastq_batch_list_p->lock);
  fastq_batch_list_p->producers++;
  pthread_mutex_unlock(&fastq_batch_list_p->lock);

  return fastq_batch_list_p->producers;
}

//-----------------------------------------------------
// fastq_batch_list_decr_producers
//
// decrement in 1 the number of producers in the list
//
//-----------------------------------------------------

int fastq_batch_list_decr_producers(fastq_batch_list_t* fastq_batch_list_p) {

  if (fastq_batch_list_p==NULL) return 0;

  pthread_mutex_lock(&fastq_batch_list_p->lock);
  fastq_batch_list_p->producers--;
  pthread_mutex_unlock(&fastq_batch_list_p->lock);

  return fastq_batch_list_p->producers;
}

//-----------------------------------------------------
// print batch list
//
//
//-----------------------------------------------------

void fastq_batch_list_print(fastq_batch_list_t* fastq_batch_list_p) {
	int i;

	if(fastq_batch_list_p==NULL) {
		return;
	}

	pthread_mutex_lock(&fastq_batch_list_p->lock);

	printf("Number of items: %i\n", fastq_batch_list_p->length);

	fastq_batch_list_item_t* item_p = fastq_batch_list_p->first_p;

	while(item_p != NULL) {
		printf("batch id: %i, source id: %i\n", item_p->id, item_p->batch_p->source_id);
		printf("\treads: %li\n", item_p->batch_p->num_reads);

		for(i=0; i<item_p->batch_p->num_reads; i++) {
			printf("i: %d\n", i);
			printf("%s\n", &(item_p->batch_p->header[item_p->batch_p->header_indices[i]]));
			//printf("%s\n", &(item_p->batch_p->data[item_p->batch_p->data_indices[i]]));
			//printf("%s\n", &(item_p->batch_p->data[(item_p->batch_p->data_indices[i]+item_p->batch_p->data_indices[i+1])/2]));
			printf("%s\n", &(item_p->batch_p->seq[item_p->batch_p->data_indices[i]]));
			printf("%s\n", &(item_p->batch_p->quality[item_p->batch_p->data_indices[i]]));
		}

		item_p = item_p->next_p;
	}

	pthread_mutex_unlock(&fastq_batch_list_p->lock);
}

//-----------------------------------------------------
// qc_batch_list_items_free
//
// free list itmes
//
//-----------------------------------------------------

void fastq_batch_list_items_free(fastq_batch_list_t* list_p) {
  fastq_batch_list_item_t* item_p;
  char log_message[50];

  while ((item_p = fastq_batch_list_remove(list_p)) != NULL) {
    sprintf(log_message, "liberating fastq item %i\n", item_p->id);
    LOG_DEBUG(log_message);
    
    fastq_batch_list_item_free(item_p, 1);
  }
}
