import torch
import os
import torch.nn as nn
import torch.nn.functional as F
from torch.utils.data import Dataset, DataLoader
import pandas as pd
import chess
from app.encoding import fen_to_tensor
from rich.progress import Progress, SpinnerColumn, BarColumn, TextColumn, TimeElapsedColumn, MofNCompleteColumn, TimeRemainingColumn

# -----------------------------
# Constants
# -----------------------------
NUM_PLANES = 18
BOARD_SIZE = 8
NUM_MOVE_PLANES = 73
POLICY_OUTPUT = BOARD_SIZE * BOARD_SIZE * NUM_MOVE_PLANES

# -----------------------------
# Utility
# -----------------------------
def move_to_index(move_uci):
    from_square = chess.parse_square(move_uci[:2])
    r_from, c_from = divmod(from_square, 8)
    move_plane = hash(move_uci) % NUM_MOVE_PLANES
    return r_from * 8 * NUM_MOVE_PLANES + c_from * NUM_MOVE_PLANES + move_plane

# -----------------------------
# Dataset (chunk-based)
# -----------------------------
class ChessDataset(Dataset):
    def __init__(self, df):
        self.df = df.reset_index(drop=True)

    def __len__(self):
        return len(self.df)

    def __getitem__(self, idx):
        row = self.df.iloc[idx]
        board_tensor = fen_to_tensor(row["FEN"])
        label = move_to_index(row["move"])
        return torch.tensor(board_tensor, dtype=torch.float32), torch.tensor(label, dtype=torch.long)

# -----------------------------
# AlphaZero-style Network
# -----------------------------
class AlphaZeroNet(nn.Module):
    def __init__(self):
        super().__init__()
        self.conv1 = nn.Conv2d(NUM_PLANES, 128, kernel_size=3, padding=1)
        self.conv2 = nn.Conv2d(128, 128, kernel_size=3, padding=1)
        self.conv3 = nn.Conv2d(128, 128, kernel_size=3, padding=1)

        self.policy_conv = nn.Conv2d(128, 2, kernel_size=1)
        self.policy_fc = nn.Linear(2 * 8 * 8, POLICY_OUTPUT)

        self.value_conv = nn.Conv2d(128, 1, kernel_size=1)
        self.value_fc1 = nn.Linear(1 * 8 * 8, 128)
        self.value_fc2 = nn.Linear(128, 1)

    def forward(self, x):
        x = F.relu(self.conv1(x))
        x = F.relu(self.conv2(x))
        x = F.relu(self.conv3(x))

        p = F.relu(self.policy_conv(x))
        p = p.view(p.size(0), -1)
        p = self.policy_fc(p)

        v = F.relu(self.value_conv(x))
        v = v.view(v.size(0), -1)
        v = F.relu(self.value_fc1(v))
        v = torch.tanh(self.value_fc2(v))

        return p, v

# -----------------------------
# Train (chunked + Rich progress)
# -----------------------------
def train_model(csv_path, model_name="alphazero_chess", chunksize=50000):
    device = torch.device("cpu")
    model = AlphaZeroNet().to(device)
    optimizer = torch.optim.Adam(model.parameters(), lr=1e-3)
    policy_criterion = nn.CrossEntropyLoss()
    value_criterion = nn.MSELoss()

    os.makedirs("models", exist_ok=True)
    num_chunks = sum(1 for _ in pd.read_csv(csv_path, chunksize=chunksize))

    with Progress(
        SpinnerColumn(),
        TextColumn("[bold green]{task.description}"),
        BarColumn(),
        MofNCompleteColumn(),
        TextColumn("Loss: {task.fields[loss]:.4f}"),
        TimeElapsedColumn(),
        TimeRemainingColumn()
    ) as progress:
        epoch_task = progress.add_task("[cyan]Training Epochs...", total=10, loss=0.0)

        for epoch in range(10):
            total_loss = 0.0
            chunk_id = 0

            reader = pd.read_csv(csv_path, chunksize=chunksize)

            for chunk in reader:
                chunk_id += 1
                dataset = ChessDataset(chunk)
                loader = DataLoader(dataset, batch_size=1024, shuffle=True, num_workers=0)

                # Update the progress description with total chunks
                batch_task = progress.add_task(
                    f"Epoch {epoch+1} Chunk {chunk_id}/{num_chunks}",
                    total=len(loader),
                    loss=0.0
                )

                for X, y_policy in loader:
                    X, y_policy = X.to(device), y_policy.to(device)
                    optimizer.zero_grad()

                    logits, v = model(X)
                    y_value = torch.zeros(X.size(0), 1).to(device)
                    loss = policy_criterion(logits, y_policy) + value_criterion(v, y_value)

                    loss.backward()
                    optimizer.step()

                    total_loss += loss.item()
                    progress.update(batch_task, advance=1, loss=loss.item())

                progress.remove_task(batch_task)
                # free memory after each chunk
                del dataset, loader, chunk
                torch.cuda.empty_cache() if torch.cuda.is_available() else None

            avg_loss = total_loss / num_chunks
            progress.update(epoch_task, advance=1, loss=avg_loss)
            progress.console.print(f"[bold yellow]Epoch {epoch+1}[/]: Avg Loss = {avg_loss:.4f}")
            if (epoch +1)%2==0:
                scripted_model = torch.jit.script(model)
                scripted_model.save(f"models/{model_name}-{epoch+1}_jit.pth")
                torch.save(model.state_dict(), f"models/{model_name}-{epoch+1}.pth")
                print(f"Model saved as models/{model_name}-{epoch+1}.pth")

    # Save model
    scripted_model = torch.jit.script(model)
    scripted_model.save("models/alphazero_chess_jit.pth")
    torch.save(model.state_dict(), f"models/{model_name}.pth")
    print(f"Model saved as models/{model_name}.pth")

# -----------------------------
# Predict Function
# -----------------------------
def predict(fen, model_name="alphazero_chess"):
    device = torch.device("cpu")
    model = AlphaZeroNet().to(device)
    model.load_state_dict(torch.load(f"models/{model_name}.pth", map_location=device))
    model.eval()

    board_tensor = torch.tensor(fen_to_tensor(fen), dtype=torch.float32).unsqueeze(0).to(device)
    logits, value = model(board_tensor)
    probs = torch.softmax(logits, dim=1)
    return probs.detach().cpu().numpy(), value.item()

# -----------------------------
# Example Usage
# -----------------------------
if __name__ == "__main__":
    train_model("./data/moves_dataset.csv", model_name="alphazero_chess", chunksize=50000)

    fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    policy_probs, value = predict(fen)
    print("Value prediction:", value)
    print("Policy probs shape:", policy_probs.shape)
