#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include "cards.h"
#include "deck.h"
#include "eval.h"
#include "future.h"
#include "input.h"

int find_winning_hand(deck_t ** hands, int n_hands) {

  // start at 1, to compare 1-1=0 and 1
  int ihand_wins = 0;  // n_hands equals a tie
  int is_tie = 0;
  for (int ihand = 1; ihand < n_hands; ++ihand) {
    int comp = compare_hands(hands[ihand], hands[ihand_wins]);
    if (comp == 0) {
      is_tie = 1;
    }
    else {
      is_tie = 0;
      if (comp > 0) {
	ihand_wins = ihand;
      }
    }
  }  // loop all hands

  // check what is the winning hand or if there is a tie
  if (is_tie == 1) {
    ihand_wins = n_hands;
  }
  return ihand_wins;
}

int main(int argc, char ** argv) {
  int nof_runs = 10000;

  // corner case: no input file
  if (argc < 2) {
    fprintf(stderr, "syntax: poker <filename> [nof_mc]\n");
    return EXIT_FAILURE;
  }

  // get filename
  char * filename = argv[1];

  // overwrite nof_mcs if given
  if (argc > 2) {
    nof_runs = atoi(argv[2]);
  }

  // init future_card_t fc
  future_cards_t * fc = malloc(sizeof(*fc));
  fc->n_decks = 0;
  fc->decks = NULL;
  size_t n_hands = 0;

  // ========== READ IN HANDS FROM FILE =======================
  FILE * f = fopen(filename, "r");
  deck_t ** hands = read_input(f, &n_hands, fc);
  fclose(f);

  // =================== deck with remaining cards: stack =================
  deck_t * stack = build_remaining_deck(hands, n_hands);

  // one extra result: the tie (two hands equal
  int results[n_hands + 1];
  for (int ihand = 0; ihand <= n_hands; ++ihand) {
    results[ihand] = 0;
  }

  for (int irun = 0; irun < nof_runs; ++irun) {
    shuffle(stack);

    // draw cards to placeholders
    future_cards_from_deck(stack, fc);

    // find winning hand and add to hand result
    int ihand_wins = find_winning_hand(hands, n_hands);
    ++(results[ihand_wins]);

  }  // all mc runs



  // conclusions
  for (int ihand = 0; ihand < n_hands; ++ihand) {
    float perc = 100*((float)results[ihand])/nof_runs;
    printf("Hand %d won %d / %d times (%.2f%%)\n", ihand, results[ihand], nof_runs, perc);
  }  // all hands
  printf("And there were %d ties\n", results[n_hands]);

  // free stuff
  free_deck(stack);
  for (int ihand = 0; ihand < n_hands; ++ihand) {
    free_deck(hands[ihand]);
  }
  free(hands);

  for (size_t ifut = 0; ifut < fc->n_decks; ++ifut) {
    free(fc->decks[ifut].cards);
  }
  free(fc->decks);
  free(fc);


  return EXIT_SUCCESS;
}
