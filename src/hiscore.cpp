
#include "hiscore.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <algorithm>
using std::sort;
#include <list>
using std::list;

#define SCOREFILE "data/hs.dat"

class score_entry
{
public:
	string name;
	int score;
	score_entry (string n, int s) {
		name = n;
		score = s;
	}
	bool operator< (const score_entry& se) {
		return score > se.score;
	}
};

static list<score_entry> scores;

/*
static int se_cmp(const score_entry*a, const score_entry*b)
{
	return a->score - b->score;
}*/

void bkHSInit()
{
	int sc_t;
	char name_t[256];

	scores = list<score_entry>();
	FILE*f;
	f = fopen (SCOREFILE, "r");
	if (f) {
		while (!feof (f)) {
			fscanf (f, "%d %200s\n", &sc_t, name_t);
			scores.push_back (score_entry (name_t, sc_t));
		}
		fclose (f);
	}
	scores.sort();
	list<score_entry>::iterator i;
	for (i = scores.begin(); i != scores.end(); ++i)
		printf ("%d>>%s\n", i->score, i->name.c_str());
}

void bkHSSave()
{
	FILE*f = fopen (SCOREFILE, "w");
	if (!f) return;
	list<score_entry>::iterator i;
	for (i = scores.begin(); i != scores.end(); ++i)
		fprintf (f, "%d %s\n", i->score, i->name.c_str());
	fclose (f);
}

