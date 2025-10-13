import chess
import numpy as np
import torch

piece_to_index = {
    "P": 0,
    "N": 1,
    "B": 2,
    "R": 3,
    "Q": 4,
    "K": 5,
    "p": 6,
    "n": 7,
    "b": 8,
    "r": 9,
    "q": 10,
    "k": 11,
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


def fen_to_king_relative(fen: str) -> torch.Tensor:
    """
    Converts a FEN string to a 2700-length king-relative NNUE encoding.
    Each feature represents a piece type at a square relative to the friendly king.
    """
    board = chess.Board(fen)

    # Friendly king square
    if board.turn == chess.WHITE:
        fking_square = board.king(chess.WHITE)
    else:
        fking_square = board.king(chess.BLACK)


    features = []

    for square, piece in board.piece_map().items():
        # Skip the friendly king itself
        if square == fking_square:
            continue

        idx = piece_to_index[piece.symbol()]

        # Compute rank/file offsets relative to friendly king
        rank_offset = (chess.square_rank(square) - chess.square_rank(fking_square)) + 7  # pyright: ignore[reportArgumentType]
        file_offset = (chess.square_file(square) - chess.square_file(fking_square)) + 7  # pyright: ignore[reportArgumentType]

        # Clamp offsets to [0,14]
        rank_offset = max(0, min(14, rank_offset))
        file_offset = max(0, min(14, file_offset))

        # Map to 12 × 15 × 15 = 2700 feature index
        feature_idx = idx * 15 * 15 + rank_offset * 15 + file_offset
        features.append(feature_idx)

    # Create sparse binary tensor
    tensor = torch.zeros(12 * 15 * 15, dtype=torch.float32)
    tensor[features] = 1.0

    return tensor  # shape (2700,)


if __name__ == "__main__":
    board = chess.Board()
    print(fen_to_tensor(board.fen()))
