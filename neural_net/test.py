import torch
from app.encoding import fen_to_king_relative, fen_to_tensor
from app.model import ChessEvaluator

# Device
device = 'mps' if torch.mps.is_available() else 'cpu'

# Load model
model = ChessEvaluator(input_size=2700)
model.load_state_dict(torch.load("models/king-reletive.pth", map_location=device))
model.to(device)
model.eval()

# Example FEN to test
fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
x = fen_to_king_relative(fen).float().unsqueeze(0).to(device)  # shape (1, 768)

# Forward pass
with torch.no_grad():
    eval_score = model(x).item()

print(f"Predicted evaluation (centipawns approx): {eval_score*1000:.1f}")
