#ifndef VCF_BATCH_H
#define VCF_BATCH_H

#include <log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "vcf_file_structure.h"

//====================================================================================
//  vcf_batch.h
//
//  vcf_batch_t structures and prototypes
//====================================================================================

/**
 * List who is equivalent to a batch of VCF records.
 */
typedef list_t vcf_batch_t;


vcf_batch_t* vcf_batch_new(size_t size);

void vcf_batch_free(vcf_batch_t *vcf_batch);

void add_record_to_batch(vcf_record_t *record, vcf_batch_t *vcf_batch);

int batch_is_empty(vcf_batch_t *vcf_batch);

int batch_is_full(vcf_batch_t *vcf_batch);

void vcf_batch_print(FILE *fd, vcf_batch_t *vcf_batch);

#endif
