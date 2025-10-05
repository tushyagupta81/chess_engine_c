import chess
import numpy as np

def fen_to_tensor(fen: str) -> np.ndarray:
    board = chess.Board(fen)
    planes = np.zeros((18, 8, 8), dtype=np.float32)
    piece_map = {
        'P': 0, 'N': 1, 'B': 2, 'R': 3, 'Q': 4, 'K': 5,
        'p': 6, 'n': 7, 'b': 8, 'r': 9, 'q': 10, 'k': 11
    }
    for sq, piece in board.piece_map().items():
        r, c = divmod(sq, 8)
        planes[piece_map[piece.symbol()], 7 - r, c] = 1.0
    # side to move
    planes[12, :, :] = 1.0 if board.turn == chess.WHITE else 0.0
    # castling
    planes[13, :, :] = 1.0 if board.has_kingside_castling_rights(chess.WHITE) else 0.0
    planes[14, :, :] = 1.0 if board.has_queenside_castling_rights(chess.WHITE) else 0.0
    planes[15, :, :] = 1.0 if board.has_kingside_castling_rights(chess.BLACK) else 0.0
    planes[16, :, :] = 1.0 if board.has_queenside_castling_rights(chess.BLACK) else 0.0
    if board.ep_square is not None:
        r, c = divmod(board.ep_square, 8)
        planes[17, 7 - r, c] = 1.0
    return planes

if __name__ == "__main__":
    board = chess.Board()
    print(encode_board(board.fen()))
