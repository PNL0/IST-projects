#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>
#include "constants.h"
#include "operations.h"
#include "parser.h"

int MAX_THREADS = 0;

void *process_file(void *arguments) {
    unsigned int event_id, delay, thread_id;
    size_t num_rows, num_columns, num_coords;
    size_t xs[MAX_RESERVATION_SIZE], ys[MAX_RESERVATION_SIZE];
    FileArgs *args = (FileArgs *)arguments;
    int command;

    while (1) {
      
      if(lock(args, args->mutex) != 0)
        return (void*) 2;

      if(args->thread_id_to_wait[args->id-1] == 1) {
        if(unlock(args, args->mutex) != 0)
          return (void*) 2;

        sleep((unsigned int)args->delay_to_wait[args->id-1] / 1000);

        if(lock(args, args->mutex) != 0)
          return (void*) 2;

        args->thread_id_to_wait[args->id-1] = 0;
        args->delay_to_wait[args->id-1] = 0;
      }

      if(*args->flag == 1 || *args->flag == 2) {
        if(unlock(args, args->mutex) != 0)
          return (void*) 2;

        break;
      }
      
      command = get_next(args->fd_jobs);
      if(command == EOC) {
        if(unlock(args, args->mutex) != 0)
          return (void*) 2;
        break;
      }
      
      switch (command) {
        case CMD_CREATE:
          
          if (parse_create(args->fd_jobs, &event_id, &num_rows, &num_columns) != 0) {
            fprintf(stderr, "Invalid command. See HELP for usage\n");
          
            if(unlock(args, args->mutex) != 0)
              return (void*) 2;

            continue;
          }
          
          if(unlock(args, args->mutex) != 0)
            return (void*) 2;

          int create_result = ems_create(event_id, num_rows, num_columns, args);

          if (create_result == 1) {
            fprintf(stderr, "Failed to create event\n");
          }
          else if(create_result == 2) {
            return (void*) 2;
          }
            
          break;

        case CMD_RESERVE:
    
          num_coords = parse_reserve(args->fd_jobs, MAX_RESERVATION_SIZE, &event_id, xs, ys);
          if (num_coords == 0) {
            fprintf(stderr, "Invalid command. See HELP for usage\n");
            
            if(unlock(args, args->mutex) != 0)
              return (void*) 2;
            continue;
          }

          if(unlock(args, args->mutex) != 0)
            return (void*) 2;

          int reserve_result = ems_reserve(event_id, num_coords, xs, ys, args);

          if (reserve_result == 1) {
            fprintf(stderr, "Failed to reserve seats\n");
          }
          else if(reserve_result == 2) {
            return (void*) 2;
          }

          break;

        case CMD_SHOW:
          
          if (parse_show(args->fd_jobs, &event_id) != 0) {
            fprintf(stderr, "Invalid command. See HELP for usage\n");
            
            if(unlock(args, args->mutex) != 0)
              return (void*) 2;

            continue;
          }
          
          if(unlock(args, args->mutex) != 0)
            return (void*) 2;

          int show_result = ems_show(event_id, args->output_filename, args->fd_out, args);

          if (show_result == 1) {
            fprintf(stderr, "Failed to show event\n");
          }
          else if(show_result == 2) {
            return (void*) 2;
          }

          break;

        case CMD_LIST_EVENTS:
          
          if(unlock(args, args->mutex) != 0)
            return (void*) 2;

          int list_result = ems_list_events(args->output_filename, args->fd_out, args);

          if (list_result == 1) {
            fprintf(stderr, "Failed to list events\n");
          }
          else if(list_result == 2) {
            return (void*) 2;
          }
          
          break;

        case CMD_WAIT:
          if (parse_wait(args->fd_jobs, &delay, &thread_id) == -1) {  
            fprintf(stderr, "Invalid command. See HELP for usage\n");
            
            if(unlock(args, args->mutex) != 0)
              return (void*) 2;

            continue;
          }

          if(thread_id == 0) { // Wait instruction for all the threads.
            if (delay > 0) {
              fprintf(stderr, "Waiting...\n");
              ems_wait(delay);
            }
            if(unlock(args, args->mutex) != 0)
              return (void*) 2;
          }
          else {
            // Wait instruction for a specific thread
            if (delay > 0 && (int)thread_id <= MAX_THREADS) { 
              fprintf(stderr, "Thread %d waiting...\n", thread_id);
              args->thread_id_to_wait[thread_id - 1] = 1;
              args->delay_to_wait[thread_id -1] = (int) delay;
              
            }
            else if((int)thread_id > MAX_THREADS){
              fprintf(stderr, "Invalid thread_id\n");
            } 
            if(unlock(args, args->mutex) != 0)
              return (void*) 2;
          }
          break;

        case CMD_INVALID:
          if(unlock(args, args->mutex) != 0)
            return (void*) 2;

          fprintf(stderr, "Invalid command. See HELP for usage\n");

          break;

        case CMD_HELP:
          if(unlock(args, args->mutex) != 0)
            return (void*) 2;

          fprintf(stderr,
              "Available commands:\n"
              "  CREATE <event_id> <num_rows> <num_columns>\n"
              "  RESERVE <event_id> [(<x1>,<y1>) (<x2>,<y2>) ...]\n"
              "  SHOW <event_id>\n"
              "  LIST\n"
              "  WAIT <delay_ms> [thread_id]\n"  
              "  BARRIER\n"                      
              "  HELP\n");

          break;

        case CMD_BARRIER:
          *args->flag = 1; // Avoid the other threads to read the next command.
          if(unlock(args, args->mutex) != 0)
            return (void*) 2;
            
          free(args);
          return (void*) 1;

        case CMD_EMPTY:
          if(unlock(args, args->mutex) != 0)
            return (void*) 2;
            
          break;
      }
    }

    free(args);
    return (void*) 0;
}

int main(int argc, char *argv[]) {
  unsigned int state_access_delay_ms = STATE_ACCESS_DELAY_MS;

  if (argc < 2) {
    fprintf(stderr, "No directory\n");
    return 1;
  }

  // argv[2] = MAX_PROC 
  if(argc < 3) {
    fprintf(stderr, "No MAX_PROC\n");
    return 1;
  }

  // argv[3] = MAX_THREADS
  if(argc < 4) {
    fprintf(stderr, "No MAX_THREADS\n");
    return 1;
  }

  int MAX_PROC;
  MAX_PROC = atoi(argv[2]);
  MAX_THREADS = atoi(argv[3]);
  
  if (argc > 4) {
    char *endptr;
    unsigned long int delay = strtoul(argv[4], &endptr, 10);

    if (*endptr != '\0' || delay > UINT_MAX) {
      fprintf(stderr, "Invalid delay value or value too large\n");
      return 1;
    }

    state_access_delay_ms = (unsigned int)delay;
  }

  if (ems_init(state_access_delay_ms)) {
    fprintf(stderr, "Failed to initialize EMS\n");
    return 1;
  }

  DIR *directory = opendir(argv[1]);
  if (directory == NULL) {
    fprintf(stderr, "Error opening the directory.\n");
    return 1;
  }

  int counterProcess = 0; 
  struct dirent *file;
  while ((file = readdir(directory))) {
    // Check if the file as the extension ".jobs"
    char *dot = strrchr(file->d_name, '.');
    if (dot == NULL || strcmp(dot, ".jobs") != 0) {
      continue;
    }

    int pid, estado;
    pid = fork(); 
    if(pid == -1) {
      fprintf(stderr, "Error creating the process.\n");
      if(closedir(directory) == -1) {
        fprintf(stderr, "Error: closing the directory\n");
      }
      return 1;
    }
    /* Child process */
    else if (pid == 0) {
      int fd_jobs, fd_out;

      char *output_filename = (char*)malloc(sizeof(char) * (strlen(file->d_name) + 6));

      if (output_filename == NULL) {
        fprintf(stderr, "Error: Unable to allocate memory for output_filename\n");
        if(closedir(directory) == -1) {
          fprintf(stderr, "Error: closing the directory\n");
        }
        return 1;
      }

      char full_path1[PATH_MAX];
      snprintf(full_path1, sizeof(full_path1), "%s/%s", argv[1], file->d_name);
      fd_jobs = open(full_path1, O_RDONLY);
      if (fd_jobs == -1) {
        fprintf(stderr, "Error opening the file called %s.\n", file->d_name);
        free(output_filename);
        if(closedir(directory) == -1) {
          fprintf(stderr, "Error: closing the directory\n");
        }
        return 1;
      }

      strcpy(output_filename,file->d_name);
      output_filename[strlen(output_filename) - 4] = '\0';
      char out_extension[5] = "out";
      strcat(output_filename, out_extension);

      char full_path2[PATH_MAX];
      snprintf(full_path2, sizeof(full_path2), "%s/%s", argv[1], output_filename);
      fd_out = open(full_path2, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      if(fd_out == -1){
        fprintf(stderr, "Error creating the file called %s.\n", output_filename);
        free(output_filename);
        close(fd_jobs);
        if(closedir(directory) == -1) {
            fprintf(stderr, "Error: closing the directory\n");
        }
        return 1;
      }
  
      int *result = (int *)malloc(sizeof(int));
      *result = 1;
      while(*result) {
        int flag = 0;
        pthread_mutex_t mutex;
        if (pthread_mutex_init(&mutex, NULL) != 0) {
          fprintf(stderr, "Error inicializating the mutex\n");
          free(result);
          free(output_filename);
          close(fd_jobs);
          close(fd_out);
          if(closedir(directory) == -1) {
            fprintf(stderr, "Error: closing the directory\n");
          }
          return 1;
        }
        pthread_t tid[MAX_THREADS];
        unsigned int id = 1;
        int thread_id_to_wait[MAX_THREADS];
        int delay_to_wait[MAX_THREADS];
        
        for(int i = 0; i < MAX_THREADS; i++) {
          thread_id_to_wait[i] = 0;
          delay_to_wait[i] = 0;
        }

        for(int i = 0; i < MAX_THREADS; i++){
          FileArgs *args = (FileArgs*)malloc(sizeof(FileArgs));
          if(args == NULL) {
            fprintf(stderr, "Error: Unable to allocate memory for file arguments\n");
            free(result);
            free(output_filename);
            close(fd_jobs);
            close(fd_out);
            if(pthread_mutex_destroy(&mutex) != 0) {
              fprintf(stderr, "Error destroying the mutex\n");
            }
            if(closedir(directory) == -1) {
              fprintf(stderr, "Error: closing the directory\n");
            }
            return 1;
          }
          args->fd_jobs = fd_jobs;
          args->fd_out = fd_out;
          args->output_filename = output_filename;
          args->mutex = &mutex;
          args->id = id;
          args->delay_to_wait = delay_to_wait;
          args->thread_id_to_wait = thread_id_to_wait;
          args->flag = &flag;
          if(pthread_create(&tid[i], 0, process_file, (void*)args) != 0) {
            fprintf(stderr, "Error: Unable to create thread\n");
            free(args);
            free(result);
            free(output_filename);
            close(fd_jobs);
            close(fd_out);
            if(pthread_mutex_destroy(&mutex) != 0) {
              fprintf(stderr, "Error destroying the mutex\n");
            }
            if(closedir(directory) == -1) {
              fprintf(stderr, "Error: closing the directory\n");
            }
            return 1;
          }
          id++;
        }

        for(int i = 0; i < MAX_THREADS; i++) {
          if(pthread_join(tid[i], (void **)result) != 0) {
            fprintf(stderr, "Error: Unable to wait for thread to finish\n");
            free(result);
            free(output_filename);
            close(fd_jobs);
            close(fd_out);
            if(pthread_mutex_destroy(&mutex) != 0) {
              fprintf(stderr, "Error destroying the mutex\n");
            }
            if(closedir(directory) == -1) {
              fprintf(stderr, "Error: closing the directory\n");
            }
            return 1;
          }

          if(*result == 1) {
            for(int j = i+1; j < MAX_THREADS; j++) {
              if(pthread_join(tid[j], NULL) != 0) {
                fprintf(stderr, "Error: Unable to wait for thread to finish\n");
                free(result);
                free(output_filename);
                close(fd_jobs);
                close(fd_out);
                if(pthread_mutex_destroy(&mutex) != 0) {
                  fprintf(stderr, "Error destroying the mutex\n");
                }
                if(closedir(directory) == -1) {
                  fprintf(stderr, "Error: closing the directory\n");
                }
                return 1;
              }
            }
            break;
          }
          else if(*result == 2) {
            for(int j = i+1; j < MAX_THREADS; j++) {
              if(pthread_join(tid[j], NULL) != 0) {
                fprintf(stderr, "Error: Unable to wait for thread to finish\n");
                free(result);
                free(output_filename);
                close(fd_jobs);
                close(fd_out);
                if(pthread_mutex_destroy(&mutex) != 0) {
                  fprintf(stderr, "Error destroying the mutex\n");
                }
                if(closedir(directory) == -1) {
                  fprintf(stderr, "Error: closing the directory\n");
                }
                return 1;
              }
            }
            free(result);
            free(output_filename);
            close(fd_jobs);
            close(fd_out);
            if(pthread_mutex_destroy(&mutex) != 0) {
              fprintf(stderr, "Error destroying the mutex\n");
            }
            if(closedir(directory) == -1) {
              fprintf(stderr, "Error: closing the directory\n");
            }
            return 1; 
          }
        }
        if(pthread_mutex_destroy(&mutex) != 0) {
          fprintf(stderr, "Error destroying the mutex\n");
          if(closedir(directory) == -1) {
            fprintf(stderr, "Error: closing the directory\n");
          }
          return 1;
        }
      }
      free(result);
      free(output_filename);
      close(fd_jobs);
      close(fd_out);
      exit(0);
    }
    // Parent process
    else {
      counterProcess++;
      if(counterProcess >= MAX_PROC) {
        pid = wait(&estado);
        if(pid == -1) {
          fprintf(stderr, "Error in the wait instruction\n");
          if(closedir(directory) == -1) {
            fprintf(stderr, "Error: closing the directory\n");
          }
          return 1;
        }
        counterProcess--;
        fprintf(stderr, "Child process state: %d\n", estado);  
      }
    }
  }
  if(closedir(directory) == -1) {
    fprintf(stderr, "Error: closing the directory\n");
    return 1;
  }
  return 0;
}