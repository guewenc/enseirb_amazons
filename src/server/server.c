#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "export.h"
#include "game.h"
#include "utils.h"

static int export = 0;

static void usage(const char* command) {
    printf("Usage: %s [-s seed] [-e] [-m size] [-t shape] player1 player2\n", command);
    printf("Options:\n");
    printf("\t-s : set the game seed [default: random]\n");
    printf("\t-e : enable game export\n");
    printf("\t-m : set the game board size (minimum: %d) [default: %d]\n", 5, 8);
    printf("\t-t : set the game board shape (c: square, 8: eight, t: clover, d: donut) [default: c]\n");
}

static int parse_int_arg(const char* arg) {
    char* end_ptr;
    long value = strtol(arg, &end_ptr, 10);

    if (*arg == '\0' || *end_ptr != '\0' || value < 0) {
        handle_error(__func__, "Invalid argument", PROGRAM_EXIT);
    }

    return (int)value;
}

void display_winner(cgame g) {
    printf("Player %s won the game\n", game__get_player_name(g, game__get_winner(g)));
}

void export_game(cgame g, uint nb_frames) {
    char name[20];
    sprintf(name, "%s/%u.dot", EXPORT_DIR, nb_frames);
    game__export(g, name);
}

static void handle_args(int argc, char* argv[], struct game_config* config) {
    int opt;

    while ((opt = getopt(argc, argv, "m:s:t:e")) != -1) {
        switch (opt) {
            case 'm':
                config->size = parse_int_arg(optarg);
                break;
            case 't':
                config->board_shape = optarg[0];
                break;
            case 's':
                config->seed = parse_int_arg(optarg);
                break;
            case 'e':
                export = 1;
                break;
            default:
                usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind + 2 != argc) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
    struct game_config config = game__default_config();

    handle_args(argc, argv, &config);

    const char* player1_path = (optind < argc) ? argv[optind] : "";
    const char* player2_path = (optind + 1 < argc) ? argv[optind + 1] : "";

    uint turn = 0;
    game g = game__new();

    game__init(g, &config, player1_path, player2_path);

    if (export) {
        export__remove_dir(EXPORT_DIR);
        export__create_dir(EXPORT_DIR);
        export_game(g, turn++);
    }

    while (!game__is_over(g)) {
        game__play(g);
        game__next_player(g);

        if (export)
            export_game(g, turn++);
    }

    if (export)
        export__compile_dot();

    display_winner(g);
    game__delete(g);

    return EXIT_SUCCESS;
}
