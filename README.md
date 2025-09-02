# ambiente-de-testes
Esse repositória será destinado para uso de testes de códigos usando o codespace.

import java.util.Random;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * Versão Java do código em C que:
 * - Cria um vetor aleatório
 * - Cria dois "filhos" (threads) para somar a 1ª e a 2ª metade
 * - Pausa/retoma um filho e mata o outro, simulando sinais
 * - Mostra somas parciais e a média ao final
 */
public class Main {

    // Worker que soma uma faixa do vetor em "passos", com pausa/retomada e kill
    static class Worker implements Runnable {
        private final int id;
        private final int inicio;
        private final int fimExclusivo;
        private final int passo;
        private final int[] vetor;
        private final AtomicInteger somaCompartilhada;

        private volatile boolean running = true;   // usado para "kill"
        private volatile boolean paused = false;   // usado para pause/resume
        private final Object pauseLock = new Object();

        Worker(int id, int inicio, int fimExclusivo, int passo, int[] vetor, AtomicInteger somaCompartilhada) {
            this.id = id;
            this.inicio = inicio;
            this.fimExclusivo = fimExclusivo;
            this.passo = passo;
            this.vetor = vetor;
            this.somaCompartilhada = somaCompartilhada;
        }

        @Override
        public void run() {
            long pid = ProcessHandle.current().pid(); // PID do processo Java
            for (int i = inicio; i < fimExclusivo && running; i += passo) {

                // Suporte a "pause"
                try {
                    synchronized (pauseLock) {
                        while (paused && running) {
                            pauseLock.wait();
                        }
                    }
                } catch (InterruptedException e) {
                    // Se for interrupção causada por kill, encerra
                    if (!running) return;
                    // Caso contrário, apenas continua
                }

                int limite = Math.min(i + passo, fimExclusivo);
                // Soma bloco [i, limite)
                int parcial = 0;
                for (int j = i; j < limite; j++) {
                    parcial += vetor[j];
                }
                int somaAtual = somaCompartilhada.addAndGet(parcial);

                System.out.printf("[Filho %d | PID=%d] Somou elementos %d até %d (soma parcial = %d)%n",
                        id, pid, i, limite - 1, somaAtual);

                // Simula trabalho (equivalente ao sleep(1) do C)
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    if (!running) return; // encerrado por "kill"
                }
            }
        }

        // "Pausar" (equivalente a SIGSTOP)
        public void pauseWork() {
            paused = true;
        }

        // "Retomar" (equivalente a SIGCONT)
        public void resumeWork() {
            synchronized (pauseLock) {
                paused = false;
                pauseLock.notifyAll();
            }
        }

        // "Matar" (equivalente a SIGKILL)
        public void killWork() {
            running = false;
            // Desbloqueia esperas e sono
            synchronized (pauseLock) {
                paused = false;
                pauseLock.notifyAll();
            }
            Thread.currentThread().interrupt(); // não tem efeito aqui; será feito de fora
        }
    }

    public static void main(String[] args) throws InterruptedException {
        final int tam = 10_000;
        int[] vetor = new int[tam];

        Random rnd = new Random();
        for (int i = 0; i < tam; i++) {
            vetor[i] = rnd.nextInt(1000); // 0..999
        }

        int meio = tam / 2;
        int passo = 1000;

        // "Memória compartilhada": duas posições de soma, uma para cada metade
        AtomicInteger somaMetade1 = new AtomicInteger(0);
        AtomicInteger somaMetade2 = new AtomicInteger(0);

        Worker w1 = new Worker(1, 0, meio, passo, vetor, somaMetade1);
        Worker w2 = new Worker(2, meio, tam, passo, vetor, somaMetade2);

        Thread t1 = new Thread(w1, "Filho-1");
        Thread t2 = new Thread(w2, "Filho-2");

        t1.start();
        t2.start();

        // "Pai" controla os "filhos"
        Thread.sleep(3000);
        System.out.printf("%n--- Pausando thread %s --- %n", t1.getName());
        w1.pauseWork();

        Thread.sleep(3000);
        System.out.printf("%n--- Retomando thread %s --- %n", t1.getName());
        w1.resumeWork();

        Thread.sleep(3000);
        System.out.printf("%n--- Matando thread %s --- %n", t2.getName());
        // Sinaliza para encerrar e interrompe o sono caso esteja dormindo
        // (chamamos interrupt no objeto Thread)
        // Primeiro, marca o worker como não-running:
        // (não dá para chamar killWork() e ele interromper de dentro; quem
        // tem a Thread é o "pai")
        // então, fazemos:
        // 1) sinalizar w2
        // 2) interromper t2
        // 3) (se estiver pausado, retoma) — por segurança
        w2.killWork();
        t2.interrupt();

        // Aguarda término
        t1.join();
        t2.join();

        int soma1 = somaMetade1.get();
        int soma2 = somaMetade2.get();
        int somaTotal = soma1 + soma2;

        System.out.println();
        System.out.printf("Soma da primeira metade: %d%n", soma1);
        System.out.printf("Soma da segunda metade (pode estar incompleta se foi morto): %d%n", soma2);
        System.out.printf("Soma total: %d%n", somaTotal);
        System.out.printf("Média (aprox): %.2f%n", somaTotal / (double) tam);
    }
}