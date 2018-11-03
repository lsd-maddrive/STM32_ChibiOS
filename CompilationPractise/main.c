#include <core.h>

static workers_count_t  g_workers = 0x14;

#define ONE_WORKER_APPLES_PER_DAY   20
#define ONE_WORKER_ORANGES_PER_DAY  15

void working_day ( garden_statistics_t *stat )
{
  stat->apple_count  += *(stat->global_workers) * ONE_WORKER_APPLES_PER_DAY * (1 - stat->insects_perc);
  stat->orange_count += *(stat->global_workers) * ONE_WORKER_ORANGES_PER_DAY * (1 - stat->insects_perc);

}

void another_working_day ( garden_statistics_t stat )
{
  stat.apple_count  += *(stat.global_workers) * ONE_WORKER_APPLES_PER_DAY * (1 - stat.insects_perc);
  stat.orange_count += *(stat.global_workers) * ONE_WORKER_ORANGES_PER_DAY * (1 - stat.insects_perc);
}

int main ( void )
{
    garden_statistics_t   day_gard;
    garden_statistics_t   night_gard;

    garden_statistics_t     *dg_ptr = &day_gard;

    // Result
    day_gard.apple_count = day_gard.orange_count = 0;
    night_gard.apple_count = night_gard.orange_count = 0;

    // Input
    day_gard.insects_perc = 0.2;
    night_gard.insects_perc = 0.3;

    day_gard.global_workers = &g_workers;
    night_gard.global_workers = &g_workers;

//    show_my_data();`
//    show_my_age( 23 );

    for ( int day = 1; day <= 10; day++ )
    {
//        printf( "Hello! +) Processing idx (%d)\n", i );
        printf( "Day %d is happening\n", day );
        working_day( &day_gard );
        working_day( &night_gard );

        // What if here we change <insects_perc>
    }

    printf( "Day garden: apples (%d), oranges (%d)\n", day_gard.apple_count, day_gard.orange_count );
    printf( "Night garden: apples (%d), oranges (%d)\n", night_gard.apple_count, night_gard.orange_count );

    return 0;
}
