import torch
import chess
import numpy as np

piece_to_index = {
    'P': 0, 'N': 1, 'B': 2, 'R': 3, 'Q': 4, 'K': 5,
    'p': 6, 'n': 7, 'b': 8, 'r': 9, 'q': 10, 'k': 11
}

def fen_to_tensor(fen: str) -> torch.Tensor:
    board = chess.Board(fen)
    tensor = np.zeros((12, 8, 8), dtype=np.float32)

    for square, piece in board.piece_map().items():
        idx = piece_to_index[piece.symbol()]
        row = 7 - (square // 8)
        col = square % 8
        tensor[idx, row, col] = 1.0

    return torch.tensor(tensor).flatten()  # shape (768,)

if __name__ == "__main__":
    board = chess.Board()
    print(fen_to_tensor(board.fen()))
