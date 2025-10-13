#include <string.h>

#define LAYERS 12
#define ROWS 8
#define COLS 8

void flatten_tensor(float tensor[LAYERS][ROWS][COLS],
                    float flat[LAYERS * ROWS * COLS]) {
  int k = 0;
  for (int l = 0; l < LAYERS; l++) {
    for (int r = 0; r < ROWS; r++) {
      for (int c = 0; c < COLS; c++) {
        flat[k++] = tensor[l][r][c];
      }
    }
  }
}

// Map piece characters to layer indices
int piece_to_index(char piece) {
  switch (piece) {
  case 'P':
    return 0;
  case 'N':
    return 1;
  case 'B':
    return 2;
  case 'R':
    return 3;
  case 'Q':
    return 4;
  case 'K':
    return 5;
  case 'p':
    return 6;
  case 'n':
    return 7;
  case 'b':
    return 8;
  case 'r':
    return 9;
  case 'q':
    return 10;
  case 'k':
    return 11;
  default:
    return -1; // empty or invalid
  }
}

// Convert FEN board part to tensor
void fen_to_tensor(const char *fen, float tensor[LAYERS][ROWS][COLS]) {
  memset(tensor, 0, sizeof(float) * LAYERS * ROWS * COLS);

  int row = 0, col = 0;
  for (int i = 0; fen[i] != ' ' && fen[i] != '\0'; i++) {
    char c = fen[i];
    if (c >= '1' && c <= '8') {
      col += c - '0'; // skip empty squares
    } else if (c == '/') {
      row++;
      col = 0;
    } else {
      int idx = piece_to_index(c);
      if (idx >= 0) {
        tensor[idx][row][col] = 1.0f;
      }
      col++;
    }
  }
}
