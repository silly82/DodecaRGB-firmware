# Projektuebersicht und Analyse (interne Notizen)

Diese Datei haelt zentrale Erkenntnisse zur DodecaRGB-Firmware fest, um spaeter schnell wieder in die Architektur und Entwicklungsablaeufe einzusteigen.

## 1. Projektziel

- Open-Source-Firmware fuer ein LED-Dodekaeder (DodecaRGB V2) mit Teensy 4.1.
- Animationssystem basiert auf FastLED und PixelTheater.
- Fokus auf geometriebasierte, modellgetriebene Szenen.

Referenzen:
- `README.md`
- `docs/index.md`

## 2. Hauptstruktur des Repositories

- `src/`: Firmware-Einstiegspunkt, Szenen und Modellintegration
  - `src/main.cpp`
  - `src/scenes/`
  - `src/models/`
- `lib/PixelTheater/`: Kernbibliothek (Theater, Platform, Scene, Model APIs)
- `docs/`: Nutzer- und Entwicklerdokumentation
- `util/`: Python-Tooling fuer Modellgenerierung und Analyse
- `test/`: Native-, Hardware- und Web-Tests
- `platformio.ini`: Build- und Testumgebungen

## 3. Architekturbild (vereinfacht)

- `Theater` ist die zentrale Fassade.
- `Platform` kapselt Hardwareabhaengigkeiten (Zeit, LED-Ausgabe, Logging).
- `IModel`/Modell liefert Geometrie und Nachbarschaften.
- `Scene` implementiert Animationslogik (`setup()`, `tick()`).
- `main.cpp` verdrahtet Platform, Modell und Szenenliste.

Wichtige Referenzen:
- `docs/PixelTheater/README.md`
- `lib/PixelTheater/include/PixelTheater/theater.h`
- `lib/PixelTheater/include/PixelTheater/platform/platform.h`
- `lib/PixelTheater/include/PixelTheater/core/imodel.h`

## 4. Build, Test und Tooling

- Teensy-Firmware:
  - `pio run -e teensy41`
- Native C++ Tests:
  - `pio test -e native`
- Web-Umgebung:
  - `pio run -e web`
  - zusaetzliche Buildlogik ueber `scripts/web_build.py` und `build_web.sh`
- Python-Utilities:
  - Modellgenerierung und Utility-Tests unter `util/`

Referenzen:
- `platformio.ini`
- `build_web.sh`
- `util/README.md`
- `docs/guides/development.md`

## 5. Integration neuer Szenen und Modelle

### Neue Szene

1. Szene in `src/scenes/<name>/` anlegen.
2. Von `PixelTheater::Scene` ableiten.
3. In `src/main.cpp` registrieren (`theater.addScene<...>()`).

### Neues oder geaendertes Modell

1. Modell-YAML und PCB-Daten unter `src/models/...` pflegen.
2. Generator ausfuehren (`util/generate_model.py`).
3. Generiertes `model.h` im Firmware-Pfad einbinden.

Referenzen:
- `docs/guides/creating_animations.md`
- `util/README.md`
- `src/main.cpp`

## 6. Beobachtete Risiken und Wartungsaspekte

- `src/main.cpp` ist umfangreich und vereint viele Verantwortlichkeiten (Init, Szenen, Eingaben, Sensorik).
- Teilweise enge Kopplung zwischen Laufzeitlogik und einzelnen Szenennamen.
- Dokumentationsverweis in `platformio.ini` auf `lib/Architecture.md` scheint nicht vorhanden.
- Pin-/Konfigurationskonflikte sollten regelmaessig gegen Hardwarebelegung geprueft werden.

## 7. Empfohlener Wiedereinstieg (Lesereihenfolge)

1. `README.md`
2. `docs/PixelTheater/README.md`
3. `platformio.ini`
4. `src/main.cpp`
5. `docs/guides/creating_animations.md`

## 8. Kurzfazit

Die Codebasis ist funktional breit aufgestellt (Firmware + Modellpipeline + Simulator + Tests). Fuer kuenftige Wartung waere eine weitere Entkopplung von `main.cpp` in klarere Runtime-Module (Input/Sensorik/Szenensteuerung) wahrscheinlich der groesste Hebel.
