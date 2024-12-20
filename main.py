from mcts2048 import initialize_tables, move, add_new_tile, is_game_over, print_board, compute_best_move, compute_simple_best_move

initialize_tables()

def show_game_with_best_move():
    board = 0
    board = add_new_tile(board)
    board = add_new_tile(board) # 2 tiles at the beginning
    while not is_game_over(board):
        print_board(board)
        best_move = compute_best_move(board,1000,10,4) # 100 samples, max depth 100 moves
        board, gain = move(board,best_move)
        print(f"Move: {best_move}, Gain: {gain}")
        board = add_new_tile(board)

    print_board(board)
    print("Game over")

show_game_with_best_move()
