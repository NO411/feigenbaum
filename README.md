# Feigenbaumdiagramm

![screenshot](https://raw.githubusercontent.com/NO411/feigenbaum/main/feigenbaum_diagramm.png?token=GHSAT0AAAAAABYUSQ7ZQ7UZF7MJHCGPDWJCYZVX3PQ)

## Installation
Install Raylib.

### Linux

```
g++ main.cpp -o ../feigenbaum -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

### Windows

```
g++ main.cpp -o feigenbaum.exe -O1 -Wall -Wno-missing-braces -I include/ -L lib/ -lraylib -lopengl132 -lgd
```
