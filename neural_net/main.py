import argparse

import torch

from app.gen_data import generate_data
from app.training import train_model


def main():
    parser = argparse.ArgumentParser(description="AlphaZero Chess Trainer CLI")
    parser.add_argument(
        "--generate",
        type=int,
        metavar="N",
        help="Generate N training samples (e.g., --generate 100000)",
    )
    parser.add_argument(
        "--train", action="store_true", help="Train the model on the generated dataset"
    )
    parser.add_argument(
        "--data",
        type=str,
        default="./data/moves_dataset.csv",
        help="Path to training CSV file (default: ./data/moves_dataset.csv)",
    )
    parser.add_argument(
        "--model-name",
        type=str,
        default="stockfish",
        help="Model name for saving and loading (default: alphazero_chess)",
    )

    args = parser.parse_args()

    if args.generate:
        print(f"Generating {args.generate:,} games of training data...")
        generate_data(args.generate)

    if args.train:
        print(f"Training model '{args.model_name}' using dataset {args.data}...")
        train_model(
            args.data,
            model_name=args.model_name,
            batch_size=512,
            device="mps" if torch.mps.is_available() else "cpu",
        )

    if not args.generate and not args.train:
        print("No action specified. Use --generate N and/or --train.")
        parser.print_help()


if __name__ == "__main__":
    main()
