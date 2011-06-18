/* OpenCABIN Library

   Copyright (C) 2006,2007,2008 KGT Inc.

    This file is part of the OpenCABIN Library developed
    under CnC project sponsored by National Institute of
    Information and Communications Technology (NICT).

    The OpenCABIN Library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    The OpenCABIN Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details.
*/
#define STANDALONE
#include <stdio.h>
#include <string.h>
#include <avlib.h>
#include <unistd.h>
#include <pthread.h>
// renderer manager
#include <pid.h>
#include <ocl.h>
#include <readline/readline.h>

#if 0
struct renderer {
  int fd;
  char *name;
  //  int rid;
};

#define RENDERERS_MAX 32
static struct renderer renderers[RENDERERS_MAX];
static int renderers_curmax = 0;

static struct renderer *
renderers_new(int fd, char *name)
{
  int i;

  for (i = 0; i < RENDERERS_MAX; ++i) {
    if (0 == renderers[i].name) {
      renderers[i].name = strdup(name);
      renderers[i].fd = fd;
      return &renderers[i];
    }
  }
  cab_error(1, "RENDERERS_MAX\n");
  return 0;
}
#endif

static
void
minit0(void)
{
  cb_init(); // cluster_base.c
}

void
init_after_connections_renderers(void)
{
  SHvar_minit(); // shared_variable.c
  vdmlm_init(0, 0); // vdml.c
  minit_defaultvars(); // defaultvars.c
  pid_init(1024); // pid.c
  mopsys_init(); // mop.c
  msgptr_init(0); // msg.c
  sensors_initM(); // sensors.c
}

#if 0
static void
connect_renderer(char *host, short port)
{
  int fd;
  char namebuf[256];

  sprintf(namebuf, "%s:%d", host, port);
  //fd = tcpconnect(host, port);
  fd = tcpconnect(namebuf);
#if 0
  renderers_new(fd, namebuf);
#else
  process_new_renderer(fd, namebuf);
#endif
}
#else
static void
connect_renderer(char *desc)
{
  int fd;

  fd = tcpconnect(desc);
  process_new_renderer(fd, desc);
}

#endif

#if 0
void renderer_input(int);

static int
process_new_client(int fd, int val)
{
  int newfd, i, retval;
  socklen_t len = sizeof(struct sockaddr);
  struct sockaddr saddr;
  unsigned int type;
  //char dummy[BUFSIZ];

  newfd = _fdproc_newclient(fd, val);

  retval = readforce(newfd, (char *)&type, sizeof(type));
  if (retval <= 0) {
    cab_perror(1, "negotiation after connection failed (1): ");
    close(newfd);
    return 0;
  }

  switch (type) {
  case CLIENTTYPE_RENDERER:
    retval = process_new_renderer(newfd);
    fdproc_register(newfd, renderer_input, retval);
    return retval;
    break;
  default:
    cab_error(1, "unknown client type 0x%08x\n", type);
    close(newfd);
    break;
  }
  return -1;
}
#endif

// ==================================================

static pthread_t loopthr;

static void *
loopthr_proc(void *v)
{
  while (1) {
    //printsyment();
    //fprintf(stderr, "tick\n");
    //mpm_procinit();
    mpm_idle();
    //usleep(1000000);
    usleep(10000);
  }
  return (void *)0;
}

static void
run_loop()
{
  pthread_create(&loopthr, 0, loopthr_proc, 0);
}

static char *g_preload = 0;

static char *hfile = ".oclcmdhistory";

int
master_end(void)
{
  
  return 0;
}

// callback function for command "jobs"
static void
cb_cmdjobs(int pid, int type_id, int local_id)
{
  int ac, i;
  char **av;
  char type;

  // PID_TYPE_APP
  type = (PID_TYPE_MOP == type_id) ? 'm' : 'g';

  if (-1 == mpm_getargv(local_id, &ac, &av)) {
    //error
    fprintf(stderr, "no such pid %d\n", pid);
  }
  printf("%c %3d ", type, pid);
  for (i = 0; i < ac; ++i) {
    printf("%s ", av[i]);
  }
  printf("\n");
}

static int
check_gapp_procid(int pid)
{
  int procid;
  unsigned int type;

  switch (type = pid_gettypeid(pid, &procid)) {
  case 0:
    fprintf(stderr, "no such pid: %d\n", pid);
    return -1;
    break;
  case PID_TYPE_MOP:
    fprintf(stderr, "pid: %d is mop (master only plugin)\n", pid);
    return -1;
    break;
  default:
    fprintf(stderr, "unknown error pid: %d\n", pid);
    return -1;
    break;
  case PID_TYPE_APP:
    // OK, go ahead. (do nothing here)
    break;
  }

  if (!mpm_proc_existp(procid)) {
    fprintf(stderr, "process id: %d is not valid.\n", procid);
    return -1;
  }

  return procid;
}

int jsnames_str2data(char *str, int *ret_type, int *ret_number); // (without "value")
void jsnames_print_jsstat(FILE *outfp, struct ocl_jsstat *stat);
void jsnames_print_allnames(FILE *outfp);

void oclJS_changeStatus(int no, int type, int number, int value, int time);
void oclJS_getStatus(int no, struct ocl_jsstat *status);
void oclJS_delete(int no);

// shell
int
master_mainloop(int ac, char **av)
{
  // read commands from stdin
  char buf[1024], *line = 0;
  struct avbuf *avptr;
  FILE *fp = stdin;
  int last_appid = 0;

  if (g_preload) {
    last_appid = cmd_loadapp(g_preload, ".");
    printf("app_id: %d\n", last_appid);
  }

  read_history(hfile);

  avptr = avbuf_new(0);

  while (1) {
    char *cmdstr;
    //printsyment();
#if 0
    printf("> ");
    fflush(stdout);
    if (NULL == fgets(buf, 1024, fp)) {
      break;
    }
    //printf("%s\n", buf);
    split2avbuf(buf, &avptr);
#else
    write_history(hfile);
    if (line != 0) free(line);
    if (0 == (line = readline("m> "))) {
      break;
    }
    strncpy(buf, line, 1024);
    split2avbuf(buf, &avptr);
#endif
    cmdstr = avptr->av[0];
    if (!cmdstr) continue;
#if 1 
    add_history(line);
#endif
    //printf("cmd: \"%s\"\n", cmdstr);
    if (!strcmp(cmdstr, "#")) {
      // comment
      continue;
    }
    if (!strcmp(cmdstr, "load")) {
      //fprintf(stderr, "LOAD\n");
      if (avptr->used == 1) {
	fprintf(stderr, "load: too few argument.\n");
	continue;
      }
      last_appid = cmd_loadapp(avptr->av[1], ".");
      printf("app_id: %d\n", last_appid);
      continue;
    }

#if 0
    if (!strcmp(cmdstr, "run")) {
      int procid;
      unsigned int pid;
      //fprintf(stderr, "RUN\n");
      //char *symentbuf;
      //printf("%x\n", (char *)symentbuf);
      procid = cmd_runapp(last_appid, avptr->used, avptr->av);
      pid = pid_new(PID_TYPE_APP, procid);
      printf("proc: pid=%d invoked.\n", pid);
      fflush(stdout);
      //printsyment();
      mpm_procinit1(procid);
      fprintf(stderr, "proc: pid=%d initialized.\n", pid);
      fflush(stdout);
      continue;
    }
#endif

#if 0
    if (!strcmp(cmdstr, "app")) { // load & run version
      int appid;
      int procid;
      unsigned int pid;

      if (avptr->used == 1) {
	fprintf(stderr, "app: too few argument.\n");
	continue;
      }
      if (0 == (appid = cmd_loadapp(avptr->av[1], "."))) {
	fprintf(stderr, "failed to load %s\n", avptr->av[1]);
	continue;
      }
      procid = cmd_runapp(appid, avptr->used, avptr->av);
      printf("proc: %d invoked.\n", procid);
      fflush(stdout);
      //printsyment();
      mpm_procinit1(procid);
      pid = pid_new(PID_TYPE_APP, procid);
      fprintf(stderr, "proc: %d initialized.\n", pid);
      fflush(stdout);
      
      continue;
    }
#endif

#if 0
    if (!strcmp(cmdstr, "mop")) { // master only plugin
      int mopid, pid;
      if (avptr->used == 1) {
	fprintf(stderr, "app: too few argument.\n");
	continue;
      }

      mopid = mop_run(avptr->av[1], &(avptr->av[1]));
      pid = pid_new(PID_TYPE_MOP, mopid);
      fprintf(stderr, "mop: %d\n", pid);
      continue;
    }
#endif

    if (!strcmp(cmdstr, "runapp")) {
      int mopid, pid;
      if (avptr->used == 1) {
	fprintf(stderr, "runapp: too few argument.\n");
	continue;
      }

      printf("mmapp_run: %d %s\n", avptr->used, avptr->av[1]);
      pid = mmapp_run(avptr->used - 1, &(avptr->av[1]));
      continue;
    }

    if (!strcmp(cmdstr, "send")) { // send message
      if (avptr->used <= 2) {
	fprintf(stderr, "Usage: send {message} {Job_ID} {args} ...\n");
	continue;
      }
      continue;
    }


    if (!strcmp(cmdstr, "js")) { // joystick emulation
      int type, number, value;
      if (avptr->used < 3) {
	fprintf(stderr, "Usage: js type value\n\ttypes: ");
	jsnames_print_allnames(stderr);
	continue;
      }
      jsnames_str2data(avptr->av[1], &type, &number);
      value = atoi(avptr->av[2]);
      oclJS_changeStatus(0, type, number, value, 0); // Warning: time = 0
      continue;
    }

    if (!strcmp(cmdstr, "jsstat")) { // joystick status
      //void oclJS_getStatus(int no, struct ocl_jsstat *stat);
      struct ocl_jsstat stat;
      int i;

      oclJS_getStatus(0, &stat);

#if 1
      jsnames_print_jsstat(stderr, &stat);
#else
      for (i = 0; i < 4; ++i) {
	printf("jsstat: button%d = %d\n", i, stat.button[i]);
      }
#endif

      continue;
    }

    if (!strcmp(cmdstr, "jobs")) { // job list (process list)
      oclint_pid_foreach(cb_cmdjobs);
      continue;
    }

    if (!strcmp(cmdstr, "dispoff")) { // display off
      int pid, procid;

      if (avptr->used < 2) {
	fprintf(stderr, "Usage: dispoff pid\n");
	continue;
      }

      pid = atoi(avptr->av[1]);
      procid = check_gapp_procid(pid);
      if (0 > procid) continue;
      cmd_dispoff(procid);
      continue;
    }

    if (!strcmp(cmdstr, "dispon")) { // display on
      int pid, procid;

      if (avptr->used < 2) {
	fprintf(stderr, "Usage: dispon pid\n");
	continue;
      }

      pid = atoi(avptr->av[1]);
      procid = check_gapp_procid(pid);
      if (0 > procid) continue;
      cmd_dispon(procid);
      continue;
    }

    if ((!strcmp(cmdstr, "end")) ||
	(!strcmp(cmdstr, "exit")) ||
	(!strcmp(cmdstr, "quit"))) {
      break;
    }
    fprintf(stderr, "unknown command\n");
  }
  return master_end();
}

#ifdef STANDALONE

#if 0
struct renddesc {
  char *desc;
  struct renddesc *next;
};
static struct renddesc rdtop;
#endif

static int rdflag = 0;
static int flag_must_renderers = 1;

void
do_getopt(int ac, char **av)
{
  int c;
  while (EOF != (c = getopt(ac, av, "l:r:n"))) {
    switch (c) { 
    case 'n':
      flag_must_renderers = 0;
      break;
    case 'l':
      g_preload = strdup(optarg);
      break;
    case 'r':
      fprintf(stderr, "renderer %s: connection ", optarg);
      connect_renderer(optarg);
      fprintf(stderr, "done.\n");
      rdflag = 1;
      break;
    }
  }
}

int
main(int ac, char **av)
{
  // // run renderers
  // ssh localhost renderer -n "front" -p 8000
  // ssh localhost renderer -n "right" -p 8001
  minit0();

  do_getopt(ac, av);

  // // connect to renderes
  //connect_renderer("localhost", 28980);
#if 0
  if (!rdtop.next) {
    connect_renderer("localhost:28980");
  } else {
    struct renddesc *ptr = rdtop.next;
    while (ptr) {
      connect_renderer(rdtop.desc);
      ptr = ptr->next;
    }
  }
#else
  if ((!rdflag) && flag_must_renderers) {
    connect_renderer("localhost:28980");
  }
#endif
  fprintf(stderr, "connection done\n");
  init_after_connections_renderers();
  run_loop();
  return master_mainloop(ac, av);
}
#endif
