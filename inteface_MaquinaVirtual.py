#!/usr/bin/env python3

import tkinter as tk
from tkinter import filedialog, scrolledtext, messagebox
import threading
import time

MEM_SIZE = 10000
STEP_DELAY = 0.05   # atraso entre instruções quando em 'Run'

# ------- Instr + Parser -------
class Instr:
    def __init__(self, label, op, a=None, b=None, raw=""):
        self.label = label
        self.op = op
        self.a = a
        self.b = b
        self.raw = raw

def parse_obj_lines(lines):
    instrs = []
    for raw in lines:
        r = raw.rstrip("\n\r")
        if r.strip() == "":
            continue
        parts = r.split()
        label = None; op=None; a=None; b=None
        idx = 0
        if parts[0].upper().startswith('X'):
            label = parts[0]
            idx = 1
        if idx < len(parts):
            op = parts[idx].upper()
            idx += 1
        if idx < len(parts):
            a = parts[idx]; idx += 1
        if idx < len(parts):
            b = parts[idx]; idx += 1
        instrs.append(Instr(label, op, a, b, raw))
    return instrs

# ------- Máquina Virtual (simulação) -------
class MV:
    def __init__(self):
        self.mem = [0]*MEM_SIZE   # vet_memoria
        self.s = -1               # topo da pilha
        self.i = 0                # PC (index na lista de instrucoes)
        self.code = []            # lista Instr
        self.label_map = {}       # label -> index
        self.output_lines = []    # buffer de linhas de saída (PRN)
        self.last_instr = None
        self.waiting_for_input = False
        self.halted = False

    def load_code(self, instrs):
        self.code = instrs
        self.label_map = {}
        for idx, ins in enumerate(instrs):
            if ins.label:
                self.label_map[ins.label] = idx
        self.reset_state()

    def reset_state(self):
        self.mem = [0]*MEM_SIZE
        self.s = -1
        self.i = 0
        self.output_lines = []
        self.last_instr = None
        self.waiting_for_input = False
        self.halted = False

    def resolve_label(self, token):
        if token is None:
            raise ValueError("token vazio")
        t = token.strip()
        if t.upper().startswith('X'):
            if t in self.label_map:
                return self.label_map[t]
            else:
                raise ValueError(f"Label {t} nao encontrada")
        else:
            return int(t)

    def push(self, v):
        self.s += 1
        if self.s >= MEM_SIZE:
            raise RuntimeError("Stack overflow")
        self.mem[self.s] = int(v)

    def pop(self):
        if self.s < 0:
            raise RuntimeError("Stack underflow")
        v = self.mem[self.s]
        self.s -= 1
        return v

    def step(self):
        """Executa uma instrução. Retorna True se executou, False se HLT ou espera input."""
        if self.halted:
            return False
        if not (0 <= self.i < len(self.code)):
            self.halted = True
            return False

        q = self.code[self.i]
        op = (q.op or "").upper()
        self.last_instr = q.raw

        # START / HLT
        if op == "START":
            self.s = -1
            self.i += 1; return True
        if op == "HLT":
            self.halted = True; return False

        # LDC / LDV / STR
        if op == "LDC":
            self.push(int(q.a))
            self.i += 1
            return True
        if op == "LDV":
            addr = int(q.a)
            self.push(self.mem[addr])
            self.i += 1
            return True
        if op == "STR":
            addr = int(q.a)
            v = self.pop()
            self.mem[addr] = v
            self.i += 1
            return True

        # arithmetic
        if op == "ADD":
            b = self.pop(); a = self.pop(); self.push(a+b); self.i+=1; return True
        if op == "SUB":
            b = self.pop(); a = self.pop(); self.push(a-b); self.i+=1; return True
        if op == "MULT":
            b = self.pop(); a = self.pop(); self.push(a*b); self.i+=1; return True
        if op == "DIVI":
            b = self.pop(); a = self.pop(); self.push(int(a/b) if b!=0 else 0); self.i+=1; return True
        if op == "INV":
            self.mem[self.s] = -self.mem[self.s]; self.i+=1; return True
        if op == "NEG":
            self.mem[self.s] = 1 - self.mem[self.s]; self.i+=1; return True

        # logical / comparison
        if op == "AND":
            b = self.pop(); a = self.pop(); self.push(1 if (a==1 and b==1) else 0); self.i+=1; return True
        if op == "OR":
            b = self.pop(); a = self.pop(); self.push(1 if (a==1 or b==1) else 0); self.i+=1; return True
        if op == "CME":
            b = self.pop(); a = self.pop(); self.push(1 if a < b else 0); self.i+=1; return True
        if op == "CMA":
            b = self.pop(); a = self.pop(); self.push(1 if a > b else 0); self.i+=1; return True
        if op == "CEQ":
            b = self.pop(); a = self.pop(); self.push(1 if a == b else 0); self.i+=1; return True
        if op == "CDIF":
            b = self.pop(); a = self.pop(); self.push(1 if a != b else 0); self.i+=1; return True
        if op == "CMEQ":
            b = self.pop(); a = self.pop(); self.push(1 if a <= b else 0); self.i+=1; return True
        if op == "CMAQ":
            b = self.pop(); a = self.pop(); self.push(1 if a >= b else 0); self.i+=1; return True

        # PRN
        if op == "PRN":
            if self.s >= 0:
                self.output_lines.append(str(self.mem[self.s]))
                self.s -= 1
            else:
                self.output_lines.append("[PRN_EMPTY]")
            self.i += 1
            return True

        # RD -> signal waiting for input (do not advance PC)
        if op == "RD":
            self.s += 1
            self.waiting_for_input = True
            return False

        # CALL / RETURN / JMP / JMPF
        if op == "CALL":
            self.push(self.i + 1)
            self.i = self.resolve_label(q.a)
            return True
        if op == "RETURN":
            if self.s < 0:
                self.halted = True; return False
            ret = self.pop()
            self.i = int(ret)
            return True
        if op == "JMP":
            self.i = self.resolve_label(q.a); return True
        if op == "JMPF":
            cond = 0
            if self.s >= 0:
                cond = self.mem[self.s]; self.s -= 1
            if cond == 0:
                self.i = self.resolve_label(q.a)
            else:
                self.i += 1
            return True

        # ALLOC / DALLOC
        if op == "ALLOC":
            m = int(q.a) if q.a else 0
            n = int(q.b) if q.b else 0
            for k in range(n):
                self.s += 1
                self.mem[self.s] = self.mem[m + k]
            self.i += 1; return True
        if op == "DALLOC":
            m = int(q.a) if q.a else 0
            n = int(q.b) if q.b else 0
            for k in range(n-1, -1, -1):
                val = 0
                if self.s >= 0:
                    val = self.mem[self.s]; self.s -= 1
                self.mem[m + k] = val
            self.i += 1; return True

        # fallback: unknown -> advance
        self.i += 1
        return True

    def provide_input(self, value):
        if not self.waiting_for_input:
            return False
        self.waiting_for_input = False
        self.mem[self.s] = int(value)
        self.i += 1
        return True

# ------- GUI -------
class DebuggerGUI:
    def __init__(self, root):
        self.root = root
        root.title("MV Debugger - Full")
        self.mv = MV()
        self.filename = None
        self.runner_thread = None
        self.runner_stop = threading.Event()

        # Controls top
        top = tk.Frame(root)
        top.pack(fill="x", padx=6, pady=6)
        tk.Button(top, text="Escolher arquivo .obj", command=self.open_file).pack(side="left")
        tk.Button(top, text="▶ Executar", command=self.run).pack(side="left", padx=4)
        tk.Button(top, text="⏸ Pausar", command=self.pause).pack(side="left", padx=4)
        tk.Button(top, text="⏭ Próxima", command=self.step).pack(side="left", padx=4)
        tk.Button(top, text="🔁 Reset", command=self.reset).pack(side="left", padx=4)

        # Main panes: left=code, center=output, right=state
        main = tk.PanedWindow(root, orient="horizontal")
        main.pack(fill="both", expand=True, padx=6, pady=6)

        # left: code list
        left = tk.Frame(main)
        tk.Label(left, text="Código (.obj)").pack(anchor="w")
        self.lst_code = tk.Listbox(left, width=60, height=35, font=("Courier",10))
        sb = tk.Scrollbar(left, command=self.lst_code.yview)
        self.lst_code.config(yscrollcommand=sb.set)
        self.lst_code.pack(side="left", fill="both", expand=True)
        sb.pack(side="left", fill="y")
        main.add(left)

        # right column (state on top, output on bottom)
        right = tk.Frame(main)

        # ----- Estado interno (topo) -----
        frame_state = tk.Frame(right)
        tk.Label(frame_state, text="Estado interno").pack(anchor="w")
        self.state_box = scrolledtext.ScrolledText(frame_state, width=40, height=25, font=("Courier",10))
        self.state_box.pack(fill="both", expand=True)
        frame_state.pack(fill="both", expand=True)

        # ----- Saída (abaixo, menor) -----
        frame_output = tk.Frame(right)
        tk.Label(frame_output, text="Saída (linha a linha)").pack(anchor="w")
        self.txt_out = scrolledtext.ScrolledText(frame_output, width=40, height=10, font=("Courier",10))
        self.txt_out.pack(fill="both", expand=True)
        frame_output.pack(fill="x")

        main.add(right)


        # bottom: input + status
        bottom = tk.Frame(root)
        bottom.pack(fill="x", padx=6, pady=6)
        tk.Label(bottom, text="Entrada manual (para RD):").pack(side="left")
        self.entry = tk.Entry(bottom, width=20); self.entry.pack(side="left", padx=6)
        tk.Button(bottom, text="Enviar", command=self.send_input).pack(side="left")
        self.status = tk.Label(bottom, text="Pronto"); self.status.pack(side="right")

        # UI update loop
        self.update_ui_loop()

    # ---------- file ----------
    def open_file(self):
        path = filedialog.askopenfilename(title="Escolher .obj", filetypes=[("OBJ","*.obj"),("Todos","*.*")])
        if not path:
            return
        self.filename = path
        with open(path, "r", encoding="utf-8", errors="ignore") as f:
            lines = f.readlines()
        instrs = parse_obj_lines(lines)
        self.mv.load_code(instrs)

        # populate list
        self.lst_code.delete(0, tk.END)
        for idx, ins in enumerate(instrs):
            self.lst_code.insert(tk.END, f"{idx:04d}    {ins.raw}")

        # show raw file content in output area (pure content, no modification)
        self.txt_out.delete("1.0", tk.END)
        self.txt_out.see(tk.END)
        self.status.config(text=f"Arquivo carregado: {path}")

    # ---------- control ----------
    def step(self):
        if self.mv.waiting_for_input:
            self.status.config(text="Aguardando input (use Enviar)")
            return
        executed = self.mv.step()
        # flush outputs
        self.flush_output()
        if not executed:
            if self.mv.waiting_for_input:
                self.status.config(text="Pausado - RD aguardando entrada")
            elif self.mv.halted:
                self.status.config(text="Halted / fim")
            else:
                self.status.config(text="Parado")
        else:
            self.status.config(text="Executou 1 instrução")
        self.highlight_current()

    def run(self):
        if self.runner_thread and self.runner_thread.is_alive():
            self.status.config(text="Já executando")
            return
        self.runner_stop.clear()
        self.runner_thread = threading.Thread(target=self._run_loop, daemon=True)
        self.runner_thread.start()
        self.status.config(text="Executando...")

    def _run_loop(self):
        try:
            while not self.runner_stop.is_set() and not self.mv.halted:
                if self.mv.waiting_for_input:
                    self.status.config(text="Pausado - RD aguardando entrada")
                    break
                ok = self.mv.step()
                self.flush_output()
                self.highlight_current()
                if not ok:
                    break
                time.sleep(STEP_DELAY)
            if self.mv.halted:
                self.status.config(text="Halted / fim")
            else:
                self.status.config(text="Execução pausada")
        except Exception as e:
            self.status.config(text=f"Erro na execução: {e}")

    def pause(self):
        self.runner_stop.set()
        self.status.config(text="Pausado pelo usuário")

    def reset(self):
        if not self.filename:
            self.mv.reset_state()
            self.txt_out.delete("1.0", tk.END)
            self.lst_code.delete(0, tk.END)
            self.state_box.delete("1.0", tk.END)
            self.status.config(text="Reset (sem arquivo)")
            return
        with open(self.filename, "r", encoding="utf-8", errors="ignore") as f:
            instrs = parse_obj_lines(f.readlines())
        self.mv.load_code(instrs)
        self.txt_out.delete("1.0", tk.END)
        with open(self.filename, "r", encoding="utf-8", errors="ignore") as f:
            self.txt_out.insert(tk.END, f.read())
        self.txt_out.insert(tk.END, "\n")
        self.lst_code.delete(0, tk.END)
        for idx, ins in enumerate(instrs):
            self.lst_code.insert(tk.END, f"{idx:04d}    {ins.raw}")
        self.state_box.delete("1.0", tk.END)
        self.status.config(text="Reset concluído")
        self.highlight_current()

    # ---------- I/O ----------
    def send_input(self):
        v = self.entry.get().strip()
        if v == "":
            return
        ok = self.mv.provide_input(v)
        self.entry.delete(0, tk.END)
        if ok:
            self.status.config(text=f"Entrada {v} enviada")
            # after providing input, process the instruction immediately
            executed = self.mv.step()
            self.flush_output()
            if self.mv.halted:
                self.status.config(text="Halted")
            else:
                self.status.config(text="Continuou após input")
            self.highlight_current()
        else:
            self.status.config(text="VM não aguardava input")

    # ---------- helpers ----------
    def flush_output(self):
        while self.mv.output_lines:
            ln = self.mv.output_lines.pop(0)
            self.txt_out.insert(tk.END, ln + "\n")
            self.txt_out.see(tk.END)

    def highlight_current(self):
        self.lst_code.selection_clear(0, tk.END)
        cur = self.mv.i
        if 0 <= cur < self.lst_code.size():
            self.lst_code.selection_set(cur)
            self.lst_code.see(cur)

    def update_state_view(self):
        mv = self.mv
        self.state_box.delete("1.0", tk.END)
        self.state_box.insert(tk.END, f"PC atual (i): {mv.i}\n")
        self.state_box.insert(tk.END, f"Stack pointer (s): {mv.s}\n")
        self.state_box.insert(tk.END, f"Última instrução: {mv.last_instr}\n\n")

        self.state_box.insert(tk.END, "Pilha (topo -> base):\n")
        if mv.s < 0:
            self.state_box.insert(tk.END, "  [Vazia]\n")
        else:
            top = mv.s
            for k in range(top, max(-1, top-40), -1):
                try:
                    self.state_box.insert(tk.END, f"  [{k:04d}] = {mv.mem[k]}\n")
                except IndexError:
                    pass

        self.state_box.insert(tk.END, "\nMemória:\n")
        used = [idx for idx,val in enumerate(mv.mem) if val != 0]
        if not used:
            self.state_box.insert(tk.END, "  Nenhum valor carregado\n")
        else:
            for addr in used[:120]:
                self.state_box.insert(tk.END, f"  MEM[{addr}] = {mv.mem[addr]}\n")

        st = []
        if mv.halted: st.append("HALTED")
        if mv.waiting_for_input: st.append("AGUARDANDO INPUT (RD)")
        if not st: st.append("PRONTO / EXECUTANDO")
        self.state_box.insert(tk.END, "\nEstado interno: " + ", ".join(st) + "\n")

    def update_ui_loop(self):
        self.flush_output()
        self.highlight_current()
        self.update_state_view()
        self.root.after(120, self.update_ui_loop)

# ---- main ----
def main():
    root = tk.Tk()
    app = DebuggerGUI(root)
    root.geometry("1350x800")
    root.mainloop()

if __name__ == "__main__":
    main()
