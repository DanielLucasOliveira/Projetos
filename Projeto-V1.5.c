//Sintese
//Objetivo: Controle Alfândegario Brasileiro.
//Autores: André Gadelha e Daniel Lucas
//Entrada Funcionario: Código e nome.
//Entrada Migrante: Código, nome, CPF, motivo, data, pais ou estado brasileiro e código do funcionário
//Saída: Informações do Migrante, informações do funcionário, população relativa e dados migratótios.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct tMigrante {
	int codigo;
	char nome[30];
	int motivo;
	int entrada_saida;
	char cpf[10];
	int data, mes, ano;
	int codFuncionario; 	
	char deletado;
	int nacionalidade; 
	char passaporte[9];
};

struct tFuncionario {
	int codigo; 
	char nome[30];
	char senha[10];
	char deletado; 
};

struct tDados {
	int Trabalho;
	int Estudo;
    int Turismo;
    int Moradia;
    int Assuntos_Diplomaticos;
    int Populacao_Relativa;
};

//*** Prototipos de funcoes ****************************************************
int telaLogin(FILE*);
int menuAdm(void);
int menufunc(void);
int menuCRUD(char[]);
int validarCPF(char *cpf);
FILE *abrirArquivo(char[]);

// CRUD dos Migrantes
void crudMigrante(FILE*, FILE*, FILE*);
void gravarMigrante(struct tMigrante, int, FILE*);
void listarMigrante(FILE*, FILE*);
int consultarMigrante(int, FILE*);
int consultarMigranteFuncionario(int, FILE*);
void excluirMigrante(int, FILE*);
struct tMigrante lerMigrante(int, FILE*);

// CRUD dos funcionarios
void crudFuncionario(FILE*, FILE*);
void gravarFuncionario(struct tFuncionario , int, FILE*);
void listarFuncionario(FILE*);
int consultarFuncionario(int, FILE*);
void excluirFuncionario(int, FILE*);
struct tFuncionario lerFuncionario(int, FILE*);

// CRUD dos dados
void gravarDados(struct tDados, FILE*);
void listarDados(FILE*);
struct tDados lerDados(FILE*);

int main(void) {
	int opcao, Adm_Func, codigo, pos;
	
	FILE *arqMigrante, *arqFuncionario, *arqDados;
	
	arqMigrante = abrirArquivo("migrante.dat");
	arqFuncionario = abrirArquivo("funcionario.dat");
	arqDados = abrirArquivo("Dados.dat");
	
	if ((arqMigrante == NULL) || (arqFuncionario == NULL) || (arqDados == NULL)) {
		printf("ERRO de abertura do arquivo!");
		return 1;
	}
	
	Adm_Func = telaLogin(arqFuncionario);
	if(Adm_Func == 1) {
		do {
	        opcao = menuAdm();
	        switch (opcao) {
	               case 1: 
						crudMigrante(arqMigrante, arqFuncionario, arqDados);
						break;
	               case 2: 
						crudFuncionario(arqFuncionario, arqMigrante);
						break;
	               case 3: 
	                    printf("\n\n*** Dados Coletados ***\n\n");
	                    lerDados(arqDados);
	                    listarDados(arqDados);
	        }
	    } while (opcao != 0);
	    fclose(arqMigrante);
	    fclose(arqFuncionario);
	    fclose(arqDados);
	}else if(Adm_Func == 2) {
		do {
	        opcao = menufunc();
	        switch (opcao) {
	               case 1: 
						crudMigrante(arqMigrante, arqFuncionario, arqDados);
						break;
	               case 2:
				        printf("\n\n*** Dados Coletados ***\n\n");
				        lerDados(arqDados);
	                    listarDados(arqDados); 
	        }
	    } while (opcao != 0);
	    fclose(arqMigrante);
	    fclose(arqFuncionario);
		fclose(arqDados);	
	}
	
	return 0;
}

//*** Login *********************************************************************
int telaLogin(FILE *arq){
	struct tFuncionario func;
	int codigo, pos, LoginAdmin = 00;
	char senhaAdmin[10] = "1234";
	char senha[10];
	
	printf("\n\n*** Login ***\n\n");
	do{
		printf("Login: ");
		scanf("%d", &codigo);
		pos = consultarFuncionario(codigo, arq);
		
		if (pos > 0 || codigo == LoginAdmin) {
			func = lerFuncionario(pos, arq);
			do{
				printf("Senha: ");
			    fflush(stdin);
		        gets(senha);
		        if(strcmp(func.senha, senha) == 0){
		        	return 2;
				}else if(strcmp(senhaAdmin, senha) == 0) {
					return 1;
				}else
					printf("senha invalida!\n");
				
			}while(strcmp(func.senha, senha) != 0 || strcmp(senhaAdmin, senha) != 0);
		}
		else
		printf("Login invalido!\n");		
    }while(pos <= 0 || codigo != LoginAdmin);
}

//*** Menu Adm *********************************************************************
int menuAdm(void) {
    int op;
    
    printf("\n\n*** MENU ADM ***\n\n");
    printf("1. Migrante\n");
    printf("2. Funcionario\n");
    printf("3. Dados Coletados\n");
    printf("0. Sair\n\n");
    printf("Escolha sua opcao: ");
    scanf("%d", &op);
    return op;
}

//*** Menu Func *********************************************************************
int menufunc(void) {
    int op;
    
    printf("\n\n*** MENU ***\n\n");
    printf("1. Migrante\n");
    printf("2. Dados Coletados\n");
    printf("0. Sair\n\n");
    printf("Escolha sua opcao: ");
    scanf("%d", &op);
    return op;
}

//*** Menu de CRUD *****************************************************************
int menuCRUD(char entidade[]) {
    int op;
    
    printf("\n\n*** %s ***\n\n", entidade);
    printf("1. Incluir\n");
    printf("2. Listar\n");
    printf("3. Consultar\n");
    printf("4. Alterar\n");
    printf("5. Excluir\n");
    printf("0. Voltar\n\n");
    printf("Escolha sua opcao: ");
    scanf("%d", &op);
    return op;
}

//*** Crud Migrante ************************************************************
void crudMigrante(FILE *arqMigrante, FILE *arqFuncionario, FILE *arqDados){
	int opcao, codigo, pos, posFuncionario, validar;
	struct tMigrante migrante;
	struct tFuncionario funcionario;
	struct tDados dados;
	
	dados.Trabalho=0;
	dados.Estudo=0;
    dados.Turismo=0;
    dados.Moradia=0;
    dados.Assuntos_Diplomaticos=0;
    dados.Populacao_Relativa=211755692;
	
	do {
        opcao = menuCRUD("Migrante");
        switch (opcao) {
               case 1: 
                    printf("\n\n*** Inclusao de Migrante ***\n\n");
					printf("Digite o codigo do migrante: ");
					scanf("%d", &migrante.codigo);
					printf("Digite o Nome do Migrante: ");
	                fflush(stdin);
	                gets(migrante.nome);
	                
					printf("Digite o motivo da viagem: \n");
	                printf("1. Trabalho\n");
	                printf("2. Estudos\n");
	                printf("3. Turismo\n");
	                printf("4. Moradia\n");
	                printf("5. Assuntos Diplomaticos\n");
	                printf("Digite a opcao: ");
					scanf("%d", &migrante.motivo);
					
					lerDados(arqDados);
					if(migrante.motivo == 1){
						dados.Trabalho = dados.Trabalho + 1;
						gravarDados(dados, arqDados);
					}else if(migrante.motivo == 2) {
						dados.Estudo = dados.Estudo +1;
						gravarDados(dados, arqDados);
					}else if(migrante.motivo == 3) {
						dados.Turismo = dados.Turismo + 1;
						gravarDados(dados, arqDados);
					}else if(migrante.motivo == 4) {
						dados.Moradia = dados.Moradia + 1;
						gravarDados(dados, arqDados);
					}else{
						dados.Assuntos_Diplomaticos = dados.Assuntos_Diplomaticos + 1;
						gravarDados(dados, arqDados);
					}
					
					printf("Digite uma das opcoes abaixo: \n");
	                printf("1. Saida\n");
	                printf("2. Entrando\n");
	                printf("Digite a opcao:");
	                scanf("%d", &migrante.entrada_saida);
	                
	                if(migrante.entrada_saida == 1){
	                	dados.Populacao_Relativa = dados.Populacao_Relativa - 1;
	                	gravarDados(dados, arqDados);
					}else{
						dados.Populacao_Relativa = dados.Populacao_Relativa + 1;
						gravarDados(dados, arqDados);
					}
					
					printf("Digite a data de saida ou entrada do pais:(ex dd/mm/aaaa) \n");
					scanf("%d/%d/%d", &migrante.data, &migrante.mes, &migrante.ano);
	                printf("Digite a opcao 1-Residente brasileiro, 2-Estrangeiro:");
	                scanf("%d", &migrante.nacionalidade);
	                
	                if(migrante.nacionalidade == 2){
	                	do{
	                		printf("Informe seu passaporte:");
	                		fflush(stdin);
	                		gets(migrante.passaporte);
	                		if(strlen(migrante.passaporte)!=8){
	                			printf("Passaporte invalido! Tente novamente.\n");
							}
	                	}while (strlen(migrante.passaporte)!=8);
	                }else{
                		printf("Digite o CPF : ");
	                	fflush(stdin);
	               		gets(migrante.cpf);
	               		validar = validarCPF(migrante.cpf);
	               		if(validar == 0){
			    			printf("\nCPF invalido.\n");
			    			break;
                    	}else if(strcmp(migrante.cpf,"54584343152") == 0 || strcmp(migrante.cpf,"26142254520") == 0) {
                    		printf("\nMigrante Proibido de deixar o pais.\n");
                    		break;
						}else{
                        	printf("CPF valido.\n");
                    	}
	            	}
	                
					listarFuncionario(arqFuncionario);
					printf("Funcionario (cod): ");
					scanf("%d", &migrante.codFuncionario);
                	gravarMigrante(migrante, -1, arqMigrante);
	                break;
               case 2: 
                    printf("\n\n*** Listagem de Migrantes ***\n\n");
                    listarMigrante(arqMigrante, arqFuncionario);
	                break;
               case 3: 
                    printf("\n\n*** Consulta de Migrantes ***\n\n");
                    printf("Digite o codigo do migrante: ");
					scanf("%d", &codigo);
					pos = consultarMigrante(codigo, arqMigrante);
					if (pos > 0) {
						migrante = lerMigrante(pos, arqMigrante);
						printf("Nome: %s\n", migrante.nome);
						printf("Motivo: %d\n",  migrante.motivo);
						printf("1 - entrada | 2 - saida: %d\n",  migrante.entrada_saida);
						printf("Data : %d/%d/%d \n", migrante.data, migrante.mes, migrante.ano);
						printf("1 - Residente brasileiro | 2 - Estrangeiro: %d\n", migrante.nacionalidade);
						if(migrante.nacionalidade == 1){
							printf("CPF: %s\n", migrante.cpf);
						}else{
							printf("Passaporte: %s\n", migrante.passaporte);
						}
						pos = consultarFuncionario(migrante.codFuncionario, arqFuncionario);
						funcionario = lerFuncionario(pos, arqFuncionario);
						printf("Funcionario: %s\n",  funcionario.nome);
					}
					else
						printf("Codigo nao encontrado!\n");
	                break;
               case 4: 
                    printf("\n\n*** Alteracao de Informacao ***\n\n");
                    printf("Digite o codigo do migrante: ");
					scanf("%d", &codigo);
					pos = consultarMigrante(codigo, arqMigrante);
					if (pos > 0) {
						migrante = lerMigrante(pos, arqMigrante);
						printf("Nome: %s\n", migrante.nome);
						printf("Digite o novo Nome do Migrante: ");
	                    fflush(stdin);
	                    gets(migrante.nome);
						printf("Motivo anterior: %d\n",  migrante.motivo);
						
						if(migrante.motivo == 1){
							dados.Trabalho = dados.Trabalho - 1;
							gravarDados(dados, arqDados);
						}else if(migrante.motivo == 2) {
							dados.Estudo = dados.Estudo - 1;
							gravarDados(dados, arqDados);
						}else if(migrante.motivo == 3) {
							dados.Turismo = dados.Turismo - 1;
							gravarDados(dados, arqDados);
						}else if(migrante.motivo == 4) {
							dados.Moradia = dados.Moradia - 1;
							gravarDados(dados, arqDados);
						}else{
							dados.Assuntos_Diplomaticos = dados.Assuntos_Diplomaticos - 1;
							gravarDados(dados, arqDados);
						}
						
						printf("Digite a novo motivo: \n");
	                    printf("1. Trabalho\n");
	                    printf("2. Estudos\n");
	                    printf("3. Turismo\n");
	                    printf("4. Moradia\n");
	                    printf("5. Assuntos Diplomaticos\n");
	                    printf("Digite nova opcao: ");
					    scanf("%d", &migrante.motivo);
					    
					    if(migrante.motivo == 1){
							dados.Trabalho = dados.Trabalho + 1;
							gravarDados(dados, arqDados);
						}else if(migrante.motivo == 2) {
							dados.Estudo = dados.Estudo +1;
							gravarDados(dados, arqDados);
						}else if(migrante.motivo == 3) {
							dados.Turismo = dados.Turismo + 1;
							gravarDados(dados, arqDados);
						}else if(migrante.motivo == 4) {
							dados.Moradia = dados.Moradia + 1;
							gravarDados(dados, arqDados);
						}else{
							dados.Assuntos_Diplomaticos = dados.Assuntos_Diplomaticos + 1;
							gravarDados(dados, arqDados);
						}
					    
					    printf("1 - Entrada | 2 - saida: %d\n",  migrante.entrada_saida);
					    
					    if(migrante.entrada_saida == 1){
		                	dados.Populacao_Relativa = dados.Populacao_Relativa + 1;
		                	gravarDados(dados, arqDados);
						}else{
							dados.Populacao_Relativa = dados.Populacao_Relativa - 1;
							gravarDados(dados, arqDados);
						}
					    
					    printf("Digite uma das opcoes abaixo: \n");
	                    printf("1. Saida\n");
	                    printf("2. Entrando\n");
	                    printf("Digite a opcao:");
	                    scanf("%d", &migrante.entrada_saida);
	                    
	                    if(migrante.entrada_saida == 1){
		                	dados.Populacao_Relativa = dados.Populacao_Relativa - 1;
		                	gravarDados(dados, arqDados);
						}else{
							dados.Populacao_Relativa = dados.Populacao_Relativa + 1;
							gravarDados(dados, arqDados);
						}
	                    
					    printf("Data: %d/%d/%d\n", migrante.data, migrante.mes, migrante.ano);
					    printf("Digite nova data de saida do pais:(ex dd/mm/aaaa)\n");
					    scanf("%d/%d/%d", &migrante.data, &migrante.mes, &migrante.ano);
					    printf("1-Residente brasileiro | 2-Estrangeiro:%d\n", migrante.nacionalidade);
	                    printf("Digite nova opcao 1-Residente brasileiro, 2-Estrangeiro:");
	                	scanf("%d", &migrante.nacionalidade);
	                
	               	 	if(migrante.nacionalidade == 2){
	                		do{
	                			printf("Informe seu passaporte:");
	                			fflush(stdin);
	                			gets(migrante.passaporte);
	                			if(strlen(migrante.passaporte)!=8){
	                				printf("Passaporte invalido! Tente novamente.\n");
								}
	                		}while (strlen(migrante.passaporte)!=8);
	                	}else{
							printf("Digite o CPF : ");
		                	fflush(stdin);
		               		gets(migrante.cpf);
		               		validar = validarCPF(migrante.cpf);
		               		if(validar == 0){
				    			printf("\nCPF invalido.\n");
				    			break;
	                    	}else if(strcmp(migrante.cpf,"54584343152") == 0 || strcmp(migrante.cpf,"26142254520") == 0) {
	                    		printf("\nMigrante Proibido de deixar o pais.\n");
	                    		break;
							}else{
	                        	printf("CPF valido.\n");
	                    	}
	            		}
	            		
	            		
	            		funcionario = lerFuncionario(consultarFuncionario(migrante.codFuncionario, arqFuncionario), arqFuncionario);
						printf("Funcionario..........: %s\n", funcionario.nome);
		                printf("Lista de funcionario:\n");
		                listarFuncionario(arqFuncionario);
		                do {
			                printf("Codigo do funcionario: ");
							scanf("%d", &codigo);
							posFuncionario = consultarFuncionario(codigo, arqFuncionario);
							if (posFuncionario == -1)
								printf("Codigo nao encontrado!\n\n");
							else
								funcionario = lerFuncionario(posFuncionario, arqFuncionario);
						} while (posFuncionario == -1);
		                migrante.codFuncionario = funcionario.codigo;
	                	gravarMigrante(migrante, pos, arqMigrante);
					}
					else
						printf("Codigo nao encontrado!\n");
	                break;
               case 5: 
                    printf("\n\n*** Exclusao de Migrante ***\n\n");
                    printf("Digite o codigo do migrante: ");
					scanf("%d", &codigo);
					pos = consultarMigrante(codigo, arqMigrante);
					if (pos > 0){
						excluirMigrante(pos, arqMigrante);
						printf("Excluido com Sucesso!\n");
				    }
					else
						printf("Codigo nao encontrado!\n");
        }
    } while (opcao != 0);

}

//*** Crud Funcionario ************************************************************
void crudFuncionario(FILE *arqFuncionario, FILE *arqMigrante){
	int opcao, codigo, pos;
	struct tFuncionario funcionario;
	
	do {
        opcao = menuCRUD("Funcionario");
        switch (opcao) {
               case 1: 
                    printf("\n\n*** Inclusao de Funcionario ***\n\n");
					printf("Digite o codigo do funcionario: ");
					scanf("%d", &funcionario.codigo);
					pos = consultarFuncionario(funcionario.codigo, arqFuncionario);
					if (pos > 0) {
						printf("Codigo existente encontrado!\n");
					}else {
						printf("Digite o nome do funcionario: ");
						fflush(stdin);
						gets(funcionario.nome);
						printf("Digite a senha do funcionario (Maximo 9 caracteres): ");
						fflush(stdin);
						gets(funcionario.senha);
	                	gravarFuncionario(funcionario, -1, arqFuncionario);
					}
	                break;
               case 2: 
                    printf("\n\n*** Listagem de Funcionario ***\n\n");
                    listarFuncionario(arqFuncionario);
	                break;
               case 3: 
                    printf("\n\n*** Consulta de Funcionario ***\n\n");
                    printf("Digite o codigo do funcionario: ");
					scanf("%d", &codigo);
					pos = consultarFuncionario(codigo, arqFuncionario);
					if (pos > 0) {
						funcionario = lerFuncionario(pos, arqFuncionario);
						printf("Nome: %s\n",  funcionario.nome);
						printf("senha: %s\n",  funcionario.senha);
					}
					else
						printf("Codigo nao encontrado!\n");
	                break;
               case 4: 
                    printf("\n\n*** Alteracao de Informacao ***\n\n");
                    printf("Digite o codigo do funcionario: ");
					scanf("%d", &codigo);
					pos = consultarFuncionario(codigo, arqFuncionario);
					if (pos > 0) {
						funcionario = lerFuncionario(pos, arqFuncionario);
						printf("Nome: %s\n", funcionario.nome);
						printf("Digite novo nome: ");
						fflush(stdin);
						gets(funcionario.nome);
						printf("Senha: %s\n",  funcionario.senha);
						printf("Digite nova senha do funcionario (Maximo 9 caracteres): ");
						fflush(stdin);
						gets(funcionario.senha);
	                	gravarFuncionario(funcionario, pos, arqFuncionario);
					}
					else
						printf("Codigo nao encontrado!\n");
	                break;
               case 5: 
                    printf("\n\n*** Exclusao de Funcionario ***\n\n");
                    printf("Digite o codigo do funcionario: ");
					scanf("%d", &codigo);
					pos = consultarFuncionario(codigo, arqFuncionario);
					if (pos > 0){
						if (!consultarMigranteFuncionario(codigo, arqMigrante)){
							excluirFuncionario(pos, arqFuncionario);
							printf("Excluido com Sucesso!\n");
						}else{
							printf("Existe Migrante cadastrado com esse Funcionario!\n");
						}
					}
					else
						printf("Codigo nao encontrado!\n");
        }
    } while (opcao != 0);
    
}

//*** Abrir arquivo ************************************************************
FILE *abrirArquivo(char arquivo[]) {
	FILE *arq = fopen(arquivo, "r+b");
	
	if (arq == NULL)
		arq = fopen(arquivo, "w+b");
	return arq;
}

//*** Gravar Migrantes ************************************************************
void gravarMigrante(struct tMigrante migr, int reg, FILE *arq) {
	if (reg <= 0) {
		migr.deletado = ' ';
		fseek(arq, 0, SEEK_END);
	}
	else
		fseek(arq, (reg-1)*sizeof(migr), SEEK_SET);
	fwrite(&migr, sizeof(migr), 1, arq);
}

//*** Listar Migrantes **********************************************************
void listarMigrante(FILE *arq, FILE *arqFuncionario) {
	struct tMigrante migr;
	struct tFuncionario func;
	
	fseek(arq, 0, SEEK_SET);
   	printf("COD       NOME        MOTIVO       ENTRADA OU SAIDA       DATA SAIDA       1-RESIDENTE BR | 2-ESTRANGEIRO       CPF  OU PASSAPORTE       FUNCIONARIO\n");
   	printf("---       ----        ------       ----------------       ----------       ------------------------------       ------------------       -----------\n");
    while (fread(&migr, sizeof(migr), 1, arq)) {
    	if (migr.deletado != '*') {
			func = lerFuncionario(consultarFuncionario(migr.codFuncionario, arqFuncionario), arqFuncionario);
			if (migr.motivo == 1){
				if(migr.entrada_saida == 1){
					if(migr.nacionalidade == 1){
    					printf("%d         %s      Trabalho     Saida                  %d/%d/%d       Residente                                    %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.cpf, func.nome);
    				}else{
    					printf("%d         %s      Trabalho     Saida                  %d/%d/%d       Estrangeiro                                   %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.passaporte, func.nome);
					}
				}if(migr.entrada_saida == 2){
					if(migr.nacionalidade == 1){
    					printf("%d         %s      Trabalho     Entrada                %d/%d/%d       Residente                                    %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.cpf, func.nome);
    				}else{
    					printf("%d         %s      Trabalho     Entrada                %d/%d/%d       Estrangeiro                                   %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.passaporte, func.nome);
					}
				}
			}if (migr.motivo == 2){
				if(migr.entrada_saida == 1){
					if(migr.nacionalidade == 1){
    					printf("%d         %s      Estudos     Saida                   %d/%d/%d       Residente                                    %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.cpf, func.nome);
    				}else{
    					printf("%d         %s      Estudos     Saida                   %d/%d/%d       Estrangeiro                                   %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.passaporte, func.nome);
					}
				}if(migr.entrada_saida == 2){
					if(migr.nacionalidade == 1){
    					printf("%d         %s      Estudos     Entrada                 %d/%d/%d       Residente                                    %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.cpf, func.nome);
    				}else{
    					printf("%d         %s      Estudos     Entrada                 %d/%d/%d       Estrangeiro                                   %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.passaporte, func.nome);
					}
				}
			}if (migr.motivo == 3){
				if(migr.entrada_saida == 1){
					if(migr.nacionalidade == 1){
    					printf("%d         %s      Turismo     Saida                   %d/%d/%d       Residente                                    %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.cpf, func.nome);
    				}else{
    					printf("%d         %s      Turismo     Saida                   %d/%d/%d       Estrangeiro                                   %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.passaporte, func.nome);
					}
				}if(migr.entrada_saida == 2){
					if(migr.nacionalidade == 1){
    					printf("%d         %s      Turismo     Entrada                 %d/%d/%d       Residente                                    %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.cpf, func.nome);
    				}else{
    					printf("%d         %s      Turismo     Entrada                 %d/%d/%d       Estrangeiro                                   %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.passaporte, func.nome);
					}
				}
			}if (migr.motivo == 4){
				if(migr.entrada_saida == 1){
					if(migr.nacionalidade == 1){
    					printf("%d         %s      Morar     Saida                   %d/%d/%d       Residente                                    %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.cpf, func.nome);
    				}else{
    					printf("%d         %s      Morar     Saida                   %d/%d/%d       Estrangeiro                                   %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.passaporte, func.nome);
					}
				}if(migr.entrada_saida == 2){
					if(migr.nacionalidade == 1){
    					printf("%d         %s      Morar     Entrada                 %d/%d/%d       Residente                                    %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.cpf, func.nome);
    				}else{
    					printf("%d         %s      Morar     Entrada                 %d/%d/%d       Estrangeiro                                   %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.passaporte, func.nome);
					}
				}
			}if (migr.motivo == 5){
				if(migr.entrada_saida == 1){
					if(migr.nacionalidade == 1){
    					printf("%d         %s      Assuntos diplomaticos     Saida                 %d/%d/%d       Residente                                    %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.cpf, func.nome);
    				}else{
    					printf("%d         %s      Assuntos diplomaticos    Saida                  %d/%d/%d       Estrangeiro                                   %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.passaporte, func.nome);
					}
				}if(migr.entrada_saida == 2){
					if(migr.nacionalidade == 1){
    					printf("%d         %s      Assuntos diplomaticos    Entrada                %d/%d/%d       Residente                                    %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.cpf, func.nome);
    				}else{
    					printf("%d         %s      Assuntos diplomaticos     Entrada               %d/%d/%d       Estrangeiro                                   %s              %s\n", migr.codigo, migr.nome, migr.data, migr.mes, migr.ano, migr.passaporte, func.nome);
					}
				}
			}
			
		}
	}
}

//*** Consultar Migrantes ********************************************************
int consultarMigrante(int cod, FILE *arq) {
	struct tMigrante migr;
	int reg=0;
	
	fseek(arq, 0, SEEK_SET);
    while (fread(&migr, sizeof(migr), 1, arq)) {
    	reg++;
    	if ((migr.codigo == cod) && (migr.deletado != '*'))
    		return reg;
	}
	return -1;
}

//*** Consultar Funcionario do Migrantes ********************************************************
int consultarMigranteFuncionario(int codFuncionario, FILE *arq) {
	struct tMigrante migr;
	
	fseek(arq, 0, SEEK_SET);
    while (fread(&migr, sizeof(migr), 1, arq)) {
    	if ((migr.codFuncionario == codFuncionario) && (migr.deletado != '*'))
    		return 1;
	}
	return 0;
}

//*** Ler Migrantes **************************************************************
struct tMigrante lerMigrante(int reg, FILE *arq) {
	struct tMigrante migr;
	
	fseek(arq, (reg-1)*sizeof(migr), SEEK_SET);
    fread(&migr, sizeof(migr), 1, arq);
 	return migr;
}

//*** Excluir Logicamente Migrantes **********************************************
void excluirMigrante(int reg, FILE *arq) {
	struct tMigrante migr;
	
	fseek(arq, (reg-1)*sizeof(migr), SEEK_SET);
    fread(&migr, sizeof(migr), 1, arq);
    migr.deletado = '*';
	fseek(arq, -sizeof(migr), SEEK_CUR);
	fwrite(&migr, sizeof(migr), 1, arq);
}

//*** Gravar Funcionarios ************************************************************
void gravarFuncionario(struct tFuncionario func, int reg, FILE *arq) {
	if (reg <= 0) {
		func.deletado = ' ';
		fseek(arq, 0, SEEK_END);
	}
	else
		fseek(arq, (reg-1)*sizeof(func), SEEK_SET);
	fwrite(&func, sizeof(func), 1, arq);
}

//*** Listar Funcionarios **********************************************************
void listarFuncionario(FILE *arq) {
	struct tFuncionario func;
	
	fseek(arq, 0, SEEK_SET);
   	printf("COD    NOME\n");
   	printf("----- -----------------------\n");
    while (fread(&func, sizeof(func), 1, arq))
    	if (func.deletado != '*')
    		printf("%d      %s\n", func.codigo, func.nome);
}

//*** Consultar Funcionarios ********************************************************
int consultarFuncionario(int cod, FILE *arq) {
	struct tFuncionario func;
	int reg=0;
	
	fseek(arq, 0, SEEK_SET);
    while (fread(&func, sizeof(func), 1, arq)) {
    	reg++;
    	if ((func.codigo == cod) && (func.deletado != '*'))
    		return reg;
	}
	return -1;
}

//*** Ler Funcionarios **************************************************************
struct tFuncionario lerFuncionario(int reg, FILE *arq) {
	struct tFuncionario func;
	
	fseek(arq, (reg-1)*sizeof(func), SEEK_SET);
    fread(&func, sizeof(func), 1, arq);
 	return func;
}

//*** Excluir Logicamente Funcionarios **********************************************
void excluirFuncionario(int reg, FILE *arq) {
	struct tFuncionario func;
	
	fseek(arq, (reg-1)*sizeof(func), SEEK_SET);
    fread(&func, sizeof(func), 1, arq);
    func.deletado = '*';
	fseek(arq, -sizeof(func), SEEK_CUR);
	fwrite(&func, sizeof(func), 1, arq);
}

//*** Validar CPF **********************************************
int validarCPF(char *cpf){
    int i, j, digito1 = 0, digito2 = 0;
    
    if(strlen(cpf) != 11)
        return 0;
    else if((strcmp(cpf,"00000000000") == 0) || (strcmp(cpf,"11111111111") == 0) || (strcmp(cpf,"22222222222") == 0) ||
            (strcmp(cpf,"33333333333") == 0) || (strcmp(cpf,"44444444444") == 0) || (strcmp(cpf,"55555555555") == 0) ||
            (strcmp(cpf,"66666666666") == 0) || (strcmp(cpf,"77777777777") == 0) || (strcmp(cpf,"88888888888") == 0) ||
            (strcmp(cpf,"99999999999") == 0))
        return 0;
    else
    {
        
        for(i = 0, j = 10; i < strlen(cpf)-2; i++, j--) 
            digito1 += (cpf[i]-48) * j;
        digito1 %= 11;
        if(digito1 < 2)
            digito1 = 0;
        else
            digito1 = 11 - digito1;
        if((cpf[9]-48) != digito1)
            return 0; 
        else
        
        {
            for(i = 0, j = 11; i < strlen(cpf)-1; i++, j--) 
                    digito2 += (cpf[i]-48) * j;
        digito2 %= 11;
        if(digito2 < 2)
            digito2 = 0;
        else
            digito2 = 11 - digito2;
        if((cpf[10]-48) != digito2)
            return 0; 
        }
    }
    return 1;
}

//*** Gravar Dados ************************************************************
void gravarDados(struct tDados dado, FILE *arq) {
	fseek(arq, 0, SEEK_END);
	fwrite(&dado, sizeof(dado), 1, arq);
}

//*** Listar Dados **********************************************************
void listarDados(FILE *arq) {
	struct tDados dado;
	
	fseek(arq, 0, SEEK_SET);
    while (fread(&dado, sizeof(dado), 1, arq))
        printf("");
    	printf("Migrantes que viajam por trabalho: %5d\n", dado.Trabalho);
    	printf("Migrantes que viajam para estudos: %5d\n", dado.Estudo);
    	printf("Migrantes que viajam por turismo: %5d\n", dado.Turismo);
    	printf("Migrantes que viajam para novo lar: %5d\n", dado.Moradia);
    	printf("Migrantes que viajam para assuntos diplomaticos: %5d\n", dado.Assuntos_Diplomaticos);
    	printf("Populacao Relativa e: %5d\n", dado.Populacao_Relativa);
}

//*** Ler Dados **************************************************************
struct tDados lerDados(FILE *arq) {
	struct tDados dado;
	
	fseek(arq, 0, SEEK_SET);
    fread(&dado, sizeof(dado), 1, arq);
 	return dado;
}


