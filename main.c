#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct veiculo {
   char placa[9];
   char modelo[50];
   int tipo;
   float consumo;
};

// Cadastrar um novo veículo
void cadastro(struct veiculo* v) {
   printf("==== Cadastro de Veículo ====\n");
   printf("Placa (até 8 caracteres): ");
   scanf(" %8s", v->placa);

   printf("Modelo: ");
   scanf(" %[^\n]", v->modelo);

   do {
      printf("Tipo (1-Carro, 2-Moto, 3-Caminhão): ");
      scanf("%d", &v->tipo);
      if (v->tipo < 1 || v->tipo > 3) {
         printf("Tipo inválido! Tente novamente.\n");
      }
   } while (v->tipo < 1 || v->tipo > 3);

   printf("Consumo médio (km/l): ");
   scanf("%f", &v->consumo);
   printf("\n");
}

// Exibir os dados de um veículo
void exibir(struct veiculo v) {
   printf("\nPlaca: %s\n", v.placa);
   printf("Modelo: %s\n", v.modelo);
   printf("Tipo: ");
   switch (v.tipo) {
      case 1: printf("Carro\n"); break;
      case 2: printf("Moto\n"); break;
      case 3: printf("Caminhão\n"); break;
      default: printf("Desconhecido\n");
   }
   printf("Consumo: %.2f km/l\n", v.consumo);
}

// Buscar veículo por placa
int buscarPlaca(struct veiculo* frota, int total, char* placa) {
   for (int i = 0; i < total; i++) {
      if (strcmp(frota[i].placa, placa) == 0) {
         return i;
      }
   }
   return -1;
}

// Remover veículo
void removerVeiculo(struct veiculo* frota, int* total, char* placa) {
   int pos = buscarPlaca(frota, *total, placa);
   if (pos == -1) {
      printf("Veículo com placa %s não encontrado.\n", placa);
      return;
   }
   for (int i = pos; i < (*total - 1); i++) {
      frota[i] = frota[i + 1];
   }
   (*total)--;
   printf("Veículo removido com sucesso!\n");
}

// Salvar dados em arquivo
void salvarArquivo(struct veiculo* frota, int total) {
   FILE* f = fopen("frota.txt", "w");
   if (f == NULL) {
      printf("Erro ao salvar arquivo.\n");
      return;
   }
   for (int i = 0; i < total; i++) {
      fprintf(f, "%s|%s|%d|%.2f\n", frota[i].placa, frota[i].modelo, frota[i].tipo, frota[i].consumo);
   }
   fclose(f);
}

// Carregar dados do arquivo
void carregarArquivo(struct veiculo** frota, int* total, int* capacidade) {
   FILE* f = fopen("frota.txt", "r");
   if (f == NULL) return; // Arquivo não existe ainda

   char linha[128];
   while (fgets(linha, sizeof(linha), f)) {
      if (*total >= *capacidade) {
         *capacidade *= 2;
         *frota = realloc(*frota, (*capacidade) * sizeof(struct veiculo));
         if (*frota == NULL) {
            printf("Erro ao alocar memória ao carregar.\n");
            exit(1);
         }
      }

      sscanf(linha, "%8[^|]|%49[^|]|%d|%f", 
             (*frota)[*total].placa,
             (*frota)[*total].modelo,
             &(*frota)[*total].tipo,
             &(*frota)[*total].consumo);

      (*total)++;
   }
   fclose(f);
}

int main() {
   struct veiculo* frota = NULL;
   int total = 0, capacidade = 5;
   int opcao;
   char placa[9];

   frota = malloc(sizeof(struct veiculo) * capacidade);
   if (frota == NULL) {
      printf("Erro ao alocar memória.\n");
      return 1;
   }

   carregarArquivo(&frota, &total, &capacidade);

   do {
      printf("\n====== MENU ======\n");
      printf("1 - Cadastrar veículo\n");
      printf("2 - Listar veículos\n");
      printf("3 - Buscar veículo por placa\n");
      printf("4 - Remover veículo\n");
      printf("5 - Sair\n");
      printf("Escolha: ");
      scanf("%d", &opcao);

      switch (opcao) {
         case 1:
            if (total == capacidade) {
               capacidade *= 2;
               frota = realloc(frota, capacidade * sizeof(struct veiculo));
               if (frota == NULL) {
                  printf("Erro ao realocar memória.\n");
                  return 1;
               }
            }
            cadastro(&frota[total]);
            total++;
            salvarArquivo(frota, total);
            break;

         case 2:
            if (total == 0) {
               printf("Nenhum veículo cadastrado.\n");
            } else {
               for (int i = 0; i < total; i++) {
                  exibir(frota[i]);
               }
            }
            break;

         case 3:
            printf("Digite a placa para buscar: ");
            scanf(" %8s", placa);
            int pos = buscarPlaca(frota, total, placa);
            if (pos == -1) {
               printf("Veículo não encontrado.\n");
            } else {
               exibir(frota[pos]);
            }
            break;

         case 4:
            printf("Digite a placa do veículo para remover: ");
            scanf(" %8s", placa);
            removerVeiculo(frota, &total, placa);
            salvarArquivo(frota, total);
            break;

         case 5:
            printf("Encerrando o programa...\n");
            break;

         default:
            printf("Opção inválida! Tente novamente.\n");
      }
   } while (opcao != 5);

   free(frota);
   return 0;
}