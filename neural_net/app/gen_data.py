import chess
import random
import pandas as pd
import os
from concurrent.futures import ProcessPoolExecutor, as_completed
from stockfish import Stockfish
from rich.progress import Progress, SpinnerColumn, BarColumn, TextColumn, TimeElapsedColumn

# Play a single game and return its moves
def play_game(seed, max_moves_per_game=100):
    random.seed(seed)
    engine = Stockfish(depth=5)
    board = chess.Board()
    for _ in range(random.randint(0, 5)):
        board.push(random.choice(list(board.legal_moves)))

    game_data = []
    moves_played = 0
    while not board.is_game_over() and moves_played < max_moves_per_game:
        engine.set_fen_position(board.fen())
        top_moves = engine.get_top_moves(3)
        next_move = random.choice(top_moves)
        fen = board.fen()
        game_data.append((fen, next_move["Move"]))
        board.push(chess.Move.from_uci(next_move["Move"]))
        moves_played += 1

    return game_data, moves_played

# Generate multiple games with progress
def generate_data(num_games=100, output_csv="data/moves_dataset.csv"):
    os.makedirs("data", exist_ok=True)
    dataset = []

    with Progress(
        SpinnerColumn(),
        TextColumn("[bold green]{task.description}"),
        BarColumn(),
        TextColumn("{task.completed}/{task.total} games"),
        TimeElapsedColumn(),
    ) as progress:
        main_task = progress.add_task("Generating games", total=num_games)

        with ProcessPoolExecutor() as executor:
            futures = {executor.submit(play_game, seed): seed for seed in range(num_games)}

            for future in as_completed(futures):
                game_data, moves_played = future.result()
                dataset.extend(game_data)
                progress.update(main_task, advance=1)

    df = pd.DataFrame(dataset, columns=["FEN", "move"])  # pyright: ignore[reportArgumentType]
    df.to_csv(output_csv, index=False)
    print(f"Generated {len(dataset)} positions and saved to {output_csv}")


if __name__ == "__main__":
    generate_data(100)
