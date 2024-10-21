# Mancala

This is a personal project to explore a topic from university that really interested me, game tree search!

I have recently been playing a lot of Mancala and wished to know how good my strategies, and specifically openers, are.
Surely I could have just googled this but that is no fun.
I am writing this in C since it is my perferred langauge and I wish for the search to be fast.
I will be following a three phase structure to it's implementation:

- [X] Phase 1:
    - console IO
    - Working mancala game
- [ ] Phase 2:
    - Multiple search strategies for best openings
        - Heuristics, such as seeing if the remaining seeds are enough to sway the game
    - Timings and simple data output / python graphing
- [ ] Phase 3:
    - Simple raylib visualization of games

I will be adding some of my results here when they are more complete.

## Building

This project is going to be kept small and simple.
A basic `Makefile` is provided with which you can simply run `make` in the current directory to build the `mancala` executable.