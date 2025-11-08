import tkinter as tk
from tkinter import filedialog, messagebox
import subprocess
import threading
import os
import re

COMPILADOR_EXEC = "main.exe"  # ou "./main" no Linux
arquivo_atual = None
saida_compilador = ""


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
            limpar_destaques()
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


def limpar_destaques():
    # remove tag se existir
    try:
        entrada_text.tag_delete("erro_linha")
    except tk.TclError:
        pass


def exibir_saida(texto):
    """Exibe texto na área de saída"""
    saida_text.config(state="normal")
    saida_text.insert(tk.END, texto)
    saida_text.see(tk.END)
    saida_text.config(state="disabled")


def destacar_erros():
    """Procura todas as linhas [n] nos erros e destaca no editor"""
    global saida_compilador

    limpar_destaques()
    entrada_text.tag_config("erro_linha", background="#ffcccc")

    # Captura todas as ocorrências do padrão [n]
    linhas_erro = re.findall(r"\[(\d+)\]", saida_compilador)

    if not linhas_erro:
        status_label.config(text="Nenhum erro detectado.")
        return

    for num in linhas_erro:
        linha = int(num)
        start = f"{linha}.0"
        end = f"{linha}.0 lineend"
        try:
            entrada_text.tag_add("erro_linha", start, end)
        except tk.TclError:
            pass

    # scroll para o primeiro erro
    entrada_text.see(f"{linhas_erro[0]}.0")
    status_label.config(text=f"Erros detectados nas linhas: {', '.join(linhas_erro)}")


def compilar():
    global arquivo_atual, saida_compilador
    if not arquivo_atual:
        messagebox.showinfo("Compilar", "Abra um arquivo antes de compilar.")
        return

    if not os.path.exists(COMPILADOR_EXEC):
        messagebox.showerror("Erro", f"Compilador não encontrado: {COMPILADOR_EXEC}")
        return

    salvar_edicoes()
    limpar_saida()
    limpar_destaques()
    saida_compilador = ""
    status_label.config(text="Compilando...")

    def thread_exec():
        # declarar global para atualizar a variável global dentro da thread
        global saida_compilador
        # Ajuste de shell: em Windows costuma ser True para executáveis, em Linux deixe False
        use_shell = os.name == "nt"
        process = subprocess.Popen(
            [COMPILADOR_EXEC, arquivo_atual],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            shell=use_shell,
            bufsize=1,
            universal_newlines=True
        )

        # leia linhas enquanto o processo produzir saída
        for line in iter(process.stdout.readline, ''):
            if line:
                saida_compilador += line
                # exibe imediatamente
                janela.after(0, lambda l=line: exibir_saida(l))

        process.stdout.close()
        process.wait()

        # Quando terminar, destaca os erros (após um pequeno delay para garantir UI pronta)
        janela.after(100, destacar_erros)
        janela.after(0, lambda: status_label.config(text="Compilação finalizada."))

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
saida_text.bind("<Key>", lambda e: "break")

status_label = tk.Label(janela, text="Pronto.", anchor="w", fg="gray")
status_label.pack(fill="x", padx=10, pady=5)

atualizar_linhas()
janela.mainloop()
