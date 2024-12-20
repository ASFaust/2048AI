import numpy as np
from mcts2048 import initialize_tables, move, add_new_tile, is_game_over, print_board, compute_scores, board_to_array

initialize_tables()

def play_game(n_samples, max_depth, policy):
    board = 0
    board = add_new_tile(board)
    board = add_new_tile(board)  # 2 tiles at the beginning
    ret_boards = []
    ret_scores = []
    it = 0
    while not is_game_over(board):
        it += 1
        print(f"\rGame iteration {it}", end="", flush=True)
        scores = compute_scores(board, n_samples, max_depth, policy)
        ret_boards.append(board)
        ret_scores.append(scores)
        best_move = max(range(4), key=lambda x: scores[x])
        board, gain = move(board, best_move)
        board = add_new_tile(board)
    print("\nGame over")
    return np.array(ret_boards), np.array(ret_scores)

def gather_training_data(total_data_points, n_samples, max_depth, policy, filename="training_data.npz"):
    all_boards = []
    all_scores = []
    current_data_points = 0

    while current_data_points < total_data_points:
        print(f"\nPlaying a new game... Current data points: {current_data_points}/{total_data_points}")
        ret_boards, ret_scores = play_game(n_samples, max_depth, policy)
        all_boards.extend(ret_boards)
        all_scores.extend(ret_scores)
        current_data_points += len(ret_boards)

    # Save the data
    all_boards = np.array(all_boards, dtype=np.uint64)
    all_scores = np.array(all_scores, dtype=np.float32)
    np.savez_compressed(filename, boards=all_boards, scores=all_scores)
    print(f"Data saved to {filename} with {len(all_boards)} samples.")

# Example usage:
gather_training_data(total_data_points=20000, n_samples=1000, max_depth=10, policy=3)
