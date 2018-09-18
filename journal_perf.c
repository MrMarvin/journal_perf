#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-journal.h>

void log_via_syslog(char* uuid) {
  char output[80];
  sprintf(output, "Hello World via syslog %s", uuid);
  syslog(LOG_NOTICE, output);
}

void log_via_stdout(char* uuid) {
  printf("<5>Hello World via stdout %s\n", uuid);
}

void log_via_journal(char* uuid) {
  char output[80];
  sprintf(output, "Hello World via sd_journal_print %s", uuid);
  sd_journal_print(LOG_NOTICE, output);
}

int main(int argc, char *argv[]) {
  long count;
  void (*chosen_output)(char[]);

  if(strcasecmp(argv[1], "syslog") == 0){
      chosen_output = log_via_syslog;
  }
  if(strcasecmp(argv[1], "stdout") == 0){
      chosen_output = log_via_stdout;
  }
  if(strcasecmp(argv[1], "journal") == 0){
      chosen_output = log_via_journal;
  }

  char *ptr;
  count = strtol(argv[3], &ptr, 10);
  while (count--)
  {
    chosen_output(argv[2]);
  }
  exit(0);
}
