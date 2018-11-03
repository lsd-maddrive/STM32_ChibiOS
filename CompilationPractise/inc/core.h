#ifndef CORE_H_
#define CORE_H_

#include <stdio.h>

typedef     uint32_t    fruit_count_t;
typedef     uint16_t    workers_count_t;

typedef struct garden_statistics {
  uint32_t          id;

  fruit_count_t     apple_count;
  fruit_count_t     orange_count;

  float             insects_perc;
  workers_count_t   *global_workers;

} garden_statistics_t;


/* 
 * Configuration block 
 */

#define MY_NAME         "Alexey"
#define MY_PROFESSION   "Engineer"

// Prototypes

void show_my_data( void );
void show_my_age ( int age );

#endif /* CORE_H_ */ 
