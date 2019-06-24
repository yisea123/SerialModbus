#ifndef _GETOPT_H_
#define _GETOPT_H_

#ifdef __cplusplus
extern "C" {
#endif

extern int getopt(int argc, char **argv, char *opts);
extern int optind;
extern int opterr;
extern char *optarg;

#ifdef __cplusplus
}
#endif

#endif
