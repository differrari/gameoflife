# Game of Life & Death

This is a simple implementation of the game of life (with extra rules) that runs on [REDACTED] OS.
As it includes extra simulation rules, it's broken down into 3 sections: Game of Life, Evolution, and Game of Death.

## Rules

### John Conway's Game of Life 

1. A cell with fewer than 2 neighbors dies as if by underpopulation
2. A cell with more than 3 neighbors dies as if by overpopulation
3. An empty cell with 3 neighbors comes to life as if by reproduction
4. A cell with 2 or 3 neighbors lives on to the next generation

### Evolution
1. Cells start with a color based off their coordinates with a random element
2. New born cells inherit the color of their 3 neighbors and a slight variation
3. Cells become brighter as they evolve

### Game of Death
1. Cells have a lifetime
2. When they reach the end of their lifetime, they will attempt to clone themselves

# The heat death of the universe is inevitable
