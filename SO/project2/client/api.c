#include "api.h"
#include "common/io.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int session_id, fd_request, fd_response;

typedef struct {
    int returno;
    size_t num_rows;
    size_t num_cols;
    unsigned int* seats;
} ShowData;

typedef struct {
    int returno;
    size_t num_events;
    unsigned int* ids;
} ListData;

int ems_setup(char const* req_pipe_path, char const* resp_pipe_path, char const* server_pipe_path) {
  //TODO: create pipes and connect to the server
  unlink(req_pipe_path);
  unlink(resp_pipe_path);

  if (mkfifo (req_pipe_path, 0666) < 0) { // Create request FIFO
    return 1;
  }
  if (mkfifo (resp_pipe_path, 0666) < 0) { // Create response FIFO
    unlink(req_pipe_path);
    return 1;
  }
  
  // Client connects to server pipe to request to start a session
  int fd_server;
  if ((fd_server = open(server_pipe_path, O_WRONLY)) < 0) {  
    unlink(req_pipe_path);
    unlink(resp_pipe_path);
    return 1;                                             
  }
  // Build and write the message to the server
  char op_code = '1';
  char msg[85];
  snprintf(msg, sizeof(msg), "%c %s %s", op_code, req_pipe_path, resp_pipe_path);
  size_t msg_size = (size_t) 3 + strlen(req_pipe_path) + strlen(resp_pipe_path);
  for(int i = (int) msg_size; i < 84; i++) {
    msg[i] = ' ';
  } 
  msg[84] = '\0';
  
  if (!check_write(fd_server, msg, sizeof(msg))) {
    unlink(req_pipe_path);
    unlink(resp_pipe_path);
    close(fd_server);
    return 1;
  }
 
  // Client disconnects from server pipe
  if (close(fd_server) < 0) { 
    unlink(req_pipe_path);
    unlink(resp_pipe_path);
    return 1; 
  }
  
  // Client can write requests to server
  if ((fd_request = open(req_pipe_path, O_WRONLY)) < 0) { 
    unlink(req_pipe_path);
    unlink(resp_pipe_path);
    return 1; 
  }
  // Client can read server responses
  if ((fd_response = open(resp_pipe_path, O_RDONLY)) < 0) { 
    close(fd_request);
    unlink(req_pipe_path);
    unlink(resp_pipe_path);
    return 1;
  }
  // Client gets the session_id 
  if(!check_read(fd_response, &session_id, sizeof(session_id))) {
    close(fd_request);
    close(fd_response);
    unlink(req_pipe_path);
    unlink(resp_pipe_path);
    return 1;
  }

  return 0;
}

int ems_quit(void) { 
  //TODO: close pipes
  // Writes quit request message to the server
  char buffer[1 + sizeof(int)];
  buffer[0] = '2';
  memcpy(buffer + 1, &session_id, sizeof(session_id));
  
  if (!check_write(fd_request, buffer, sizeof(buffer))) {
    return 1;
  }

  if(close(fd_request) < 0) { // Close request pipe
    return 1;
  }
  if(close(fd_response) < 0) { // Close response pipe
    return 1;
  }

  return 0;
}

int ems_create(unsigned int event_id, size_t num_rows, size_t num_cols) {
  //TODO: send create request to the server (through the request pipe) and wait for the response (through the response pipe)
  // Build create request message
  char buffer[1 + sizeof(int)+ sizeof(unsigned int) + 2*sizeof(size_t)];
  memset(buffer, 0, sizeof(buffer));
  buffer[0] = '3';
  memcpy(buffer + 1, &session_id, sizeof(int));
  memcpy(buffer + 1 + sizeof(int), &event_id, sizeof(unsigned int));
  memcpy(buffer + 1 + sizeof(int) + sizeof(unsigned int), &num_rows, sizeof(size_t));
  memcpy(buffer + 1 + sizeof(int) + sizeof(unsigned int) + sizeof(size_t), &num_cols, sizeof(size_t));

  // Send create request to the server (through the request pipe)
  if(!check_write(fd_request, buffer, sizeof(buffer))) {
    return 1;
  }
  // Wait for the response (through the response pipe)
  int response;
  if(!check_read(fd_response, &response, sizeof(response))) {
    return 1;
  }
  return response;
}

int ems_reserve(unsigned int event_id, size_t num_seats, size_t* xs, size_t* ys) {
  //TODO: send reserve request to the server (through the request pipe) and wait for the response (through the response pipe)
  // Build reserve request message
  char buffer[1 + sizeof(int) + sizeof(unsigned int) + sizeof(size_t) + 2 * sizeof(size_t) * num_seats];
  buffer[0] = '4';
  memcpy(buffer + 1, &session_id, sizeof(int));
  memcpy(buffer + 1 + sizeof(int), &event_id, sizeof(unsigned int));
  memcpy(buffer + 1 + sizeof(int) + sizeof(unsigned int), &num_seats, sizeof(size_t));
  memcpy(buffer + 1 + sizeof(int) + sizeof(unsigned int) + sizeof(size_t), xs, num_seats * sizeof(size_t));
  memcpy(buffer + 1 + sizeof(int) + sizeof(unsigned int) + sizeof(size_t) + num_seats * sizeof(size_t), ys, num_seats * sizeof(size_t));

  // Send reserve request to the server (through the request pipe)
  if(!check_write(fd_request, buffer, sizeof(buffer))) {
    return 1;
  }
  // Wait for the response (through the response pipe)
  int response;
  if(!check_read(fd_response, &response, sizeof(response))) {
    return 1;
  }
  return response;
}

int ems_show(int out_fd, unsigned int event_id) {
  //TODO: send show request to the server (through the request pipe) and wait for the response (through the response pipe)
  // Build show request message
  char buffer1[1 + sizeof(int)+ sizeof(unsigned int)];
  buffer1[0] = '5';
  memcpy(buffer1 + 1, &session_id, sizeof(int));
  memcpy(buffer1 + 1 + sizeof(int), &event_id, sizeof(unsigned int));

  // Send show request to the server (through the request pipe)
  if(!check_write(fd_request, buffer1, sizeof(buffer1))) {
    return 1;
  }
  
  // Reads the server response (through the response pipe)
  ShowData showData;
  if(!check_read(fd_response, &showData.returno, sizeof(int))) {
    return 1;
  }
  if(showData.returno == 1) { // ems_show in server failed
    return 1;
  }
  if(!check_read(fd_response, &showData.num_rows, sizeof(size_t))) {
    return 1;
  }
  if(!check_read(fd_response, &showData.num_cols, sizeof(size_t))) {
    return 1;
  }
  showData.seats = malloc(showData.num_rows * showData.num_cols * sizeof(unsigned int));
  if (showData.seats == NULL) {   
    return 1;
  }
  if(!check_read(fd_response, showData.seats, sizeof(unsigned int) * showData.num_rows * showData.num_cols)) {
    free(showData.seats);
    return 1;
  }

  // Writes the show event output
  for (size_t i = 1; i <= showData.num_rows; i++) {
    for (size_t j = 1; j <= showData.num_cols; j++) {
      char buffer2[16];
      sprintf(buffer2, "%u", showData.seats[(i - 1)*showData.num_cols + j - 1]);
      if (print_str(out_fd, buffer2)) {
        perror("Error writing to file descriptor");
        free(showData.seats);
        return 1;
      }
      if (j < showData.num_cols) {
        if (print_str(out_fd, " ")) {
          perror("Error writing to file descriptor");
          free(showData.seats);
          return 1;
        }
      }
    }
    if (print_str(out_fd, "\n")) {
      perror("Error writing to file descriptor");
      free(showData.seats);
      return 1;
    }
  }
  free(showData.seats);
  return 0;
}

int ems_list_events(int out_fd) {
  //TODO: send list request to the server (through the request pipe) and wait for the response (through the response pipe)
  // Build list events request message
  char buffer[1 + sizeof(int)];
  buffer[0] = '6';
  memcpy(buffer + 1, &session_id, sizeof(int));
  
  // Send list events request to the server (through the request pipe)
  if(!check_write(fd_request, buffer, sizeof(buffer))) {
    return 1;
  }

  // Reads the server response (through the response pipe)
  ListData listData;
  if(!check_read(fd_response, &listData.returno, sizeof(int))) {
    return 1;
  }
  if(listData.returno == 1) { // ems_list in server failed
    return 1;
  }
  if(!check_read(fd_response, &listData.num_events, sizeof(size_t))) {
    return 1;
  }
  listData.ids = (unsigned int*)malloc(listData.num_events * sizeof(unsigned int));
  if (listData.ids == NULL) {   
    return 1;
  }
  if(!check_read(fd_response, listData.ids, sizeof(unsigned int) * listData.num_events)) {
    free(listData.ids);
    return 1;
  }

  // Writes in the output file the list of the events
  if(listData.num_events == (size_t) 0) {
    char buff[] = "No events\n";
    if (print_str(out_fd, buff)) {
      perror("Error writing to file descriptor");
      free(listData.ids);
      return 1;
    }
  }
  else { 
    for(size_t i = 0; i < listData.num_events; i++) {
      char buff[] = "Event: ";
      if (print_str(out_fd, buff)) {
        perror("Error writing to file descriptor");
        free(listData.ids);
        return 1;
      }

      char id[16];
      sprintf(id, "%u\n", listData.ids[i]);
      if (print_str(out_fd, id)) {
        perror("Error writing to file descriptor");
        free(listData.ids);
        return 1;
      }       
    }  
  }
  free(listData.ids);
  return 0;
}