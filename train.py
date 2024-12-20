import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader, Dataset
from mcts2048 import board_to_array
import time

# Preprocessing functions
def preprocess_board(board):
    board = np.array(board_to_array(int(board)), dtype=np.float32)
    board[board != 0] = np.log2(board[board != 0])
    if board.max() == 0:
        print(board)

    board /= board.max()
    vertical_edges = np.zeros((3, 4))
    horizontal_edges = np.zeros((4, 3))
    # edges get filled with the info wether the two tiles are the same or not
    for i in range(4):
        for j in range(3):
            horizontal_edges[i, j] = board[i, j] == board[i, j + 1]
    for i in range(3):
        for j in range(4):
            vertical_edges[i, j] = board[i, j] == board[i + 1, j]
    # then we also need to get the empty tiles
    empty = np.zeros_like(board)
    empty[board == 0] = 1
    ret = np.concatenate([board.flatten(), vertical_edges.flatten(), horizontal_edges.flatten(), empty.flatten()])
    return ret

def preprocess_scores(scores):
    return scores / scores.sum()

# Custom Dataset
class MCTS2048Dataset(Dataset):
    def __init__(self, boards, scores):
        self.boards = np.array([preprocess_board(board) for board in boards], dtype=np.float32)
        self.scores = np.array([preprocess_scores(score) for score in scores], dtype=np.float32)

    def __len__(self):
        return len(self.boards)

    def __getitem__(self, idx):
        return torch.tensor(self.boards[idx]), torch.tensor(self.scores[idx])

# Define the dense neural network
class DenseNetwork(nn.Module):
    def __init__(self, input_size=56, output_size=4, hidden_units=[128, 32]):
        super(DenseNetwork, self).__init__()
        layers = []
        last_size = input_size
        for units in hidden_units:
            layers.append(nn.Linear(last_size, units))
            layers.append(nn.SiLU())
            last_size = units
        layers.append(nn.Linear(last_size, output_size))
        self.network = nn.Sequential(*layers)

    def forward(self, x):
        return self.network(x)

# Training function
def train_model(model, dataloader, optimizer, epochs=10):
    model.train()
    for epoch in range(epochs):
        epoch_loss = 0.0
        for X, y in dataloader:
            X, y = X.to(device), y.to(device)
            optimizer.zero_grad()
            outputs = model(X)
            loss = torch.mean((outputs - y) ** 2)
            loss.backward()
            optimizer.step()
            epoch_loss += loss.item()
        print(f"Epoch {epoch + 1}/{epochs}, Loss: {epoch_loss / len(dataloader):.4f}")

# Testing function
def predict_move(board, model):
    model.eval()
    with torch.no_grad():
        processed_board = torch.tensor(preprocess_board(board), dtype=torch.float32).unsqueeze(0).to(device)
        probabilities = model(processed_board)
        return probabilities.squeeze(0).cpu().numpy()

# Main script
if __name__ == "__main__":
    # Load data
    data = np.load("training_data.npz")
    boards = data["boards"]
    scores = data["scores"]

    # Prepare dataset and dataloader
    dataset = MCTS2048Dataset(boards, scores)
    dataloader = DataLoader(dataset, batch_size=256, shuffle=True)

    # Set up model, loss, optimizer
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    model = DenseNetwork().to(device)
    optimizer = optim.Adam(model.parameters(), lr=0.01)

    # Train the model
    train_model(model, dataloader, optimizer, epochs=100)

    # Example prediction
    example_board = boards[0]  # Take the first board for testing
    probabilities = predict_move(example_board, model)
    print("Predicted probabilities for moves:", probabilities)
