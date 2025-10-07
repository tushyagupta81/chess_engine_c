#ifndef NN_MODEL_H
#define NN_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

// C-compatible wrapper function
float predict_fen(const char* fen);
int model_is_valid();
void initialize_model();

#ifdef __cplusplus
}
#endif

#endif // !NN_MODEL_H
