# Patogênico.exe

## README EM ANDAMENTO!!!!


## Tabela de conteúdos
- [Sobre](#sobre)
- [A biologia por trás](#biologia)
- [Algoritmos e programação](#algoritmos)
- [Última Atualização](#atualizacoes)

## Sobre <a name="sobre"></a>

Patogênico é um jogo desenvolvido para a faculdade como projeto integrador do 2° semestre. A proposta é desenvolver um jogo usando a biblioteca Allegro 5, na linguagem de programação C, que seja educativo para alunos do ensino médio. <br>
Nosso jogo é sobre o mosquito *Aedes aegypti* e o vírus da dengue, que o jogador controla para  infectar o ser-humano, compreendendo os processos que passa para nos infectar e deixar doentes.

## A biologia por trás <a name="biologia"></a>

O jogador controla, na primeira etapa do jogo, um mosquito infectado com dengue, que pica um ser-humano. Após isso, controla o vírus absorvendo nutrientes e se clonando para se propagar pelo resto do corpo. A última etapa é quando o vírus já está em grande número e pode se propagar pela nossa corrente sanguínea. Cada etapa será bem definida a seguir:

* ### A picada

Um mosquito fêmea da espécie *Aedes aegypti* é o **vetor** da dengue; isto quer dizer que não pegamos a doença direto do mosquito, mas sim *através* dele. O processo é o seguinte: o mosquito pica alguém que está infectado, e então se torna infectivo. Após dias, ele pica outra pessoa, saudável, e agora sim o vírus é transmitido diretamente para o sangue da pessoa.

* ### Fagocitose

Agora no nosso corpo, o vírus penetra uma célula e muda seu "código", fazendo com que ela consuma nutrientes e comece a clonar o vírus ela mesma. Este processo acaba destruindo a célula no fim. Após ser clonado, cada vírus sai da célula que estava e penetra outra célula, crescendo seu número exponencialmente. <br>
Porém, o sistema imune do nosso corpo tenta destruir essas células infectadas através de um processo chamado **fagocitose**.

![Processo de fagocitose.](https://s1.static.brasilescola.uol.com.br/be/2023/01/ilustracao-dos-neutrofilos-realizando-a-fagocitose.jpg "Fagócito em ação")

Como possível ver na imagem acima, a célula saudável cobre a partícula indesejada (no nosso caso a célula infectada com a dengue), a digere por dentro e por fim despeja seus destroços. Nesta etapa do jogo, o objetivo é o vírus conseguir ser clonado enquanto evita ser destruido pelos fagócitos (células capazes de fazer fagocitose).

* ### Viremia

Caso consiga sobreviver, após se reproduzir os vírus no corpo se espalham pela nossa corrente sanguínea, podendo então atingir basicamente qualquer órgão. Esta presença do vírus no sangue é o que chamamos de **viremia**. Na dengue, especificamente, categorizamos como viremia passiva pois o vírus é depositado diretamente no nosso sangue (por conta da picada do mosquito).


## Algoritmos e programação <a name="algoritmos"></a>

O jogo possui 3 mini-jogos

## Última atualização <a name="atualizacoes"></a>
Este documento foi atualizado pela última vez no dia 10/10.
