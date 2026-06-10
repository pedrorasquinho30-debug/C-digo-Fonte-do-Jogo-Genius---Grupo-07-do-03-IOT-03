// **************** MONTAGEM DO DISPLAY
// Mapa visual dos segmentos do display de 7 segmentos
// L L G L L
// e e N e e
// d d D d d
// 2 4   3 0
//| ----------
//| . . . . . |
//|           |
//|           |
//|           |
//|           |
//| . . . . . |
//| ----------
// 6 5   1 P
// d d D d d
// e e N e e
// L L L L L
// ************************************

// --- Pinos dos segmentos do display de 7 segmentos ---
#define ledP A3   // Segmento ponto decimal
#define led0 A2   // Segmento superior direito
#define led1 A4   // Segmento inferior direito
#define led2 A0   // Segmento superior
#define led3 A1   // Segmento do meio
#define led4 9    // Segmento superior esquerdo
#define led5 8    // Segmento inferior
#define led6 7    // Segmento inferior esquerdo

// --- Pinos dos botões (Simon) ---
#define BTN_0 0
#define BTN_1 1
#define BTN_2 2
#define BTN_3 3

// --- Pinos dos LEDs coloridos (feedback visual de cada botão) ---
#define LED_0 10
#define LED_1 11
#define LED_2 12
#define LED_3 13

// --- Pino do buzzer ---
#define BUZZ 6

// --- Variáveis globais ---
byte mem[40];   // Sequência gerada pelo jogo (até 40 rodadas)
byte botao = 4; // Último botão pressionado (4 = nenhum)
byte sort = 4;  // Valor sorteado para a rodada (4 = aguardando sorteio)
byte aux = 0;   // Contador de acertos na rodada atual

int stagio = 1; // Estágio/rodada atual do jogo

// --- Apaga todos os segmentos do display ---
void limpadisplay()
{
  digitalWrite(led0, LOW);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  digitalWrite(led5, LOW);
  digitalWrite(led6, LOW);
  digitalWrite(ledP, LOW);
}

// --- Teste automático do display e dos segmentos ---
// Acende cada segmento individualmente, depois todos juntos
// TempoTeste: tempo (ms) que cada segmento fica aceso
void AutoTeste(int TempoTeste)
{
  limpadisplay();

  digitalWrite(led0, HIGH); delay(TempoTeste); digitalWrite(led0, LOW);
  digitalWrite(led1, HIGH); delay(TempoTeste); digitalWrite(led1, LOW);
  digitalWrite(led5, HIGH); delay(TempoTeste); digitalWrite(led5, LOW);
  digitalWrite(led6, HIGH); delay(TempoTeste); digitalWrite(led6, LOW);
  digitalWrite(led4, HIGH); delay(TempoTeste); digitalWrite(led4, LOW);
  digitalWrite(led3, HIGH); delay(TempoTeste); digitalWrite(led3, LOW);
  digitalWrite(led2, HIGH); delay(TempoTeste); digitalWrite(led2, LOW);
  digitalWrite(ledP, HIGH); delay(TempoTeste); digitalWrite(ledP, LOW);

  // Acende todos os segmentos ao mesmo tempo
  digitalWrite(led0, HIGH); digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH); digitalWrite(led3, HIGH);
  digitalWrite(led4, HIGH); digitalWrite(led5, HIGH);
  digitalWrite(led6, HIGH); digitalWrite(ledP, HIGH);

  delay(TempoTeste + 200);
  limpadisplay();
}

// --- Configuração inicial do Arduino ---
void setup()
{
  // Configura pinos do display como saída
  pinMode(led0, OUTPUT); pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT); pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT); pinMode(led5, OUTPUT);
  pinMode(led6, OUTPUT); pinMode(ledP, OUTPUT);

  // Configura buzzer como saída
  pinMode(BUZZ, OUTPUT);

  // Configura LEDs coloridos como saída
  pinMode(LED_0, OUTPUT); pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT); pinMode(LED_3, OUTPUT);

  // Configura botões com resistor de pull-up interno (LOW = pressionado)
  pinMode(BTN_0, INPUT_PULLUP); pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP); pinMode(BTN_3, INPUT_PULLUP);

  limpadisplay();
  AutoTeste(200); // Testa todos os segmentos do display

  // Animação de inicialização: acende cada LED com um tom crescente
  tone(BUZZ, 200); digitalWrite(LED_0, HIGH); delay(400); noTone(BUZZ);
  tone(BUZZ, 300); digitalWrite(LED_1, HIGH); delay(400); noTone(BUZZ);
  tone(BUZZ, 400); digitalWrite(LED_2, HIGH); delay(400); noTone(BUZZ);
  tone(BUZZ, 500); digitalWrite(LED_3, HIGH); delay(400); noTone(BUZZ);

  // Apaga todos os LEDs após a animação
  digitalWrite(LED_0, LOW); digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW); digitalWrite(LED_3, LOW);

  NumeroNoDisplay(stagio); // Exibe o estágio inicial (1) no display
}

// --- Loop principal do jogo Simon ---
void loop()
{
  NumeroNoDisplay(stagio); // Atualiza o display com o estágio atual

  // Se ainda não foi sorteado o próximo LED desta rodada...
  if (sort == 4)
  {
    // Aguarda qualquer botão ser pressionado para iniciar o sorteio
    while (digitalRead(BTN_0) && digitalRead(BTN_1) &&
           digitalRead(BTN_2) && digitalRead(BTN_3))
    {
      Sort(); // Fica sorteando aleatoriamente enquanto nenhum botão é pressionado
    }

    // Aguarda o botão ser solto antes de continuar
    while (!digitalRead(BTN_0) || !digitalRead(BTN_1) ||
           !digitalRead(BTN_2) || !digitalRead(BTN_3))
    {
      leds(4); // Nenhum LED acende (led=4 é inválido)
    }
  }

  mem[stagio - 1] = sort; // Armazena o LED sorteado na posição da rodada atual

  // Exibe toda a sequência acumulada até o estágio atual
  for (int i = 0; i < stagio; i++)
  {
    leds(mem[i]);
  }

  aux = 0; // Reseta o contador de acertos do jogador

  // Loop de leitura dos botões: o jogador deve repetir a sequência
  while (aux < stagio)
  {
    // Aguarda pressionar algum botão
    while (digitalRead(BTN_0) && digitalRead(BTN_1) &&
           digitalRead(BTN_2) && digitalRead(BTN_3)) {}

    // Identifica qual botão foi pressionado
    if (!digitalRead(BTN_0)) botao = 0;
    if (!digitalRead(BTN_1)) botao = 1;
    if (!digitalRead(BTN_2)) botao = 2;
    if (!digitalRead(BTN_3)) botao = 3;

    // Aguarda o botão ser solto
    while (!digitalRead(BTN_0) || !digitalRead(BTN_1) ||
           !digitalRead(BTN_2) || !digitalRead(BTN_3)) {}

    leds(botao); // Acende o LED e toca o som do botão pressionado

    // Verifica se o botão pressionado está errado
    if (botao != mem[aux])
    {
      // Sinaliza erro: acende todos os LEDs e toca tom grave
      digitalWrite(LED_0, HIGH); digitalWrite(LED_1, HIGH);
      digitalWrite(LED_2, HIGH); digitalWrite(LED_3, HIGH);
      tone(BUZZ, 80); delay(1000); noTone(BUZZ);
      delay(500);
      digitalWrite(LED_0, LOW); digitalWrite(LED_1, LOW);
      digitalWrite(LED_2, LOW); digitalWrite(LED_3, LOW);

      // Reinicia todas as variáveis do jogo (volta ao estágio 1)
      for (int i = 0; i < 40; i++) mem[i] = 0;
      stagio = 1; aux = 0; botao = 4; sort = 4;

      NumeroNoDisplay(1);
      return; // Sai do loop e recomeça
    }

    aux++;      // Incrementa acertos
    botao = 4;  // Reseta o botão lido
  }

  // Jogador acertou toda a sequência: avança para o próximo estágio
  stagio++;

  // Limita o display ao máximo de 9 (1 dígito)
  if (stagio > 9)
    NumeroNoDisplay(9);
  else
    NumeroNoDisplay(stagio);

  delay(500);

  sort = 4; // Marca que um novo sorteio deve ocorrer na próxima rodada
}

// --- Acende o LED correspondente ao botão e toca seu tom ---
// led 0-3: acende LED_0 a LED_3 com tom de 200 a 500 Hz
// led = 4: nenhuma ação (usado para "limpar")
void leds(int led)
{
  digitalWrite(LED_0, LOW); digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW); digitalWrite(LED_3, LOW);

  if (led < 4)
  {
    digitalWrite(led + 10, HIGH);       // Acende o LED correspondente
    tone(BUZZ, (led + 2) * 100);        // Tom proporcional ao LED (200~500Hz)
    delay(500);
    noTone(BUZZ);
    digitalWrite(led + 10, LOW);        // Apaga o LED
  }

  delay(200); // Pausa entre flashes
}

// --- Gera um "sorteio" pseudo-aleatório ciclando de 0 a 3 muito rapidamente ---
// Como Sort() é chamada milhares de vezes antes do botão ser pressionado,
// o valor final de `sort` é imprevisível para o jogador
void Sort()
{
  if (sort)
    sort--;   // Decrementa até 0
  else
    sort = 3; // Volta ao 3 e repete o ciclo

  delayMicroseconds(500);
}

// --- Exibe um número (0-9) no display de 7 segmentos ---
// Cada case acende os segmentos corretos para representar o dígito
// Valor fora do range 0-9: acende apenas o ponto decimal (indicador de erro)
void NumeroNoDisplay(int valor)
{
  limpadisplay();

  switch (valor)
  {
    case 0: // Segmentos: todos exceto o do meio
      digitalWrite(led0, HIGH); digitalWrite(led1, HIGH);
      digitalWrite(led3, HIGH); digitalWrite(led4, HIGH);
      digitalWrite(led5, HIGH); digitalWrite(led6, HIGH);
      break;

    case 1: // Segmentos: lado direito
      digitalWrite(led0, HIGH); digitalWrite(led1, HIGH);
      break;

    case 2: // Segmentos: superiores + meio + inferiores sem lados esquerdos
      digitalWrite(led0, HIGH); digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH); digitalWrite(led5, HIGH);
      digitalWrite(led6, HIGH);
      break;

    case 3:
      digitalWrite(led0, HIGH); digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH); digitalWrite(led3, HIGH);
      digitalWrite(led5, HIGH);
      break;

    case 4:
      digitalWrite(led0, HIGH); digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH); digitalWrite(led4, HIGH);
      break;

    case 5:
      digitalWrite(led1, HIGH); digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH); digitalWrite(led4, HIGH);
      digitalWrite(led5, HIGH);
      break;

    case 6:
      digitalWrite(led1, HIGH); digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH); digitalWrite(led4, HIGH);
      digitalWrite(led5, HIGH); digitalWrite(led6, HIGH);
      break;

    case 7:
      digitalWrite(led0, HIGH); digitalWrite(led1, HIGH);
      digitalWrite(led3, HIGH);
      break;

    case 8: // Todos os segmentos acesos
      digitalWrite(led0, HIGH); digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH); digitalWrite(led3, HIGH);
      digitalWrite(led4, HIGH); digitalWrite(led5, HIGH);
      digitalWrite(led6, HIGH);
      break;

    case 9:
      digitalWrite(led0, HIGH); digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH); digitalWrite(led3, HIGH);
      digitalWrite(led4, HIGH); digitalWrite(led5, HIGH);
      break;

    default: // Valor inválido: acende o ponto decimal como indicador de erro
      digitalWrite(ledP, HIGH);
      break;
  }
}