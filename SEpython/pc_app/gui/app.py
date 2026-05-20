from datetime import datetime, timedelta
import tkinter as tk
from tkinter import messagebox, ttk

from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure

from pc_app.services.database import fetch_readings
from pc_app.services.esp32_api import get_state, update_config


class UserApp(tk.Tk):
    def __init__(self) -> None:
        super().__init__()
        self.title("SafeHome")
        self.geometry("1080x700")

        self.umbral_luz = tk.StringVar()
        self.umbral_temp = tk.StringVar()
        self.tiempo_servo = tk.StringVar()
        self.intervalo_sensado = tk.StringVar()
        self.period = tk.StringVar(value="Ultima hora")
        self.status = tk.StringVar(value="Sin datos")

        self._build_layout()
        self.refresh_state()
        self.refresh_chart()

    def _build_layout(self) -> None:
        top = ttk.Frame(self, padding=12)
        top.pack(fill="x")

        fields = [
            ("Umbral luz", self.umbral_luz),
            ("Umbral temp", self.umbral_temp),
            ("Tiempo servo", self.tiempo_servo),
            ("Intervalo sensado", self.intervalo_sensado),
        ]
        for col, (label, var) in enumerate(fields):
            ttk.Label(top, text=label).grid(row=0, column=col, sticky="w")
            ttk.Entry(top, textvariable=var, width=15).grid(row=1, column=col, padx=(0, 12))

        ttk.Button(top, text="Guardar configuracion", command=self.save_config).grid(row=1, column=4)
        ttk.Label(top, textvariable=self.status).grid(row=2, column=0, columnspan=5, pady=(10, 0), sticky="w")
        ttk.Label(top, text="Periodo grafica").grid(row=0, column=5, sticky="w")
        period_selector = ttk.Combobox(
            top,
            textvariable=self.period,
            values=("Ultima hora", "6 horas", "24 horas"),
            width=14,
            state="readonly",
        )
        period_selector.grid(row=1, column=5, padx=(12, 0))
        period_selector.bind("<<ComboboxSelected>>", lambda _event: self.refresh_chart())

        body = ttk.Frame(self, padding=12)
        body.pack(fill="both", expand=True)

        self.physical_view = tk.Canvas(body, width=330, bg="#f4f6f8", highlightthickness=1)
        self.physical_view.pack(side="left", fill="y", padx=(0, 12))

        self.figure = Figure(figsize=(7, 4.5), dpi=100)
        self.axes = self.figure.add_subplot(111)
        self.chart = FigureCanvasTkAgg(self.figure, master=body)
        self.chart.get_tk_widget().pack(side="left", fill="both", expand=True)

    def refresh_state(self) -> None:
        try:
            state = get_state()
            config = state["config"]
            sensores = state["sensores"]
            actuadores = state["actuadores"]

            self.umbral_luz.set(str(config["umbralLuz"]))
            self.umbral_temp.set(str(config["umbralTemp"]))
            self.tiempo_servo.set(str(config["tiempoServo"]))
            self.intervalo_sensado.set(str(config["intervaloSensado"]))
            self.status.set(
                f"AHT20 {sensores['tempAHT']:.1f} C | "
                f"Humedad {sensores['humedad']:.1f}% | "
                f"LDR {sensores['ldr']} | "
                f"PIR {'movimiento' if sensores['pir'] else 'sin movimiento'}"
            )
            self.draw_physical_view(sensores, actuadores)
        except Exception as exc:
            self.status.set(f"ESP32 no disponible: {exc}")
        self.after(3000, self.refresh_state)

    def save_config(self) -> None:
        try:
            update_config(
                umbral_luz=int(self.umbral_luz.get()),
                umbral_temp=float(self.umbral_temp.get()),
                tiempo_servo=int(self.tiempo_servo.get()),
                intervalo_sensado=int(self.intervalo_sensado.get()),
            )
        except Exception as exc:
            messagebox.showerror("Error", str(exc))

    def draw_physical_view(self, sensores: dict, actuadores: dict) -> None:
        self.physical_view.delete("all")
        self.physical_view.create_text(165, 28, text="SafeHome", font=("Arial", 15, "bold"))
        self.physical_view.create_rectangle(45, 60, 285, 235, fill="#ffffff", outline="#9aa7b2")
        self.physical_view.create_text(165, 90, text=f"AHT20: {sensores['tempAHT']:.1f} C")
        self.physical_view.create_text(165, 120, text=f"Humedad: {sensores['humedad']:.1f}%")
        self.physical_view.create_text(165, 150, text=f"BMP280: {sensores['tempBMP']:.1f} C")
        self.physical_view.create_text(165, 180, text=f"Presion: {sensores['presion']:.1f} hPa")
        self.physical_view.create_text(165, 210, text=f"LDR: {sensores['ldr']} ADC")

        pir_color = "#d1242f" if sensores["pir"] else "#8c959f"
        relay_color = "#2da44e" if actuadores["relay"] else "#8c959f"
        servo_color = "#2da44e" if actuadores["servo"] else "#8c959f"

        self.physical_view.create_oval(45, 280, 105, 340, fill=pir_color)
        self.physical_view.create_oval(135, 280, 195, 340, fill=relay_color)
        self.physical_view.create_oval(225, 280, 285, 340, fill=servo_color)
        self.physical_view.create_text(75, 365, text="PIR")
        self.physical_view.create_text(165, 365, text="Relay")
        self.physical_view.create_text(255, 365, text="Servo")

    def refresh_chart(self) -> None:
        end = datetime.now()
        hours_by_period = {
            "Ultima hora": 1,
            "6 horas": 6,
            "24 horas": 24,
        }
        selected_hours = hours_by_period.get(self.period.get(), 1)
        start = end - timedelta(hours=selected_hours)
        readings = list(fetch_readings(start, end))

        self.axes.clear()
        if readings:
            x_values = [reading.captured_at for reading in readings]
            self.axes.plot(x_values, [reading.temp_aht for reading in readings], label="Temp AHT20")
            self.axes.plot(x_values, [reading.humedad for reading in readings], label="Humedad")
            self.axes.plot(x_values, [reading.ldr for reading in readings], label="LDR")
            self.axes.legend()
        self.axes.set_title(self.period.get())
        self.axes.set_xlabel("Tiempo")
        self.axes.set_ylabel("Valor")
        self.figure.autofmt_xdate()
        self.chart.draw()
        self.after(10000, self.refresh_chart)


def main() -> None:
    UserApp().mainloop()


if __name__ == "__main__":
    main()
