#include "game.h"
//#include "LRUCache.h"
#include <array>
#include <random>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <cstdint>

// Random number generator for adding tiles
std::random_device rd;
std::mt19937 rng(rd());
std::uniform_real_distribution<double> dist(0.0, 1.0);

uint16_t move_result_left[65536];
unsigned move_score_left[65536];

uint16_t move_result_right[65536];
unsigned move_score_right[65536];

uint16_t move_result_up[65536];
unsigned move_score_up[65536];

uint16_t move_result_down[65536];
unsigned move_score_down[65536];

inline uint16_t reverse_nibbles(uint16_t val) {
    return ((val & 0xF) << 12) |
           ((val & 0xF0) << 4)  |
           ((val & 0xF00) >> 4) |
           ((val & 0xF000) >> 12);
}

void initialize_tables() {
    for (uint32_t row = 0; row < 65536; ++row) {
        uint16_t c0 = (row >>  0) & 0xF;
        uint16_t c1 = (row >>  4) & 0xF;
        uint16_t c2 = (row >>  8) & 0xF;
        uint16_t c3 = (row >> 12) & 0xF;

        uint16_t filtered[4] = {0,0,0,0};
        int idx = 0;
        if (c0 != 0) filtered[idx++] = c0;
        if (c1 != 0) filtered[idx++] = c1;
        if (c2 != 0) filtered[idx++] = c2;
        if (c3 != 0) filtered[idx++] = c3;

        unsigned score = 0;
        for (int i = 0; i < 3; ++i) {
            if (filtered[i] != 0 && filtered[i] == filtered[i + 1]) {
                filtered[i]++;
                score += (1U << filtered[i]);
                // Shift tiles after position i
                for (int k = i + 1; k < 3; ++k) {
                    filtered[k] = filtered[k + 1];
                }
                filtered[3] = 0;
            }
        }

        uint16_t new_row = (filtered[0] & 0xF)
                         | ((filtered[1] & 0xF) << 4)
                         | ((filtered[2] & 0xF) << 8)
                         | ((filtered[3] & 0xF) << 12);

        move_result_left[row] = new_row;
        move_score_left[row] = score;
    }

    for (uint32_t row = 0; row < 65536; ++row) {
        uint16_t reversed = reverse_nibbles((uint16_t)row);
        uint16_t res = move_result_left[reversed];
        unsigned scr = move_score_left[reversed];
        uint16_t final_res = reverse_nibbles(res);

        move_result_right[row] = final_res;
        move_score_right[row] = scr;
    }

    for (uint32_t col = 0; col < 65536; ++col) {
        uint16_t up_res = move_result_left[col];
        unsigned up_scr = move_score_left[col];
        move_result_up[col] = up_res;
        move_score_up[col] = up_scr;

        uint16_t reversed = reverse_nibbles((uint16_t)col);
        uint16_t res = move_result_left[reversed];
        unsigned scr = move_score_left[reversed];
        uint16_t final_res = reverse_nibbles(res);

        move_result_down[col] = final_res;
        move_score_down[col] = scr;
    }
}

// Check if a move in the given direction is possible
bool can_move(uint64_t board, int direction) {
    uint64_t new_board = get<0>(cached_move(board, direction));
    if (new_board != board) return true;
    return false;
}

// Check if the board is in a game over state
bool is_game_over(uint64_t board) {
    for (int direction = 0; direction < 4; ++direction) {
        if (can_move(board, direction)) return false;
    }
    return true;
}

// Add a new tile (2 or 4) to the board
uint64_t add_new_tile(uint64_t board) {
    std::vector<int> empty_positions;
    for (int i = 0; i < 16; ++i) {
        if (((board >> (i * 4)) & 0xF) == 0) {
            empty_positions.push_back(i);
        }
    }

    if (empty_positions.empty()) return board; // No space to add a tile

    int pos = empty_positions[rng() % empty_positions.size()];
    uint16_t new_tile = (dist(rng) < 0.9) ? 1 : 2; // 2 with 90%, 4 with 10%

    return board | ((uint64_t)new_tile << (pos * 4));
}


inline std::tuple<uint64_t, unsigned> move(uint64_t board, int direction) {
    unsigned out_score = 0;
    uint64_t new_board = 0;

    switch (direction) {
        case 0: { // Left
            {
                uint16_t line = (uint16_t)((board >> 0) & 0xFFFF);
                new_board |= ((uint64_t)move_result_left[line]) << 0;
                out_score += move_score_left[line];
            }
            {
                uint16_t line = (uint16_t)((board >> 16) & 0xFFFF);
                new_board |= ((uint64_t)move_result_left[line]) << 16;
                out_score += move_score_left[line];
            }
            {
                uint16_t line = (uint16_t)((board >> 32) & 0xFFFF);
                new_board |= ((uint64_t)move_result_left[line]) << 32;
                out_score += move_score_left[line];
            }
            {
                uint16_t line = (uint16_t)((board >> 48) & 0xFFFF);
                new_board |= ((uint64_t)move_result_left[line]) << 48;
                out_score += move_score_left[line];
            }
            break;
        }

        case 1: { // Right
            {
                uint16_t line = (uint16_t)((board >> 0) & 0xFFFF);
                new_board |= ((uint64_t)move_result_right[line]) << 0;
                out_score += move_score_right[line];
            }
            {
                uint16_t line = (uint16_t)((board >> 16) & 0xFFFF);
                new_board |= ((uint64_t)move_result_right[line]) << 16;
                out_score += move_score_right[line];
            }
            {
                uint16_t line = (uint16_t)((board >> 32) & 0xFFFF);
                new_board |= ((uint64_t)move_result_right[line]) << 32;
                out_score += move_score_right[line];
            }
            {
                uint16_t line = (uint16_t)((board >> 48) & 0xFFFF);
                new_board |= ((uint64_t)move_result_right[line]) << 48;
                out_score += move_score_right[line];
            }
            break;
        }

        case 2: { // Up
            for (int i = 0; i < 4; ++i) {
                uint16_t line =
                    ((board >> (i*4)) & 0xF) |
                    (((board >> (16 + i*4)) & 0xF) << 4) |
                    (((board >> (32 + i*4)) & 0xF) << 8) |
                    (((board >> (48 + i*4)) & 0xF) << 12);

                uint16_t result = move_result_up[line];
                out_score += move_score_up[line];

                new_board |= ((uint64_t)(result & 0xF) << (i*4));
                new_board |= ((uint64_t)((result >> 4) & 0xF) << (16 + i*4));
                new_board |= ((uint64_t)((result >> 8) & 0xF) << (32 + i*4));
                new_board |= ((uint64_t)((result >> 12) & 0xF) << (48 + i*4));
            }
            break;
        }

        case 3: { // Down
            for (int i = 0; i < 4; ++i) {
                uint16_t line =
                    ((board >> (i*4)) & 0xF) |
                    (((board >> (16 + i*4)) & 0xF) << 4) |
                    (((board >> (32 + i*4)) & 0xF) << 8) |
                    (((board >> (48 + i*4)) & 0xF) << 12);

                uint16_t result = move_result_down[line];
                out_score += move_score_down[line];

                new_board |= ((uint64_t)(result & 0xF) << (i*4));
                new_board |= ((uint64_t)((result >> 4) & 0xF) << (16 + i*4));
                new_board |= ((uint64_t)((result >> 8) & 0xF) << (32 + i*4));
                new_board |= ((uint64_t)((result >> 12) & 0xF) << (48 + i*4));
            }
            break;
        }

        default:
            return std::make_tuple(board, 0U);
    }

    return std::make_tuple(new_board, out_score);
}

tuple<uint64_t, uint> cached_move(uint64_t board, int direction) {
    /*LRUCache::Key key = {board, direction}; //actually slower! so we will not use this

    if (lruCache.contains(key)) {
        // Return cached result
        return lruCache.get(key);
    }*/

    auto val = move(board, direction);

    // Cache the result
    //lruCache.put(key, val);

    return val;
}

void print_board(uint64_t board) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            uint16_t tile = (board >> (i * 16 + j * 4)) & 0xF;
            if (tile == 0) {
                std::cout << "\t";
            } else {
                std::cout << (1 << tile) << "\t";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

vector<uint> get_possible_moves(uint64_t board) {
    vector<uint> moves;
    for (uint i = 0; i < 4; ++i) {
        if (can_move(board, i)) {
            moves.push_back(i);
        }
    }
    return moves;
}

uint count_zeros(uint64_t board) {
    uint count = 0;
    for (int i = 0; i < 16; ++i) {
        if (((board >> (i * 4)) & 0xF) == 0) {
            count++;
        }
    }
    return count;
}

uint count_min_adjacent_diff(uint64_t board) {
    uint16_t tiles[4][4];

    // Pre-extract all tile values
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            tiles[i][j] = (board >> (i * 16 + j * 4)) & 0xF;
        }
    }

    uint diff_sum = 0;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            int tile = (int)tiles[i][j];
            int min_diff = 16; // max difference for nibble-sized values is 15

            // Check up
            if (i > 0) {
                int diff = tile - (int)tiles[i - 1][j];
                if (diff < 0) diff = -diff;
                if (diff < min_diff) min_diff = diff;
            }

            // Check down
            if (i < 3) {
                int diff = tile - (int)tiles[i + 1][j];
                if (diff < 0) diff = -diff;
                if (diff < min_diff) min_diff = diff;
            }

            // Check left
            if (j > 0) {
                int diff = tile - (int)tiles[i][j - 1];
                if (diff < 0) diff = -diff;
                if (diff < min_diff) min_diff = diff;
            }

            // Check right
            if (j < 3) {
                int diff = tile - (int)tiles[i][j + 1];
                if (diff < 0) diff = -diff;
                if (diff < min_diff) min_diff = diff;
            }

            diff_sum += min_diff;
        }
    }

    return diff_sum;
}


uint compute_simple_best_move(uint64_t board, int policy) {
    if(policy == 0){ //avg score: 4100
        uint lowest_score = 0;
        uint best_direction = 4;
        const vector<uint> moves = get_possible_moves(board);
        if(moves.size() == 1){
            return moves[0];
        }
        for (uint i = 0; i < moves.size(); ++i) {
            uint direction = moves[i];
            auto [new_board, unused] = cached_move(board, direction);
            uint score = count_min_adjacent_diff(new_board);
            if (i == 0 || score < lowest_score) {
                lowest_score = score;
                best_direction = direction;
            }
        }
        return best_direction;
    } else if (policy == 1){ //avg score: 2980
        uint highest_score = 0;
        uint best_direction = 4;
        const vector<uint> moves = get_possible_moves(board);
        if(moves.size() == 1){
            return moves[0];
        }
        for (uint i = 0; i < moves.size(); ++i) {
            uint direction = moves[i];
            auto [new_board, unused] = cached_move(board, direction);
            uint score = count_zeros(new_board);
            if (i == 0 || score > highest_score) {
                highest_score = score;
                best_direction = direction;
            }
        }
        return best_direction;
    } else if (policy == 2){ //avg score: 830
        //just use the first possible direction
        return get_possible_moves(board)[0];
    } else if (policy == 3){ //avg score
        //randomly choose a direction
        return get_possible_moves(board)[rng() % get_possible_moves(board).size()];
    } else if (policy == 4){
        //use policy 0, but use the scores as probabilities
        vector<uint> moves = get_possible_moves(board);
        if(moves.size() == 1){
            return moves[0];
        }
        vector<double> scores;
        for (uint i = 0; i < moves.size(); ++i) {
            uint direction = moves[i];
            auto [new_board, unused] = cached_move(board, direction);
            uint score = count_min_adjacent_diff(new_board);
            scores.push_back(score);
        }
        //get the maximum score
        double max_score = *max_element(scores.begin(), scores.end());
        //subtract the maximum score from all scores
        for (int i = 0; i < scores.size(); i++) {
            scores[i] = max_score - scores[i]; //invert the scores - the higher the score, the lower the value
        }
        //normalize the scores
        double sum = 0;
        for (int i = 0; i < scores.size(); i++) {
            sum += scores[i];
        }
        for (int i = 0; i < scores.size(); i++) {
            scores[i] /= sum;
        }
        //now we have a probability distribution, we can sample from it
        double r = dist(rng);
        double sum2 = 0;
        for (int i = 0; i < scores.size(); i++) {
            sum2 += scores[i];
            if (r < sum2) {
                return moves[i];
            }
        }
        return moves[0]; //should never happen

    } else { //default policy: return the first possible direction
        return get_possible_moves(board)[0];
    }
}


uint compute_best_move(const uint64_t board, const int samples, const int depth, const int policy) {
    //this works by doing a mtcs search on the board
    //first we determine all possible moves
    vector<uint> moves = get_possible_moves(board);
    //if there is only one move possible we return that move
    if (moves.size() == 1) return moves[0];
    //no need to handle the case where there are no moves possible
    //so now we start the search
    //we will do 1000 simulations for each move with a maximum of 1000 moves each
    vector<double> scores = compute_scores(board, samples, depth, policy, moves);
    return moves[std::distance(scores.begin(), std::max_element(scores.begin(), scores.end()))];
}

vector<double> compute_scores(const uint64_t board, const int samples, const int depth, const int policy, vector<uint> moves) {
    //this works by doing a mtcs search on the board
    //if there is only one move possible we return that move
    if (moves.size() == 1) return {0};
    //no need to handle the case where there are no moves possible
    //so now we start the search
    //we will do 1000 simulations for each move with a maximum of 1000 moves each
    vector<double> scores;
    for (int i = 0; i < moves.size(); i++) {
        const uint direction = moves[i];
        const auto [base_board, base_score] = cached_move(board, direction); //semi-state after moving, before sampling a new tile
        double overall_score = 0;
        for(int j = 0; j < samples; j++){
            uint current_score = base_score;
            uint score_add = 0;
            uint64_t current_board = add_new_tile(base_board); //add a new random tile, completing the first move
            for(int k = 0; k < depth; k++){
                if(is_game_over(current_board)) break;

                const uint direction = compute_simple_best_move(current_board, policy); //policy 4 is random, but guided. should be best.
                tie(current_board, score_add) = cached_move(current_board, direction);

                current_score += score_add;
                current_board = add_new_tile(current_board);
            }
            overall_score += current_score;
        }
        scores.push_back(overall_score);
    }
    return scores;
}

vector<double> py_compute_scores(const uint64_t board, const int samples, const int depth, const int policy) {
    //this is a wrapper for the compute_scores function, so that it can be called from python. it returns the scores for all moves, not just the possible ones
    vector<double> ret;
    vector<uint> moves = {0, 1, 2, 3};
    vector<uint> possible_moves = get_possible_moves(board);
    vector<double> scores = compute_scores(board, samples, depth, policy, moves);
    for (int i = 0; i < 4; i++) {
        if (find(possible_moves.begin(), possible_moves.end(), i) != possible_moves.end()) {
            ret.push_back(scores[i]);
        } else {
            ret.push_back(0);
        }
    }
    return ret;
}

vector<vector<uint>> board_to_array(const uint64_t board) {
    //this function converts the board to a 4x4 array
    vector<vector<uint>> ret;
    for (int i = 0; i < 4; i++) {
        vector<uint> row;
        for (int j = 0; j < 4; j++) {
            row.push_back((board >> (i * 16 + j * 4)) & 0xF);
        }
        ret.push_back(row);
    }
    return ret;
}
