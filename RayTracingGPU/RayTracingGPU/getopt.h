#pragma once
#ifndef GETOPT_H
#define GETOPT_H

extern int optind;		/* index of first non-option in argv      */
extern int optopt;		/* single option character, as parsed     */
extern int opterr;		/* flag to enable built-in diagnostics... */
/* (user may set to zero, to suppress)    */
extern char *optarg;		/* pointer to argument of current option  */

int getopt(int nargc, char * const nargv[], const char *ostr);

#endif

