extern "C" {
#include "board.h"
#include "memory.h"
#include "pieces.h"
}
#include "nn_model.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <torch/script.h>
#include <torch/torch.h>

torch::jit::script::Module model;
torch::Device device(torch::Device::Type::CPU); // or kCUDA if GPU is compatible
bool is_init = false;
bool valid_model = false;

void initialize_model() {
  is_init = true;
  try {
    valid_model = true;
    model =
        torch::jit::load("neural_net/models/alphazero_chess_jit.pth", device);
  } catch (const c10::Error &e) {
    std::cerr << "Error loading the model\n";
  }
}

int model_is_valid() { return valid_model ? 1 : -1; }

// -----------------------------
// Convert FEN to 18x8x8 tensor
// -----------------------------
torch::Tensor fen_to_tensor(const std::string &fen) {
  torch::Tensor planes = torch::zeros({18, 8, 8}, torch::kFloat32);

  // Parse FEN manually or with a chess library
  Board *board = new_board(fen.c_str());
  std::map<int, char> pieces; // map from square index 0-63 -> piece char

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      Pieces p_enum =
          *(Pieces *)get_at(board->board, (uint16_t)i * BOARD_ROW + j);
      int plane_index;
      switch (p_enum) {
      case WhitePawn:
        plane_index = 0;
        break;
      case WhiteRook:
        plane_index = 1;
        break;
      case WhiteKnight:
        plane_index = 2;
        break;
      case WhiteBishop:
        plane_index = 3;
        break;
      case WhiteQueen:
        plane_index = 4;
        break;
      case WhiteKing:
        plane_index = 5;
        break;
      case BlackPawn:
        plane_index = 6;
        break;
      case BlackRook:
        plane_index = 7;
        break;
      case BlackKnight:
        plane_index = 8;
        break;
      case BlackBishop:
        plane_index = 9;
        break;
      case BlackQueen:
        plane_index = 10;
        break;
      case BlackKing:
        plane_index = 11;
        break;

      case Blank:
      default:
        continue; // skip empty squares
      }
      planes[plane_index][7 - i][j] = 1.0;
    }
  }

  planes[12].fill_(board->player == White ? 1.0 : 0.0);
  planes[13].fill_(board->castling.WhiteKingSide ? 1.0 : 0.0);
  planes[14].fill_(board->castling.WhiteQueenSide ? 1.0 : 0.0);
  planes[15].fill_(board->castling.BlackKingSide ? 1.0 : 0.0);
  planes[16].fill_(board->castling.BlackQueenSide ? 1.0 : 0.0);

  if (board->enpassant.valid) {
    int r = board->enpassant.row;
    int c = board->enpassant.col;
    planes[17][r][c] = 1.0;
  }

  deinit_board(board);
  return planes.unsqueeze(0); // Add batch dim
}

// -----------------------------
// Main prediction
// -----------------------------
float predict_fen(const char *fen) {

  // Load scripted model
  if (!is_init) {
    initialize_model();
  }

  if (model_is_valid() == -1) {
    return -1;
  }

  torch::Tensor board_tensor = fen_to_tensor(fen).to(device);

  // Forward pass
  std::vector<torch::jit::IValue> inputs;
  inputs.push_back(board_tensor);

  auto output = model.forward(inputs).toTuple();
  torch::Tensor policy = output->elements()[0].toTensor();
  torch::Tensor value = output->elements()[1].toTensor();

  // std::cout << "Value prediction: " << value.item<float>() << std::endl;
  // std::cout << "Policy logits size: " << policy.sizes() << std::endl;

  return value.item<float>();
}
