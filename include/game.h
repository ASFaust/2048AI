#ifndef GAME_H
#define GAME_H

#include <cstdint>
#include <array>
#include <vector>
#include <iostream>
#include <random>
#include <unordered_map>
#include <list>
#include <tuple>
#include <cstdint>
#include <utility>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

using namespace std;

void initialize_tables();

bool can_move(uint64_t board, int direction);

tuple<uint64_t, uint> move(uint64_t board, int direction);

tuple<uint64_t, uint> cached_move(uint64_t board, int direction);

uint64_t add_new_tile(uint64_t board);

bool is_game_over(uint64_t board);

void print_board(uint64_t board);

vector<uint> get_possible_moves(uint64_t board);

uint compute_best_move(const uint64_t board, const int samples, const int depth, const int policy);

vector<double> compute_scores(const uint64_t board, const int samples, const int depth, const int policy, vector<uint> moves);

vector<double> py_compute_scores(const uint64_t board, const int samples, const int depth, const int policy);

vector<vector<uint>> board_to_array(const uint64_t board);

uint compute_simple_best_move(uint64_t board, int policy);

#endif // GAME_H