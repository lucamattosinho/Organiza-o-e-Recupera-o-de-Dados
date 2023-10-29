#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAM_MAX_REG 256

FILE *arq;

//struct LED
struct{
	int cab;
}LED;

//ASTERISCO
char asterisco = '*';

//INSERCAO
void insereReg(FILE *arq, char *buffer){
    int byte_offset, byte_offset_ant, byte_offset_post, inserido = 0;
    short tam, comp_reg, disp, sobra, posinserida;
    char bufferaux[TAM_MAX_REG], delim = '\0';
    rewind(arq);
    
    comp_reg = strlen(buffer);
    
    
    if(LED.cab == -1){
    	//Lista de espacos disponiveis vazia: insercao no fim do arquivo
        fseek(arq, 0, SEEK_END);
        fwrite(&comp_reg, sizeof(comp_reg), 1, arq);
        fwrite(buffer, 1, comp_reg, arq);
        printf("Local: fim do arquivo\n");
    }
    else{
        byte_offset = LED.cab;
        byte_offset_ant = byte_offset;
        fseek(arq, LED.cab, SEEK_SET);
        fread(&tam, sizeof(short), 1, arq);
        fread(&asterisco, 1, 1, arq);
        fread(&byte_offset_post, sizeof(byte_offset_ant), 1, arq);
        while(!inserido){
    		if(tam >= comp_reg){
    			posinserida = ftell(arq);
                fseek(arq, byte_offset, SEEK_SET);
                fwrite(&comp_reg, sizeof(short), 1, arq);
        		fwrite(buffer, 1, comp_reg, arq);
        		sobra = tam - comp_reg - 2;
                fwrite(&delim, sizeof(delim), 1, arq);
                fseek(arq, byte_offset, SEEK_SET);
                comp_reg = comp_reg + sobra + 2;
                fwrite(&comp_reg, sizeof(comp_reg), 1, arq);
                if(byte_offset == LED.cab){
                    LED.cab = byte_offset_post;
					rewind(arq);
					fwrite(&LED.cab, sizeof(LED), 1, arq);
                }
                else{
                    fseek(arq, byte_offset_ant + 2, SEEK_SET);
                    fwrite(&asterisco, 1, 1, arq);
                    fwrite(&byte_offset_post, sizeof(byte_offset_post), 1, arq);
                }
            	printf("Tamanho do espaco reutilizado: %d bytes\nLocal: offset = %d\n", tam, byte_offset);
            	inserido = 1;
            	break;
            }
        	if(byte_offset_post == -1){
            	fseek(arq, 0, SEEK_END);
            	fwrite(&comp_reg, sizeof(comp_reg), 1, arq);
        		fwrite(buffer, 1, comp_reg, arq);
            	printf("Local: fim do arquivo\n");
				break;
        	}
        	else{
            	fseek(arq, byte_offset_post, SEEK_SET);
            	byte_offset_ant = byte_offset;
            	byte_offset = byte_offset_post;
            	fread(&tam, sizeof(short), 1, arq);
            	fread(&asterisco, 1, 1, arq);
            	fread(&byte_offset_post, sizeof(int), 1, arq);
        	}
    	}
    }
}

//REMOCAO
void removeReg(FILE *arq, char *ident){
	short comp_reg;
    char *ident_busca, buffer[TAM_MAX_REG], buffer2[TAM_MAX_REG];
    int achou = 0, byte_offset, posicaorem, i;
    	
    //busca primeiro
    fseek(arq, sizeof(LED), SEEK_SET);
    fread(&comp_reg, sizeof(short), 1, arq);
   	while(fread(buffer, sizeof(char), comp_reg, arq) != 0){
   		buffer[comp_reg] = '\0';
        strcpy(buffer2, buffer);
   		ident_busca = strtok(buffer2, "|");
        if(strcmp(ident_busca, ident) == 0){
        	achou = 1;
	    	break;
		}
        else{
            fread(&comp_reg, sizeof(short), 1, arq);
        }
	}
    if(achou==0){
        printf("Erro: registro nao encontrado!\n");
	}
    //remove
    else{
        if(LED.cab == -1){
        	byte_offset = comp_reg;
        	fseek(arq, -byte_offset, SEEK_CUR);
			posicaorem = ftell(arq);
        	fwrite(&asterisco, sizeof(char), 1, arq);
            fwrite(&LED.cab, sizeof(LED), 1, arq);
			LED.cab = posicaorem-2;
			rewind(arq);
			fwrite(&LED.cab, sizeof(LED), 1, arq);
		}
		else{
			byte_offset = comp_reg + 2;
        	fseek(arq, -byte_offset, SEEK_CUR);
           	posicaorem = ftell(arq);
           	fseek(arq, 2, SEEK_CUR);
           	fwrite(&asterisco, sizeof(char), 1, arq);
           	fwrite(&LED.cab, sizeof(LED), 1, arq);
           	LED.cab = posicaorem;
           	rewind(arq);
			fwrite(&LED.cab, sizeof(LED), 1, arq);
		}
		printf("Registro removido!   (%d bytes)\nLocal: offset = %d\n", comp_reg, LED.cab);                                                                            
	}
}

//IMPRESSAO LED
void imprimeLED(FILE *arq){
	int byte_offset, byte_offset_post, tam;
	if(LED.cab == -1){
		printf("\nLista vazia. Nao ha espacos disponiveis.\n");
	}
	else{
		byte_offset = LED.cab;
		printf("\nLista de espacos disponiveis:\n\n");
		printf("%d", byte_offset);
		fseek(arq, LED.cab, SEEK_SET);
		fread(&tam, sizeof(short), 1, arq);
    	fread(&asterisco, 1, 1, arq);
    	fread(&byte_offset_post, sizeof(int), 1, arq);
		while(byte_offset != -1){
			byte_offset = byte_offset_post;
			printf(" -> ");
			fseek(arq, byte_offset_post, SEEK_SET);
			fread(&tam, sizeof(short), 1, arq);
    		fread(&asterisco, 1, 1, arq);
    		fread(&byte_offset_post, sizeof(byte_offset), 1, arq);
    		printf("%d", byte_offset);
    	}
    }
}
	
//BUSCA
void buscaReg(FILE *arq, char *ident){
    short comp_reg;
    char *ident_busca, buffer2[TAM_MAX_REG], buffer[TAM_MAX_REG];
    int achou = 0;
	fseek(arq, sizeof(LED), SEEK_SET);
	fread(&comp_reg, sizeof(short), 1, arq);
    while(!achou){
    	fread(buffer, sizeof(char), comp_reg, arq);
        buffer[comp_reg] = '\0';
        strcpy(buffer2, buffer);
        ident_busca = strtok(buffer2, "|");
        if(strcmp(ident_busca, ident) == 0){
            achou = 1;
            break;
		}
        else{
            fread(&comp_reg, sizeof(short), 1, arq);
        }
    }
    if(achou == 0){
        printf("Erro: registro nao encontrado!\n");
    }
    else{
        printf("Registro encontrado:\n%s  (%d bytes)\n", buffer, comp_reg);
    }
}

int main(int argc, char *argv[]){
	
	LED.cab = -1;

    if(argc == 3 && strcmp(argv[1], "-i") == 0){
		
        printf("Modo de importacao ativado ... nome do arquivo = %s\n", argv[2]);
        //IMPORTACAO
        FILE *novo_arq;
        arq = fopen(argv[2],"r+b");
    	novo_arq = fopen("dados.dat", "wb");
		fwrite(&LED.cab, sizeof(LED), 1, novo_arq);
        char buffer[TAM_MAX_REG];
        short comp_reg;
        while(!feof(arq)){
            fgets(buffer, TAM_MAX_REG, arq);
            if(buffer[strlen(buffer) - 1] == '\n'){
                buffer[strlen(buffer) - 1] = '\0';
            }
        	comp_reg = strlen(buffer);
            printf("\nJogo: %s\nTamanho do registro: %d\n", buffer, comp_reg);
			fwrite(&comp_reg, sizeof(comp_reg), 1, novo_arq);
			fwrite(buffer, sizeof(char), strlen(buffer), novo_arq);
        }
        printf("\n\nArquivo importado com sucesso.\n");
        fclose(arq);
        fclose(novo_arq);
	} 
	
	else if(argc == 3 && strcmp(argv[1], "-e") == 0){

        printf("Modo de execucao de operacoes ativado ... nome do arquivo = %s\n", argv[2]);
        // executar_operacoes(argv[2]));
        
        arq = fopen("dados.dat", "r+b");
        if(arq == NULL){
            fprintf(stderr, "Arquivo dados.dat nao existe!\n");
            exit(EXIT_FAILURE);
        }

        FILE *arq_op = fopen(argv[2], "r+b");
        
        if(arq_op == NULL){
            fprintf(stderr, "Esse arquivo nao existe!\n");
            exit(EXIT_FAILURE);
        }

        char op, buffer2[TAM_MAX_REG], buffer3[TAM_MAX_REG];
    	
    	rewind(arq);
		fread(&LED, sizeof(LED), 1, arq);
	
    	while(!feof(arq_op)){
    		op = fgetc(arq_op);
    		fseek(arq_op, 1, SEEK_CUR);
        	fgets(buffer2, TAM_MAX_REG, arq_op);
            if(buffer2[strlen(buffer2) - 1] == '\n'){
            	buffer2[strlen(buffer2) - 1] = '\0';
            }
        	switch(op){
        	case 'b':
                printf("\nBusca pelo registro de chave '%s'\n", buffer2);
                buscaReg(arq, buffer2);
                break;
            case 'i':
            	strcpy(buffer3, buffer2);
            	strtok(buffer2, "|");
            	printf("\nInsercao do registro de chave '%s' (%d bytes)\n", buffer2, strlen(buffer3));
            	insereReg(arq, buffer3);
            	break;
			case 'r':
				printf("\nRemocao do registro de chave '%s'\n", buffer2);
				removeReg(arq, buffer2);
				rewind(arq);
				break;	
			}
		}
	fclose(arq);
	fclose(arq_op);
	}
	
	else if (argc == 2 && strcmp(argv[1], "-p") == 0){
        printf("Modo de impressao da LED ativado ...\n");
        arq = fopen("dados.dat", "r+b");
        if(arq == NULL){
            fprintf(stderr, "Arquivo dados.dat nao existe!\n");
            exit(EXIT_FAILURE);
        }
        rewind(arq);
        fread(&LED, sizeof(LED), 1, arq);
    	imprimeLED(arq);
    } 
	
	else{
		fprintf(stderr, "Argumentos incorretos!\n");
        fprintf(stderr, "Modo de uso:\n");
        fprintf(stderr, "$ %s (-i|-e) nome_arquivo\n", argv[0]);
        fprintf(stderr, "$ %s -p\n", argv[0]);
        exit(EXIT_FAILURE);
	}

    return 0;
}
