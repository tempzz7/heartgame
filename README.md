# HEART - Entre o Vazio e a Esperança

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