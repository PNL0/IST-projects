#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include "eventlist.h"
#include "parser.h"
#include "operations.h"

static struct EventList* event_list = NULL;
static unsigned int state_access_delay_ms = 0;

/// Calculates a timespec from a delay in milliseconds.
/// @param delay_ms Delay in milliseconds.
/// @return Timespec with the given delay.
static struct timespec delay_to_timespec(unsigned int delay_ms) {
  return (struct timespec){delay_ms / 1000, (delay_ms % 1000) * 1000000};
}

/// Gets the event with the given ID from the state.
/// @note Will wait to simulate a real system accessing a costly memory resource.
/// @param event_id The ID of the event to get.
/// @return Pointer to the event if found, NULL otherwise.
static struct Event* get_event_with_delay(unsigned int event_id) {
  struct timespec delay = delay_to_timespec(state_access_delay_ms);
  nanosleep(&delay, NULL);  // Should not be removed

  return get_event(event_list, event_id);
}

/// Gets the seat with the given index from the state.
/// @note Will wait to simulate a real system accessing a costly memory resource.
/// @param event Event to get the seat from.
/// @param index Index of the seat to get.
/// @return Pointer to the seat.
static unsigned int* get_seat_with_delay(struct Event* event, size_t index) {
  struct timespec delay = delay_to_timespec(state_access_delay_ms);
  nanosleep(&delay, NULL);  // Should not be removed

  return &event->data[index];
}

/// Gets the index of a seat.
/// @note This function assumes that the seat exists.
/// @param event Event to get the seat index from.
/// @param row Row of the seat.
/// @param col Column of the seat.
/// @return Index of the seat.
static size_t seat_index(struct Event* event, size_t row, size_t col) { return (row - 1) * event->cols + col - 1; }

int lock(FileArgs *args, pthread_mutex_t *mutex) {
    if (pthread_mutex_lock(mutex) != 0) {
      fprintf(stderr, "unable to lock thread\n");
      *args->flag = 2;
      free(args);
      return 1;
    }

    return 0;
}

int unlock(FileArgs *args, pthread_mutex_t *mutex) {
    if (pthread_mutex_unlock(mutex) != 0) {
      fprintf(stderr, "unable to unlock thread\n");
      *args->flag = 2;
      free(args);
      return 1;
    }

    return 0;
}

int read_lock(FileArgs *args, pthread_rwlock_t *lock){
    if (pthread_rwlock_rdlock(lock) != 0) {
      fprintf(stderr, "unable to lock thread\n");
      *args->flag = 2;
      free(args);
      return 1;
    }

    return 0;
}

int write_lock(FileArgs *args, pthread_rwlock_t *lock){
    if (pthread_rwlock_wrlock(lock) != 0) {
      fprintf(stderr, "unable to lock thread\n");
      *args->flag = 2;
      free(args);
      return 1;
    }

    return 0;
}

int rd_unlock(FileArgs *args, pthread_rwlock_t *lock) {
    if (pthread_rwlock_unlock(lock) != 0) {
      fprintf(stderr, "unable to unlock thread\n");
      *args->flag = 2;
      free(args);
      return 1;
    }

    return 0;
}

int ems_init(unsigned int delay_ms) {
  if (event_list != NULL) {
    fprintf(stderr, "EMS state has already been initialized\n");
    return 1;
  }

  event_list = create_list();
  state_access_delay_ms = delay_ms;

  return event_list == NULL;
}

int ems_terminate() {
  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  free_list(event_list);
  return 0;
}

int ems_create(unsigned int event_id, size_t num_rows, size_t num_cols, FileArgs *args) {

  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  if(write_lock(args, &event_list->list_lock) != 0) {
    return 2;
  }

  if (get_event_with_delay(event_id) != NULL) {
    fprintf(stderr, "Event already exists\n");
    if(rd_unlock(args, &event_list->list_lock) != 0) {
      return 2;
    }
    return 1;
  }

  struct Event* event = malloc(sizeof(struct Event));

  if (event == NULL) {
    fprintf(stderr, "Error allocating memory for event\n");
    if(rd_unlock(args, &event_list->list_lock) != 0) {
      return 2;
    }
    return 1;
  }

  if(pthread_mutex_init(&event->mutex, NULL) != 0){
    fprintf(stderr, "Error in the event mutex init instruction\n");
    rd_unlock(args, &event_list->list_lock);
    *args->flag = 2;
    free(args);
    return 2;
  }

  event->id = event_id;
  event->rows = num_rows;
  event->cols = num_cols;
  event->reservations = 0;
  event->data = malloc(num_rows * num_cols * sizeof(unsigned int));

  if (event->data == NULL) {
    fprintf(stderr, "Error allocating memory for event data\n");
    free(event);
    if(rd_unlock(args, &event_list->list_lock) != 0) {
      return 2;
    }
    return 1;
  }

  for (size_t i = 0; i < num_rows * num_cols; i++) {
    event->data[i] = 0;
  }
  
  if (append_to_list(event_list, event) != 0) {
    fprintf(stderr, "Error appending event to list\n");
    free(event->data);
    free(event);
    if(rd_unlock(args, &event_list->list_lock) != 0) {
      return 2;
    }
    return 1;
  }

  if(rd_unlock(args, &event_list->list_lock) != 0) {
    return 2;
  }

  return 0;
}

int ems_reserve(unsigned int event_id, size_t num_seats, size_t* xs, size_t* ys, FileArgs *args) {
  
  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  if(read_lock(args, &event_list->list_lock) != 0) {
    return 2;
  }

  struct Event* event = get_event_with_delay(event_id);

  if(rd_unlock(args, &event_list->list_lock) != 0) {
    return 2;
  }
  
  if (event == NULL) {
    fprintf(stderr, "Event not found\n");
    return 1;
  }

  if(lock(args, &event->mutex) != 0)
    return 2;

  unsigned int reservation_id = ++event->reservations;

  size_t i = 0;
  for (; i < num_seats; i++) {
    size_t row = xs[i];
    size_t col = ys[i];

    if (row <= 0 || row > event->rows || col <= 0 || col > event->cols) {
      fprintf(stderr, "Invalid seat\n");
      break;
    }

    if (*get_seat_with_delay(event, seat_index(event, row, col)) != 0) {
      fprintf(stderr, "Seat already reserved\n");
      break;
    }

    *get_seat_with_delay(event, seat_index(event, row, col)) = reservation_id;
  }

  // If the reservation was not successful, free the seats that were reserved.
  if (i < num_seats) {
    event->reservations--;
    for (size_t j = 0; j < i; j++) {
      *get_seat_with_delay(event, seat_index(event, xs[j], ys[j])) = 0;
    }
    if(unlock(args, &event->mutex) != 0)
      return 2;

    return 1;
  }
  if(unlock(args, &event->mutex) != 0)
    return 2;

  return 0;
}

int ems_show(unsigned int event_id, char *output_filename, int fd_out, FileArgs *args) {
  
  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  if(read_lock(args, &event_list->list_lock) != 0) {
    return 2;
  }

  struct Event* event = get_event_with_delay(event_id);

  if(rd_unlock(args, &event_list->list_lock) != 0) {
    return 2;
  }

  if (event == NULL) {
    fprintf(stderr, "Event not found\n");
    return 1;
  }

  if(lock(args, &event->mutex) != 0)
    return 2;

  for (size_t i = 1; i <= event->rows; i++) {
    for (size_t j = 1; j <= event->cols; j++) {
      unsigned int* seat = get_seat_with_delay(event, seat_index(event, i, j));
      char str[32];
      sprintf(str, "%u", *seat);
      if(!check_write(output_filename,fd_out,str,strlen(str))){
        if(unlock(args, &event->mutex) != 0)
          return 2;
        return 1;
      }
      
      if (j < event->cols) {
        char *buffer1 = " ";
        if(!check_write(output_filename,fd_out,buffer1,strlen(buffer1))) {
          if(unlock(args, &event->mutex) != 0)
            return 2;
          return 1;
        }
      }
    }
    char *buffer2 = "\n";
    if(!check_write(output_filename,fd_out, buffer2, strlen(buffer2))) {
      if(unlock(args, &event->mutex) != 0)
        return 2;
      return 1;
    }
  }
  if(unlock(args, &event->mutex) != 0)
    return 2;
  return 0;
}

int ems_list_events(char *output_filename, int fd_out, FileArgs *args) {
 
  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  if(read_lock(args, &event_list->list_lock) != 0) {
    return 2;
  }

  if (event_list->head == NULL) {
    char *buffer3 = "No events\n";
    if(!check_write(output_filename, fd_out, buffer3, strlen(buffer3))){
      if(rd_unlock(args, &event_list->list_lock) != 0) {
        return 2;
      }
      return 1;
    }
    if(rd_unlock(args, &event_list->list_lock) != 0) {
      return 2;
    }
    return 0;
  }

  struct ListNode* current = event_list->head;
  while (current != NULL) {
    char *buffer4 = "Event: ";
    if(!check_write(output_filename, fd_out, buffer4, strlen(buffer4))) {
      if(rd_unlock(args, &event_list->list_lock) != 0) {
        return 2;
      }
      return 1;
    }

    char str[32];
    sprintf(str, "%u\n", (current->event)->id);
    if(!check_write(output_filename, fd_out, str, strlen(str))) {
      if(rd_unlock(args, &event_list->list_lock) != 0) {
        return 2;
      }
      return 1;
    }

    current = current->next;
  }
  if(rd_unlock(args, &event_list->list_lock) != 0) {
    return 2;
  }
  return 0;
}

void ems_wait(unsigned int delay_ms) {
  struct timespec delay = delay_to_timespec(delay_ms);
  nanosleep(&delay, NULL);
}