# ProgrammingContest
- this program was used to solve TSP

To solve TSP, we generated some good algorithm like cheapest insertion, nearest neighbour.
However I select random insertion and two-opt because of easily.In this program, I use random numbers to buil sequential circuit.
If you can guess that I use rand() function in stdlib.h, but it's not.
I heard rand() function gives not good random number even if we use srand() function.So, I use XorShift algorithm to make.

- このプログラムは巡回セールスマン問題(以後TSP)を解くためのものです.

TSPを解くにあたり,私たちは最安挿入法やら最近近傍法などのいい(?)アルゴリズムを開発しました.
けれども,私はランダム挿入法とtwo-optのアルゴリズムを選択しました,楽だし...このプログラムでは順回路を構築するにあたり乱数を用いてます.
Cで乱数と聞くと,多分stdlib.hのrand()関数使った?みたいに思われるかもしれませんが,違います.
どっかで,rand()関数はsrand()を使ってもそこそこの乱数しか返してくれないみたいな話を聞いたことがあるので,XorShiftというアルゴリズムで乱数を作りました
