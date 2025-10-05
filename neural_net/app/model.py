import torch
import os
import torch.nn as nn
import torch.nn.functional as F
from torch.utils.data import Dataset, DataLoader
import pandas as pd
import chess
from app.encoding import fen_to_tensor

NUM_PLANES = 18  # input planes
BOARD_SIZE = 8
NUM_MOVE_PLANES = 73
POLICY_OUTPUT = BOARD_SIZE * BOARD_SIZE * NUM_MOVE_PLANES

def move_to_index(move_uci):
    from_square = chess.parse_square(move_uci[:2])
    r_from, c_from = divmod(from_square, 8)
    # Simplified: hash into NUM_MOVE_PLANES
    move_plane = hash(move_uci) % NUM_MOVE_PLANES
    return r_from * 8 * NUM_MOVE_PLANES + c_from * NUM_MOVE_PLANES + move_plane

class ChessDataset(Dataset):
    def __init__(self, df):
        self.boards = [fen_to_tensor(fen) for fen in df['FEN']]
        self.labels = [move_to_index(m) for m in df['move']]

    def __len__(self):
        return len(self.labels)

    def __getitem__(self, idx):
        return torch.tensor(self.boards[idx], dtype=torch.float32), torch.tensor(self.labels[idx], dtype=torch.long)

# -----------------------------
# AlphaZero-style network
# -----------------------------
class AlphaZeroNet(nn.Module):
    def __init__(self):
        super().__init__()
        self.conv1 = nn.Conv2d(NUM_PLANES, 128, kernel_size=3, padding=1)
        self.conv2 = nn.Conv2d(128, 128, kernel_size=3, padding=1)
        self.conv3 = nn.Conv2d(128, 128, kernel_size=3, padding=1)

        # Policy head
        self.policy_conv = nn.Conv2d(128, 2, kernel_size=1)
        self.policy_fc = nn.Linear(2*8*8, POLICY_OUTPUT)

        # Value head
        self.value_conv = nn.Conv2d(128, 1, kernel_size=1)
        self.value_fc1 = nn.Linear(1*8*8, 128)
        self.value_fc2 = nn.Linear(128, 1)

    def forward(self, x):
        x = F.relu(self.conv1(x))
        x = F.relu(self.conv2(x))
        x = F.relu(self.conv3(x))

        # Policy
        p = F.relu(self.policy_conv(x))
        p = p.view(p.size(0), -1)
        p = self.policy_fc(p)

        # Value
        v = F.relu(self.value_conv(x))
        v = v.view(v.size(0), -1)
        v = F.relu(self.value_fc1(v))
        v = torch.tanh(self.value_fc2(v))

        return p, v

# -----------------------------
# Train function
# -----------------------------
def train_model(csv_path, model_name="model"):
    df = pd.read_csv(csv_path)
    dataset = ChessDataset(df)
    loader = DataLoader(dataset, batch_size=32, shuffle=True)

    device = torch.device("cpu")
    model = AlphaZeroNet().to(device)
    optimizer = torch.optim.Adam(model.parameters(), lr=1e-3)
    policy_criterion = nn.CrossEntropyLoss()
    value_criterion = nn.MSELoss()

    for epoch in range(10):
        total_loss = 0
        for X, y_policy in loader:
            X, y_policy = X.to(device), y_policy.to(device)
            optimizer.zero_grad()
            logits, v = model(X)
            # placeholder for value targets (use actual outcomes if available)
            y_value = torch.zeros(X.size(0), 1).to(device)
            loss = policy_criterion(logits, y_policy) + value_criterion(v, y_value)
            loss.backward()
            optimizer.step()
            total_loss += loss.item()
        print(f"Epoch {epoch+1} Loss: {total_loss/len(loader):.4f}")

    os.makedirs("models", exist_ok=True)
    scripted_model = torch.jit.script(model)  # or torch.jit.trace(model, example_input)
    scripted_model.save("models/alphazero_chess.pth")
    print(f"Model saved as models/{model_name}.pth")

# -----------------------------
# Predict function
# -----------------------------
def predict(fen, model_name="model"):
    device = torch.device("cpu")
    model = AlphaZeroNet().to(device)
    model.load_state_dict(torch.load(f"models/{model_name}.pth", map_location=device))
    model.eval()

    board_tensor = torch.tensor(fen_to_tensor(fen), dtype=torch.float32).unsqueeze(0).to(device)
    logits, value = model(board_tensor)
    probs = torch.softmax(logits, dim=1)
    return probs.detach().cpu().numpy(), value.item()

# -----------------------------
# Example usage
# -----------------------------
if __name__ == "__main__":
    # Train on CSV dataset
    train_model("./data/moves_dataset.csv", model_name="alphazero_chess")

    # Predict on a FEN
    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    policy_probs, value = predict(fen, model_name="alphazero_chess")
    print("Value prediction:", value)
    print("Policy probs shape:", policy_probs.shape)
