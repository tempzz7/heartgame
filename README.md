# Sans Battle - Jogo Estilo Undertale

Um clone da batalha contra Sans no estilo Undertale, desenvolvido em C usando a biblioteca raylib.

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
wsl -- ./sans_battle
```

### Usando Windows Nativo

```
.\sans_battle.exe
```

## Controles

- Setas direcionais: Mover a alma vermelha
- Tecla de seta para cima ou Barra de espaço: Pular (no modo alma azul)

## Descrição do Jogo

Este jogo recria a famosa batalha contra Sans do jogo Undertale. Inclui:

- Diferentes padrões de ataque com ossos
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