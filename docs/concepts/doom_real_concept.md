# Konzept: Echtes DOOM auf DodecaRGB

Dieses Dokument beschreibt ein Konzept fuer eine moeglichst echte Portierung von DOOM auf den DodecaRGB-Formfaktor mit Kugelprojektion auf die LEDs.

## Ziel

- Eine lauffaehige Variante von DOOM (Game-Logik + Renderer) auf dem vorhandenen System.
- Anzeige als kontinuierliche, kugelartige Projektion auf den Dodekaeder.
- Steuerung ueber vorhandene Eingaben (Button) plus optional zusaetzliche Eingabemoeglichkeiten.

## Grundannahmen

- LED-Geometrie ist ueber das Modell (`model().point(i)`) verfguegbar.
- Es gibt nur sehr begrenzte lokale Eingabe- und Audio-Moeglichkeiten.
- Die Rechenleistung und RAM-Budgets sind eng fuer ein "echtes" DOOM.

## Architekturvarianten

## Variante A: Native Embedded-Port auf Teensy (hohes Risiko)

- Ein existierender Minimal-DOOM-Port wird auf das Zielsystem angepasst.
- Die Bildausgabe geht in einen kleinen Framebuffer (z. B. 64x32 oder 80x40), nicht direkt in LEDs.
- Danach erfolgt Kugelprojektion von Framebuffer nach LEDs.

### Vorteile

- Maximal autark, keine externe Recheneinheit noetig.

### Nachteile

- Sehr hohes Integrationsrisiko (CPU, RAM, Portierungsaufwand).
- Eingabe und Audio nur stark reduziert realistisch.

## Variante B: Externer Render-Host + Streaming (pragmatisch)

- Echtes DOOM laeuft auf Host (PC/RPi).
- Host streamt stark reduzierte Frames (z. B. 64x32 RGB) per seriell/WLAN an Teensy.
- Teensy uebernimmt nur Projektion und LED-Ausgabe.

### Vorteile

- Deutlich realistischer und stabiler.
- Engine bleibt "echt", Output ist dennoch auf Dodeca projiziert.

### Nachteile

- Zusaetzliche externe Laufzeitkomponente.
- Synchronisation und Transportprotokoll noetig.

## Projektion fuer die "Wahrsagekugel"-Wirkung

- Jede LED-Position wird normalisiert (`x,y,z -> dir`).
- Richtung wird in sphärische Koordinaten gewandelt (`u,v`).
- Das DOOM-Frame wird ueber `u,v` gesampelt.
- Optional: Yaw/Pitch-Offset zur Kameradrehung und Weltrotation.

## Eingabekonzept (minimal)

- Kurz druecken: Action/Fire.
- Lang druecken: Weapon/Use.
- Optional zusaetzlich:
  - IMU-Neigung fuer horizontalen Blickwinkel.
  - Externe Eingabequelle (Host/Controller).

## Rendering-Pipeline

1. Engine liefert Framebuffer (`W x H`).
2. Optionales Post-Processing:
   - Gamma
   - Dithering
   - Helligkeitslimit
3. Projektion auf LEDs.
4. Ausgabe ueber FastLED.

## Performance-Budget (qualitativ)

- Ziel ist visuelle Lesbarkeit, nicht Pixelperfektion.
- Kleine interne Aufloesung ist Pflicht.
- Feste Tickrate fuer Engine und LED-Refresh entkoppeln.

## Implementierungsphasen

1. Proof of Concept:
   - Statisches Testbild auf Kugelprojektion.
2. Bewegtes Testsignal:
   - Scroll/Rotation aus Framebuffer auf LEDs.
3. Engine-Integration:
   - Entweder Embedded-Port oder Host-Stream.
4. Input-Mapping:
   - Button/IMU/extern.
5. Stabilisierung:
   - Frame pacing, Fehlerhandling, Fallbacks.

## Risiken

- Embedded-Port kann an Speichergrenzen scheitern.
- Rendering + Projektion + LED-Ausgabe kann Framerate stark begrenzen.
- Wartbarkeit bei stark spezialisierten Port-Patches.

## Empfehlung

- Fuer "echtes DOOM" auf diesem Formfaktor ist **Variante B (Host + Stream)** technisch am robustesten.
- Fuer reinen On-Device-Gag ist **Pseudo-DOOM** das deutlich bessere Preis-Leistungs-Verhaeltnis.
