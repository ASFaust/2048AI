#include "game.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;


PYBIND11_MODULE(mcts2048, m) {
    m.doc() = "mcts2048 python bindings";
    //m.def("flood_fill", &flood_fill, "Flood fill algorithm");
    //m.def("distances", &distances, "Distances of all grid cells to a given point. unreachable cells have distance -1");
    m.def("initialize_tables", &initialize_tables, "Initialize lookup tables");
    m.def("can_move", &can_move, "Check if a move is possible");
    m.def("move", &cached_move, "Move tiles in a given direction");
    m.def("add_new_tile", &add_new_tile, "Add a new tile to the board");
    m.def("is_game_over", &is_game_over, "Check if the game is over");
    m.def("print_board", &print_board, "Print the board");
    m.def("get_possible_moves", &get_possible_moves, "Get all possible moves");
    m.def("compute_best_move", &compute_best_move, "Compute the best move");
    m.def("compute_simple_best_move", &compute_simple_best_move, "Compute the best move using a simple heuristic");
    m.def("compute_scores", &py_compute_scores, "Compute scores for all possible moves");
    m.def("board_to_array", &board_to_array, "Convert board to 2D array");
};
