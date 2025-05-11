# HEART - Fangame Definitivo

Um fangame modular, inspirado em Undertale/Sans Fight, feito em C com Raylib.

## Build rápido (WSL ou Linux)

1. Instale Raylib:
   ```sh
   sudo apt install libraylib-dev
   ```
2. Compile:
   ```sh
   make
   ```
3. Rode:
   ```sh
   ./heartgame
   ```

No Windows puro: use MinGW + Raylib ou compile no WSL.

---

## Estrutura do Projeto
- `main.c`: Loop principal, inicialização.
- `game.[ch]`: Estado global, fases, transições.
- `player.[ch]`: Movimento, física, animação, dano.
- `attack.[ch]`: Padrões de ataque (ossos, magenta, amarelo, dinâmico).
- `hud.[ch]`: HUD, barra de vida, mensagens, telas de morte/vitória.
- `utils.[ch]`: Funções auxiliares (timer, random, colisão).

---

## Como expandir
- **Adicionar ataques**: Edite `attack.c` e crie novos padrões em `AttackManagerUpdate`.
- **Novos efeitos**: Use `utils.c` para helpers, adicione partículas em `player.c` ou `hud.c`.
- **Novas fases**: Controle a variável `phase` em `game.c` para lógica especial.
- **HUD customizado**: Expanda `hud.c` para mostrar mais informações.

---

## Dicas de design
- Use feedback visual (piscada, partículas, cor) para deixar o jogo mais "vivo".
- Balanceie a dificuldade alternando padrões e ajustando spawnRate.
- Teste sempre: jogabilidade justa é mais importante que dificuldade extrema!
- Separe lógica de desenho e update para facilitar manutenção.

---

## Créditos
- Código base: você + Cascade AI.
- Biblioteca gráfica: [Raylib](https://www.raylib.com/)
- Inspiração: Undertale, Sans Fight, comunidade fangame.

---

Dúvidas ou bugs? Edite, brinque, expanda! Este projeto é seu laboratório de criatividade.


HEART é um jogo sombrio que coloca você no controle do coração de uma pessoa que perdeu todas as emoções, restando apenas uma última centelha de sentimento. Preso em uma dimensão entre o vazio e a esperança, o coração enfrenta seus próprios medos e arrependimentos em uma jornada por sobrevivência e redenção. Cada ponto representa um passo para recuperar as memórias e sentimentos perdidos. Mas cuidado: se o HP do coração chegar a zero, ele se despedaça, simbolizando a perda completa de suas emoções e a dissolução dessa alma que já foi humana.

## Requisitos

- Windows com WSL (Windows Subsystem for Linux) instalado
- Biblioteca raylib (incluída como subdiretório)
- Compilador GCC

## Como Compilar

### Usando WSL (Recomendado)

1. Abra um terminal PowerShell e navegue até a pasta do projeto
2. Execute: `wsl -- make`

### Usando Windows Nativo (Se tiver MinGW configurado)

1. Abra um terminal PowerShell e navegue até a pasta do projeto
2. Execute: `make`

## Como Executar

### Usando WSL (Recomendado)

```
wsl -- ./heart_battle
```

### Usando Windows Nativo

```
.\heart_battle.exe
```

## Controles

- Setas direcionais: Mover o coração
- Tecla de seta para cima ou Barra de espaço: Pular (no modo alma azul)

## Descrição do Jogo

Este jogo é uma jornada de superação e autodescoberta, onde cada ataque representa um fragmento de memória ou emoção perdida. Enfrente o vazio, recupere sentimentos e sobreviva à dissolução da alma.

- Diferentes padrões de ataque
- Mudança de alma (vermelha para azul)
- Sistema de diálogos
- Mecânica de gravidade com a alma azul
- Barra de HP
- Efeitos visuais de invencibilidade

## Desenvolvimento no VS Code

Este projeto está configurado para desenvolvimento no VS Code. 

Para compilar o jogo, use o atalho `Ctrl+Shift+B` ou execute a tarefa "Compilar com WSL".
Para depurar, use o menu de depuração e selecione "Depurar com WSL".

---

Desenvolvido com raylib - www.raylib.com