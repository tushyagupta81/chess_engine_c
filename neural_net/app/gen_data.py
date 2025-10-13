import multiprocessing
import os
import random
from concurrent.futures import ProcessPoolExecutor, as_completed

import chess
import pandas as pd
from rich.progress import (BarColumn, Progress, SpinnerColumn, TextColumn,
                           TimeElapsedColumn, TimeRemainingColumn)
from stockfish import Stockfish


def play_game():
    engine = Stockfish(depth=5)
    board = chess.Board()

    # randomize starting moves a bit
    for _ in range(random.randint(0, 5)):
        board.push(random.choice(list(board.legal_moves)))

    game_data = []
    while not board.is_game_over():
        engine.set_fen_position(board.fen())
        top_moves = engine.get_top_moves(3)
        if not top_moves:
            break
        next_move = random.choice(top_moves)

        fen = board.fen()
        game_data.append((fen, next_move["Centipawn"]))

        move = chess.Move.from_uci(next_move["Move"])
        if move not in board.legal_moves:
            break
        board.push(move)

    return game_data


def generate_data(num_games=100, output_csv="data/moves_dataset.csv"):
    os.makedirs("data", exist_ok=True)
    dataset = []

    num_cpus = max(1, multiprocessing.cpu_count() // 2)

    with Progress(
        SpinnerColumn(),
        TextColumn("[bold green]{task.description}"),
        BarColumn(),
        TextColumn("{task.completed}/{task.total} games"),
        TimeElapsedColumn(),
        TimeRemainingColumn(),
    ) as progress:
        main_task = progress.add_task("Generating games", total=num_games)

        with ProcessPoolExecutor(max_workers=num_cpus) as executor:
            futures = [executor.submit(play_game) for _ in range(num_games)]

            for future in as_completed(futures):
                game_data = future.result()
                dataset.extend(game_data)
                progress.update(main_task, advance=1)

    df = pd.DataFrame(dataset, columns=["FEN", "Centipawn"])  # pyright: ignore[reportArgumentType]
    df.to_csv(output_csv, index=False)
    print(f"Generated {len(dataset)} positions and saved to {output_csv}")


if __name__ == "__main__":
    generate_data(100)
