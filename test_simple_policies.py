from mcts2048 import initialize_tables, move, add_new_tile, is_game_over, print_board, compute_best_move, compute_simple_best_move

initialize_tables()


def eval_simple_best_move(n_games,policy):
    score = 0
    for i in range(n_games):
        board = 0
        board = add_new_tile(board)
        board = add_new_tile(board) # 2 tiles at the beginning
        for i in range(10000):
            best_move = compute_simple_best_move(board,policy)
            board, gain = move(board,best_move)
            score += gain
            board = add_new_tile(board)
            if is_game_over(board):
                break
    return score / n_games

for policy in range(5):
    avg_score = eval_simple_best_move(1000, policy)
    print(f"For policy {policy}, average score of simple best move: {avg_score}")
