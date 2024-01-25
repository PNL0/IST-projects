#ifndef EMS_OPERATIONS_H
#define EMS_OPERATIONS_H

#include <stddef.h>
#include <dirent.h>
#include <pthread.h>

typedef struct {
    int fd_jobs, fd_out;
    unsigned int id; 
    int *thread_id_to_wait, *delay_to_wait;
    char *output_filename;
    int *flag;
    pthread_mutex_t *mutex;
} FileArgs;

int lock(FileArgs *args, pthread_mutex_t *mutex);
int unlock(FileArgs *args, pthread_mutex_t *mutex);
int read_lock(FileArgs *args, pthread_rwlock_t *lock);
int write_lock(FileArgs *args, pthread_rwlock_t *lock);
int rd_unlock(FileArgs *args, pthread_rwlock_t *lock);

/// Initializes the EMS state.
/// @param delay_ms State access delay in milliseconds.
/// @return 0 if the EMS state was initialized successfully, 1 otherwise.
int ems_init(unsigned int delay_ms);

/// Destroys the EMS state.
int ems_terminate();

/// Creates a new event with the given id and dimensions.
/// @param event_id Id of the event to be created.
/// @param num_rows Number of rows of the event to be created.
/// @param num_cols Number of columns of the event to be created.
/// @return 0 if the event was created successfully, 1 otherwise.
int ems_create(unsigned int event_id, size_t num_rows, size_t num_cols, FileArgs *args);

/// Creates a new reservation for the given event.
/// @param event_id Id of the event to create a reservation for.
/// @param num_seats Number of seats to reserve.
/// @param xs Array of rows of the seats to reserve.
/// @param ys Array of columns of the seats to reserve.
/// @return 0 if the reservation was created successfully, 1 otherwise.
int ems_reserve(unsigned int event_id, size_t num_seats, size_t *xs, size_t *ys, FileArgs *args);

/// Prints the given event.
/// @param event_id Id of the event to print.
/// @return 0 if the event was printed successfully, 1 otherwise.
int ems_show(unsigned int event_id, char *output_filename, int fd_out, FileArgs *args);

/// Prints all the events.
/// @return 0 if the events were printed successfully, 1 otherwise.
int ems_list_events(char *output_filename, int fd_out, FileArgs *args);

/// Waits for a given amount of time.
/// @param delay_us Delay in milliseconds.
void ems_wait(unsigned int delay_ms);

#endif  // EMS_OPERATIONS_H