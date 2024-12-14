#define _POSIX_C_SOURCE 200112L
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define GRID_SIZE 20
#define NUM_THREADS 4
#define GENERATIONS 32

int grid[GRID_SIZE][GRID_SIZE];
int temp[GRID_SIZE][GRID_SIZE];
pthread_barrier_t barrier;

void print_grid()
{
    system("clear");
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (grid[i][j] == 1)
            {
                printf("# ");
            }
            else
            {
                printf("  ");
            }
        }
        printf("\n");
    }
    usleep(500000);
}

void *compute_next_gen(void *arg)
{
    int id = *(int *)arg;
    int thread_r = GRID_SIZE / NUM_THREADS;
    int start = id * thread_r;
    int end;
    if (id == NUM_THREADS - 1)
    {
         end = GRID_SIZE;
    }
    else
    {
         end = start + thread_r;
    }

    for (int gen_index = 0; gen_index < GENERATIONS; gen_index++)
    {
        for (int i = start; i < end; i++)
        {
            for (int j = 0; j < GRID_SIZE; j++)
            {
                int Alive = 0;

                for (int x = -1; x <= 1; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        if (x == 0 && y == 0)
                            continue; 
                        Alive += grid[(i + x + GRID_SIZE) % GRID_SIZE][(j + y + GRID_SIZE) % GRID_SIZE];
                    }
                }

                if (grid[i][j] == 1)
                {
                    if (Alive == 2 || Alive == 3)
                    {
                        temp[i][j] = 1;
                    }
                    else
                    {
                        temp[i][j] = 0;
                    }
                }
                else
                {
                    if (Alive == 3)
                    {
                        temp[i][j] = 1;
                    }
                    else
                    {
                        temp[i][j] = 0;
                    }
                }
            }
        }

        pthread_barrier_wait(&barrier);

        if (id == 0)
        {
            for (int i = 0; i < GRID_SIZE; i++)
            {
                for (int j = 0; j < GRID_SIZE; j++)
                {
                    grid[i][j] = temp[i][j];
                }
            }
            print_grid();
        }

        pthread_barrier_wait(&barrier);
    }

    return NULL;
}

void initialize_grid(int grid[GRID_SIZE][GRID_SIZE])
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            grid[i][j] = 0; 
        }
    }
}

void initialize_patterns(int grid[GRID_SIZE][GRID_SIZE])
{
    initialize_grid(grid);

    grid[1][1] = 1;
    grid[1][2] = 1;
    grid[2][1] = 1;
    grid[2][2] = 1;

    grid[5][6] = 1;
    grid[6][6] = 1;
    grid[7][6] = 1;

    grid[10][10] = 1;
    grid[11][11] = 1;
    grid[12][9] = 1;
    grid[12][10] = 1;
    grid[12][11] = 1;
}

int main()
{
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    initialize_patterns(grid);
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);

    print_grid();

    for (int i = 0; i < NUM_THREADS; i++)
    {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, compute_next_gen, &thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);
    return 0;
}
