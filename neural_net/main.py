from app.gen_data import generate_data
from app.model import train_model

def main():
    # generate_data(10000)
    train_model("./data/moves_dataset.csv")

if __name__ == "__main__":
    main()
