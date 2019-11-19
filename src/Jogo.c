#include "ArquivoJogo.h"
#include "Validacoes.h"
#include "Captura.h"
#include "Util.h"
#include "Jogo.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/**
	A função iniciarJogo inicializa todas as variáveis do jogo
	Parâmetro: O jogo
**/
void iniciarJogo(Jogo jogo){
	jogo.id=0;
	limparBuffer();
	inicializarJogadores(&jogo.jogador1, &jogo.jogador2);
	limparTela();
	inicializarTabuleiro(&jogo.goban);
	jogar(&jogo);
	
}

/**
	A função reiniciarJogo carrega um jogo salvo em um arquivo
	Parâmetro: o jogo
**/
void reiniciarJogo(Jogo jogo){
	if(existeArquivoJogo(&jogo)){
		jogo.goban.matriz=inicializarMatriz(jogo.goban.dimensao);
		jogar(&jogo);
	}
}

/**
	A função jogar executa o jogo e as funções de fim de jogo
	Parâmetro: o jogo
**/
void jogar(Jogo *jogo){
	do{
		limparTela();
		sortearPecas(&jogo->jogador1, &jogo->jogador2);
		inicializarJogo(jogo);
		zerarCapturas(&jogo->jogador1, &jogo->jogador2);
		limparMatriz(jogo->goban.matriz, jogo->goban.dimensao);
		loopJogo(jogo);
	}while(continuarJogo());
	salvarJogo(jogo);
	liberarMatriz(jogo->goban.matriz, jogo->goban.dimensao);
}


/**
	A função inicializarJogo atribui valores iniciais a estrutura Jogo
	Parâmetros: O jogo
**/
void inicializarJogo(Jogo *jogo) {
	jogo->proximoJogador=P;
	jogo->ganhando = -1;
}

/**
	A função loopJogo é onde acontece a partida em si, todas as jogadas, 
	verificações e impressões na tela
	Parâmetro: O jogo
**/
void loopJogo(Jogo *jogo){
	Peca peca;
	int vitoriaPorCaptura;
	int lin,col;
	do{
		informarQntCapturas(jogo->jogador1, jogo->jogador2);
		imprimirTabuleiro(jogo->goban);
		informarProximoJogador(jogo);
		do{
			printf("Onde deseja inserir a peca (lin col)? ");
			scanf("%d",&lin);
			scanf("%d",&col);
		}while(!validarInsercao(jogo->goban, lin, col, jogo->proximoJogador));
		limparTela();
		jogo->goban.matriz[lin][col] = jogo->proximoJogador;
		verificarCaptura(jogo, lin, col);
		alternarJogador(&jogo->proximoJogador);
		peca = -1;
	}while(!verificarFimDeJogo(jogo, &peca, &vitoriaPorCaptura));
	informarQntCapturas(jogo->jogador1, jogo->jogador2);
	imprimirTabuleiro(jogo->goban);
	imprimirGanhador(&jogo->jogador1, &jogo->jogador2, peca, vitoriaPorCaptura);
}

/**
	A função informarProximoJogador informa quem deve jogar no momento
	Parâmetro: O jogo
**/
void informarProximoJogador(Jogo *jogo){
	if(jogo->jogador1.peca == jogo->proximoJogador){
		printf("Vez de %s (%c)\n",jogo->jogador1.nome, caracterPeca(jogo->jogador1.peca));
	}else{
		printf("Vez de %s (%c)\n",jogo->jogador2.nome, caracterPeca(jogo->jogador2.peca));
	}
}

/**
	A função alternarJogador faz a alternância dos jogadores
	Parâmetro: O proximoJogador
**/
void alternarJogador(Peca *proximoJogador){
	*proximoJogador=(1-*proximoJogador);
}

/**
	A função validarInsercao verifica se a posição escolhida pelo jogador
	existe no tabuleiro, não está ocupada e não gera a formação 3x3.
	Parâmetros: tabuleiro, linha e coluna(escolhidos pelo usuário) e peca
	Retorno: Será 0(se a jogada for inválida) ou 1(se não tiver nenhum impedimento)
**/
int validarInsercao(Tabuleiro tabuleiro, int lin, int col, Peca peca){
	if (lin < 0 || lin >= tabuleiro.dimensao || col < 0 || col >= tabuleiro.dimensao){
		printf("----Jogada invalida. Esta posicao esta fora do goban.----\n");
		return 0;
	}

	if (tabuleiro.matriz[lin][col] != -1){
		printf("----Jogada invalida. Ja existe uma peca nesta posicao.----\n");
		return 0;
	}

	if (verificarFormacao3x3(tabuleiro, lin, col, peca)){
		printf("----Jogada invalida. Nao eh permitido fazer a formacao 3x3.----\n");
		return 0;
	}

	return 1;
}

/**
	A função verificarFormacao3x3 verifica se a posição que o jogador
	escolheu no tabuleiro gera uma formação 3x3.
	Parâmetros: tabuleiro, linha, coluna e peca
	Retorno: Será 0(se nem um problema) ou 1(se gera uma formação 3x3)
**/
int verificarFormacao3x3(Tabuleiro goban, int i, int j, Peca peca) {
	Casa c0 = {i, j};
	Casa c1, c2, c3, c4;

	if (formaSequenciaDe5(goban, c0, peca)) {
		return 0;
	}
	else if (validarPrimeiraSeq(goban, c0, &c1, &c2, &c3, &c4, 0, 1, peca)) {
		return analisarSegundaSeq(goban, 0, 1, c1, c2, c3, c4, peca);
	}
    else if (validarPrimeiraSeq(goban, c0, &c1, &c2, &c3, &c4, 1, 0, peca)) {
    	return analisarSegundaSeq(goban, 1, 0, c1, c2, c3, c4, peca);
    }
	else if (validarPrimeiraSeq(goban, c0, &c1, &c2, &c3, &c4, 1, 1, peca)) {
		return analisarSegundaSeq(goban, 1, 1, c1, c2, c3, c4, peca);
	}
	else if (validarPrimeiraSeq(goban, c0, &c1, &c2, &c3, &c4, -1, 1, peca)) {
		return analisarSegundaSeq(goban, -1, 1, c1, c2, c3, c4, peca);
	}

	return 0;

}

/**
	A função existeSeqMaiorQ3 verifica se a posição escolhida
	pelo jogador gera uma sequência de mais de 3 peças.
	Parâmetros: tabuleiro, posição/casa e peca
	Retorno: Será 0(se não gera uma sequência de 4 ou 5 peças) ou 1(se gera)
**/
int formaSequenciaDe5(Tabuleiro goban, Casa c, Peca peca) {
	return    contarSequencia(goban, c, 0, 1, peca) == 5
	       || contarSequencia(goban, c, 1, 0, peca) == 5
	       || contarSequencia(goban, c, 1, 1, peca) == 5
	       || contarSequencia(goban, c,-1, 1, peca) == 5;
}

/**
	A função contarSequencia conta o número de peças em sequência na direção di dj
	Parâmetros: tabuleiro, casa, direção da linha, direção da coluna e peca
	Retorno: O número de peças da sequência
**/
int contarSequencia(Tabuleiro goban, Casa c, int di, int dj, Peca peca) {
	int n1 = 1;
	int n2 = 1;

	while (verificarSeSeqContinua(goban, c, di, dj, peca, n1)) {
		n1++;
	}

	while (verificarSeSeqContinua(goban, c, -di, -dj, peca, n2)) {
		n2++;
	}

	return n1 + n2 - 1;
}

/**
	A função verificarSeSeqContinua vê se a posição é valida e pertence ao jogador da vez
	Parâmetros: tabuleiro, casa, direção da linha, direção da coluna, peca e n
	Retorno: Será 0(se está fora do goban ou não é do jogador) ou 1(se é do jogador)
**/
int verificarSeSeqContinua(Tabuleiro goban, Casa casa, int di, int dj, Peca peca, int n) {
	return    casa.lin + di*n >= 0
		   && casa.lin + di*n < goban.dimensao
		   && casa.col + dj*n >= 0
		   && casa.col + dj*n < goban.dimensao
		   && goban.matriz[casa.lin + di*n][casa.col + dj*n] == peca;
}

/**
	A função validarPrimeiraSeq verifica se a posição gera uma sequência de 3 peças
	Parâmetros: tabuleiro, casa central e as duas pontas, direção da linha e coluna e peca
	Retorno: Será 0(se não foi encontrada sequência de 3) ou 1(se foi encontrado)
**/
int validarPrimeiraSeq(Tabuleiro goban, Casa c0, Casa *c1, Casa *c2, Casa *c3, Casa *c4, int di, int dj, Peca peca) {
	int n1 = 1;
	int n2;
	int n3;
    int n4 = 1;

	while (verificarSeSeqContinua(goban, c0, di, dj, peca, n1)) {
		n1++;
	}

	while (verificarSeSeqContinua(goban, c0, -di, -dj, peca, n4)) {
		n4++;
	}

	c1->lin = c0.lin + di*(n1 - 1);
	c1->col = c0.col + dj*(n1 - 1);

	c4->lin = c0.lin - di*(n4 - 1);
	c4->col = c0.col - dj*(n4 - 1);

	if (n1 + n4 > 4) {
		n2 = (n1 - 2 < 0 ? 0 : n1 - 2);
		n3 = (n4 - 2 < 0 ? 0 : n4 - 2);

		c2->lin = c0.lin + di*n2;
		c2->col = c0.col + dj*n2;

		c3->lin = c0.lin - di*n3;
		c3->col = c0.col - dj*n3;
	}
	else {
		c2->lin = -9;
		c2->col = -9;

		c3->lin = -9;
		c3->col = -9;
	}

	if (n1 + n4 > 3) {
		return 1;
	}

	return 0;
}

/**
	A função analisarSegundaSeq verifica em ambas as pontas
	da primeira sequência encontrada se existe sequência de 3 conectada.
	Parâmetros: tabuleiro, direção da linha e coluna, casas das duas pontas e peca
	Retorno: Será 0(se não existe sequência de 3 a partir de c1 ou c2) ou 1(se existe)
**/
int analisarSegundaSeq(Tabuleiro goban, int di, int dj, Casa c1, Casa c2, Casa c3, Casa c4, Peca peca) {
	return    verificarSegundaSeq(goban, di, dj, c1, peca)
	       || verificarSegundaSeq(goban, di, dj, c2, peca)
	       || verificarSegundaSeq(goban, di, dj, c3, peca)
	       || verificarSegundaSeq(goban, di, dj, c4, peca);
		
}

/**
	A função verificarSegundaSeq verifica se existe sequência de tamanho 3 ou não
	Parâmetros: tabuleiro, direção da linha e coluna, casa e peca
	Retorno: Será 0(se não existe sequência de 3) ou 1(se existe)
**/
int verificarSegundaSeq(Tabuleiro goban, int di, int dj, Casa c, Peca peca) {
	return    (!(di ==  0 && dj == 1) && validarSegundaSeq(goban, c,  0, 1, peca))
           || (!(di ==  1 && dj == 0) && validarSegundaSeq(goban, c,  1, 0, peca))
		   || (!(di ==  1 && dj == 1) && validarSegundaSeq(goban, c,  1, 1, peca))
		   || (!(di == -1 && dj == 1) && validarSegundaSeq(goban, c, -1, 1, peca));
}

/**
	A função validarSegundaSeq verifica se existe sequência de tamanho 3
	na direção di dj
	Parâmetros: tabuleiro, casa, direção da linha e coluna e peca
	Retorno: Será 0(se não existe sequência de 3) ou 1(se existe)
**/
int validarSegundaSeq(Tabuleiro goban, Casa c0, int di, int dj, Peca peca) {
	int n1 = 1;
	int n2 = 1;

	while (verificarSeSeqContinua(goban, c0, di, dj, peca, n1)) {
		n1++;
	}

	while (verificarSeSeqContinua(goban, c0, -di, -dj, peca, n2)) {
		n2++;
	}

	if (n1 > 2 || n2 > 2) {
		return 1;
	}

	return 0;
}

/**
	A função continuarJogo é responsável por perguntar ao jogadores se eles desejam
	continuar o jogo.
	Retorno: Será 0(caso não queira continuar o jogo) ou 1(se o jogo pode continuar)
**/
int continuarJogo(){
	char resposta[3];

	do{
		printf("Deseja continuar (sim | nao)? ");
		scanf("%s",resposta);
		strcpy(resposta,converterParaMinusculo(resposta));
	}while(strcmp("sim", resposta) != 0 && strcmp("nao", resposta) != 0);
	if(strcmp("sim", resposta) == 0){
		return 1;
	}
	return 0;
}


/**
	A função imprimirGanhador é responsável por imprimir o ganhador
	juntamente com o placar do jogo.
	Parâmetros: Os dois jogadores, peca(ganhadora) e vitoriaPorCaptura(0 ou 1)

**/
void imprimirGanhador(Jogador *jogador1, Jogador *jogador2, Peca peca, int vitoriaPorCaptura) {
	if (peca == jogador1->peca) {
		jogador1->vitorias+=1;
		printf("Vitoria %sde %s\n", (vitoriaPorCaptura ? "por captura " : ""), jogador1->nome);
	}
	else if (peca == jogador2->peca) {
		jogador2->vitorias+=1;
		printf("Vitoria %sde %s\n", (vitoriaPorCaptura ? "por captura " : ""), jogador2->nome);
	}
	else{
		printf("Empate\n");
	}
	printf("Placar: %s %d x %d %s \n",jogador1->nome,jogador1->vitorias,jogador2->vitorias, jogador2->nome);
}