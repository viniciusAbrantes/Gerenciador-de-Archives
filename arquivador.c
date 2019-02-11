/*
 * Trabalho realizado na disciplina ST562 - Estrutura de Arquivos
 * pelo grupo:
 *    Alfredo Albélis Batista Filho RA: 193532
 *    Cléofas Peres Santos          RA: 195741
 *    Vinícius Abrantes Pereira     RA: 206681
*/

#include<stdio.h>
#include<string.h>


//funções principais
int criaArchive(int argc, char *argv[]);
int insereArquivo(char nomeArchive[], char nomeArquivo[]);
int listaArquivos(char nomeArchive[]);
int extraiArquivo(char nomeArchive[], char nomeArquivo[]);
int removeArquivo(char nomeArchive[], char nomeArquivo[]);

//funções auxiliares
int getTamanhoArquivo(char nomeArquivo[]);
int procuraArquivo(char nomeArchive[], char nomeArquivo[]);


/**
verificar se o arquivo já existe no archive antes de inserí-lo
*/

/**
 * Função principal que recebe argumentos pela linha de comando 
 * e invoca a função que implementa o comando desejado
 * Retorna:
 * 0 - comando executado com sucesso
 * 1 - execução do comando falhou
 * 2 - comando inválido
*/
int main(int argc, char *argv[]) {
    int retorno;
    char cmd[3];
    strcpy(cmd,(argv[1])); //pegando o comando de argv[]

    if(strcmp(cmd, "-c") == 0) {
        if(argc < 4) { //verifica quantidade de argumentos
            printf("Quantidade de argumentos invalida, por favor informe ao menos 3 argumentos!");
            return 1;
        }
        //tenta abrir em modo leitura para verificar se já existe um archive com o nome informado
        FILE *file = fopen(argv[2], "rb");
        if(file != NULL) {
            printf("Ja existe um arquivo chamado %s!\n", argv[2]);
            fclose(file);
            return 1;
        }

        retorno = criaArchive(argc, argv);
        if(retorno != 0) {
            printf("\nErro ao criar archive!\n");
            return 1;
        }
        else return 0;
    }
    else if(strcmp(cmd, "-i") == 0) {
        if(argc != 4) { //verifica quantidade de argumentos
            printf("Quantidade de argumentos invalida, por favor informe 3 argumentos!");
            return 1;
        }
        //tenta abrir em modo leitura para verificar se o archive existe
        FILE *file = fopen(argv[2], "rb");
        if(file == NULL) {
            printf("Archive %s nao existe!\nErro ao inserir!\n", argv[2]);
            return 1;
        }
        fclose(file); //fecha arquivo em modo leitura

        retorno = insereArquivo(argv[2], argv[3]);
        if(retorno == 0) {
            printf("%s inserido com sucesso!\n", argv[3]);
            return 0;
        }
        else if(retorno == 1) {
            printf("Erro ao abrir %s!\n", argv[2]);
            return 1;
        }
        else if(retorno == 2) {
            printf("\nErro ao abrir %s\nVerifique se o arquivo existe e tente inseri-lo novamente!\n", argv[3]);
            return 1;
        }
        else if(retorno == 3) {
            printf("\n%s ja contem %s!\n", argv[2], argv[3]);
            return 1;
        }
    }
    else if(strcmp(cmd, "-l") == 0) {
        if(argc != 3) { //verifica quantidade de argumentos
            printf("Quantidade de argumentos invalida, por favor informe 2 argumentos!");
            return 1;
        }
        //tenta abrir em modo leitura para verificar se o archive existe
        FILE *file = fopen(argv[2], "rb");
        if(file == NULL) {
            printf("Archive %s nao existe!\nErro ao listar!\n", argv[2]);
            return 1;
        }
        fclose(file); //fecha arquivo em modo leitura

        retorno = listaArquivos(argv[2]);
        if(retorno == 1) {
            printf("\nErro ao listar!\n");
            return 1;
        }
        if(retorno == 2) printf("\nArchive nao contem arquivos!\n");
        return 0;
    }
    else if(strcmp(cmd, "-e") == 0) {
        if(argc != 4) { //verifica quantidade de argumentos
            printf("Quantidade de argumentos invalida, por favor informe 3 argumentos!\n");
            return 1;
        }
        //tenta abrir em modo leitura para verificar se o archive existe
        FILE *file = fopen(argv[2], "rb");
        if(file == NULL) {
            printf("Archive %s nao existe!\nErro ao extrair!\n", argv[2]);
            return 1;
        }
        fclose(file); //fecha arquivo em modo leitura

        retorno = extraiArquivo(argv[2], argv[3]);
        if(retorno == 0) {
            printf("\nArquivo extraido com sucesso!\n");
            return 0;
        }
        else if(retorno == 1) {
            printf("\nJa existe um arquivo chamado %s no diretorio!\nErro ao extrair!\n", argv[3]);
            return 1;
        }
        else if(retorno == 2) {
            printf("\nNao existe um arquivo chamado %s no archive!\nErro ao extrair!\n", argv[3]);
            return 1;
        }
    }
    else if(strcmp(cmd, "-r") == 0) {
        if(argc != 4) { //verifica quantidade de argumentos
            printf("Quantidade de argumentos invalida, por favor informe 3 argumentos!\n");
            return 1;
        }
        //tenta abrir em modo leitura para verificar se o archive existe
        FILE *file = fopen(argv[2], "rb");
        if(file == NULL) {
            printf("Archive %s nao existe!\nErro ao remover!\n", argv[2]);
            return 1;
        }
        fclose(file); //fecha arquivo em modo leitura

        retorno = removeArquivo(argv[2], argv[3]);
        if(retorno == 0) {
            printf("\nArquivo removido com sucesso!\n");
            return 0;
        }
        else if(retorno == 1) {
            printf("\nNao existe um arquivo chamado %s no archive!\nErro ao remover!\n", argv[3]);
            return 1;
        }
    }
    else {
        printf("\nComando invalido!\n");
        return 1;
    }
    return 0;
}


/**
 * A seguinte função recebe a quantidade argc de parâmetros passados à função principal
 * e o vetor argv com as strings, criando um archive chamado argv[2] e tenta inserir neste archive
 * todos os arquivos contidos de argv[3] até argv[argc]. Caso algum arquivo informado seja inválido, 
 * o usuário é informado e a função passa a inserir o próximo arquivo
 * Retorna:
 * 0 - Archive criado com sucesso
 * 1 - Erro ao criar archive
*/
int criaArchive(int argc, char *argv[]) {
    char nomeArchive[220]; //verificar tamanho máximo de caracteres em um arquivo
    strcpy(nomeArchive, argv[2]);

    //insere os arquivos informados dentro do archive
    for(int i = 3; i < argc; i++) {
        int retorno = insereArquivo(nomeArchive, argv[i]);
        if(retorno == 0) {
            printf("\n%s inserido com sucesso!\n", argv[i]);
        }
        else if(retorno == 1) {
            printf("Erro ao abrir %s!\n", nomeArchive);
            return 1;
        }
        else if(retorno == 2) printf("\nErro ao abrir %s\nVerifique se o arquivo existe e tente inseri-lo novamente!\n", argv[i]);
        else if(retorno == 3) printf("\n%s ja contem %s!\n", nomeArchive, argv[i]);
    }
    return 0;
}


/**
 * A seguinte função recebe duas strings nomeArchive e nomeArquivo. Se o archive nomeArchive não existir,
 * este é criado e nomeArquivo é inserido em seu início. Caso o archive exista, o arquivo nomeArquivo é 
 * inserido dentro do archive.
 * Retorna:
 * 0 - arquivo inserido com sucesso
 * 1 - erro ao abrir archive
 * 2 - erro ao abrir arquivo
 * 3 - archive já contém arquivo
*/
int insereArquivo(char nomeArchive[], char nomeArquivo[]) {
    char c, stringNome[220];
    int tamanhoArchive = getTamanhoArquivo(nomeArchive), inicioArquivo, i, deslocamento;
    int tamanhoArquivo = getTamanhoArquivo(nomeArquivo);
    int posicaoAtual, posicaoRelativa;
    short int bytesDisponiveis, exit = 0, cont = 0;
    short int qtdBytesNome = strlen(nomeArquivo);
    FILE *archive;

    //abre o arquivo informado em modo leitura
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if(arquivo == NULL) return 2;

    if(procuraArquivo(nomeArchive, nomeArquivo) == 0) return 3;

    if(tamanhoArchive == 0) {
        archive = fopen(nomeArchive, "wb");
        if(archive == NULL) return 1;
    }
    else {
        archive = fopen(nomeArchive, "r+b");
        if(archive == NULL) return 1;
    }

    //quantidade total de bytes que as informações do arquivo gastarão no archive
    int bytesNecessarios = qtdBytesNome + 1 + sizeof(int) + tamanhoArquivo;

    //quantidade de clusters que o arquivo ocupará no archive além do primeiro
    int clustersNecessarios = bytesNecessarios / (4096 - sizeof(short int));

    //quantidade de bytes que ficarão no último cluster
    int bytesExcedentes = bytesNecessarios - (4096 - sizeof(short int)) * clustersNecessarios;

    //entra somente se o archive não estiver vazio e executa enquanto exit == 0 (false)
    while(exit == 0 && tamanhoArchive != 0) {
        inicioArquivo = (int) ftell(archive); //guarda a posição do primeiro cluster de cada iteração
        fread(&bytesDisponiveis, sizeof(short int), 1, archive);
        cont = 0;

        //se o cluster atual estiver 'vazio' (significa que o arquivo que ocupava este espaço foi excluído)
        if(bytesDisponiveis == 4096 - sizeof(short int)) {

            //enquanto houver clusters vazios
            while(bytesDisponiveis == 4096 - sizeof(short int)) {
                //entrará no if caso o arquivo caiba no buraco
                if(cont == clustersNecessarios) {
                    fseek(archive, inicioArquivo, SEEK_SET); //volta para o primeiro cluster livre
                    exit++;
                    break;
                }
                else {
                    cont++;
                    fseek(archive, 4096 - sizeof(short int), SEEK_CUR);
                    fread(&bytesDisponiveis, sizeof(short int), 1, archive);
                }
            }
        }
        if(exit == 1) break;

        //caso o cluster esteja ocupado
        if((bytesDisponiveis != 4096 - sizeof(short int))) {
            //lê o nome do arquivo para ler seu tamanho e calcular o deslocamento
            i = 0;
            while((c = getc(archive)) != '|') {
                stringNome[i] = c;
                i++;
            }
            stringNome[i] = '\0';
            fread(&tamanhoArquivo, sizeof(int), 1, archive);
            posicaoAtual = (int) ftell(archive); //pega a posição atual do arquivo
            posicaoRelativa = posicaoAtual - ((posicaoAtual / 4096) * 4096); //calcula a posição relativa
            //deslocamento para o final do conteúdo do arquivo
            deslocamento = tamanhoArquivo + sizeof(short int) * (tamanhoArquivo / (4096 - sizeof(short int)));
            fseek(archive, deslocamento, SEEK_CUR);
            c = getc(archive);
        }
        //caso esteja no final do arquivo
        if(feof(archive)) {
            exit++;
        }
        posicaoAtual = (int) ftell(archive);
        posicaoRelativa = posicaoAtual - ((posicaoAtual / 4096) * 4096);
        //deslocamento para o próximo cluster
        fseek(archive, 4096 - posicaoRelativa, SEEK_CUR);
    }
    tamanhoArquivo = getTamanhoArquivo(nomeArquivo);

    //armazena a quantidade de bytes disponíveis no cluster atual
    bytesDisponiveis = 4096 - sizeof(short int); //cada cluster começa com 4096 menos o tamanho da variável bytesDisponiveis
    bytesDisponiveis -= (qtdBytesNome + 1); //desconta a quantidade de bytes do nome do arquivo mais o pipe
    bytesDisponiveis -= sizeof(int); //desconta a quantidade de bytes da variável tamanhoArquivo (int)

    if(clustersNecessarios == 0) { //arquivo cabe em um único cluster
        bytesDisponiveis -= tamanhoArquivo; //desconta a quantidade de bytes do conteúdo do arquivo

        //gravando dados no primeiro cluster
        fwrite(&bytesDisponiveis, sizeof(short int), 1, archive);
        fprintf(archive, "%s", nomeArquivo);
        fprintf(archive, "|");
        fwrite(&tamanhoArquivo, sizeof(int), 1, archive);

        while((c = getc (arquivo)) != EOF)
            putc(c, archive);
    }
    else { //arquivo utiliza mais que um cluster
        short int i, j, zero = 0;
        for(i = 0; i < clustersNecessarios; i++) { //grava o cluster completo
            fwrite(&zero, sizeof(short int), 1, archive); //grava bytesDisponiveis = 0 (cluster completo)
            if(i == 0) { //somente na primeira iteração
                fprintf(archive, "%s", nomeArquivo);
                fprintf(archive, "|");
                fwrite(&tamanhoArquivo, sizeof(int), 1, archive);
                for(j = 0; j < bytesDisponiveis; j++) {
                    c = getc (arquivo);
                    putc(c, archive);
                }
            }
            else { //grava o restante dos clusters completos
                for(j = 0; j < 4096 - sizeof(short int); j++) {
                    c = getc (arquivo);
                    putc(c, archive);
                }
            }
        }
        //grava o excedente
        zero = 4096 - sizeof(short int) - bytesExcedentes;
        fwrite(&zero, sizeof(short int), 1, archive);
        for(j = 0; j < bytesExcedentes; j++) {
            c = getc (arquivo);
            putc(c, archive);
        }
    }
    fclose(arquivo);
    fclose(archive);
    return 0;
}


/**
 * A seguinte função recebe uma string nomeArchive e lista todos os arquivos contidos no mesmo
 * Retorna:
 * 0 - listagem feita com sucesso
 * 1 - erro ao abrir archive
 * 2 - archive vazio
*/
int listaArquivos(char nomeArchive[]) {
    short int bytesDisponiveis, cont = 0;
    int tamanhoArquivo, deslocamento, posicaoRelativa, posicaoAtual, i;
    char stringNome[220];
    char c;
    FILE *archive = fopen(nomeArchive, "rb");
    if(archive == NULL) return 1;

    while(!feof(archive)) {
        fread(&bytesDisponiveis, sizeof (short int), 1, archive);

        //se o cluster atual estiver 'vazio' (significa que o arquivo que ocupava este espaço foi excluído)
        while(bytesDisponiveis == 4096 - sizeof(short int)) {
            if(feof(archive)) {
               if(cont == 0) return 2;
               return 0;
            }
            fseek(archive, 4096 - sizeof(short int), SEEK_CUR);
            fread(&bytesDisponiveis, sizeof (short int), 1, archive);
        }

        i = 0;
        while((c = getc(archive)) != '|') {
            stringNome[i] = c;
            i++;
        }
        stringNome[i] = '\0';
        printf("%s\n", stringNome);
        cont++; //incrementa a quantidade de arquivos printados
        fread(&tamanhoArquivo, sizeof(int), 1, archive);

        deslocamento = tamanhoArquivo + sizeof(short int) * (tamanhoArquivo / (4096 - sizeof(short int)));
        fseek(archive, deslocamento, SEEK_CUR);
        c = getc(archive);
        if(feof(archive)) {
            if(cont == 0) return 2;
            return 0;
        }
        else {
            posicaoAtual = (int) ftell(archive); //pega a posição atual do arquivo
            posicaoRelativa = posicaoAtual - ((posicaoAtual / 4096) * 4096); //calcula a posição relativa
            fseek(archive, 4096 - posicaoRelativa, SEEK_CUR);
        }
    }
    return 0;
}


/**
 * A seguinte função recebe duas strings nomeArchive e nomeArquivo e procura o nomeArquivo 
 * em nomeArchive e faz uma cópia do arquivo caso o encontre.
 * Retorna:
 * 0 - arquivo extraído com sucesso
 * 1 - já existe um arquivo de mesmo nome no diretório
 * 2 - não existe o arquivo dentro do archive
*/
int extraiArquivo(char nomeArchive[], char nomeArquivo[]) {
    short int bytesDisponiveis;
    int tamanhoArquivo, deslocamento, posicaoRelativa, posicaoAtual, i;
    char stringNome[220];
    char c;
    FILE *archive = fopen(nomeArchive, "rb");

    //tenta abrir em modo leitura para verificar se já existe um arquivo com o nome informado
    FILE *file = fopen(nomeArquivo, "rb");
    if(file != NULL) {
        fclose(file);
        return 1;
    }

    while(!feof(archive)) {
        fread(&bytesDisponiveis, sizeof(short int), 1, archive);

        //se o cluster atual estiver 'vazio' (significa que o arquivo que ocupava este espaço foi excluído)
        while(bytesDisponiveis == 4096 - sizeof(short int)) {
            if(feof(archive)) return 2;
            fseek(archive, 4096 - sizeof(short int), SEEK_CUR);
            fread(&bytesDisponiveis, sizeof (short int), 1, archive);
        }

        i = 0;
        while((c = getc(archive)) != '|') {
            stringNome[i] = c;
            i++;
        }
        stringNome[i] = '\0';
        fread(&tamanhoArquivo, sizeof(int), 1, archive);

        posicaoAtual = (int) ftell(archive); //pega a posição atual do arquivo
        posicaoRelativa = posicaoAtual - ((posicaoAtual / 4096) * 4096); //calcula a posição relativa

        if(strcmp(stringNome, nomeArquivo) == 0) {
            //cria o arquivo que irá receber o conteúdo
            FILE *novoArquivo = fopen(stringNome, "wb");

            if(tamanhoArquivo <= (4096 - posicaoRelativa)) { //arquivo ocupa somente o cluster atual
                for(i = 0; i < tamanhoArquivo; i++) {
                    c = getc(archive);
                    putc(c, novoArquivo);
                }
            }
            else { //arquivo ocupa mais que o cluster atual
                for(i = 0; i < 4096 - posicaoRelativa; i++) { //copia o pedaço do primeiro cluster que o arquivo ocupa
                    c = getc(archive);
                    putc(c, novoArquivo);
                }
                fread(&bytesDisponiveis, sizeof(short int), 1, archive);
                while(bytesDisponiveis == 0) { //copia todos os clusters completos que o arquivo ocupa
                    for(i = 0; i < 4096 - sizeof(short int); i++) {
                        c = getc(archive);
                        putc(c, novoArquivo);
                    }
                    fread(&bytesDisponiveis, sizeof(short int), 1, archive);
                }
                for(i = 0; i < (4096 - sizeof(short int) - bytesDisponiveis); i++) {
                    c = getc(archive);
                    putc(c, novoArquivo);
                }
            }
            fclose(archive);
            fclose(novoArquivo);
            return 0;
        }
        else {
            deslocamento = tamanhoArquivo + sizeof(short int) * (tamanhoArquivo / (4096 - sizeof(short int)));
            fseek(archive, deslocamento, SEEK_CUR);
            c = getc(archive);
            if(feof(archive))
                return 2;
            else {
                posicaoAtual = (int) ftell(archive); //pega a posição atual do arquivo
                posicaoRelativa = posicaoAtual - ((posicaoAtual / 4096) * 4096); //calcula a posição relativa
                fseek(archive, 4096 - posicaoRelativa, SEEK_CUR);
            }
        }
    }
    return 2;
}


/**
 * A seguinte função recebe duas strings nomeArchive e nomeArquivo e procura o nomeArquivo 
 * em nomeArchive e remove o arquivo caso o encontre.
 * Retorna:
 * 0 - arquivo removido com sucesso
 * 1 - não existe o arquivo dentro do archive
*/
int removeArquivo(char nomeArchive[], char nomeArquivo[]) {
    short int bytesDisponiveis, max = 4096 - sizeof(short int);
    int tamanhoArquivo, deslocamento, posicaoRelativa, posicaoAtual, inicioArquivo;
    char stringNome[220];
    char c;

    FILE *archive = fopen(nomeArchive, "r+b");

    while(!feof(archive)) {
        inicioArquivo = (int) ftell(archive); //guarda a posição do primeiro cluster de cada iteração 
        fread(&bytesDisponiveis, sizeof(short int), 1, archive);

        //se o cluster atual estiver 'vazio' (significa que o arquivo que ocupava este espaço foi excluído)
        while(bytesDisponiveis == 4096 - sizeof(short int)) {
            if(feof(archive)) return 1;
            fseek(archive, 4096 - sizeof(short int), SEEK_CUR);
            fread(&bytesDisponiveis, sizeof (short int), 1, archive);
        }

        int i = 0;
        while((c = getc(archive)) != '|') {
            stringNome[i] = c;
            i++;
        }
        stringNome[i] = '\0';
        fread(&tamanhoArquivo, sizeof(int), 1, archive);

        //quantidade total de bytes que as informações do arquivo gastarão no archive
        int bytesNecessarios = strlen(nomeArquivo) + 1 + sizeof(int) + tamanhoArquivo;

        //quantidade de clusters que o arquivo ocupará no archive além do primeiro
        int clustersNecessarios = bytesNecessarios / (4096 - sizeof(short int));

        posicaoAtual = (int) ftell(archive); //pega a posição atual do arquivo
        posicaoRelativa = posicaoAtual - ((posicaoAtual / 4096) * 4096); //calcula a posição relativa

        if(strcmp(stringNome, nomeArquivo) == 0) {
            if(bytesDisponiveis == max) return 1; //caso arquivo existe mas esteja apagado
            fseek(archive, inicioArquivo, SEEK_SET);
            fwrite(&max, sizeof(short int), 1, archive);

            if(clustersNecessarios == 0) return 0;

            else { //arquivo ocupa mais que o cluster atual
                for(i = 0; i < clustersNecessarios; i++) {
                    fseek(archive, 4096 - sizeof(short int), SEEK_CUR);
                    fwrite(&max, sizeof(short int), 1, archive);
                }
                return 0;
            }
            fclose(archive);
            return 0;
        }
        else {
            deslocamento = tamanhoArquivo + sizeof(short int) * (tamanhoArquivo / (4096 - sizeof(short int)));
            fseek(archive, deslocamento, SEEK_CUR);
            c = getc(archive);
            if(feof(archive))
                return 1;
            else {
                posicaoAtual = (int) ftell(archive); //pega a posição atual do arquivo
                posicaoRelativa = posicaoAtual - ((posicaoAtual / 4096) * 4096); //calcula a posição relativa
                fseek(archive, 4096 - posicaoRelativa, SEEK_CUR);
            }
        }
    }
    return 1;
}


/**
 * A seguinte função recebe uma string nomeArquivo e retorna o tamanho em bytes do arquivo
 * Retorna:
 * tamanhoArquivo = 0 caso arquivo não exista
 * tamanhoArquivo > 0 indicando a quantidade de bytes do arquivo 
*/
int getTamanhoArquivo(char nomeArquivo[]) {
    int tamanhoArquivo;

    FILE *arquivo = fopen(nomeArquivo, "rb");
    if(arquivo == NULL) return 0; //verifica se o arquivo não existe

    fseek(arquivo, 0, SEEK_END); //caminha até o final do arquivo
    tamanhoArquivo = ftell(arquivo); //pega a posição atual, que indica o tamanho do arquivo
    fclose(arquivo);
    return tamanhoArquivo;
}


/**
 * A seguinte função recebe duas strings nomeArchive e nomeArquivo e verifica se 
 * nomeArquivo existe em nomeArchive.
 * Retorna:
 * 0 - arquivo encontrado
 * 1 - erro ao abrir archive
 * 2 - arquivo não encontrado
*/
int procuraArquivo(char nomeArchive[], char nomeArquivo[]) {
    short int bytesDisponiveis;
    int tamanhoArquivo, deslocamento, posicaoRelativa, posicaoAtual, i;
    char stringNome[220];
    char c;

    //verifica se o archive existe
    FILE *archive = fopen(nomeArchive, "rb");
    if(archive == NULL) return 1;
    while(!feof(archive)) {
        fread(&bytesDisponiveis, sizeof (short int), 1, archive);

        //se o cluster atual estiver 'vazio' (significa que o arquivo que ocupava este espaço foi excluído)
        while(bytesDisponiveis == 4096 - sizeof(short int)) {
            if(feof(archive)) return 2;
            fseek(archive, 4096 - sizeof(short int), SEEK_CUR);
            fread(&bytesDisponiveis, sizeof (short int), 1, archive);
        }

        i = 0;
        while((c = getc(archive)) != '|') {
            stringNome[i] = c;
            i++;
        }
        stringNome[i] = '\0';
        if(strcmp(stringNome, nomeArquivo) == 0) return 0; //arquivo encontrado
        fread(&tamanhoArquivo, sizeof(int), 1, archive);

        deslocamento = tamanhoArquivo + sizeof(short int) * (tamanhoArquivo / (4096 - sizeof(short int)));
        fseek(archive, deslocamento, SEEK_CUR);
        c = getc(archive);
        if(feof(archive)) return 2;
        else {
            posicaoAtual = (int) ftell(archive); //pega a posição atual do arquivo
            posicaoRelativa = posicaoAtual - ((posicaoAtual / 4096) * 4096); //calcula a posição relativa
            fseek(archive, 4096 - posicaoRelativa, SEEK_CUR);
        }
    }
    return 2;
}