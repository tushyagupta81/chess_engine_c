import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader

from app.model import ChessDataset, ChessEvaluator


def train_model(
    csv_path, epochs=10, batch_size=64, lr=1e-4, device="cpu", model_name="stockfish"
):
    dataset = ChessDataset(csv_path)
    dataloader = DataLoader(dataset, batch_size=batch_size, shuffle=True)

    model = ChessEvaluator().to(device)
    criterion = nn.MSELoss()
    optimizer = optim.Adam(model.parameters(), lr=lr)

    for epoch in range(epochs):
        running_loss = 0.0
        for X_batch, y_batch in dataloader:
            X_batch, y_batch = X_batch.to(device), y_batch.to(device)

            optimizer.zero_grad()
            outputs = model(X_batch)
            loss = criterion(outputs.squeeze(), y_batch)
            loss.backward()
            optimizer.step()

            running_loss += loss.item() * X_batch.size(0)

        epoch_loss = running_loss / len(dataset)
        print(f"Epoch {epoch + 1}/{epochs}, Loss: {epoch_loss:.6f}")

    # Save trained model
    torch.save(model.state_dict(), "chess_evaluator.pt")
    print("Model saved as chess_evaluator.pt")

    # Save TorchScript JIT model for C++ inference
    scripted_model = torch.jit.script(model)
    scripted_model.save(f"models/{model_name}_jit.pth")
    torch.save(model.state_dict(), f"models/{model_name}.pth")
    print(f"Model saved as models/{model_name}.pth")


if __name__ == "__main__":
    train_model(
        "data/moves_dataset.csv",
        epochs=20,
        batch_size=128,
        lr=1e-4,
        device="mps" if torch.mps.is_available() else "cpu",
    )
