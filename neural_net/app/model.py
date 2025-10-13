import pandas as pd
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.utils.data import Dataset

from app.encoding import fen_to_king_relative, fen_to_tensor


class ChessEvaluator(nn.Module):
    def __init__(self, input_size=768, hidden_sizes=[512, 256, 128, 64]):
        super(ChessEvaluator, self).__init__()
        self.fc1 = nn.Linear(input_size, hidden_sizes[0])
        self.fc2 = nn.Linear(hidden_sizes[0], hidden_sizes[1])
        self.fc3 = nn.Linear(hidden_sizes[1], hidden_sizes[2])
        self.fc4 = nn.Linear(hidden_sizes[2], hidden_sizes[3])
        self.fc5 = nn.Linear(hidden_sizes[3], 1)  # scalar output

    def forward(self, x):
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = F.relu(self.fc3(x))
        x = F.relu(self.fc4(x))
        return self.fc5(x)


class ChessDataset(Dataset):
    def __init__(self, csv_file, king_reletive=False):
        self.df = pd.read_csv(csv_file)
        self.king_reletive = king_reletive
        self.df = self.df.dropna()
        self.df = self.df.reset_index(drop=True)
        self.X = [fen for fen in self.df["FEN"]]
        # Normalize centipawn targets
        self.y = [
            torch.clamp(torch.tensor(score / 1000.0, dtype=torch.float32), -10.0, 10.0)
            for score in self.df["Centipawn"]
        ]

    def __len__(self):
        return len(self.X)

    def __getitem__(self, idx):
        if self.king_reletive:
            return fen_to_king_relative(self.X[idx]), self.y[idx]
        else:
            return fen_to_tensor(self.X[idx]), self.y[idx]
