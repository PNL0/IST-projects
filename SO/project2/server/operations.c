#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "common/io.h"
#include "eventlist.h"

static struct EventList* event_list = NULL;
static unsigned int state_access_delay_us = 0;

/// Gets the event with the given ID from the state.
/// @note Will wait to simulate a real system accessing a costly memory resource.
/// @param event_id The ID of the event to get.
/// @param from First node to be searched.
/// @param to Last node to be searched.
/// @return Pointer to the event if found, NULL otherwise.
static struct Event* get_event_with_delay(unsigned int event_id, struct ListNode* from, struct ListNode* to) {
  struct timespec delay = {0, state_access_delay_us * 1000};
  nanosleep(&delay, NULL);  // Should not be removed

  return get_event(event_list, event_id, from, to);
}

/// Gets the index of a seat.
/// @note This function assumes that the seat exists.
/// @param event Event to get the seat index from.
/// @param row Row of the seat.
/// @param col Column of the seat.
/// @return Index of the seat.
static size_t seat_index(struct Event* event, size_t row, size_t col) { return (row - 1) * event->cols + col - 1; }

int ems_init(unsigned int delay_us) {
  if (event_list != NULL) {
    fprintf(stderr, "EMS state has already been initialized\n");
    return 1;
  }

  event_list = create_list();
  state_access_delay_us = delay_us;

  return event_list == NULL;
}

int ems_terminate() {
  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  if (pthread_rwlock_wrlock(&event_list->rwl) != 0) {
    fprintf(stderr, "Error locking list rwl\n");
    return 1;
  }

  pthread_rwlock_unlock(&event_list->rwl);
  free_list(event_list);
  return 0;
}

int ems_create(unsigned int event_id, size_t num_rows, size_t num_cols) {
  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  if (pthread_rwlock_wrlock(&event_list->rwl) != 0) {
    fprintf(stderr, "Error locking list rwl\n");
    return 1;
  }

  if (get_event_with_delay(event_id, event_list->head, event_list->tail) != NULL) {
    fprintf(stderr, "Event already exists\n");
    pthread_rwlock_unlock(&event_list->rwl);
    return 1;
  }

  struct Event* event = malloc(sizeof(struct Event));

  if (event == NULL) {
    fprintf(stderr, "Error allocating memory for event\n");
    pthread_rwlock_unlock(&event_list->rwl);
    return 1;
  }

  event->id = event_id;
  event->rows = num_rows;
  event->cols = num_cols;
  event->reservations = 0;
  if (pthread_mutex_init(&event->mutex, NULL) != 0) {
    pthread_rwlock_unlock(&event_list->rwl);
    free(event);
    return 1;
  }
  event->data = calloc(num_rows * num_cols, sizeof(unsigned int));

  if (event->data == NULL) {
    fprintf(stderr, "Error allocating memory for event data\n");
    pthread_rwlock_unlock(&event_list->rwl);
    free(event);
    return 1;
  }

  if (append_to_list(event_list, event) != 0) {
    fprintf(stderr, "Error appending event to list\n");
    pthread_rwlock_unlock(&event_list->rwl);
    free(event->data);
    free(event);
    return 1;
  }

  pthread_rwlock_unlock(&event_list->rwl);
  return 0;
}

int ems_reserve(unsigned int event_id, size_t num_seats, size_t* xs, size_t* ys) {
  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  if (pthread_rwlock_rdlock(&event_list->rwl) != 0) {
    fprintf(stderr, "Error locking list rwl\n");
    return 1;
  }

  struct Event* event = get_event_with_delay(event_id, event_list->head, event_list->tail);

  pthread_rwlock_unlock(&event_list->rwl);

  if (event == NULL) {
    fprintf(stderr, "Event not found\n");
    return 1;
  }

  if (pthread_mutex_lock(&event->mutex) != 0) {
    fprintf(stderr, "Error locking mutex\n");
    return 1;
  }

  for (size_t i = 0; i < num_seats; i++) {
    if (xs[i] <= 0 || xs[i] > event->rows || ys[i] <= 0 || ys[i] > event->cols) {
      fprintf(stderr, "Seat out of bounds\n");
      pthread_mutex_unlock(&event->mutex);
      return 1;
    }
  }

  for (size_t i = 0; i < event->rows * event->cols; i++) {
    for (size_t j = 0; j < num_seats; j++) {
      if (seat_index(event, xs[j], ys[j]) != i) {
        continue;
      }

      if (event->data[i] != 0) {
        fprintf(stderr, "Seat already reserved\n");
        pthread_mutex_unlock(&event->mutex);
        return 1;
      }

      break;
    }
  }

  unsigned int reservation_id = ++event->reservations;

  for (size_t i = 0; i < num_seats; i++) {
    event->data[seat_index(event, xs[i], ys[i])] = reservation_id;
  }

  pthread_mutex_unlock(&event->mutex);
  return 0;
}

int ems_show(int out_fd, unsigned int event_id) {
  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  if (pthread_rwlock_rdlock(&event_list->rwl) != 0) {
    fprintf(stderr, "Error locking list rwl\n");
    return 1;
  }

  struct Event* event = get_event_with_delay(event_id, event_list->head, event_list->tail);

  pthread_rwlock_unlock(&event_list->rwl);

  if (event == NULL) {
    fprintf(stderr, "Event not found\n");
    return 1;
  }

  if (pthread_mutex_lock(&event->mutex) != 0) {
    fprintf(stderr, "Error locking mutex\n");
    return 1;
  }

  // Write response to client ems_show
  char buffer[sizeof(int) + 2*sizeof(size_t) + sizeof(unsigned int) * event->rows * event->cols];
  int returno = 0;
  memcpy(buffer, &returno, sizeof(int));
  memcpy(buffer + sizeof(int), &event->rows, sizeof(size_t));
  memcpy(buffer + sizeof(int) + sizeof(size_t), &event->cols, sizeof(size_t));
  memcpy(buffer + sizeof(int) + 2*sizeof(size_t), event->data, sizeof(unsigned int) * event->rows * event->cols);
  
  if(!check_write(out_fd, buffer, sizeof(buffer))) {
    fprintf(stderr, "Error writing to client response FIFO \n");
    pthread_mutex_unlock(&event->mutex);
    return 1;
  }

  pthread_mutex_unlock(&event->mutex);
  return 0;
}

int ems_list_events(int out_fd) {
  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  if (pthread_rwlock_rdlock(&event_list->rwl) != 0) {
    fprintf(stderr, "Error locking list rwl\n");
    return 1;
  }

  // Find num_events
  struct ListNode* to = event_list->tail;
  struct ListNode* current = event_list->head;
  size_t num_events = (size_t) 0;
  if (current != NULL) {
    while (1) {
      num_events++;
      if (current == to) {
        break; 
      }
      current = current->next;
    }
  }
  current = event_list->head;
  // Write response to client ems_list_events
  char buffer[sizeof(int) + sizeof(size_t) + sizeof(unsigned int) * num_events];
  int returno = 0;
  memcpy(buffer, &returno, sizeof(int));
  memcpy(buffer + sizeof(int), &num_events, sizeof(size_t));

  if(num_events > (size_t) 0) {
    unsigned int ids[num_events];
    for(size_t i = 0; i < num_events; i++) {
      ids[i] = current->event->id;
      if (current == to) {
          break;
      }
      current = current->next;
    }
    memcpy(buffer + sizeof(int) + sizeof(size_t), ids, sizeof(unsigned int) * num_events);
  }
  
  if(!check_write(out_fd, buffer, sizeof(buffer))) {
    fprintf(stderr, "Error writing to client response FIFO \n");
    pthread_rwlock_unlock(&event_list->rwl);
    return 1;
  }
  
  pthread_rwlock_unlock(&event_list->rwl);
  return 0;
}

int list_and_show_each_event() {
  if (event_list == NULL) {
    fprintf(stderr, "EMS state must be initialized\n");
    return 1;
  }

  if (pthread_rwlock_rdlock(&event_list->rwl) != 0) {
    fprintf(stderr, "Error locking list rwl\n");
    return 1;
  }

  if(event_list->head == NULL && event_list->tail == NULL) {
    char buff[] = "No events\n";
    if (print_str(STDOUT_FILENO, buff)) {
      perror("Error writing to file descriptor");
      pthread_rwlock_unlock(&event_list->rwl);
      return 1;
    }
  }
  else { 
    struct ListNode* current = event_list->head;
    while(current != NULL) {
      // Writes the current event_id
      char buff[] = "Event: ";
      if (print_str(STDOUT_FILENO, buff)) {
        perror("Error writing to file descriptor");
        pthread_rwlock_unlock(&event_list->rwl);
        return 1;
      }
      char id[16];
      sprintf(id, "%u\n", current->event->id);
      if (print_str(STDOUT_FILENO, id)) {
        perror("Error writing to file descriptor");
        pthread_rwlock_unlock(&event_list->rwl);
        return 1;
      }  
      // Writes the show current event output 
      for (size_t i = 1; i <= current->event->rows; i++) {
        for (size_t j = 1; j <= current->event->cols; j++) {
          char buffer2[16];
          sprintf(buffer2, "%u", current->event->data[seat_index(current->event, i, j)]);
          if (print_str(STDOUT_FILENO, buffer2)) {
            perror("Error writing to file descriptor");
            pthread_rwlock_unlock(&event_list->rwl);
            return 1;
          }
          if (j < current->event->cols) {
            if (print_str(STDOUT_FILENO, " ")) {
              perror("Error writing to file descriptor");
              pthread_rwlock_unlock(&event_list->rwl);
              return 1;
            }
          }
        }
        if (print_str(STDOUT_FILENO, "\n")) {
          perror("Error writing to file descriptor");
          pthread_rwlock_unlock(&event_list->rwl);
          return 1;
        }
      }
      current = current->next;
    }
  }
  pthread_rwlock_unlock(&event_list->rwl);
  return 0;
}