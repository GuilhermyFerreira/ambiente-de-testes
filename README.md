# ambiente-de-testes
Esse repositória será destinado para uso de testes de códigos usando o codespace.


import javax.swing.*;
import javax.swing.border.TitledBorder;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.util.Random;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * GUI para o somador paralelo:
 * - Gera vetor aleatório
 * - Dois "filhos" (threads) somam 1ª e 2ª metade por blocos
 * - Botões: Iniciar, Pausar/Retomar Filho 1, Matar Filho 2, Reset
 * - Log estilo console, com somas parciais
 */
public class Main extends JFrame {

    // Configurações
    private static final int TAM_DEFAULT = 10_000;
    private static final int PASSO_DEFAULT = 1000;

    // Estado/Modelo
    private int[] vetor;
    private int tam = TAM_DEFAULT;
    private int passo = PASSO_DEFAULT;
    private int meio;

    private final AtomicInteger somaMetade1 = new AtomicInteger(0);
    private final AtomicInteger somaMetade2 = new AtomicInteger(0);

    // Workers (threads)
    private SumWorker w1;
    private SumWorker w2;

    // UI
    private JButton btnIniciar;
    private JButton btnPausar1;
    private JButton btnRetomar1;
    private JButton btnMatar2;
    private JButton btnReset;

    private JProgressBar progress1;
    private JProgressBar progress2;

    private JLabel lblSoma1;
    private JLabel lblSoma2;
    private JLabel lblTotal;
    private JLabel lblMedia;

    private JTextArea txtLog;
    private JSpinner spTam;
    private JSpinner spPasso;

    public Main() {
        super("Somador Paralelo - GUI (Threads)");
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setSize(920, 640);
        setLocationRelativeTo(null);
        setLayout(new BorderLayout(10, 10));

        add(criaPainelTopo(), BorderLayout.NORTH);
        add(criaPainelCentro(), BorderLayout.CENTER);
        add(criaPainelRodape(), BorderLayout.SOUTH);

        atualizaBotoesEstadoInicial();
    }

    private JPanel criaPainelTopo() {
        JPanel p = new JPanel(new GridBagLayout());
        p.setBorder(new TitledBorder("Configurações"));

        spTam = new JSpinner(new SpinnerNumberModel(TAM_DEFAULT, 1_000, 5_000_000, 1_000));
        spPasso = new JSpinner(new SpinnerNumberModel(PASSO_DEFAULT, 10, 100_000, 10));

        btnIniciar = new JButton(new AbstractAction("Iniciar") {
            @Override public void actionPerformed(ActionEvent e) { iniciar(); }
        });
        btnReset = new JButton(new AbstractAction("Reset") {
            @Override public void actionPerformed(ActionEvent e) { resetar(); }
        });

        GridBagConstraints c = new GridBagConstraints();
        c.insets = new Insets(6, 6, 6, 6);
        c.gridx = 0; c.gridy = 0; c.anchor = GridBagConstraints.WEST;
        p.add(new JLabel("Tamanho do vetor:"), c);
        c.gridx = 1; p.add(spTam, c);

        c.gridx = 2; p.add(new JLabel("Passo:"), c);
        c.gridx = 3; p.add(spPasso, c);

        c.gridx = 4; c.weightx = 1; c.anchor = GridBagConstraints.EAST;
        p.add(btnReset, c);
        c.gridx = 5;
        p.add(btnIniciar, c);

        return p;
    }

    private JPanel criaPainelCentro() {
        JPanel p = new JPanel(new GridBagLayout());
        p.setBorder(new TitledBorder("Execução"));

        // Controles dos filhos
        btnPausar1 = new JButton(new AbstractAction("Pausar Filho 1") {
            @Override public void actionPerformed(ActionEvent e) { if (w1 != null) w1.pauseWork(); atualizaEstadoBotoes(); }
        });
        btnRetomar1 = new JButton(new AbstractAction("Retomar Filho 1") {
            @Override public void actionPerformed(ActionEvent e) { if (w1 != null) w1.resumeWork(); atualizaEstadoBotoes(); }
        });
        btnMatar2 = new JButton(new AbstractAction("Matar Filho 2") {
            @Override public void actionPerformed(ActionEvent e) { if (w2 != null) w2.killWork(); atualizaEstadoBotoes(); }
        });

        progress1 = new JProgressBar(0, 100);
        progress2 = new JProgressBar(0, 100);

        lblSoma1 = new JLabel("0");
        lblSoma2 = new JLabel("0");
        lblTotal = new JLabel("0");
        lblMedia = new JLabel("0.0");

        txtLog = new JTextArea();
        txtLog.setEditable(false);
        txtLog.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 12));
        JScrollPane scroll = new JScrollPane(txtLog);

        GridBagConstraints c = new GridBagConstraints();
        c.insets = new Insets(6,6,6,6);
        c.fill = GridBagConstraints.HORIZONTAL;

        // Linha 1: Filho 1
        c.gridx = 0; c.gridy = 0;
        p.add(new JLabel("Filho 1:"), c);
        c.gridx = 1; c.weightx = 1;
        p.add(progress1, c);
        c.weightx = 0;
        c.gridx = 2;
        p.add(btnPausar1, c);
        c.gridx = 3;
        p.add(btnRetomar1, c);

        // Linha 2: Filho 2
        c.gridx = 0; c.gridy = 1;
        p.add(new JLabel("Filho 2:"), c);
        c.gridx = 1; c.weightx = 1;
        p.add(progress2, c);
        c.weightx = 0;
        c.gridx = 2;
        p.add(btnMatar2, c);

        // Linha 3: Log
        c.gridx = 0; c.gridy = 2; c.gridwidth = 4; c.weightx = 1; c.weighty = 1; c.fill = GridBagConstraints.BOTH;
        p.add(scroll, c);

        return p;
    }

    private JPanel criaPainelRodape() {
        JPanel p = new JPanel(new GridBagLayout());
        p.setBorder(new TitledBorder("Resultados"));

        GridBagConstraints c = new GridBagConstraints();
        c.insets = new Insets(6,6,6,6);
        c.gridx = 0; c.gridy = 0; c.anchor = GridBagConstraints.WEST;
        p.add(new JLabel("Soma 1ª metade:"), c);
        c.gridx = 1; p.add(lblSoma1, c);

        c.gridx = 2; p.add(new JLabel("Soma 2ª metade:"), c);
        c.gridx = 3; p.add(lblSoma2, c);

        c.gridx = 4; p.add(new JLabel("Soma total:"), c);
        c.gridx = 5; p.add(lblTotal, c);

        c.gridx = 6; p.add(new JLabel("Média (aprox):"), c);
        c.gridx = 7; p.add(lblMedia, c);

        return p;
    }

    private void iniciar() {
        if (w1 != null || w2 != null) {
            appendLog("⚠️ Já está em execução. Clique em Reset para reiniciar.\n");
            return;
        }

        // Lê config
        tam = (Integer) spTam.getValue();
        passo = (Integer) spPasso.getValue();
        if (passo <= 0) passo = 1;

        // Gera vetor
        vetor = new int[tam];
        Random rnd = new Random();
        for (int i = 0; i < tam; i++) vetor[i] = rnd.nextInt(1000);
        meio = tam / 2;

        somaMetade1.set(0);
        somaMetade2.set(0);
        atualizaLabels();

        // Cria workers
        w1 = new SumWorker(
                1, 0, meio, passo, vetor, somaMetade1,
                this::appendLog, this::onProgress1, this::onFinishOne);
        w2 = new SumWorker(
                2, meio, tam, passo, vetor, somaMetade2,
                this::appendLog, this::onProgress2, this::onFinishOne);

        // Start
        w1.execute();
        w2.execute();

        appendLog("✅ Iniciado.\n");
        atualizaEstadoBotoes();
    }

    private void resetar() {
        // Tenta parar
        if (w1 != null) w1.killWork();
        if (w2 != null) w2.killWork();

        w1 = null;
        w2 = null;
        somaMetade1.set(0);
        somaMetade2.set(0);
        progress1.setValue(0);
        progress2.setValue(0);
        txtLog.setText("");
        atualizaLabels();
        atualizaBotoesEstadoInicial();
        appendLog("🔁 Reset concluído.\n");
    }

    private void onProgress1(int percent) { progress1.setValue(percent); atualizaLabels(); }
    private void onProgress2(int percent) { progress2.setValue(percent); atualizaLabels(); }

    private void onFinishOne() {
        // Quando um termina/morre, atualiza totais
        atualizaLabels();
        // Se ambos já não estão rodando, calcula média e imprime resultado final
        boolean done1 = w1 == null || w1.isDone() || w1.isKilled();
        boolean done2 = w2 == null || w2.isDone() || w2.isKilled();

        if (done1 && done2) {
            appendLog("\n✅ Execução finalizada.\n");
            atualizaEstadoBotoes();
        }
    }

    private void atualizaLabels() {
        int s1 = somaMetade1.get();
        int s2 = somaMetade2.get();
        int total = s1 + s2;
        double media = vetor != null && vetor.length > 0 ? total / (double) vetor.length : 0.0;

        lblSoma1.setText(String.valueOf(s1));
        lblSoma2.setText(String.valueOf(s2));
        lblTotal.setText(String.valueOf(total));
        lblMedia.setText(String.format("%.4f", media));
    }

    private void appendLog(String text) {
        SwingUtilities.invokeLater(() -> {
            txtLog.append(text);
            txtLog.setCaretPosition(txtLog.getDocument().getLength());
        });
    }

    private void atualizaEstadoBotoes() {
        boolean running1 = w1 != null && !w1.isDone() && !w1.isKilled();
        boolean paused1  = running1 && w1.isPaused();

        boolean running2 = w2 != null && !w2.isDone() && !w2.isKilled();

        btnIniciar.setEnabled(w1 == null && w2 == null);
        btnReset.setEnabled(true);

        btnPausar1.setEnabled(running1 && !paused1);
        btnRetomar1.setEnabled(running1 && paused1);
        btnMatar2.setEnabled(running2);
    }

    private void atualizaBotoesEstadoInicial() {
        btnIniciar.setEnabled(true);
        btnReset.setEnabled(true);
        btnPausar1.setEnabled(false);
        btnRetomar1.setEnabled(false);
        btnMatar2.setEnabled(false);
    }

    // ------------ Worker com pausa/retomada/kill e log ----------
    private static class SumWorker extends SwingWorker<Void, String> {
        private final int id;
        private final int inicio;
        private final int fimExclusivo;
        private final int passo;
        private final int[] vetor;
        private final AtomicInteger somaCompartilhada;
        private final LogFn logFn;
        private final ProgressFn progressFn;
        private final FinishFn finishFn;

        private volatile boolean running = true;  // kill
        private volatile boolean paused = false;  // pause
        private final Object pauseLock = new Object();

        private int iAtual; // para progresso

        SumWorker(int id, int inicio, int fimExclusivo, int passo, int[] vetor, AtomicInteger somaCompartilhada,
                  LogFn logFn, ProgressFn progressFn, FinishFn finishFn) {
            this.id = id;
            this.inicio = inicio;
            this.fimExclusivo = fimExclusivo;
            this.passo = passo;
            this.vetor = vetor;
            this.somaCompartilhada = somaCompartilhada;
            this.logFn = logFn;
            this.progressFn = progressFn;
            this.finishFn = finishFn;
        }

        @Override
        protected Void doInBackground() {
            long pid = ProcessHandle.current().pid();
            for (int i = inicio; i < fimExclusivo && running; i += passo) {
                iAtual = i;

                // Pause
                try {
                    synchronized (pauseLock) {
                        while (paused && running) pauseLock.wait();
                    }
                } catch (InterruptedException ignored) {
                    if (!running) break;
                }

                int limite = Math.min(i + passo, fimExclusivo);
                int parcial = 0;
                for (int j = i; j < limite; j++) parcial += vetor[j];
                int somaAtual = somaCompartilhada.addAndGet(parcial);

                logFn.log(String.format(
                        "[Filho %d | PID=%d] Somou elementos %d até %d (soma parcial = %d)\n",
                        id, pid, i, limite - 1, somaAtual));

                // Progresso
                int lenFaixa = (fimExclusivo - inicio);
                int perc = (int) (100.0 * (Math.min(limite, fimExclusivo) - inicio) / Math.max(1, lenFaixa));
                progressFn.progress(Math.min(perc, 100));

                // Simula trabalho
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException ignored) {
                    if (!running) break;
                }
            }
            return null;
        }

        @Override
        protected void done() {
            finishFn.onFinish();
        }

        // Controles
        void pauseWork() {
            if (!running) return;
            paused = true;
            logFn.log(String.format("--- Pausando Filho %d ---\n", id));
        }

        void resumeWork() {
            if (!running) return;
            synchronized (pauseLock) {
                paused = false;
                pauseLock.notifyAll();
            }
            logFn.log(String.format("--- Retomando Filho %d ---\n", id));
        }

        void killWork() {
            running = false;
            synchronized (pauseLock) {
                paused = false;
                pauseLock.notifyAll();
            }
            logFn.log(String.format("--- Matando Filho %d ---\n", id));
            this.cancel(true);
        }

        boolean isPaused() { return paused; }
        boolean isKilled() { return !running; }
    }

    // ------------ Funcionais simples para callbacks -------------
    @FunctionalInterface private interface LogFn      { void log(String s); }
    @FunctionalInterface private interface ProgressFn { void progress(int percent); }
    @FunctionalInterface private interface FinishFn   { void onFinish(); }

    // ------------------ main ------------------
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            try {
                // Look & Feel do sistema
                UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
            } catch (Exception ignored) {}
            new Main().setVisible(true);
        });
    }
}
