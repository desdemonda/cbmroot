static char *rcsId = "$Header: /misc/hadesprojects/daq/cvsroot/eventbuilder/hadaq/hldread.c,v 6.10 2010-01-29 18:24:05 hadaq Exp $";


#define _POSIX_C_SOURCE 199309L
#define SYSLOG_NAMES
#include <unistd.h>

#include <stddef.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <limits.h>

#include <syslog.h>


#include "evt.h"
#include "hldread.h"

static unsigned long processedEvts = 0;
static unsigned int timeout = 0;

static int excludeAnal = 0;

static void showProcessedEvts(unsigned long processedEvts)
{
	static unsigned long keepProcessedEvts = 0;
	int level;
	char *notice;

	if (processedEvts > keepProcessedEvts) {
		level = LOG_INFO;
		notice = "";
	} else {
		level = LOG_NOTICE;
		notice = ", \ano new events arrived";
	}
	syslog(level, "%5u events processed %-30s\r", processedEvts, notice);
}

static void atexit0(void)
{
	if (!excludeAnal)
		stopAnalysis();
	syslog(LOG_INFO, "%5u events processed %-30s", processedEvts, "");
}





void sigHandler(int sig)
{
	exit(128 + sig);
}


void alarmHandler(int x)
{
	signal(SIGALRM, alarmHandler);
	alarm(timeout);
	showProcessedEvts(processedEvts);
}

int main(int argc, char *argv[])
{
	extern char *optarg;
	extern int optind;
	int i = 0;
	FILE *file = NULL;
	void *evt;
	unsigned long readEvts = 0;
	unsigned long fEvts = 0;
	unsigned long nEvts = ULONG_MAX;
	unsigned long echoRate = ULONG_MAX;
	char **analArgv = malloc(sizeof(char *));
	int analArgc = 1;			/* Filename is in analArgv[0] */

	openlog(argv[0], LOG_PID | LOG_PERROR, LOG_LOCAL0);
	setlogmask(LOG_UPTO(LOG_INFO));


	while ((i = getopt(argc, argv, "n:f:e:t:xo:v:")) != -1) {
		switch (i) {
		case 'f':
			fEvts = atol(optarg);
			break;
		case 'n':
			nEvts = atol(optarg);
			break;
		case 'e':
			echoRate = atol(optarg);
			break;
		case 't':
			timeout = atoi(optarg);
			break;
		case 'x':
			excludeAnal = 1;
			break;
		case 'o':
			analArgv = realloc(analArgv, sizeof(char *) * (analArgc + 1));

			analArgv[analArgc++] = optarg;
			break;
		case 'v':
			for (i = 0; prioritynames[i].c_name != NULL && 0 != strcmp(prioritynames[i].c_name, optarg); i++) {
			}
			if (prioritynames[i].c_name != NULL) {
				setlogmask(LOG_UPTO(prioritynames[i].c_val));
				break;
			}					/* FALLTHROUGH to default */
		default:
			syslog(LOG_ERR, "Usage: %s -f firstEvent -n numEvents -e echoRate", argv[0]
				);
			syslog(LOG_ERR, "-t timeout -o analArgs -x (excludeAnal) [fileName]");
			exit(EXIT_FAILURE);
			break;
		}						/* switch (i ... */
	}							/* while ((i ... */
	if (argv[optind] == NULL) {
		file = stdin;
		analArgv[0] = "-";
	} else {
		file = fopen(argv[optind], "r");
		analArgv[0] = argv[optind];
	}
	if (file == NULL) {
		syslog(LOG_ERR, "Opening input file: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (!excludeAnal) {
		startAnalysis(analArgc, analArgv);
	}
	atexit(atexit0);

	signal(SIGINT, sigHandler);
	signal(SIGTERM, sigHandler);
	signal(SIGHUP, sigHandler);

	signal(SIGALRM, alarmHandler);
	alarm(timeout);


	while ((evt = Evt_fget(file)) != NULL && processedEvts < nEvts) {
		readEvts++;
		if (readEvts >= fEvts) {
			if (!excludeAnal)
				analyseEvt(evt);
			processedEvts++;
			if (processedEvts == 1) {
				syslog(LOG_INFO, "First event  processed %-30s\r", "");
			} else if (processedEvts % echoRate == 0) {
				alarm(timeout);
				showProcessedEvts(processedEvts);
			}
		}
	}
	exit(EXIT_SUCCESS);
}
