/*
 * This getopt version is based on the AT&T public-domain code as published 
 * in Mod.sources Volume 3, Issue 58.
 * 
 * 2006-10-14, Henrik Maier (HM):
 * Modifications to compile with Visual C++.
 */
/*
Archive-name: att_getopt
Mod.sources:  Volume 3, Issue 58
From: talcott!seismo!ut-sally!jsq (John Quarterman)
Subject: public domain AT&T getopt(3)
Newsgroups: mod.sources
Organization: IEEE/P1003 Portable Operating System Environment Committee
Approved: jpn@panda.UUCP

Posting-number:  Volume 3, Issue 58
Submitted by: seismo!ut-sally!jsq (John Quarterman, Moderator mod.std.unix)

[
  There are two articles here, forwarded from mod.std.unix.  Also, the
  getopt source code is NOT in shar format - you will have to hand
  edit this file.       -   John P. Nelson,  moderator, mod.sources
]

************************

Newsgroups: mod.std.unix
Subject: public domain AT&T getopt source
Date: 3 Nov 85 19:34:15 GMT

Here's something you've all been waiting for:  the AT&T public domain
source for getopt(3).  It is the code which was given out at the 1985
UNIFORUM conference in Dallas.  I obtained it by electronic mail
directly from AT&T.  The people there assure me that it is indeed
in the public domain.

There is no manual page.  That is because the one they gave out at
UNIFORUM was slightly different from the current System V Release 2
manual page.  The difference apparently involved a note about the
famous rules 5 and 6, recommending using white space between an option
and its first argument, and not grouping options that have arguments.
Getopt itself is currently lenient about both of these things White
space is allowed, but not mandatory, and the last option in a group can
have an argument.  That particular version of the man page evidently
has no official existence, and my source at AT&T did not send a copy.
The current SVR2 man page reflects the actual behavor of this getopt.
However, I am not about to post a copy of anything licensed by AT&T.

I will submit this source to Berkeley as a bug fix.

I, personally, make no claims or guarantees of any kind about the
following source.  I did compile it to get some confidence that
it arrived whole, but beyond that you're on your own.
*/

#include <string.h>
#include <stdio.h>
#undef NULL
#undef EOF

/*LINTLIBRARY*/
#define NULL	0
#define EOF	(-1)
#define ERR(s, c)	if(opterr){fprintf(stderr, "%s%s%c\n", argv[0], s, c);}

int	opterr = 1;
int	optind = 1;
int	optopt;
char	*optarg;

int 
getopt(int argc, char **argv, char *opts)
{
	static int sp = 1;
	register int c;
	register char *cp;

	if(sp == 1)
		if(optind >= argc ||
		   argv[optind][0] != '-' || argv[optind][1] == '\0')
			return(EOF);
		else if(strcmp(argv[optind], "--") == NULL) {
			optind++;
			return(EOF);
		}
	optopt = c = argv[optind][sp];
	if(c == ':' || (cp=strchr(opts, c)) == NULL) {
		ERR(": illegal option -- ", c);
		if(argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		return('?');
	}
	if(*++cp == ':') {
		if(argv[optind][sp+1] != '\0')
			optarg = &argv[optind++][sp+1];
		else if(++optind >= argc) {
			ERR(": option requires an argument -- ", c);
			sp = 1;
			return('?');
		} else
			optarg = argv[optind++];
		sp = 1;
	} else {
		if(argv[optind][++sp] == '\0') {
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return(c);
}

