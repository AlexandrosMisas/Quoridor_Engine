# Quoridor Engine

## About
An engine made in pure C11 simulating the board game [Quoridor](http://quoridor.di.uoa.gr/rules.html) on a terminal. </br>
More information about the game can be found [here](https://en.wikipedia.org/wiki/Quoridor) and an online board to try the game [here](http://quoridor.di.uoa.gr/). </br>



## Quoridor Text Protocol
The communication protocol between the user/programs that they play Quoridor is well described [here](http://quoridor.di.uoa.gr/qtp/qtp.html). </br>
It is obligatory for the user to be well informed for the way the parsing is implemented and the usage of each command and its requested arguments.
Although, there is enough error checking and no graceful exit for any mistake a user makes :).

## AI Agent
The engine is based on minimax with alpha-beta pruning  that uses iterative deepening in order to search at the maximum depth possible at a fixed time.
Although it would be better the time for generating a move to be parameterized based on the board size and the available moves the player has in every snapshot of the game.
## Compilation/Example

A Makefile is provided for the creation of the executable file that is stored at the ./build folder.
The game initially starts with a board size of 7 but the user can choose any available combination for the board size and the number of walls as shown below. </br>
```
make run

boardsize 5
=

clear_board
=

walls 2
=

showboard
=

     A   B   C   D   E
   +---+---+---+---+---+
5  |   |   | B |   |   |  5  black walls: 2
   +---+---+---+---+---+
4  |   |   |   |   |   |  4  white walls: 2
   +---+---+---+---+---+
3  |   |   |   |   |   |  3
   +---+---+---+---+---+
2  |   |   |   |   |   |  2
   +---+---+---+---+---+
1  |   |   | W |   |   |  1
   +---+---+---+---+---+
     A   B   C   D   E

 
genmove black
= C4

playmove white c2
=

genmove black
= B5 horizontal

playwall white c2 horizontal
=

showboard
=

     A   B   C   D   E
   +---+---+---+---+---+
5  |   |   |   |   |   |  5  black walls: 1
   +---+=======+---+---+
4  |   |   | B |   |   |  4  white walls: 1
   +---+---+---+---+---+
3  |   |   |   |   |   |  3
   +---+---+---+---+---+
2  |   |   | W |   |   |  2
   +---+---+=======+---+
1  |   |   |   |   |   |  1
   +---+---+---+---+---+
     A   B   C   D   E

winner
= false

quit
=

make clean
```
## Referee
A referee  is also provided that allows two programs to play against each other. </br>
Both programs must follow the protocol that was mentioned above. </br>
We can also use a program to play against itself.
```
Run with:
chmod +x quoridor_referee.py < program_1 > < program_2 >
./quoridor_referee.py --white < program_1 > --black < program_2 > possible_args

possible_args = {
  --verbose 1 (to list moves) or --verbose 2 (to draw board)
  --size <board size>               (default 9)
  --walls <number of walls>         (default 7/4*boardsize-23/4)
  --games <number of games to play> (default 1)
  --maximum_time <time(in seconds)> (default boardsize/3)
  --move_time <time(in seconds)>    (default 30)
  --memory_limit <memory(in MB)>    (default 950MB)
  --seed <seed>                     (default current unix timestamp)
}
```
