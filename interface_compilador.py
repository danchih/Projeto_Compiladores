import tkinter as tk
from tkinter import filedialog, messagebox
import subprocess
import threading
import os
import re

COMPILADOR_EXEC = "main.exe"  # ou "./main" no Linux
arquivo_atual = None


# ---------- Funções principais ----------
def abrir_arquivo():
    global arquivo_atual
    caminho = filedialog.askopenfilename(
        title="Selecione o arquivo a ser compilado",
        filetypes=[("Todos os arquivos", "*.*")]
    )
    if caminho:
        try:
            with open(caminho, "r", encoding="utf-8", errors="ignore") as f:
                conteudo = f.read()
            entrada_text.delete("1.0", tk.END)
            entrada_text.insert("1.0", conteudo)
            arquivo_atual = caminho
            status_label.config(text=f"Arquivo aberto: {caminho}")
            limpar_saida()
            atualizar_linhas()
        except Exception as e:
            messagebox.showerror("Erro", f"Falha ao abrir o arquivo:\n{e}")


def salvar_edicoes():
    global arquivo_atual
    if not arquivo_atual:
        messagebox.showinfo("Salvar", "Abra um arquivo antes de salvar.")
        return
    try:
        with open(arquivo_atual, "w", encoding="utf-8") as f:
            f.write(entrada_text.get("1.0", tk.END))
        status_label.config(text=f"Arquivo salvo: {arquivo_atual}")
    except Exception as e:
        messagebox.showerror("Erro ao salvar", str(e))


def limpar_saida():
    saida_text.config(state="normal")
    saida_text.delete("1.0", tk.END)
    saida_text.config(state="disabled")


def exibir_saida(texto):
    """Exibe texto na área de saída, colorindo se for erro"""
    def inserir():
        saida_text.config(state="normal")
        saida_text.insert(tk.END, texto)
        saida_text.see(tk.END)
        saida_text.config(state="disabled")

        # Detecta "linha X" e destaca
        match = re.search(r"[Ll]inha\s+(\d+)", texto)
        if match:
            linha = match.group(1)
            saida_text.tag_add("erro", "end-2l linestart", "end-2l lineend")
            saida_text.tag_config("erro", foreground="red", font=("Consolas", 10, "bold"))
            status_label.config(text=f"Erro detectado na linha {linha}")

    janela.after(0, inserir)


def compilar():
    global arquivo_atual
    if not arquivo_atual:
        messagebox.showinfo("Compilar", "Abra um arquivo antes de compilar.")
        return

    if not os.path.exists(COMPILADOR_EXEC):
        messagebox.showerror("Erro", f"Compilador não encontrado: {COMPILADOR_EXEC}")
        return

    salvar_edicoes()
    limpar_saida()
    status_label.config(text="Compilando...")

    def thread_exec():
        # Usa shell=True no Windows para garantir o flush imediato
        process = subprocess.Popen(
            [COMPILADOR_EXEC, arquivo_atual],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            shell=True,
            bufsize=1,
            universal_newlines=True
        )

        for line in iter(process.stdout.readline, ''):
            if line:
                exibir_saida(line)

        process.stdout.close()
        process.wait()
        status_label.config(text="Compilação finalizada.")

    threading.Thread(target=thread_exec, daemon=True).start()


# ---------- Numeração de linhas ----------
def atualizar_linhas(event=None):
    entrada_linhas.config(state="normal")
    entrada_linhas.delete("1.0", tk.END)
    total = int(entrada_text.index("end-1c").split(".")[0])
    numeros = "\n".join(str(i) for i in range(1, total + 1))
    entrada_linhas.insert("1.0", numeros)
    entrada_linhas.config(state="disabled")


def rolar_juntas(*args):
    entrada_text.yview(*args)
    entrada_linhas.yview(*args)


def sincronizar_scroll(event):
    entrada_linhas.yview_moveto(entrada_text.yview()[0])


# ---------- Interface ----------
janela = tk.Tk()
janela.title("Compilador - Interface Gráfica")
janela.geometry("950x700")

frame_editor = tk.Frame(janela)
frame_editor.pack(padx=10, pady=5, fill="both", expand=True)

scroll = tk.Scrollbar(frame_editor)
scroll.pack(side="right", fill="y")

entrada_linhas = tk.Text(frame_editor, width=5, padx=5, takefocus=0, border=0,
                         background="#f0f0f0", state="disabled", font=("Consolas", 11))
entrada_linhas.pack(side="left", fill="y")

entrada_text = tk.Text(frame_editor, font=("Consolas", 11), undo=True, wrap="none")
entrada_text.pack(side="left", fill="both", expand=True)

scroll.config(command=rolar_juntas)
entrada_text.config(yscrollcommand=scroll.set)
entrada_linhas.config(yscrollcommand=scroll.set)

entrada_text.bind("<KeyRelease>", atualizar_linhas)
entrada_text.bind("<MouseWheel>", sincronizar_scroll)

botoes_frame = tk.Frame(janela)
botoes_frame.pack(pady=10)

tk.Button(botoes_frame, text="Abrir Arquivo", command=abrir_arquivo).pack(side="left", padx=10)
tk.Button(botoes_frame, text="Salvar", command=salvar_edicoes).pack(side="left", padx=10)
tk.Button(botoes_frame, text="Compilar", command=compilar).pack(side="left", padx=10)

tk.Label(janela, text="Saída do Compilador:").pack(anchor="w", padx=10, pady=5)
saida_text = tk.Text(janela, height=12, bg="#f5f5f5", font=("Consolas", 10), state="disabled")
saida_text.pack(padx=10, pady=5, fill="x")

status_label = tk.Label(janela, text="Pronto.", anchor="w", fg="gray")
status_label.pack(fill="x", padx=10, pady=5)

atualizar_linhas()
janela.mainloop()
