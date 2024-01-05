A simple editor made using raylib for our Sokoban style game.

Provides a graphical interface to generate plain text grids.

## Controls:
Click to place tiles or entities, starts in entity mode with player entity selected.

Be careful, mistakes cannot be undone, and do not try to replace a tile or entity, it will just place them both.

`t` tile mode
`1` select green floor tiles
`2` select red goal tiles

`e` entity mode
`p` select player entity
`b` select box entity

`s` generates map file named map in directory

## Windows Build Instructions:
Using tcc compiler

Drop in raylib lib and include into tcc dir

Drop in SokobanEditor repo into tcc dir

`.\tcc.exe -lraylib .\SokobanEditor\main.c -o editor.exe`
