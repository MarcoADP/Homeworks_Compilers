#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include "set.h"
#include "producao.h"
#include "err.h"


struct set terminais;
struct set nao_terminais;
struct producoes producoes;
extern char buffer_err[];

void ajuda();
FILE *abre_arquivo(char * nome);
void leitura(FILE *arquivo);
void parse_linha(char *linha);
void parse_left(char *left, char *middle, struct regra *producao);
void parse_right(char *right, struct regra *producao);
void parse_terminais();
int convertCharToInt(char ch);
char convertIntToChar(int num);
void new_first();
void first();
void follow();

int main(int argc, char *argv[]) {


  int opcao;
  char nome_arquivo[60];

  bool i_flag = false;
  while ((opcao = getopt(argc,argv,"hi:")) != -1) {
    switch (opcao) {
      case 'i':
        strcpy(nome_arquivo, optarg);
        i_flag = true;
        break;
      case 'h':
      case '?':
      case ':':
      default:
        ajuda(argv[0]);
    }
  }
  if (!i_flag) {
    ajuda(argv[0]);
  }

  FILE * arquivo = abre_arquivo(nome_arquivo);
  producoes_init(&producoes);
  set_init(&nao_terminais);
  set_init(&terminais);
  leitura(arquivo);

  first();
  follow();

  printf("\n\n");
  fclose(arquivo);
  exit(EXIT_SUCCESS);
}

void ajuda(char *nome_programa) {
  sprintf(buffer_err, " Usage: %s -i arquivo_entrada\n", nome_programa);
  errexit(buffer_err);
}

FILE *abre_arquivo(char * nome) {
  FILE *arquivo = fopen(nome, "r");
  if (arquivo == NULL) {
    sprintf(buffer_err, "Erro ao abrir o arquivo \"%s\"\n", nome);
    errexit(buffer_err);
  }
  return arquivo;
}


void leitura(FILE *arquivo) {
  int linha_tam = 256;
  char linha[linha_tam];
  while (fgets(linha, linha_tam, arquivo) != NULL) {
    parse_linha(linha);
  }
  parse_terminais();
  printf("Terminais:\n");
  set_print(&terminais);
  printf("\n");
  printf("Não terminais:\n");
  set_print(&nao_terminais);
  printf("\n");
  printf("Regras de Produção:\n");
  producoes_print(&producoes);
  printf("\n");
}

void parse_linha(char *linha) {
  char *left, *right, *middle;
  struct regra producao;
  regra_init(&producao);
  left = linha;
  middle = strchr(linha, '-');
  right = middle + 1;
  parse_left(left, middle, &producao);
  parse_right(right, &producao);
  producoes_add(&producoes, &producao);
}

void parse_left(char *left, char *middle, struct regra *producao) {
  // avança ponteiro até o símbolo não terminal
  while (isspace(*left)) {
    left++;
  }
  set_add(&nao_terminais, *left);
  regra_add(producao, *left);
  printf(" NAO TERMINAL(%c) - ", *left);
}

void parse_right(char *right, struct regra *producao) {
  // avança ponteiro até o primeiro símbolo
  while (isspace(*right)) {
    right++;
  }
  do {
    while (!isspace(*right) && *right != '\0' && *right != '|') {
      regra_add(producao, *right);
      right++;
    }
    if (*right == '|') {
      producoes_add(&producoes, producao);
      producao->tamanho = 1;
    }
    if (*right != '\0') {
      right++;
    }
  } while (*right != '\0');
}

void parse_terminais() {
  struct regra *producao;
  for (int i = 0; i < producoes.tamanho; i++) {
    producao = &producoes.regras[i];
    for (size_t j = 1; j < producao->tamanho; j++) {
      if (!set_contains(&nao_terminais, producao->elementos[j])) {
        set_add(&terminais, producao->elementos[j]);
      }
    }
  }
}

int convertCharToInt(char ch) {
  return ch;
}

char convertIntToChar(int num) {
  return num;
}

void first() {
  //FIRST(A) = { t | A =>* tw for some w }
  bool mudou;
  struct regra *producao;
  char *elemento;
  do {
    mudou = false;
    for (int i = 0; i < producoes.tamanho; i++) {
      producao = &producoes.regras[i];
      elemento = &producao->elementos[1];
      if (set_contains(&terminais, *elemento)) {
        printf("first(%c) = %c\n", producao->elementos[0], *elemento);
      }
    }
  } while (mudou);
}

void follow() {
  //if()
}
