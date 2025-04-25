# 🚀 Praktika – Echtzeitsteuerung mit STM32 & Simulink

Willkommen in meinem Projekt-Portfolio des **Praktikums** an der TU Darmstadt – ein anwendungsorientiertes Laborpraktikum mit Fokus auf **Echtzeitsysteme**, **digitale Motorsteuerung** und **bare-metal STM32-Programmierung** mit **C und Simulink**.

> 🧠 _Technisch tiefgehend: STM32-Mikrocontroller-Programmierung, Echtzeitkommunikation (UART/SPI), PWM-Generierung, PI-Regler, Modellbasierte Entwicklung mit Simulink._
![image](https://github.com/user-attachments/assets/af48464a-ecc9-4bcf-a014-559e22d3d686)

---

## 📌 Inhaltsverzeichnis

- 🔍 [Projektübersicht](#-projektübersicht)
- 🛠️ [Erworbene Kompetenzen](#-erworbene-kompetenzen)
- 📊 [Ergebnisse und Visualisierungen](#-ergebnisse-und-visualisierungen)
- ⚙️ [Technische Highlights](#️-technische-highlights)
- 🧪 [Build- und Deployment-Anleitung](#-build--und-deployment-anleitung)
- 📁 [Projektstruktur](#-projektstruktur)

---

## 🔍 Projektübersicht

Das Projekt gliedert sich in **vier Versuche**, die jeweils unterschiedliche Aspekte eingebetteter Systeme vertiefen:

| 🔢 Versuch | 🎯 Thema | 🧩 Fokus |
|:-----------|:---------|:--------|
| 1️⃣ | Mikrocontroller-Programmierung | UART, SPI, GPIO, OLED-Display |
| 2️⃣ | PWM- und U/f-Steuerung | Festkommaarithmetik, Sinus-Generator |
| 3️⃣ | Drehzahlregelung | PI-Regler-Entwurf, Drehzahlschätzung |
| 4️⃣ | Modellbasierte Entwicklung | Simulink, Embedded Coder, Autocode-Generierung |

📌 Alle Versuche wurden auf einem **STM32F429ZIT6** Mikrocontroller (ARM Cortex-M4) umgesetzt und an einer **Asynchronmaschine** getestet.

---

## 🛠️ Erworbene Kompetenzen

| ✅ | Kompetenz |
|----|-----------|
| 💻 | Bare-Metal-C-Programmierung (ohne HAL) |
| 📡 | Implementierung von UART- und SPI-Kommunikation |
| 🎛️ | Entwurf von Echtzeit-Regelkreisen (PI-Regler) |
| 🔢 | Festkommaarithmetik für optimierte Rechenleistung |
| 📈 | Signalfilterung und Interpolation |
| 🧮 | Modellbasierte Entwicklung in Simulink |
| 🧰 | Nutzung von STM32-Toolchains, OpenOCD, GDB |

---

## 📊 Ergebnisse und Visualisierungen

| 📌 Thema | 🖼️ Abbildung | 📁 Quelle |
|:---------|:------------:|:---------|
| PWM-Sinusgenerator | ![image](https://github.com/user-attachments/assets/50465a21-6f95-4379-b4d1-9a4a5dfedab7) 
| Gefilterte Ist-Drehzahl ±3000rpm | ![Speed](figures/Abbildung2_IstDrehzahl_3000.png) 
| Sprungantwort bei PI-Regelung |![image](https://github.com/user-attachments/assets/7b136884-7e70-4d80-ba4d-e83697ce3688)
| Sensordatenfusion |![image](https://github.com/user-attachments/assets/963fbde7-ba2c-447c-b3c4-e648ac93d824)


---

## ⚙️️ Technische Highlights

| 🔧 Komponente | 📝 Beschreibung |
|---------------|-----------------|
| 🧠 Mikrocontroller | STM32F429ZIT6 (ARM Cortex-M4, 180 MHz) |
| 💬 Kommunikation | UART (115200 Baud), SPI (16 Bit) |
| 🎨 Anzeige | OLED über SPI (NHD-2.8-25664UCB2) |
| 📊 Steuerung | PWM mit U/f-Strategie, PI-Regler |
| 🔢 Arithmetik | 16.16-Festkommaarithmetik |
| 🧩 IDEs | CLion, OpenOCD, `arm-none-eabi-gcc` |
| 🧠 Modellierung | Simulink & Embedded Coder |


---
