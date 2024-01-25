#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h> 
#include <signal.h>
#include "common/constants.h"
#include "common/io.h"
#include "operations.h"
#include "eventlist.h"

int global_flag = 0; // Active when a SIGUSR1 is detected
int waiting_requests = 0;

pthread_mutex_t read_buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition2 = PTHREAD_COND_INITIALIZER;

int prd_ind = 0;
int con_ind = 0;
typedef struct {
  char req_pipe_path[40];
  char resp_pipe_path[40];
} ClientFIFOs;

ClientFIFOs requests_buffer[MAX_SESSION_COUNT];

int process_client(int fd_request, int fd_response, int session_id) {
  int flag = 0;
  while(!flag) {
    unsigned int event_id;
    char op_code;
    size_t num_rows, num_cols, num_seats, *xs, *ys;
    int returned;
    
    if(!check_read(fd_request, &op_code, sizeof(op_code))) {
      fprintf(stderr, "Failed to read request from client\n");
      return 1;
    }
  
    int req_session_id;
    if(!check_read(fd_request, &req_session_id, sizeof(req_session_id))) {
      fprintf(stderr, "Failed to read request from client\n");
      return 1;
    }
    if(req_session_id != session_id) {
      fprintf(stderr, "Client session_id is diferent from what was expected\n");
      return 1;
    }
    
    switch (op_code) {
      case '2': // client ems_quit
        flag = 1;
        break;
      
      case '3': // client ems_create
        if(!check_read(fd_request, &event_id, sizeof(event_id))) {
          fprintf(stderr, "Failed to read request from client\n");
          return 1;
        }
        if(!check_read(fd_request, &num_rows, sizeof(num_rows))) {
          fprintf(stderr, "Failed to read request from client\n");
          return 1;
        }
        if(!check_read(fd_request, &num_cols, sizeof(num_cols))) {
          fprintf(stderr, "Failed to read request from client\n");
          return 1;
        }
        
        returned = ems_create(event_id, num_rows, num_cols);
        if(!check_write(fd_response, &returned, sizeof(returned))) {
          fprintf(stderr, "Failed to write response for client\n");
          return 1;
        }
        break;

      case '4': // client ems_reserve
        if(!check_read(fd_request, &event_id, sizeof(event_id))) {
          fprintf(stderr, "Failed to read request from client\n");
          return 1;
        }
        if(!check_read(fd_request, &num_seats, sizeof(num_seats))) {
          fprintf(stderr, "Failed to read request from client\n");
          return 1;
        }

        xs = (size_t*)malloc(sizeof(size_t) * num_seats);
        if(xs == NULL) {
          fprintf(stderr, "Unable to allocate memory for xs array\n");
          return 1;
        }
        ys = (size_t*)malloc(sizeof(size_t) * num_seats);
        if(ys == NULL) {
          fprintf(stderr, "Unable to allocate memory for ys array\n");
          free(xs);
          return 1;
        }
        if(!check_read(fd_request, xs, sizeof(size_t) * num_seats)) {
          fprintf(stderr, "Failed to read request from client\n");
          free(xs);
          free(ys);
          return 1;
        }
        if(!check_read(fd_request, ys, sizeof(size_t) * num_seats)) {
          fprintf(stderr, "Failed to read request from client\n");
          free(xs);
          free(ys);
          return 1;
        }
      
        returned = ems_reserve(event_id, num_seats, xs, ys);
        if(!check_write(fd_response, &returned, sizeof(returned))) {
          fprintf(stderr, "Failed to write response for client\n");
          free(xs);
          free(ys);
          return 1;
        }
        free(xs);
        free(ys);
        break;

      case '5': // client ems_show
        if(!check_read(fd_request, &event_id, sizeof(event_id))) {
          fprintf(stderr, "Failed to read request from client\n");
          return 1;
        }
        returned = ems_show(fd_response, event_id);
        if(returned == 1) { // ems_show failed
          if(!check_write(fd_response, &returned, sizeof(returned))) {
            fprintf(stderr, "Failed to write response for client\n");
            return 1;
          }
        }
        break;

      case '6': // client ems_list_events
        returned = ems_list_events(fd_response);
        if(returned == 1) { // ems_list_events failed
          if(!check_write(fd_response, &returned, sizeof(returned))) {
            fprintf(stderr, "Failed to write response for client\n");
            return 1;
          }
        }
        break;
    }
  }
  return 0;
}

void *process_thread(void* id) {
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);
  if (pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0) {
    fprintf(stderr, "Error masking the working thread\n");
    return (void*) 1;
  }

  long session_id_aux = (long) id;
  int session_id = (int) session_id_aux;

  while(1) { 
    int fd_request, fd_response;
    char req_pipe_path[40], resp_pipe_path[40];

    if(pthread_mutex_lock(&read_buffer_mutex) != 0) {
      fprintf(stderr, "mutex lock failed\n");
      return (void*) 1;
    }
    
    while(waiting_requests == 0) {
      pthread_cond_wait(&condition1, &read_buffer_mutex);
    }
    
    // Get the client FIFOs from the requests_buffer
    strcpy(req_pipe_path, requests_buffer[con_ind].req_pipe_path);
    strcpy(resp_pipe_path, requests_buffer[con_ind].resp_pipe_path);
    waiting_requests--;

    if (con_ind == MAX_SESSION_COUNT-1) {
      con_ind = 0;
    }
    else {
      con_ind++;
    }

    pthread_cond_signal(&condition2); // Warn the server that a request was consumed
    
    if(pthread_mutex_unlock(&read_buffer_mutex) != 0) {
      fprintf(stderr, "mutex unlock failed\n");
      return (void*) 1;
    }
    
    // Thread can read client requests 
    if ((fd_request = open(req_pipe_path, O_RDONLY)) < 0) { 
      if(unlink(req_pipe_path) < 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to unlink client request FIFO\n");
      }
      if(unlink(resp_pipe_path) < 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to unlink client response FIFO\n");
      }
      return (void*) 1; 
    }
    
    // Thread can write responses to the client
    if ((fd_response = open(resp_pipe_path, O_WRONLY)) < 0) { 
      if(close(fd_request) < 0) {
        fprintf(stderr, "Failed to close client request FIFO\n");
      }
      if(unlink(req_pipe_path) < 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to unlink client request FIFO\n");
      }
      if(unlink(resp_pipe_path) < 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to unlink client response FIFO\n");
      }
      return (void*) 1;
    } 
    
    // Thread gives session_id to client
    if(!check_write(fd_response, &session_id, sizeof(session_id))) {
      if(close(fd_request) < 0) {
        fprintf(stderr, "Failed to close client request FIFO\n");
      }
      if(close(fd_response) < 0) {
        fprintf(stderr, "Failed to close client response FIFO\n");
      }
      if(unlink(req_pipe_path) < 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to unlink client request FIFO\n");
      }
      if(unlink(resp_pipe_path) < 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to unlink client response FIFO\n");
      }
      return (void*) 1;
    }
 
    if(process_client(fd_request, fd_response, session_id)) {
      return (void*) 1;
    }
   
    if(close(fd_request) < 0) {
      fprintf(stderr, "Failed to close client request FIFO\n");
      if(close(fd_response) < 0) {
        fprintf(stderr, "Failed to close client response FIFO\n");
      }
      if(unlink(req_pipe_path) < 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to unlink client request FIFO\n");
      }
      if(unlink(resp_pipe_path) < 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to unlink client response FIFO\n");
      }
      return (void*) 1;
    }
    if(close(fd_response) < 0) {
      fprintf(stderr, "Failed to close client response FIFO\n");
      if(unlink(req_pipe_path) < 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to unlink client request FIFO\n");
      }
      if(unlink(resp_pipe_path) < 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to unlink client response FIFO\n");
      }
      return (void*) 1;
    }
    if(unlink(req_pipe_path) < 0 && errno != ENOENT) {
      fprintf(stderr, "Failed to unlink client request FIFO\n");
      if(unlink(resp_pipe_path) < 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to unlink client response FIFO\n");
      }
      return (void*) 1;
    }
    if(unlink(resp_pipe_path) < 0 && errno != ENOENT) {
      fprintf(stderr, "Failed to unlink client response FIFO\n");
      return (void *) 1;
    }
  }
  return (void*) 0;
}

static void sig_handler(int sig) {
  if (sig == SIGUSR1) {
    global_flag = 1;
    if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
      fprintf(stderr, "Error setting the signal handler\n");
    }
  }
  return; // Resume execution at point of interruption
}

int main(int argc, char* argv[]) {
  if (argc < 2 || argc > 3) {
    fprintf(stderr, "Usage: %s\n <pipe_path> [delay]\n", argv[0]);
    return 1;
  }

  char* endptr;
  unsigned int state_access_delay_us = STATE_ACCESS_DELAY_US;
  if (argc == 3) {
    unsigned long int delay = strtoul(argv[2], &endptr, 10);

    if (*endptr != '\0' || delay > UINT_MAX) {
      fprintf(stderr, "Invalid delay value or value too large\n");
      return 1;
    }

    state_access_delay_us = (unsigned int)delay;
  }

  if (ems_init(state_access_delay_us)) {
    fprintf(stderr, "Failed to initialize EMS\n");
    return 1;
  }

  // Create signal SIGUSR1 routine
  if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
    fprintf(stderr, "Error setting up SIGUSR1 signal routine\n");
    return 1;
  }

  // Create server FIFO
  if (mkfifo (argv[1], 0666) < 0) {  
    fprintf(stderr, "Failed to create server FIFO\n");
    return 1;
  }

  // Create worker threads
  pthread_t tid[MAX_SESSION_COUNT];
  for(int i = 0; i < MAX_SESSION_COUNT; i++) {
    if(pthread_create(&tid[i], 0, process_thread, (void *)(long)i) != 0) {
      fprintf(stderr, "Failed to create worker thread\n");
      if(unlink(argv[1]) < 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to unlink server FIFO\n");
      }
      return 1;
    }
  }

  // Server can read session requests
  int fd_server;
  if ((fd_server = open(argv[1], O_RDONLY)) < 0) {  
    fprintf(stderr, "Failed to open server FIFO\n");
    if(unlink(argv[1]) < 0 && errno != ENOENT) {
      fprintf(stderr, "Failed to unlink server FIFO\n");
    }
    return 1;                                             
  }
  
  while (1) {
    // Check for SIGUSR1 signal
    if(global_flag) {
      if(list_and_show_each_event()) {
        fprintf(stderr, "list_and_show_each_event function failed\n");
      }
      global_flag = 0;
    }

    // Read client setup request
    char *req_pipe_path, *resp_pipe_path;
    char msg[85];
    int value = check_read(fd_server, msg, sizeof(msg));
    if(!value) { 
      fprintf(stderr, "Failed to read from server FIFO\n");
      if(close(fd_server) < 0) { 
        fprintf(stderr, "Failed to close server FIFO\n");
      }
      if(unlink(argv[1]) < 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to unlink server FIFO\n");
      }
      return 1;
    }
    else if(value == 2) { // No clients to produce
      continue;
    }
    strtok(msg," ");
    req_pipe_path = strtok(NULL," ");
    resp_pipe_path = strtok(NULL," ");
   
    // Write new client to the producer-consumer buffer
    if (pthread_mutex_lock(&read_buffer_mutex) != 0) {
      fprintf(stderr, "mutex lock failed\n");
      if(close(fd_server) < 0) { 
        fprintf(stderr, "Failed to close server FIFO\n");
      }
      if(unlink(argv[1]) < 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to unlink server FIFO\n");
      }
      return 1;
    }
    
    while(waiting_requests >= MAX_SESSION_COUNT) { // Buffer full with requests
      pthread_cond_wait(&condition2, &read_buffer_mutex);
    }

    strcpy(requests_buffer[prd_ind].req_pipe_path, req_pipe_path);
    strcpy(requests_buffer[prd_ind].resp_pipe_path, resp_pipe_path);
    waiting_requests++;

    if (prd_ind == MAX_SESSION_COUNT-1) {
      prd_ind = 0;
    }
    else {
      prd_ind++;
    }

    pthread_cond_signal(&condition1); // Warn the threads that a request was produced

    if (pthread_mutex_unlock(&read_buffer_mutex) != 0) {
      fprintf(stderr, "mutex unlock failed\n");
      if(close(fd_server) < 0) { 
        fprintf(stderr, "Failed to close server FIFO\n");
      }
      if(unlink(argv[1]) < 0 && errno != ENOENT) {
        fprintf(stderr, "Failed to unlink server FIFO\n");
      }
      return 1;
    }
  }

  //TODO: Close Server
  if(close(fd_server) < 0) {
    fprintf(stderr, "Failed to close server FIFO\n");
    if(unlink(argv[1]) < 0 && errno != ENOENT) {
      fprintf(stderr, "Failed to unlink server FIFO\n");
    }
    return 1;
  }
  if(unlink(argv[1]) < 0 && errno != ENOENT) {
    fprintf(stderr, "Failed to unlink server FIFO\n");
    return 1;
  }
  ems_terminate();
  return 0;
}